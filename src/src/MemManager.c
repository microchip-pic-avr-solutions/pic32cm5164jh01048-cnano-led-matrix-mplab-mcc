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
 *  @file  MemManager.c
 *  @brief This file contains the needed functions for the memory operations.
 */

#include "../include/MemManager.h"
#include "../include/panel.h"

static uint8_t selectedMemory = 0;

static uint8_t ramImage[DISPLAY_PAGE_SIZE] = {0};

static const uint8_t* flashImagePtr;

void MEMORY_FlashAddressSet(const uint8_t* address)
{
    flashImagePtr = address;
}

const uint8_t* MEMORY_FlashAddressGet(void)
{
   return flashImagePtr; 
}

uint8_t* MEMORY_RamAddressGet(void)
{
   return ramImage; 
}

uint8_t MEMORY_SelectedMemoryGet(void)
{
    return selectedMemory;
}

void MEMORY_FlashSelect(void) 
{ 
    selectedMemory = FLASH_MEMORY; 
}

void MEMORY_RAMSelect(void)
{
    selectedMemory = RAM_MEMORY; 
}

void  MEMORY_Write(uint32_t offset, uint8_t * data, uint16_t data_len)
{  
    for(uint32_t i = 0; i < data_len; i++)
    {
        ramImage[offset + i] = data[i];
    }
}

void   MEMORY_Read(uint32_t offset, uint8_t * data, uint16_t data_len)
{
    for(uint32_t i = 0; i < data_len; i++)
    {
        data[i] = ramImage[offset + i];
    }
}