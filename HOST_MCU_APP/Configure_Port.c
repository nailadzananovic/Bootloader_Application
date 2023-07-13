
#include "main.h"

HANDLE hComm;

void Configure_Port(void)
{
    char   ComPortName[] = "\\\\.\\COM5";
    BOOL   Status;

    hComm = CreateFile( ComPortName,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);

    if (hComm == INVALID_HANDLE_VALUE)
    {
         printf("\n   Error occured! Please verify the connection of the board and check the assigned port number.\n");
         exit(-1);
    }


    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    Status = GetCommState(hComm, &dcbSerialParams);

    if (Status == FALSE)
        printf("\n   Error! in GetCommState()");

    dcbSerialParams.BaudRate = 115200;      //9600
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.fBinary = 1;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    Status = SetCommState(hComm, &dcbSerialParams);

    if (Status == TRUE)

    {
        printf("\n       Baudrate = %ld", dcbSerialParams.BaudRate);
        printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
        printf("\n       StopBits = %d", dcbSerialParams.StopBits);
        printf("\n       Parity   = %d", dcbSerialParams.Parity);
    }

#if 1
    COMMTIMEOUTS timeouts = { 0 };

    timeouts.ReadIntervalTimeout         = 300;
    timeouts.ReadTotalTimeoutConstant    = 300;
    timeouts.ReadTotalTimeoutMultiplier  = 300;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (SetCommTimeouts(hComm, &timeouts) == FALSE)
        printf("\n   An error has occurred while setting the timeouts.");

    Status = SetCommMask(hComm, EV_RXCHAR);

    if (Status == FALSE)
        printf("\n\n   There was an error while setting the communication mask.");
#endif



}


uint32_t read_serial_port(uint8_t *data, uint32_t len)
{
    uint32_t no_of_bytes_read;

    ReadFile(hComm, data, len, &no_of_bytes_read, NULL);

    return no_of_bytes_read;

}


void Close_serial_port(void)
{
    CloseHandle(hComm);
}


void purge_serial_port(void)
{
     PurgeComm(hComm,PURGE_RXCLEAR|PURGE_TXCLEAR);

}

void Write_to_serial_port(uint8_t *data, uint32_t len)
{
    DWORD  dNoOfBytesWritten = 0;
    BOOL   Status;

    Status = WriteFile( hComm,
                        data,
                        len,
                        &dNoOfBytesWritten,
                        NULL);

    if (Status == FALSE)
    {
        printf("\n  An error occurred while writing to the serial port.");
    }
}


