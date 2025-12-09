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
 *  @file  scroll_animation.c
 *  @brief This file contains the needed functions for the Scrolling Text application.
 */
#include <xc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/mainApp.h"

#define TEXT_YPOSITION          8U
#define SCROLL_LEADING_SPACES   9U


extern const ws2812_color_t myColors[];
extern const uint8_t font_fixed_5x7_data[];

uint16_t scrollBufferOffset = 0; 
uint8_t scrollBuffer[500]; 
char textBuffer[] = "PIC32CM-JH01 LED MATRIX";

void textScrollingInitialize(void)
{
   uint8_t ch_font;
   uint8_t data; 
   uint8_t ch;
   
   //add leading blank spaces
   for(uint8_t i = 0; i < SCROLL_LEADING_SPACES; i++)
   {
        ch = ' ';
        ch_font = ch - CHAR_OFFSET;
        for (uint16_t j = 0; j < FONT_XSIZE; j++)
        {
            data = font_fixed_5x7_data[(FONT_XSIZE*ch_font)+j];
            scrollBuffer[j + (i*(FONT_XSIZE + 1U))] = data;
        }
        scrollBuffer[((i+1U)*FONT_XSIZE) + i] = 0x00;
   }
   
   for(uint16_t i = SCROLL_LEADING_SPACES; i < strlen(textBuffer) + SCROLL_LEADING_SPACES; i++)
   {
        ch = textBuffer[i - SCROLL_LEADING_SPACES];
        if ((ch < CHAR_OFFSET)||(ch > 127U)) return;
        ch_font = ch - CHAR_OFFSET;
        for (uint16_t j = 0; j < FONT_XSIZE; j++)
        {
            data = font_fixed_5x7_data[(FONT_XSIZE*ch_font)+j];
            scrollBuffer[j + (i*(FONT_XSIZE + 1U))] = data;
        }
        scrollBuffer[(((i+1U)*FONT_XSIZE) + i)] = 0x00;
   }

}


void textScrollinglPrint(uint16_t x, uint16_t y, uint16_t pos)
{
    uint32_t offset;
    for(uint16_t i = pos; i <(pos + PANEL_XSIZE); i++)
    {
        for (uint16_t j = 0; j < FONT_YSIZE; j++){
            if ((scrollBuffer[i] & (uint16_t)(1U << j)) != 0 )
            {
                offset = WS2812_PanelXYOffsetGet((x+i-pos ),(y + j));
                WS2812_PixelSet(offset);
            }
        }
    }
}

void textScrollinglUpdate(void)
{
    WS2812_DrawColorSet(BLACK);
    textScrollinglPrint(0,TEXT_YPOSITION,scrollBufferOffset);
    scrollBufferOffset++;
    
    if(scrollBufferOffset == (strlen(textBuffer) +9U) * (FONT_XSIZE + 1U))
    {
        scrollBufferOffset = 0;
    }

    WS2812_DrawColorSet(RED);
    textScrollinglPrint(0,TEXT_YPOSITION,scrollBufferOffset);
}


bool SCROLL_Main(uint8_t rdKey)
{
    switch (rdKey)
    {
    case INIT_APP:
        WS2812_MatrixClear(0);
        textScrollingInitialize();
        return true;
        break;
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

    textScrollinglUpdate();
    return true;
}
