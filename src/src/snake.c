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
 *  @file  snake.c
 *  @brief This file contains the needed functions for the Snake application.
 */

#include <stdlib.h>
#include "../include/mainApp.h"

#define SNAKE_MAX_LENGTH   250U
#define SNAKE_COLOR        GREEN
#define FOOD_COLOR         RED
#define POINTS_COLOR       BLUE

#define SNAKE_PANEL_XSIZE  PANEL_XSIZE 
#define SNAKE_PANEL_YSIZE  (PANEL_YSIZE - 8U)


typedef struct {
    uint8_t snake[SNAKE_MAX_LENGTH][2];
} snake_buff_t;

static snake_buff_t* work_buffer;
static int8_t direction[2];
uint8_t snake_head_index;
int16_t snake_next_head[2];
uint8_t snake_length;
uint8_t food_position[2];

void SNAKE_SpeedSet(void)
{
    uint8_t tmp;  
    tmp = (snake_length - 3U)/12U;
    
    if (tmp >= 9U)
    {
        APP_RunRateSet(APP_RUN_RATE_MIN);
    }
    else 
    {
        APP_RunRateSet(12U - tmp);       
    }
}

void SNAKE_ScorePrint(uint8_t score)
{
    WS2812_DrawColorSet(POINTS_COLOR);
    WS2812_ValuePrint( 3U * (FONT_XSIZE+1U), (SNAKE_PANEL_YSIZE + 1U), score);
}

void SNAKE_Initialize(void)
{   
    uint32_t offset;
    work_buffer = (snake_buff_t*) bufferApp;
    
    for (uint16_t i = 0; i < 256U; i++)
    {
       work_buffer->snake[i][0] = 0;
       work_buffer->snake[i][1] = 0;
    }
    snake_length = 3;
    
    WS2812_DrawColorSet(SNAKE_COLOR);
    
    for (uint8_t i = 0;  i < snake_length; i++) 
        {
            work_buffer->snake[i][0] = (SNAKE_PANEL_XSIZE/2U) - 1u;
            work_buffer->snake[i][1] = (uint8_t)(SNAKE_PANEL_YSIZE/2U) + i;
            offset = WS2812_PanelXYOffsetGet(work_buffer->snake[i][0], work_buffer->snake[i][1]);
            WS2812_PixelSet(offset);
        }

    snake_head_index = snake_length - 1U; 
            
    food_position[0] = (SNAKE_PANEL_XSIZE/2U) - 1U;
    food_position[1] = (SNAKE_PANEL_YSIZE/2U) + 5U;
 
    offset = WS2812_PanelXYOffsetGet(food_position[0], food_position[1]);
    WS2812_DrawColorSet(FOOD_COLOR);
    WS2812_PixelSet(offset);
   
    direction[0] = 0;
    direction[1] = 1;   
}

bool SNAKE_Advance(void)
{
    uint32_t offset;

    snake_next_head[0] = (int8_t)work_buffer->snake[snake_head_index][0U] + direction[0];
    snake_next_head[1] = (int8_t)work_buffer->snake[snake_head_index][1U] + direction[1];
    
    //check for matrix margins
    
    if ((snake_next_head[0] < 1)||(snake_next_head[0] > (int8_t)(SNAKE_PANEL_XSIZE-2U))) return false;
    if ((snake_next_head[1] < 1)||(snake_next_head[1] > (int8_t)(SNAKE_PANEL_YSIZE-2U))) return false;
    
    uint16_t index = snake_head_index; 
    //check for snake 
    for (uint8_t i = 0; i < snake_length; i++)
    {
        if ((snake_next_head[0] == (int8_t)work_buffer->snake[index][0]) && (snake_next_head[1] == (int8_t)work_buffer->snake[index][1U])) return false;
        if (index > 0U) index--;
        else index = (SNAKE_MAX_LENGTH - 1U);
    }
    
    snake_head_index++;
    if ( snake_head_index >= SNAKE_MAX_LENGTH ) snake_head_index = 0;
    
    work_buffer->snake[snake_head_index][0] = (uint8_t) snake_next_head[0];
    work_buffer->snake[snake_head_index][1] = (uint8_t) snake_next_head[1];

    // Update snake head 
    offset = WS2812_PanelXYOffsetGet(snake_next_head[0], snake_next_head[1]);
    
    while (WS2812_IsBusy() == true);
    WS2812_DrawColorSet(SNAKE_COLOR);
    WS2812_PixelSet(offset);
    
    if ((snake_next_head[0] == (int8_t)food_position[0]) && (snake_next_head[1] == (int8_t)food_position[1])) 
    { 
        //increase the snake length
        snake_length++;
        //new food, randomize
        uint8_t food_ok = 0x00;
        do {
            do {
                food_position[0] = 1U + ((uint8_t)rand() % (SNAKE_PANEL_XSIZE-1U));
            }while (food_position[0] >= (SNAKE_PANEL_XSIZE-1U));

            do {
                food_position[1] = 1U + ((uint8_t)rand() % (SNAKE_PANEL_YSIZE-1U));
            }while (food_position[1] >= (SNAKE_PANEL_YSIZE-1U));
            
            index = snake_head_index; 
            //check for snake 
            food_ok = 0x00;
            for (uint8_t i = 0; i < snake_length; i++)
            {
                if ((food_position[0] == work_buffer->snake[index][0]) && (food_position[1] == work_buffer->snake[index][1])) 
                {
                    food_ok = 0x0F;
                }
                if (index > 0u) index--;
                else index = (SNAKE_MAX_LENGTH - 1U);
            }
        }while (food_ok != 0x00U);

        offset = WS2812_PanelXYOffsetGet(food_position[0], food_position[1]);
        WS2812_DrawColorSet(FOOD_COLOR);
        WS2812_PixelSet(offset);
        
        SNAKE_ScorePrint(snake_length - 3U);
     }
    else
    {
        //Snake does not grow, delete the tail 
        index++;
        if (index > (SNAKE_MAX_LENGTH - 1U)) index = 0;

        offset = WS2812_PanelXYOffsetGet(work_buffer->snake[index][0], work_buffer->snake[index][1]);
        WS2812_DrawColorSet(BLACK);
        WS2812_PixelSet(offset);
    }
    SNAKE_SpeedSet();
    return true;
}

// Should be called from main loop periodically. The calling rate controls the snake speed.
// Maximum rate - 10 Hz 
bool SNAKE_Main(uint8_t rdKey)
{
    bool refreshDisplay;
 
    switch (rdKey)
    {
        case INIT_APP:
            WS2812_MatrixClear(0);
            WS2812_DrawColorSet(WHITE);
            WS2812_RectangleDraw(0U,0U, SNAKE_PANEL_XSIZE, SNAKE_PANEL_YSIZE);
            SNAKE_Initialize();
            WS2812_DrawColorSet(POINTS_COLOR);
            WS2812_Printf(1, (SNAKE_PANEL_YSIZE + 1U), (uint8_t*)"PTS\0");
            SNAKE_ScorePrint(snake_length - 3U);
            SNAKE_SpeedSet();
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
            break;        
        case UP:
            direction[1] = -1;
            direction[0] = 0;
            break;
        case DOWN:
            direction[1] = 1;
            direction[0] = 0;
            break;
        case LEFT:
            direction[0] = -1;
            direction[1] = 0;
            break;
        case RIGHT:
            direction[0] = 1;
            direction[1] = 0;
            break;
        default:
           break;
    } 
    
    if (APP_StatusGet() != (uint8_t)APP_RUN) return false;
    
    refreshDisplay = SNAKE_Advance(); 
    
    if (refreshDisplay == false)
    {
        //end game 
        APP_StatusSet(APP_FAIL);
        return false;
    }
    
    if (snake_length == SNAKE_MAX_LENGTH)   
    {
        APP_StatusSet(APP_WON);
        return false;
    }

    return refreshDisplay;
}