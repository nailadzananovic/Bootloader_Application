#include "main.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#define STYLE_UNDERLINE    "\033[4m"
#define STYLE_NO_UNDERLINE "\033[24m"

int main()
{
        system("color 90");
        printf("\n\n");
         printf(STYLE_UNDERLINE);
        printf("         BootLoader Application       ");
        printf(STYLE_NO_UNDERLINE);
        printf("\nc");
        printf("\nManufacturer: STM");
        printf("\n");
        printf("Board: Nucleo\t\t");
        printf("\n\n");


        printf("\n\tBasic info about serial port:");
        Configure_Port();

    while(1)
    {
       // printf("Red \033[0;31m");
        printf(STYLE_UNDERLINE);
        printf("\n\n\n             LIST OF COMMANDS             ");
        printf(STYLE_NO_UNDERLINE);
        printf("\n\nPlease select your prefered operation: \n");
        printf("\n   - Type 1 for HELP                      ");
        printf("\n   - Type 2 to jump to a specific adress  ");
        printf("\n   - Type 3 to write to a specific adress ");
        printf("\n   - Type 4 to erase from a specific adress");
        printf("\n   - Type 0 for exit");
        printf("\n\n   Type the choosen number here --> ");

        uint32_t number;
        scanf(" %d",&number);

        decode(number);

        printf("\n\n   To continue press any character on your keyboard: ");
        uint8_t garb = getch();
        purge_serial_port();
   }


}



