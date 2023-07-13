
/* This file implements logic to decode the user input , prepare and send the bootloader command packet
 * over the serial port. This file is common across win/linux/mac
 */

#include "main.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>


#define STYLE_BOLD         "\033[1;31m"
#define STYLE_NO_BOLD      "\033[0m"

void red () {
  printf("\033[1;31m");
}


void purple () {
   printf("033[0;35m");
}


void reset () {
  printf("\033[0m");
}


void decode(uint32_t command_code)
{

    unsigned char data2[255];
    uint32_t crc=0; int ret_value=0;

    switch(command_code)
    {
    case 0:
        printf("\nProgram is closing!");
        exit(0);

    case 1:
         printf("\n\n");
          printf("\n\n");
        printf("\nYou activated GIVE_HELP command");
        printf("\n\n");
        printf("\nWelcome to the Nucleo board management and programming application. \nThe steps you need to follow to correctly use the application are as follows:");
        printf("\n\n");
        printf("\n    - Connect the microcontroller to the PC using the appropriate USB cable.");
        printf("\n    - Press the blue user button integrated on the board, and then (while the \n    blue button is still pressed) press the reset button (black button).");
        printf("\n\n");
         printf("\nAfter successfully completing these two steps, you have successfully connected \nthe microcontroller and activated the 'Bootloader mode'.");
        printf("\nNow you can choose one of the remaining four commands. Below, each of them will \nbe briefly explained.");
        printf("\n\n");

        printf( "\n    Command number 1 -> Here you can find more information about the application.");
        printf(STYLE_BOLD);
         printf( "\n    Command number 2 -> ");
         printf(STYLE_NO_BOLD); system("color 90");


         printf("With this command, you can jump to any location in the flash \n    memory and continue program execution from there. You will also be prompted \n    to enter the desired address.");

         printf("\n    Command number 3 -> This command enables microcontroller programming. The suitable \n    file type for storage is .bin. You need to generate this file beforehand and \n    place it in the same directory as this application you are using. After that, simply \n    enter the base address where your new application will start writing. The storage \n    process is very fast, and it is possible to store larger applications \n    that occupy more KB.");
         printf("\n    Command number 4 -> This command allows you to erase a few sectors or even the entire \n    flash memory space. If you enter FF in the 'sector field', it will perform a \n    complete memory erase. However, if you specify the sector number and the total number \n    of sectors to erase, only those particular sectors will be erased. Please note that \n    this operation permanently deletes the data stored in the specified \n    sectors, so use it with caution.");
         printf("\n    Command number 0 -> With this command, you can easily exit the application.");



        break;

    case 2:
        printf("\n\n");
        printf("\n\n");
        printf("\nYou activated DO_JUMP command");
        printf("\n\nEnter address here: ");
        uint32_t go_address;
        scanf(" %x",&go_address);

        data2[0] = COMMAND_BL_GO_TO_ADDR_LEN-1;
        data2[1] = DO_JUMP;
        data2[2] = convert_byte(go_address,1,1);
        data2[3] = convert_byte(go_address,2,1);
        data2[4] = convert_byte(go_address,3,1);
        data2[5] = convert_byte(go_address,4,1);
        crc      = compute_crc(data2,COMMAND_BL_GO_TO_ADDR_LEN-4);
        data2[6] = convert_byte(crc,1,1);
        data2[7] = convert_byte(crc,2,1);
        data2[8] = convert_byte(crc,3,1);
        data2[9] = convert_byte(crc,4,1);

        Write_to_serial_port(&data2[0],1);
        Write_to_serial_port(&data2[1],COMMAND_BL_GO_TO_ADDR_LEN-1);

        ret_value = read_data(data2[1]);

        break;

    case 3:
        printf("\n\n");
        printf("\n\n");
        printf("\nYou activated DO_WRITE command");
        uint32_t bytes_remaining=0;
        uint32_t t_len_of_file=0;
        uint32_t bytes_so_far_sent = 0,len_to_read=0;
        uint32_t base_mem_address=0;

        data2[1] = DO_WRITE;

        //First get the total number of bytes in the .bin file.
        t_len_of_file =f_len();

        //keep opening the file
        open_f();

        bytes_remaining = t_len_of_file - bytes_so_far_sent;

#if 0
        //Here you should  get it from user input . but
        //for testing purpose just hard coded.
        base_mem_address = USE_APP_FLASH_BASE_ADDR;
#endif
        printf("\n\nEnter the starting adress: ");
        scanf(" %x",&base_mem_address);

        while(bytes_remaining)
        {

            if(bytes_remaining >= 128)
            {
                len_to_read = 128;
            }else
            {
                len_to_read = bytes_remaining;
            }

            //get the bytes in to buffer by reading file
            read_f(&data2[7],len_to_read);

            printf("\nStarting adress: %#.8x\n",base_mem_address);

            //populate base mem address
            data2[2] = convert_byte(base_mem_address,1,1);
            data2[3] = convert_byte(base_mem_address,2,1);
            data2[4] = convert_byte(base_mem_address,3,1);
            data2[5] = convert_byte(base_mem_address,4,1);

            data2[6] = len_to_read;

            /* 1 byte len + 1 byte command code + 4 byte mem base address
             * 1 byte payload len + len_to_read is amount of bytes read from file + 4 byte CRC
             */
            uint32_t mem_write_cmd_total_len = COMMAND_BL_MEM_WRITE_LEN(len_to_read);

            //first field is "len_to_follow"
            data2[0] =mem_write_cmd_total_len-1;

            crc       = compute_crc(&data2[0],mem_write_cmd_total_len-4);
            data2[7+len_to_read] = convert_byte(crc,1,1);
            data2[8+len_to_read] = convert_byte(crc,2,1);
            data2[9+len_to_read] = convert_byte(crc,3,1);
            data2[10+len_to_read] = convert_byte(crc,4,1);

            //update base mem address for the next loop
            base_mem_address+=len_to_read;

            Write_to_serial_port(&data2[0],1);
            Write_to_serial_port(&data2[1],mem_write_cmd_total_len-1);

            bytes_so_far_sent+=len_to_read;
            bytes_remaining = t_len_of_file - bytes_so_far_sent;

            printf("\n\n Bytes left: %d\n", bytes_remaining);

            ret_value = read_data(data2[1]);

         }
        break;

    case 4:
        printf("\n\n");
        printf("\n\n");
         printf("\nYou activateD DO_ERASE command");
         printf("\nPlease provide the starting sector number that you would like \n to use for the erasure. You can choose a sector between 0 and 7.  ");
         printf("\nIf you wish to clear the entire flash memory area, please enter \n 0xFF in the 'starting sector' field. \n\n");
        data2[0] = COMMAND_BL_FLASH_ERASE_LEN-1;
        data2[1] = DO_ERASE;
        uint32_t sector_num,nsec;

        printf("\nEnter the starting sector here: ");
        scanf(" %x",&sector_num);
        if(sector_num != 0xff)
        {
            printf("\nHow many sectors would you like to erase? --> ");
            scanf(" %d",&nsec);

        }


        data2[2]= sector_num;
        data2[3]= nsec;
       // printf(" sector num : %d %d \n",sector_num,nsec);

        crc     = compute_crc(data2,COMMAND_BL_FLASH_ERASE_LEN-4);
        data2[4] = convert_byte(crc,1,1);
        data2[5] = convert_byte(crc,2,1);
        data2[6] = convert_byte(crc,3,1);
        data2[7] = convert_byte(crc,4,1);

        Write_to_serial_port(&data2[0],1);
        Write_to_serial_port(&data2[1],COMMAND_BL_FLASH_ERASE_LEN-1);

        ret_value = read_data(data2[1]);

        break;

}}
