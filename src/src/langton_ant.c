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
 *  @file  langton_ant.c
 *  @brief This file contains the needed functions for the Langton Ants application.
 */

#include <xc.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../include/mainApp.h"

extern const ws2812_color_t myColors[];

#define N (PANEL_MATRIX_NO * 2)

static int8_t positions[N][2];
static int8_t directions[N];
static color_type_t drawColor = RED;

static bool LANT_ColorCheck(uint8_t *data, color_type_t colorIndex)
{
    const uint8_t *color = (const uint8_t *)&myColors[colorIndex];
    if (data[0] == color[0] && data[1] == color[1] && data[2] == color[2])
    {
        return true;
    }
    return false;
}

static void LANT_DirectionSet(int8_t id)
{
    switch (directions[id])
    {
    case 0: // Up
        positions[id][0]--;
        break;
    case 1: // Right
        positions[id][1]++;
        break;
    case 2: // Down
        positions[id][0]++;
        break;
    case 3: // Left
        positions[id][1]--;
        break;
    }
}

static void LANT_DirectionCheck(int8_t id)
{
    uint32_t offset = WS2812_PanelXYOffsetGet(positions[id][0], positions[id][1]);
    uint8_t data[3];

    MEMORY_Read(offset, data, 3);

    if (LANT_ColorCheck(data, BLACK))
    {
        directions[id]++;
    }
    else
    {
        directions[id]--;
    }

    if (directions[id] > 3)
    {
        directions[id] = 0;
    }
    else if (directions[id] < 0)
    {
        directions[id] = 3;
    }
}

static void LANT_PositionCheck(int8_t id)
{
    if (positions[id][0] > PANEL_XSIZE - 1)
    {
        positions[id][0] = 0;
    }

    if (positions[id][1] > PANEL_YSIZE - 1)
    {
        positions[id][1] = 0;
    }

    if (positions[id][0] < 0)
    {
        positions[id][0] = PANEL_XSIZE - 1;
    }

    if (positions[id][1] < 0)
    {
        positions[id][1] = PANEL_YSIZE - 1;
    }
}

static void LANT_PixelColorCheck(int8_t id)
{
    uint32_t offset = WS2812_PanelXYOffsetGet(positions[id][0], positions[id][1]);
    uint8_t data[3];

    MEMORY_Read(offset, data, 3);

    if (LANT_ColorCheck(data, BLACK))
    {
        WS2812_DrawColorSet(drawColor++);
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(positions[id][0], positions[id][1]));

        if (drawColor >= 9U)
            drawColor = 1; // 9 = colors number
    }
    else
    {
        WS2812_DrawColorSet(BLACK);
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(positions[id][0], positions[id][1]));
    }
}

void LANT_Initialize(void)
{

    for (int8_t i = 0; i < N; i++)
    {
        positions[i][0] = (rand() % (PANEL_XSIZE - 1));
        positions[i][1] = (rand() % (PANEL_YSIZE - 1));
        directions[i] = (rand() & 0x03);
    }
}

bool LANT_Main(uint8_t rdKey)
{
    static uint8_t appTimer;

    switch (rdKey)
    {
    case INIT_APP:
        WS2812_MatrixClear(0);
        LANT_Initialize();
        appTimer = 3;
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

    if (appTimer > 0U)
    {
        appTimer--;
        return false;
    }

    for (int8_t i = 0; i < N; i++)
    {
        LANT_PixelColorCheck(i);
        LANT_DirectionSet(i);
        LANT_PositionCheck(i);
        LANT_DirectionCheck(i);
    }
    return true;
}