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
 *  @file  ws2812_helpers.c
 *  @brief This file contains the needed functions for the WS2812 configuration.
 */

/* The following functions are applicable for WS2812B 32x8 panels. 6 Panels are used */

#include "definitions.h"
#include "../include/panel.h"
#include "../include/MemManager.h"
#include "../include/mainApp.h"

#if (MATRIX_TYPE != SERPENTINE)

#error The Matrix is not yet supported 

#else

extern const panel_chain_t myPanel[PANEL_MATRIX_NO];
extern const uint8_t font_fixed_5x7_data[];
extern const ws2812_color_t myColors[];

static uint8_t colorIndex = 0;

volatile uint16_t frameLength  = 0;
volatile uint16_t frameCounter = 0;

void WS2812_newFrameTimer(TC_TIMER_STATUS status, uintptr_t context)
{
    WS2812_FrameStart();
}

bool WS2812_IsBusy(void)
{
    bool status;
    status = false;

    NVIC_INT_Disable();
    if (frameCounter != frameLength) status = true;
    NVIC_INT_Enable();

    return status;
}

void WS2812_FrameStart(void) 
{
    TC2_TimerStart();
    
    TCC1_PWMStart();
    
    if(MEMORY_SelectedMemoryGet() == RAM_MEMORY)
    {
        DMAC_ChannelTransfer(DMAC_CHANNEL_0, MEMORY_RamAddressGet(), (uint32_t*)(SERCOM1_BASE_ADDRESS + SERCOM_SPIM_DATA_REG_OFST), DISPLAY_PAGE_SIZE);
    }
    else if(MEMORY_SelectedMemoryGet() == FLASH_MEMORY)
    {
        DMAC_ChannelTransfer(DMAC_CHANNEL_0, MEMORY_FlashAddressGet() + (uint32_t)frameCounter * DISPLAY_PAGE_SIZE, (uint32_t*)(SERCOM1_BASE_ADDRESS + SERCOM_SPIM_DATA_REG_OFST), DISPLAY_PAGE_SIZE);
    }
}

void WS2812_FrameReset(void)
{
    frameLength  = 0;
    frameCounter = 0;    
}

static void WS2812_FrameEnd(TC_TIMER_STATUS status, uintptr_t context)
{
    TCC1_PWMStop();
    
    frameCounter++;
   
    if (frameCounter < frameLength)
    {
        if (APP_StatusGet() != (uint8_t)APP_PAUSE)
        {
            // 70 us delay between frames
            TC1_TimerStart();
        }
    }
}

void WS2812_Initialize(void)
{
    frameLength  = 0;
    frameCounter = 0;
    
    TC1_TimerCallbackRegister(WS2812_newFrameTimer, 0);
    TC2_TimerCallbackRegister(WS2812_FrameEnd, 0);
}

void WS2812_MatrixDisplay(uint16_t framesNumber)
{  
    frameLength = framesNumber;
 
    frameCounter = 0;
    
    WS2812_FrameStart();
}

void WS2812_DrawColorSet(uint8_t color)
{
    colorIndex = color;
}

void WS2812_PixelSet(uint32_t offset)
{   
    uint8_t * addrPtr = ( uint8_t *)&myColors[colorIndex];
    
    MEMORY_Write(offset, addrPtr, 3);  
}

void WS2812_MatrixClear(uint32_t offset)
{
    uint8_t buffer[32];
   
    for (uint8_t i = 0; i < 32U; i ++)
    {
        buffer[i] = 0x00;
    }

    for (uint32_t j = 0; j < DISPLAY_PAGE_SIZE; j += 32U)
    {
       MEMORY_Write(offset + j, buffer, 32);
    }
}

void WS2812_AreaClear(uint8_t x, uint8_t y, uint8_t x_length, uint8_t y_length)
{
    uint32_t offset;
    uint8_t saveColor;
    
    if (((x + x_length) > PANEL_XSIZE)||((y + y_length) > PANEL_YSIZE)) return;
    saveColor = colorIndex;
    
    WS2812_DrawColorSet(BLACK);
   
    for (uint8_t i = x; i < (x + x_length); i ++)
    {
        for (uint8_t j = y; j < (y + y_length); j ++)
        {
            offset = WS2812_PanelXYOffsetGet(i, j);
            WS2812_PixelSet(offset);
        }
    }       
    WS2812_DrawColorSet(saveColor);
}

void WS2812_CharPrint(uint8_t x, uint8_t y, uint8_t ch)
{
    uint8_t data;
    uint32_t offset;
    uint8_t ch_font;
    
    if ((ch < CHAR_OFFSET)||(ch > 127U)) return;
    ch_font = ch - CHAR_OFFSET;
    
    for (uint8_t i = 0; i < FONT_XSIZE; i++)
    {
        data = font_fixed_5x7_data[(FONT_XSIZE*ch_font)+i];
        for (uint8_t j = 0; j < FONT_YSIZE; j++)
        {
             if ((data & (1U << j)) != 0U ) 
            {
                if ((x+i) < (PANEL_XSIZE-1U)) 
                {
                    offset = WS2812_PanelXYOffsetGet((uint16_t)(x + i),(uint16_t)(y + j));
                }
                else
                {
                    offset = WS2812_PanelXYOffsetGet(((x + i)% (PANEL_XSIZE-2U)),((uint32_t)(y + j) + (x / (PANEL_XSIZE-2U))*8U));
                }
                WS2812_PixelSet(offset);
            }        
        }
    }
}

void WS2812_Printf(uint8_t x, uint8_t y, uint8_t* ch)
{
    uint8_t* charPtr;
    uint8_t x_pos = x;
    charPtr = ch;
    
    while (0 != *charPtr)
    {
        WS2812_CharPrint(x_pos, y, (uint8_t)(*charPtr));
        x_pos += (FONT_XSIZE+1U);
        charPtr++;
    }
}

void WS2812_ValuePrint(uint8_t x, uint8_t y, uint16_t value)
{
    uint8_t ch;
    uint16_t result;
    uint8_t x_pos;
     
    WS2812_AreaClear(x, y, (3U * (FONT_XSIZE+1)), FONT_YSIZE);

    x_pos = x + 2U*(FONT_XSIZE+1U);
    result = value;   
    do
    {
        ch = result % 10U; 
        ch += 0x30U;
        result = result/10U; 
        WS2812_CharPrint(x_pos, y,ch); 
        x_pos -= (FONT_XSIZE+1U);         
    }while (result > 0U);  
}

void WS2812_RectangleDraw(uint8_t x, uint8_t y, uint8_t x_length, uint8_t y_length)
{
    for (uint8_t i = x; i < (x + x_length); i++)
    {
       WS2812_PixelSet(WS2812_PanelXYOffsetGet(i,y)); 
       WS2812_PixelSet(WS2812_PanelXYOffsetGet(i,(y + y_length - 1U))); 
    }    
    
    for (uint8_t i = y; i < (y + y_length); i++)
    {
       WS2812_PixelSet(WS2812_PanelXYOffsetGet(x,i)); 
       WS2812_PixelSet(WS2812_PanelXYOffsetGet((x + x_length - 1U), i)); 
    }    
}

uint16_t WS2812_PanelXYOffsetGet(uint16_t x, uint16_t y)
{
    int16_t address; 
    uint8_t panel_position = (uint8_t)(x / MATRIX_XSIZE) + (uint8_t)(y / MATRIX_YSIZE)*(PANEL_XSIZE/MATRIX_XSIZE);
    uint8_t x_pos = (x % MATRIX_XSIZE);
    uint8_t y_pos = (y % MATRIX_YSIZE);
    
    address = 0x00;
    switch (myPanel[panel_position].m_type)
    {
        case TOP_LEFT:
            address += (int16_t)MATRIX_LED_NO;
            address -= (int16_t)(y_pos * MATRIX_XSIZE);
            if ((y_pos & 0x01U) == 0x01U) address -= (int8_t)(MATRIX_XSIZE - x_pos);
            else  address -= (int8_t)(x_pos +1);
            break;  
            
        case TOP_RIGHT:
            address += (int16_t)MATRIX_LED_NO;
            address -= (int16_t)(y_pos * MATRIX_XSIZE);
            if ((y_pos & 0x01U) == 0x01U) address -= (int8_t)(MATRIX_XSIZE - x_pos);
            else  address -= (int8_t)(x_pos +1);
            break;
           
        case BOTTOM_LEFT:
            address += (int16_t)(y_pos * MATRIX_XSIZE);
            if ((y_pos & 0x01U) == 0x01U) address += (int8_t)x_pos;
            else address += (int8_t)(MATRIX_XSIZE - 1U - x_pos);
            break;
            
        case BOTTOM_RIGHT:
            address += (int16_t)(y_pos * MATRIX_XSIZE);
            if ((y_pos & 0x01U) == 0x00U) address += (int8_t)(MATRIX_XSIZE - 1U - x_pos);
            else address += (int8_t)x_pos;
            break;
    }
    //compute the offset of the first LED in matrix
    address += (int8_t)myPanel[panel_position].index * MATRIX_LED_NO;
    
    return (uint16_t) (3U * address); 
} 
#endif

