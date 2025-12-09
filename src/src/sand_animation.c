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
 *  @file  sand_animation.c
 *  @brief This file contains the needed functions for the Sand Animation application.
 */
#include <xc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "../include/mainApp.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define COLORS_ARRAY_RED grbValues[1]
#define COLORS_ARRAY_GREEN grbValues[0]
#define COLORS_ARRAY_BLUE grbValues[2]
#define PANEL_STATE_NONE        0U
#define PANEL_STATE_NEW         1U
#define PANEL_STATE_FALLING     2U
#define PANEL_STATE_COMPLETE    3U

uint8_t grbValues[3] = {0x00, 0x31, 0x00};
uint8_t colorChangeTime = 3;
uint8_t colorAllChangeTime = 2;
uint8_t colorTimerCounter = 0;
uint8_t colorAllTimerCounter = 0;
uint8_t inputXChangeTime = 10;
uint8_t inputXChangeTimeCounter = 0;
uint8_t newKValue = 0;

uint16_t maxVelocity = 2;
uint16_t gravity = 1;
int16_t adjacentVelocityResetValue = 3;

uint16_t inputWidth = 2;
uint16_t inputX = 24;
uint16_t inputY = 0;
uint16_t percentInputFill = 20;

uint32_t dropCounter = 0;

extern const ws2812_color_t myColors[];

typedef struct {
    uint8_t state;
    uint8_t colorIndex;
    uint8_t velocity;
} PanelState;

PanelState actualStateA[PANEL_YSIZE][PANEL_XSIZE];
PanelState nextStateA[PANEL_YSIZE][PANEL_XSIZE];
PanelState lastStateA[PANEL_YSIZE][PANEL_XSIZE];

PanelState (*actualState)[PANEL_YSIZE][PANEL_XSIZE] = &actualStateA;
PanelState (*nextState)[PANEL_YSIZE][PANEL_XSIZE] = &nextStateA;
PanelState (*lastState)[PANEL_YSIZE][PANEL_XSIZE] = &lastStateA;


void changeColorGRB(uint8_t* color,uint8_t* kValue)
{
    switch(*kValue)
    {
        case 0:
          color[0] += 2U;
           
           if(color[0] >= 64U)
           {
               color[0] = 63;
               *kValue = 1;
           }
           break;
        case 1:
             color[1]--;
             if(color[1] == 255U)
             {
                 color[1] = 0;
                 *kValue = 2;
             }
             break;
        case 2:
            color[2]++;
            if(color[2] >= 32U)
            {
                color[2] = 31;
                *kValue = 3;
            }
            break;
        case 3:
            color[0] -= 2U;
            if(color[0] == 255U)
            {
                color[0] = 0;
                *kValue = 4;  
            }
            break;
        case 4:
            color[1]++;
            if(color[1] >= 32U)
            {
                color[1] = 31;
                *kValue = 5;
            }
            break;
        case 5:
            color[2] --;
            if(color[2] == 255U)
            {
                color[2] = 0;
                *kValue = 0;
            }
            break;
           
    }
}
void getCrgb(uint16_t x, uint16_t y, uint8_t* actualColor)
{
    uint32_t offset = WS2812_PanelXYOffsetGet(x,y);
    MEMORY_Read(offset, actualColor, 3);
}

void setCrgb(uint16_t x, uint16_t y, uint8_t* actualColor)
{
    uint32_t offset = WS2812_PanelXYOffsetGet(x,y);
    MEMORY_Write(offset,actualColor, 3);
}



void setNextColor(uint16_t x, uint16_t y, uint8_t* kValue)
{
    uint8_t actualColor[3];
    getCrgb(x,y,actualColor);
    changeColorGRB(actualColor, kValue);
    setCrgb(x,y,actualColor);
}


bool withinCols(int16_t value)
{
    return value >= 0 && value <= (int16_t)PANEL_XSIZE - 1;
}

bool withinRows(int16_t value)
{
    return value >= 0 && value <= (int16_t)PANEL_YSIZE - 1;
}

void setNextColorAll(void)
{
    for(uint16_t i = 0; i < PANEL_YSIZE; i++)
    {
        for(uint16_t j = 0; j < PANEL_XSIZE; j++)
        {
            if((*actualState)[i][j].state != PANEL_STATE_NONE)
            {
                setNextColor(j,i,&(*actualState)[i][j].colorIndex);
            }
        }
    }
}

void resetAdjacentPixels(int16_t y, int16_t x)
{
    int16_t yPlus = y + 1;
    int16_t yMinus = y - 1;
    int16_t xPlus = x + 1;
    int16_t xMinus = x - 1;

    // Row above
    if (withinRows(yMinus))
    {
        if (withinCols(xMinus) && (*nextState)[yMinus][xMinus].state == PANEL_STATE_COMPLETE)
        {
            (*nextState)[yMinus][xMinus].state = PANEL_STATE_FALLING;
            (*nextState)[yMinus][xMinus].velocity = adjacentVelocityResetValue;
        }
        if ((*nextState)[yMinus][x].state == PANEL_STATE_COMPLETE)
        {
            (*nextState)[yMinus][x].state = PANEL_STATE_FALLING;
            (*nextState)[yMinus][x].velocity = adjacentVelocityResetValue;
        }
        if (withinCols(xPlus) && (*nextState)[yMinus][xPlus].state == PANEL_STATE_COMPLETE)
        {
            (*nextState)[yMinus][xPlus].state = PANEL_STATE_FALLING;
            (*nextState)[yMinus][xPlus].velocity = adjacentVelocityResetValue;
        }
    }

    // Current row
    if (withinCols(xMinus) && (*nextState)[y][xMinus].state == PANEL_STATE_COMPLETE)
    {
        (*nextState)[y][xMinus].state = PANEL_STATE_FALLING;
        (*nextState)[y][xMinus].velocity = adjacentVelocityResetValue;
    }
    if (withinCols(xPlus) && (*nextState)[y][xPlus].state == PANEL_STATE_COMPLETE)
    {
        (*nextState)[y][xPlus].state = PANEL_STATE_FALLING;
        (*nextState)[y][xPlus].velocity = adjacentVelocityResetValue;
    }

    // Row below
    if (withinRows(yPlus))
    {
        if (withinCols(xMinus) && (*nextState)[yPlus][xMinus].state == PANEL_STATE_COMPLETE)
        {
            (*nextState)[yPlus][xMinus].state = PANEL_STATE_FALLING;
            (*nextState)[yPlus][xMinus].velocity = adjacentVelocityResetValue;
        }
        if ((*nextState)[yPlus][x].state == PANEL_STATE_COMPLETE)
        {
            (*nextState)[yPlus][x].state = PANEL_STATE_FALLING;
            (*nextState)[yPlus][x].velocity = adjacentVelocityResetValue;
        }
        if (withinCols(xPlus) && (*nextState)[yPlus][xPlus].state == PANEL_STATE_COMPLETE)
        {
            (*nextState)[yPlus][xPlus].state = PANEL_STATE_FALLING;
            (*nextState)[yPlus][xPlus].velocity = adjacentVelocityResetValue;
        }
    }
}


void sandAnimationInitialize(void)
{
    WS2812_MatrixClear(0);
    for (uint16_t i = 0; i < PANEL_YSIZE; i++)
    {
        for (uint16_t j = 0; j < PANEL_XSIZE; j++)
        {
            (*actualState)[i][j].state = PANEL_STATE_NONE;
            (*actualState)[i][j].velocity = 0;
            (*actualState)[i][j].colorIndex = 0;
            (*nextState)[i][j].state = PANEL_STATE_NONE;
            (*nextState)[i][j].velocity = 0;
            (*nextState)[i][j].colorIndex = 0;
            (*lastState)[i][j].state = PANEL_STATE_NONE;
            (*lastState)[i][j].velocity = 0;
            (*lastState)[i][j].colorIndex = 0;
        }
    }
}

void sandAnimationUpdate(void)
{
    if(colorTimerCounter < colorChangeTime)
    {
        colorTimerCounter++;
        
    }
    else
    {
       colorTimerCounter = 0;
       changeColorGRB(grbValues,&newKValue);

    }
    
    if(colorAllTimerCounter < colorAllChangeTime)
    {
        colorAllTimerCounter++;
        
    }
    else
    {
       colorAllTimerCounter = 0;
       setNextColorAll();
    }
    if((*actualState)[inputY][inputX].state != PANEL_STATE_NONE || inputXChangeTimeCounter > inputXChangeTime)
    {
        inputX = ((uint16_t)rand() % (PANEL_XSIZE - 1U));
        inputXChangeTimeCounter = 0;
    }
    else
    {
        inputXChangeTimeCounter++;
    }
    int16_t halfInputWidth = inputWidth / 2U;
    for (int16_t i = -halfInputWidth; i <= halfInputWidth; i++)
    {
        for (int16_t j = -halfInputWidth; j <= halfInputWidth; j++)
        {
            if ((uint16_t)(rand() % 100) < percentInputFill)
            {
                dropCounter++;
                if (dropCounter > (inputWidth * PANEL_XSIZE * PANEL_YSIZE))
                {
                    dropCounter = 0;
                    sandAnimationInitialize();
                }

                uint16_t x = inputX + (uint16_t)j;
                uint16_t y = inputY + (uint16_t)i;

                if (withinCols(x) && withinRows(y) &&
                    ((*actualState)[y][x].state == PANEL_STATE_NONE || (*actualState)[y][x].state == PANEL_STATE_COMPLETE))
                {
                    
                    uint8_t actualColor[3]={COLORS_ARRAY_GREEN, COLORS_ARRAY_RED, COLORS_ARRAY_BLUE};
                    setCrgb(x,y,actualColor);
                    
                    (*actualState)[y][x].state = PANEL_STATE_NEW;
                    (*actualState)[y][x].velocity = 1;
                    (*actualState)[y][x].colorIndex = newKValue;
                }
            }
        }
    }

    // Clear the next state frame of animation
    for (uint16_t i = 0; i < PANEL_YSIZE; i++)
    {
        for (uint16_t j = 0; j < PANEL_XSIZE; j++)
        {
            (*nextState)[i][j].state = PANEL_STATE_NONE;
            (*nextState)[i][j].velocity = 0;
            (*nextState)[i][j].colorIndex = 0;
        }
    }
    
    for (int16_t i = PANEL_YSIZE - 1U; i >= 0; i--)
    {
        for (int16_t j = 0; j < (int16_t)PANEL_XSIZE; j++)
        {
            uint8_t pixelColor[3];
            getCrgb(j,i,pixelColor);
            
            uint16_t pixelState = (*actualState)[i][j].state;
            uint16_t pixelVelocity = (*actualState)[i][j].velocity;
            uint16_t pixelKValue = (*actualState)[i][j].colorIndex;

            bool moved = false;

            if (pixelState != PANEL_STATE_NONE && pixelState != PANEL_STATE_COMPLETE)
            {
                uint16_t newPos = ((uint16_t)i + (uint16_t)MIN(maxVelocity, pixelVelocity));
                for (uint16_t ny = newPos; ny > (uint16_t)i; ny--)
                {
                    if (!withinRows(ny))
                    {
                        continue;
                    }

                    PanelState belowState = (*actualState)[ny][j];
                    PanelState belowNextState = (*nextState)[ny][j];

                    int16_t direction = 1;
                    if ((rand() % 100) < 50)
                    {
                        direction *= -1;
                    }

                    PanelState *belowStateA = NULL;
                    PanelState *belowNextStateA = NULL;
                    PanelState *belowStateB = NULL;
                    PanelState *belowNextStateB = NULL;

                    if (withinCols((int16_t)j + direction))
                    {
                        belowStateA = &(*actualState)[ny][j + direction];
                        belowNextStateA = &(*nextState)[ny][j + direction];
                    }
                    if (withinCols(j - direction))
                    {
                        belowStateB = &(*actualState)[ny][j - direction];
                        belowNextStateB = &(*nextState)[ny][j - direction];
                    }

                    if (belowState.state == PANEL_STATE_NONE && belowNextState.state == PANEL_STATE_NONE)
                    {
                        MEMORY_Write(WS2812_PanelXYOffsetGet(j,ny),pixelColor,3);
                        (*nextState)[ny][j].state = PANEL_STATE_FALLING;
                        (*nextState)[ny][j].velocity = pixelVelocity + gravity;
                        (*nextState)[ny][j].colorIndex = pixelKValue;
                        moved = true;
                        break;
                    }
                    else if ((belowStateA != NULL && (*belowStateA).state == PANEL_STATE_NONE) && (belowNextStateA != NULL && (*belowNextStateA).state == PANEL_STATE_NONE))
                    {
                        MEMORY_Write(WS2812_PanelXYOffsetGet(j + direction,ny),pixelColor,3);
                        (*nextState)[ny][j + direction].state = PANEL_STATE_FALLING;
                        (*nextState)[ny][j + direction].velocity = pixelVelocity + gravity;
                        (*nextState)[ny][j + direction].colorIndex = pixelKValue;
                        moved = true;
                        break;
                    }
                    else if ((belowStateB != NULL && (*belowStateB).state == PANEL_STATE_NONE) && (belowNextStateB != NULL && (*belowNextStateB).state == PANEL_STATE_NONE))
                    {
                        MEMORY_Write(WS2812_PanelXYOffsetGet(j - direction,ny),pixelColor,3);
                        
                        (*nextState)[ny][j - direction].state = PANEL_STATE_FALLING;
                        (*nextState)[ny][j - direction].velocity = pixelVelocity + gravity;
                        (*nextState)[ny][j - direction].colorIndex = pixelKValue;
                        moved = true;
                        break;
                    }
                }
            }

            if (moved)
            {
                uint8_t data[3] = {0,0,0};
                MEMORY_Write(WS2812_PanelXYOffsetGet(j,i),data,3);
                
                resetAdjacentPixels(i, j);
            }

            if (pixelState != PANEL_STATE_NONE && !moved)
            {
                (*nextState)[i][j].velocity = pixelVelocity + gravity;
                (*nextState)[i][j].colorIndex = pixelKValue;

                if (pixelState == PANEL_STATE_NEW)
                    (*nextState)[i][j].state = PANEL_STATE_FALLING;
                else if (pixelState == PANEL_STATE_FALLING && pixelVelocity > 2U)
                    (*nextState)[i][j].state = PANEL_STATE_COMPLETE;
                else
                    (*nextState)[i][j].state = pixelState;
            }
        }
    }

    // Swap the state pointers.
    lastState = actualState;
    actualState = nextState;
    nextState = lastState;
    
}

bool SAND_Main(uint8_t rdKey)
{
    switch (rdKey)
    {
    case INIT_APP:
        WS2812_MatrixClear(0);
        sandAnimationInitialize();
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

    sandAnimationUpdate();
    return true;
}