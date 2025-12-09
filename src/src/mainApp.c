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
 *  @file  mainApp.c
 *  @brief This file contains the needed functions for the main app operations.
 */

#include "../include/mainApp.h"

const uint8_t* APP_MENU_OFFSET[LAST_APP+1] =
{
    mchp_main,
#if UPLOAD_SNAKE      
    snake_main,
#endif
    
#if UPLOAD_ANTS 
    ants_main,
#endif   
    
#if UPLOAD_SCROLL 
    scroll_main,   
#endif 

#if UPLOAD_SAND 
    sand_main,   
#endif

#if UPLOAD_FIRE 
    fire_main,   
#endif
    
#if UPLOAD_PONG 
    pong_main,   
#endif
    gover_main,
};

const uint8_t* APP_START_OFFSET[LAST_APP+1] =
{
    mchp_start,   
#if UPLOAD_SNAKE  
    snake_start,
#endif

#if UPLOAD_ANTS
    ants_start,
#endif
    
#if UPLOAD_SCROLL 
    scroll_start,
#endif
    
#if UPLOAD_SAND 
    sand_start,   
#endif

#if UPLOAD_FIRE 
    fire_start,   
#endif    
    
#if UPLOAD_PONG
    pong_start,
#endif   
    gover_start,
};
