
#include "main.h"

FILE *f=NULL;


#define USER_APPLICATION "C:\\Users\\User\\Desktop\\STM32-ZAV\\KEIL_WS\\User_application\\MDK-ARM\\User_application.bin"


uint32_t f_len(void)
{
    FILE *f;
    uint32_t Len;
    f = fopen(USER_APPLICATION, "rb");
    if(! f){
        perror("\n\n   The .bin file could not be found.");
        exit(0);
    }

	fseek(f, 0, SEEK_END);
	Len=ftell(f);
	fseek(f, 0, SEEK_SET);
    fclose(f);
    return Len;

}

void open_f(void)
 {
    f = fopen(USER_APPLICATION, "rb");
 }

uint32_t read_f(uint8_t *data, uint32_t len)
{
    uint32_t retval=0;

	retval=fread(data, 1, len, f);

    return retval;

}

void close_f(void)
{
    fclose(f);
}
