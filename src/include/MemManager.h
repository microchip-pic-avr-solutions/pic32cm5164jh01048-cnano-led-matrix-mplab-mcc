/**
 *  (c) 2024 Microchip Technology Inc. and its subsidiaries.
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
 *  @file  MemManager.h
 *  @brief This file contains the prototypes and other data types for the MemManager functions
 */

#ifndef MEM_MANAGER_H
#define	MEM_MANAGER_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define RAM_MEMORY          1U
#define FLASH_MEMORY        2U

#ifdef	__cplusplus
extern "C" {
#endif

void MEMORY_FlashSelect(void);
void MEMORY_RAMSelect(void);
uint8_t MEMORY_SelectedMemoryGet(void);

void MEMORY_FlashAddressSet(const uint8_t* address);
const uint8_t* MEMORY_FlashAddressGet(void);
uint8_t* MEMORY_RamAddressGet(void);

void MEMORY_Read(uint32_t offset, uint8_t* data, uint16_t data_len);
void MEMORY_Write(uint32_t offset, uint8_t* data, uint16_t data_len);

#ifdef	__cplusplus
}
#endif

#endif	/* MEM_MANAGER_H */

