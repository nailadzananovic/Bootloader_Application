
#include "main.h"

int read_data(uint8_t instruction)
{
    uint8_t good[2]={0};
    uint32_t bytes=0;
    int returnval = -2;

    read_serial_port(good,2);
    if(good[0] == 0xA5)
    {
        bytes=good[1];
        printf("\n\n   The CRC check was successful, indicating that the data integrity is good.");

        switch(instruction)
        {

        case DO_JUMP:
            GO(bytes);
            break;
        case DO_WRITE:
            WRITE(bytes);
            break;
       case DO_ERASE:
            ERASE(bytes);
            break;

        default:
            printf("\n  Enter a proper command code\n");

        }

          returnval = 0;
    }
    else if( good[0] == 0x7F)
    {
        printf("\n   The CRC check has failed, indicating that the transfer of data was not successful.\n");
        returnval= -1;
    }

    return returnval;
}

void GO(uint32_t l)
{
    uint8_t status=0;
    read_serial_port(&status,l);
}


void ERASE(uint32_t l)
{
    uint8_t status=0;
    read_serial_port(&status,l);
    if(status == Flash_HAL_ERROR || Flash_HAL_BUSY || Flash_HAL_TIMEOUT || Flash_HAL_INV_ADDR)
    {
        printf("An error occurred while erasing the memory.");
    }
    else
    {
       //all good
    }
}


void WRITE(uint32_t len)
{
    uint8_t status=0;
    read_serial_port(&status,len);
     if(status == Flash_HAL_ERROR || Flash_HAL_BUSY || Flash_HAL_TIMEOUT || Flash_HAL_INV_ADDR)
    {
        printf("\n  An error occurred while writing to the memory.");
    }
    else
    {
        //all gooood
    }
}
