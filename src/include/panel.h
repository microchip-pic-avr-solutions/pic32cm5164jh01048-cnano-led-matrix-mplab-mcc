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
 *  @file  panel.h
 *  @brief This file contains the prototypes and other data types for the WS2812 functions
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PANEL_H
#define	PANEL_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdbool.h>


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define MATRIX_XSIZE    8U
#define MATRIX_YSIZE    32U
//    
//#define MATRIX_XSIZE    16
//#define MATRIX_YSIZE    16
    
#define PANEL_XSIZE     48U
#define PANEL_YSIZE     32U
#define PANEL_LED_NO         (PANEL_XSIZE * PANEL_YSIZE)

#define MATRIX_LED_NO       (MATRIX_XSIZE * MATRIX_YSIZE)   
#define PANEL_MATRIX_NO     ((PANEL_XSIZE/MATRIX_XSIZE) * (PANEL_YSIZE/MATRIX_YSIZE))   
    
#define DISPLAY_PAGE_SIZE   (PANEL_XSIZE * PANEL_YSIZE * 3UL)         //No of LEBs * 3 bytes / color
   
typedef enum {
    TOP_LEFT = 0,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
} matrix_last_entry_t;    

typedef enum {
    HORIZONTAL = 0,
    VERTICAL
} matrix_orientation_t; 

typedef enum {
    SERPENTINE = 0,
    NORMAL
} matrix_type_t;    

typedef struct {
    uint8_t index;
    uint8_t m_type;
} panel_chain_t;
 
#define     MATRIX_TYPE             SERPENTINE
#define     MATRIX_ORIENTATION      HORIZONTAL  


#define CHAR_OFFSET  0x20U
#define FONT_XSIZE  5U
#define FONT_YSIZE  7U


typedef struct {
    uint8_t green;
    uint8_t red;
    uint8_t blue;
} ws2812_color_t;


uint16_t WS2812_PanelXYOffsetGet(uint16_t x, uint16_t y);
void WS2812_DrawColorSet(uint8_t Color);
void WS2812_PixelSet(uint32_t address);
void WS2812_MatrixClear(uint32_t address);
void WS2812_CharPrint(uint8_t x, uint8_t y, uint8_t ch);
void WS2812_Printf(uint8_t x, uint8_t y, uint8_t* ch);
void WS2812_ValuePrint(uint8_t x, uint8_t y, uint16_t value);
void WS2812_AreaClear(uint8_t x, uint8_t y, uint8_t x_length, uint8_t y_length);
void WS2812_RectangleDraw(uint8_t x, uint8_t y, uint8_t x_length, uint8_t y_length);
void WS2812_FrameStart(void);
void WS2812_FrameReset(void);
void WS2812_MatrixDisplay(uint16_t framesNumber);
void WS2812_Initialize(void);
bool WS2812_IsBusy(void);

#define WS2812_AddPanelEdge()   WS2812_RectangleDraw(0,0, PANEL_XSIZE, PANEL_YSIZE)

typedef enum {
    BLACK = 0,
    RED,
    GREEN,
    BLUE,
    ORANGE,
    PINK,
    PURPLE,
    YELLOW,
    TBD,
    WHITE,
    FIRE_ANIMATION_COLORS
}color_type_t;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

