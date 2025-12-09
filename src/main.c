/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "../src/include/mainApp.h"


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

extern const uint8_t* APP_MENU_OFFSET[LAST_APP+1];
extern const uint8_t* APP_START_OFFSET[LAST_APP+1];

static volatile bool newTick = false;
static volatile bool runApp = false;
static volatile bool newFrame = false;

static bool (*selectedApp_Callback)(uint8_t key);
static uint8_t appStatus;
static uint8_t selectedApp;
static uint8_t appCallRate = APP_RUN_RATE_DEFAULT;

uint8_t bufferApp[512];

void selectedApp_CallbackRegister(bool (* CallbackHandler)(uint8_t));
bool WS2812_Player(uint8_t rdKey);
void showAppInit(void);
void APP_StatusSet(uint8_t status);
void APP_RunRateSet(uint8_t appRate);
uint8_t APP_StatusGet(void);
void APP_tickTimer(TC_TIMER_STATUS status, uintptr_t context);
bool startApp(uint8_t rdKey);


int main ( void )
{   
    static uint8_t AppCount = 0;
/*
 * MISRA Suppression
 *
 * Rule: 10.3
 *
 * Category: Required
 *
 * Reason: The enum value NONE is a non-negative integer constant
 *         that fits in uint8_t. Assignment is permitted by MISRA C:2023
 *         Rule 10.3 Exception 1.
 */

/* cppcheck-suppress misra-c2023-10.3 */
    static uint8_t rdKey = NONE;
    static uint8_t appDelayTimer = 0x00;
/*
 * MISRA Suppression
 *
 * Rule: 10.3
 *
 * Category: Required
 *
 * Reason: The enum value NONE is a non-negative integer constant
 *         that fits in uint8_t. Assignment is permitted by MISRA C:2023
 *         Rule 10.3 Exception 1.
 */

/* cppcheck-suppress misra-c2023-10.3 */
    uint8_t tmpKey = NONE;
/*
 * MISRA Suppression
 *
 * Rule: 10.3
 *
 * Category: Required
 *
 * Reason: The enum value NONE is a non-negative integer constant
 *         that fits in uint8_t. Assignment is permitted by MISRA C:2023
 *         Rule 10.3 Exception 1.
 */

/* cppcheck-suppress misra-c2023-10.3 */
    uint8_t prevKey = NONE;
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    WS2812_Initialize();
    KEY_Initialize();
    srand(PANEL_LED_NO);
    
    TC0_TimerCallbackRegister(APP_tickTimer, 0);
    TC0_TimerStart();
    
    selectedApp_CallbackRegister(LANT_Main);
    
    appStatus = MAIN_MENU;
    selectedApp = FIRST_APP;
    
    
    MEMORY_FlashSelect();
    MEMORY_FlashAddressSet(APP_START_OFFSET[selectedApp]);
    newFrame = true;
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        if(newTick == true)
        {   
            NVIC_INT_Disable();
            newTick = false;
            NVIC_INT_Enable();
            
            tmpKey = KEY_Task();

            if (prevKey != tmpKey)  
            {
                rdKey = tmpKey;
            }
            AppCount++;
            if (AppCount >= appCallRate) 
            {
                runApp = true;
                AppCount = 0;
            }
            tmpKey = (uint8_t) rand();
        }
        
        if (runApp == true)
        {
            runApp = false;
            
            switch (appStatus)
            {
                case MAIN_MENU:
                        
                    if (rdKey == (uint8_t)MENU_SELECT) 
                    {
                        if (selectedApp != (uint8_t)FIRST_APP) 
                            appStatus = APP_SELECTED;
                        MEMORY_FlashAddressSet(APP_START_OFFSET[selectedApp]);
                        newFrame = true; 
                        appDelayTimer = 0;
                    }
                    else 
                    {        
                        if (rdKey == (uint8_t)MENU_PREV)
                        {
                            if (selectedApp == 0x00U) 
                                selectedApp = (uint8_t)(LAST_APP - 1);
                            else  
                                selectedApp--;
                            MEMORY_FlashAddressSet(APP_MENU_OFFSET[selectedApp]);
                            newFrame = true; 
                        }
                        if (rdKey == (uint8_t)MENU_NEXT)
                        {
                            if (selectedApp == (uint8_t)(LAST_APP - 1)) 
                                selectedApp = 0;
                            else  
                                selectedApp++;
                            MEMORY_FlashAddressSet(APP_MENU_OFFSET[selectedApp]);
                            newFrame = true; 
                        }                         
                    }
                    break;
                        
                case APP_SELECTED:
                    if ((rdKey == (uint8_t)MENU_SELECT) || (appDelayTimer >= 3U))
                    {
                        showAppInit();                          
                        appStatus = APP_RUN;  
                    }
                    else 
                        appDelayTimer++;
                    break;
                        
                case APP_RUN:
                    newFrame = selectedApp_Callback(rdKey);
                    break;
                        
                case APP_PAUSE:
                    selectedApp_Callback(rdKey);
                    if ((rdKey == (uint8_t)MENU_PREV)  || (rdKey == (uint8_t)MENU_NEXT))  
                    {
                        MEMORY_FlashSelect(); 
                        MEMORY_FlashAddressSet(APP_MENU_OFFSET[selectedApp]);
                        newFrame = true; 
                        appStatus = MAIN_MENU;
                    }
                    break;
                        
                case APP_SHOW_SCORE:
                    if (rdKey == (uint8_t)MENU_SELECT) 
                    {
                        showAppInit();                          
                        appStatus = APP_RUN;  
                    }                        
                    break;  
                    
                case APP_FAIL:
                        
                case APP_WON:
                    MEMORY_FlashSelect();
                    MEMORY_FlashAddressSet(APP_MENU_OFFSET[LAST_APP]);
                    newFrame = true;
                    appStatus = APP_END; 
                    break;
                        
                case APP_END:
                    appStatus = MAIN_MENU;
                    break;
            }
            prevKey = rdKey;
            rdKey = NONE; 
            
            if (newFrame == true)
            {
                while (WS2812_IsBusy() == true);
                WS2812_MatrixDisplay(1);
                newFrame = false;
            }
        }
        
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

void selectedApp_CallbackRegister(bool (* CallbackHandler)(uint8_t))
{
    selectedApp_Callback = CallbackHandler;
}

bool WS2812_Player(uint8_t rdKey)
{
    switch (rdKey)
    {
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */
        case INIT_APP:
            WS2812_MatrixDisplay(1);
            break;
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */ 
        case MENU_SELECT:
            if (APP_StatusGet() == (uint8_t)APP_RUN)
/*
 * MISRA Suppression
 *
 * Rule: 10.3
 *
 * Category: Required
 *
 * Reason: The enum value APP_PAUSE is a non-negative integer constant
 *         that fits in uint8_t. Assignment is permitted by MISRA C:2023
 *         Rule 10.3 Exception 1.
 */

/* cppcheck-suppress misra-c2023-10.3 */
                APP_StatusSet(APP_PAUSE);
            else if (APP_StatusGet() == (uint8_t)APP_PAUSE) 
            {
                if (WS2812_IsBusy() == true) 
                     WS2812_FrameStart();

/*
 * MISRA Suppression
 *
 * Rule: 10.3
 *
 * Category: Required
 *
 * Reason: The enum value APP_RUN is a non-negative integer constant
 *         that fits in uint8_t. Assignment is permitted by MISRA C:2023
 *         Rule 10.3 Exception 1.
 */

/* cppcheck-suppress misra-c2023-10.3 */                
                APP_StatusSet(APP_RUN);
            }
            break;
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */   
        case MENU_PREV:
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */ 
        case MENU_NEXT:
            if ((WS2812_IsBusy() == true) && (APP_StatusGet() == (uint8_t)APP_PAUSE)) 
                WS2812_FrameReset();
            break;
        default:
           break;
    }  
    if ((APP_StatusGet() == (uint8_t)APP_RUN) && (WS2812_IsBusy() == false)) 
    {
        WS2812_MatrixDisplay(FRAMES_NUMBER); 
    }
    return false;

}
 
void showAppInit(void)
{
    APP_RunRateSet(APP_RUN_RATE_DEFAULT);
    MEMORY_RAMSelect();    
    switch (selectedApp)
    {
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */ 
        case FIRST_APP: selectedApp_CallbackRegister(startApp);break;
#if UPLOAD_SNAKE
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */ 
        case SNAKE_APP: selectedApp_CallbackRegister(SNAKE_Main);break;
#endif

/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */ 
#if UPLOAD_ANTS
        case ANTS_APP: selectedApp_CallbackRegister(LANT_Main);break;
#endif

/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */  
#if UPLOAD_SCROLL
        case SCROLL_DEMO: 
            selectedApp_CallbackRegister(SCROLL_Main);break;
#endif
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */ 
#if UPLOAD_SAND
        case SAND_DEMO: selectedApp_CallbackRegister(SAND_Main);break;
#endif
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */         
#if UPLOAD_FIRE
        case FIRE_DEMO: selectedApp_CallbackRegister(FIRE_Main);break;
#endif
/* 
 * MISRA Suppression
 * 
 * Rule: 10.3
 * 
 * Category: Required
 * 
 * Reason: The switch statement uses case labels that are non-negative integer
 *         constant expressions from an enum. The controlling expression is of
 *         essentially unsigned type (uint8_t), and all case label values can
 *         be represented in this type. This is permitted by MISRA C:2023 Rule
 *         10.3 Exception 3.
 */

/* cppcheck-suppress misra-c2023-10.3 */        
#if UPLOAD_PONG
        case PONG_APP:  selectedApp_CallbackRegister(PONG_Main); break;
#endif
    } 
    newFrame = selectedApp_Callback(INIT_APP);    
}

void APP_RunRateSet(uint8_t appRate)
{
    appCallRate = appRate;
}



void APP_StatusSet(uint8_t status)
{
    appStatus = status;    
}

uint8_t APP_StatusGet(void)
{
    return appStatus;    
}

void APP_tickTimer(TC_TIMER_STATUS status, uintptr_t context)
{
    newTick = true;
}



bool startApp(uint8_t rdKey)
{
 /*
      Static image
 */
    return false;
}
/*******************************************************************************
 End of File
*/

