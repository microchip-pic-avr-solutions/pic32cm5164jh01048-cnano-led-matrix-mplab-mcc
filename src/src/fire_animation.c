/**
 *  (c) 2025 Microchip Technology Inc. and its subsidiaries.
 *
 *  Subject to your compliance with these terms, you may use Microchip software
 *  and any derivatives exclusively with Microchip products. You're responsible
 *  for complying with 3rd party license terms applicable to your use of 3rd
 *  party software (including open source software) that may accompany Microchip
 *  software.
 *
 *  SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 *  APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 *  INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 *  WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
 *  HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
 *  THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
 *  CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY,
 *  YOU PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 *  @file  fire_animation.c
 *  @brief This file contains the needed functions for the Fire Animation application.
 */
#include <xc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "../include/mainApp.h"

#define FLAREROWS   8U    
#define MAXFLARE    10U  
#define FLAREPROB   60U 
#define FLAREDECAY  14U 

uint8_t pix[PANEL_YSIZE][PANEL_XSIZE];
uint8_t nflare = 0;
uint32_t flare[MAXFLARE];

extern const ws2812_color_t myColors[];
extern const uint8_t FIRE_NCOLORS;

uint32_t isqrt(uint32_t n) 
{
    if (n < 2U)
    {
        return n;
    }
    
    uint32_t smallCandidate = isqrt(n >> 2U) << 1;
    uint32_t largeCandidate = smallCandidate + 1U;
    
    return (largeCandidate*largeCandidate > n) ? smallCandidate : largeCandidate;
}


// Set pixels to intensity around flare

void fireIntensity(uint16_t x, uint16_t y, uint16_t z_intensity)
{
    uint16_t b_flareArea = z_intensity * 10U / FLAREDECAY + 1U;
    for(uint16_t i = (y-b_flareArea); i < (y+b_flareArea); i++)
    {
        for(uint16_t j = (x-b_flareArea); j < (x+b_flareArea); j++)
        {
            if (i >=0U && j >= 0U && i < PANEL_YSIZE && j < PANEL_XSIZE)
            {
                uint16_t d_distance = (FLAREDECAY * isqrt((x-j)*(x-j) + (y-i)*(y-i)) + 5U) / 10U;
                uint8_t n = 0;
                
                if(z_intensity > d_distance)
                {
                    n = z_intensity - d_distance;
                }
                if(n > pix[i][j])
                {
                    pix[i][j] = n;
                }
            }
        }
    }
}

void newFlare(void) 
{
    if (nflare < MAXFLARE && (rand() % 100U) <= FLAREPROB) 
    {
        uint16_t x = rand() % (PANEL_XSIZE);
        uint16_t y = rand() % FLAREROWS;
        uint16_t z = FIRE_NCOLORS - 1U;

        flare[nflare++] = ((uint32_t)z<<16U) | ((uint32_t)y<<8U) | ((uint32_t)x&0xFFU);
        fireIntensity(x,y,z);
    }
}

void fireAnimationUpdate(void)
{
    // First, move all existing heat points up the display and fade
    for(uint16_t i = PANEL_YSIZE - 1U; i > 0U; i--)
    {
        for(uint16_t j = 0; j < PANEL_XSIZE; j++)
        {
            uint8_t n = 0;
            if(pix[i-1U][j] > 0U)
            {
                n = pix[i-1U][j] - 1U;
                pix[i][j] = n;
            }
            else
            {
                pix[i][j] = 0;
            }
        }
    }
    // Heat the bottom row
    for(uint16_t j = 0; j < PANEL_XSIZE; j++)
    {
        if(pix[0][j] > 0U)
        {
            pix[0][j] = rand() % 5U + (FIRE_NCOLORS - 6U);
        }
    }
    
    for(uint16_t i = 0; i < nflare; i++)
    {
        uint16_t x = (uint16_t)flare[i] & 0xFFU;
        uint16_t y = ((uint16_t)flare[i] >> 8U) & 0xFFU;
        uint32_t z = (flare[i] >> 16U) & 0xFFU;
        fireIntensity(x,y,z);
        
        if(z>1U)
        {
            flare[i] =((uint32_t)flare[i] &0xFFFFU) | ((uint32_t)(z-1U)<<16U);
        }
        else
        {
            // This flare is out
            for(uint16_t j = i+1U; j < nflare; j++)
            {
                flare[j-1U]=flare[j];
            }
            nflare--;
        }
    }
    newFlare();
    // Set and draw
    for (uint16_t i=0; i<PANEL_YSIZE; i++) 
    {
        for (uint16_t j=0; j<PANEL_XSIZE; j++) 
        {
            WS2812_DrawColorSet(pix[PANEL_YSIZE - i - 1U][PANEL_XSIZE - j - 1U] + (uint8_t)FIRE_ANIMATION_COLORS);
            WS2812_PixelSet(WS2812_PanelXYOffsetGet(j,i));
        }
    }
    
}


void fireAnimationInitialize(void)
{

    for (uint16_t i=0; i<PANEL_YSIZE; i++) 
    {
        for (uint16_t j=0; j<PANEL_XSIZE; j++) 
        {
            if (i == 0U) 
                pix[i][j] = FIRE_NCOLORS - 1U;
            else 
                pix[i][j] = 0;
        }
        
    }
}

bool FIRE_Main(uint8_t rdKey)
{
    switch (rdKey)
    {
        case INIT_APP:
            WS2812_MatrixClear(0);
            fireAnimationInitialize();
            return true;
        case MENU_SELECT:
            if (APP_StatusGet() == (uint8_t)APP_RUN)
            {
                APP_StatusSet(APP_PAUSE);
            }
            else if (APP_StatusGet() == (uint8_t)APP_PAUSE)
            {
                APP_StatusSet(APP_RUN);
            }
            return false;
        default:
            break;
    }

    if (APP_StatusGet() != (uint8_t)APP_RUN)
        return false;
    fireAnimationUpdate();
    return true;
}