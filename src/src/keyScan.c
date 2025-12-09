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
 *  @file  keyScan.c
 *  @brief This file contains the implementation of functions used to interact
    with the switches and joystick on the Curiosity Nano Explorer board.
 */

#include "../include/keyScan.h"

void KEY_Initialize(void)
{
    uint8_t dataConfig[2]; 
    
    while(SERCOM0_I2C_IsBusy());
    dataConfig[0] = 0x06;
    dataConfig[1] = 0xFF;
    SERCOM0_I2C_Write(KEY_SCAN_ADDRESS, &dataConfig[0], 2);

    while(SERCOM0_I2C_IsBusy());
    dataConfig[0] = 0x04;
    dataConfig[1] = 0x00;
    SERCOM0_I2C_Write(KEY_SCAN_ADDRESS, &dataConfig[0], 2);  

    while(SERCOM0_I2C_IsBusy());
    dataConfig[0] = 0x02;
    dataConfig[1] = 0xFF;
    SERCOM0_I2C_Write(KEY_SCAN_ADDRESS, &dataConfig[0], 2);

    while(SERCOM0_I2C_IsBusy());
    dataConfig[0] = 0x05;
    dataConfig[1] = 0x24;
    SERCOM0_I2C_Write(KEY_SCAN_ADDRESS, &dataConfig[0], 2);
}

uint8_t KEY_Task(void)
{
    uint8_t result = NONE;
    uint8_t regAddr = 0x09;
    uint8_t GPIOsData;  
    
    while(SERCOM0_I2C_IsBusy());
    SERCOM0_I2C_WriteRead(KEY_SCAN_ADDRESS, &regAddr, 1, &GPIOsData, 1);
    
    while(SERCOM0_I2C_IsBusy());    /* wait for data to be available in GPIOsData */
    switch(GPIOsData){
        case 0xFE:
        case 0xEE:
            result =  UP;
            break;
        case 0xFD:
        case 0xED:
            result =  LEFT;
            break;
        case 0xFB:
        case 0xEB:
            result =  DOWN;
            break;
        case 0xF7:
        case 0xE7:
            result =  RIGHT;
            break;
        case 0xEF:
            result =  PUSH;
            break;
        case 0x7F:
            result =  MENU_PREV;
            break;
        case 0xBF:
            result =  MENU_SELECT;
            break;
        case 0xDF:
            result =  MENU_NEXT;
            break;
        default:
            result =  NONE;
            break;
    }
    return result;
}


/* *****************************************************************************
 End of File
 */
