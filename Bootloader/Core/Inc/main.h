/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

#include<stdint.h>
#include "stm32f4xx_hal.h"

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB


#define FLASH_SECTOR2_BASE_ADDRESS 0x08008000U

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)


/*Bootloader function prototypes */
void from_bl_2_host(uint8_t *pBuffer,uint32_t len);
void start_bl_mode(void);
void start_app_mode(void);

//void bootloader_handle_getver_cmd(uint8_t *bl_rx_buffer);
void give_all_codes(uint8_t *data_received);
void jump_to_given_adress(uint8_t *pBuffer);
void look_into_rdp( uint8_t *data_received);
void clear_flash_memory(uint8_t *data_received);
void download_code_to_given_adress(uint8_t *data_received);
void activate_protection(uint8_t *data_received);
void deactivate_protection(uint8_t *data_received);
void read_from_memory(uint8_t *data_received); 


void error_occured(void);
uint8_t crc_check (uint8_t *pData, uint32_t len,uint32_t crc_host);
void send_message_2_host(uint8_t *pBuffer,uint32_t len);
void all_good_continue(uint8_t command_code, uint8_t follow_len);
uint8_t correct_address(uint32_t go_address);
uint8_t write(uint8_t *pBuffer, uint32_t mem_address, uint32_t len);
uint8_t erase(uint8_t sector_number , uint8_t number_of_sector);





// our bootloader commands


//This command is used to know what are the commands supported by the bootloader
#define GIVE_HELP				0x61

//This command is used to jump bootloader to specified address.
#define DO_JUMP			0x62

//This command is used to write data in to different memories of the MCU
#define DO_WRITE			0x63


//This command is used to mass erase or sector erase of the user flash .
#define DO_ERASE          0x64


//This command is used to read the FLASH Read Protection level.
#define GIVE_RDP		0x65

//This command is used to enable or disable read/write protect on different sectors of the user flash .
#define E_PROTECT		0x66


//This command is used disable all sector read/write protection 
#define D_PROTECT				0x67





#define SUCCESS   0XA5
#define FAILURE  0X7F





#define CORRECT 0x55
#define INCORRECT 0xAA

#define INVALID_SECTOR 0x04

/*Some Start and End addresses of different memories of STM32F446xx MCU */
/*Change this according to your MCU */
#define SRAM1_SIZE            112*1024     // STM32F446RE has 112KB of SRAM1
#define SRAM1_END             (SRAM1_BASE + SRAM1_SIZE)
#define SRAM2_SIZE            16*1024     // STM32F446RE has 16KB of SRAM2
#define SRAM2_END             (SRAM2_BASE + SRAM2_SIZE)
#define FLASH_SIZE             512*1024     // STM32F446RE has 512KB of SRAM2
#define BKPSRAM_SIZE           4*1024     // STM32F446RE has 4KB of SRAM2
#define BKPSRAM_END            (BKPSRAM_BASE + BKPSRAM_SIZE)


/**
  * @}
  */

/**
  * @}
*/

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/