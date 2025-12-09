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
 *  @file  mainApp.h
 *  @brief This file contains the prototypes and other data types for the mainApp functions
 */

#ifndef MAINAPP_H
#define	MAINAPP_H


#ifdef	__cplusplus
extern "C" {
#endif

#include "panel.h"
#include "MemManager.h"
#include "keyScan.h"

    
#define UPLOAD_SNAKE        1
#define UPLOAD_ANTS         1
#define UPLOAD_SCROLL       1
#define UPLOAD_FIRE         1
#define UPLOAD_SAND         1
#define UPLOAD_PONG         1

#define FRAMES_NUMBER       90
    
#define SCROLL_IMAGE_SIZE   (DISPLAY_PAGE_SIZE * FRAMES_NUMBER)

#define APP_RUN_RATE_DEFAULT                3
#define APP_RUN_RATE_MIN                    2

typedef enum
{
	MAIN_MENU = 0U,
	APP_SELECTED,
	APP_RUN,
    APP_PAUSE,
	APP_FAIL,
	APP_WON,
	APP_SHOW_SCORE,
	APP_END,
}app_state_t;

//Supported applications
typedef enum
{
    FIRST_APP = 0,
#if UPLOAD_SNAKE            
    SNAKE_APP,
#endif
            
#if UPLOAD_ANTS
    ANTS_APP,
#endif

#if UPLOAD_SCROLL            
    SCROLL_DEMO,
#endif

#if UPLOAD_SAND            
    SAND_DEMO,
#endif
            
#if UPLOAD_FIRE            
    FIRE_DEMO,
#endif  
            
#if UPLOAD_PONG  
    PONG_APP,
#endif
    LAST_APP,
    IMAGE_SCROLL_APP,
}selectedApp_t;

extern uint8_t bufferApp[512];

extern const uint8_t mchp_main[DISPLAY_PAGE_SIZE];
extern const uint8_t mchp_start[DISPLAY_PAGE_SIZE];
extern const uint8_t snake_main[DISPLAY_PAGE_SIZE];
extern const uint8_t snake_start[DISPLAY_PAGE_SIZE];
extern const uint8_t ants_main[DISPLAY_PAGE_SIZE];
extern const uint8_t ants_start[DISPLAY_PAGE_SIZE];
extern const uint8_t pong_main[DISPLAY_PAGE_SIZE];
extern const uint8_t pong_start[DISPLAY_PAGE_SIZE];
extern const uint8_t gover_main[DISPLAY_PAGE_SIZE];
extern const uint8_t gover_start[DISPLAY_PAGE_SIZE];
extern const uint8_t scroll_main[DISPLAY_PAGE_SIZE];
extern const uint8_t scroll_start[DISPLAY_PAGE_SIZE];
extern const uint8_t sand_main[DISPLAY_PAGE_SIZE];
extern const uint8_t sand_start[DISPLAY_PAGE_SIZE];
extern const uint8_t fire_main[DISPLAY_PAGE_SIZE];
extern const uint8_t fire_start[DISPLAY_PAGE_SIZE];


bool SNAKE_Main(uint8_t rdKey);
bool LANT_Main(uint8_t rdKey);
bool PONG_Main(uint8_t rdKey);
bool SCROLL_Main(uint8_t rdKey);
bool FIRE_Main(uint8_t rdKey);
bool SAND_Main(uint8_t rdKey);

uint8_t APP_StatusGet(void);
void APP_StatusSet(uint8_t status);
void APP_RunRateSet(uint8_t appRate);


#ifdef	__cplusplus
}
#endif

#endif	/* MAINAPP_H */

