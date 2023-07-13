#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

//enable one of the below macro according to your HOST
#define WINDOWS_HOST
//#define LINUX_HOST
//s#define OSX_HOST

#ifdef WINDOWS_HOST
    #include "Configure_port.h"
#endif

#ifdef LINUX_HOST
    #include "LinuxSerialPort.h"
#endif

#ifdef OSX_HOST
    #include "OSxSerialPort.h"
#endif

//Bl commands prototypes
void decode(uint32_t command_code);



void WRITE(uint32_t l);
void ERASE(uint32_t l);
void GO(uint32_t len);
int read_data(uint8_t command_code);
int check_flash_status(void);

//utilities Prototypes
uint32_t compute_crc(uint8_t *data, uint32_t l);
uint8_t convert_byte(uint32_t a, uint8_t ind, uint8_t lf);

//file ops
void close_f(void);
uint32_t read_f(uint8_t *data, uint32_t l);
void open_f(void);
uint32_t f_len(void);

//BL Commands






#define GIVE_HELP	    0x61


#define DO_JUMP		    0x62

#define DO_WRITE		0x63

#define DO_ERASE        0x64



//len details of the command

#define COMMAND_BL_GO_TO_ADDR_LEN           10
#define COMMAND_BL_FLASH_ERASE_LEN          8
#define COMMAND_BL_MEM_WRITE_LEN(x)         (7+x+4)

#define USE_APP_FLASH_BASE_ADDR 0X08008000


 /**
  * @brief  HAL Status structures definition
  */
typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;


 /**
  * @brief  Flash HAL Status structures definition
  */
typedef enum
{
  Flash_HAL_OK       = 0x00U,
  Flash_HAL_ERROR    = 0x01U,
  Flash_HAL_BUSY     = 0x02U,
  Flash_HAL_TIMEOUT  = 0x03U,
  Flash_HAL_INV_ADDR = 0x04U
} HAL_FlashStatusTypeDef;


typedef union
{
    uint16_t flash_sector_status;
    struct
    {
        uint16_t sector0:2;
        uint16_t sector1:2;
        uint16_t sector2:2;
        uint16_t sector3:2;
        uint16_t sector4:2;
        uint16_t sector5:2;
        uint16_t sector6:2;
        uint16_t sector7:2;

    }sectors;

}t_sector_status;


#endif
