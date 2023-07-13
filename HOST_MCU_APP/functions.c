

#include "main.h"

uint8_t convert_byte(uint32_t addr, uint8_t index, uint8_t lowerfirst)
{
      uint8_t value = (addr >> ( 8 * ( index -1)) & 0x000000FF );
      return value;
}


//function for CRC - polynomial method

uint32_t compute_crc(uint8_t *data, uint32_t l)
{
    uint32_t i;
    uint32_t Crc = 0XFFFFFFFF;
    for(uint32_t j = 0 ; j < l ; j++ )
    {
        uint32_t data1 = data[l];
        Crc = Crc ^ data1;
        for(i=0; i<32; i++)
        {
        if (Crc & 0x80000000)
            Crc = (Crc << 1) ^ 0x04C11DB7;
        else
            Crc = (Crc << 1);
        }

    }

  return(Crc);
}
