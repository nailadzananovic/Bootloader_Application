#ifndef WINDOWS_SERIAL_H_INCLUDED
#define WINDOWS_SERIAL_H_INCLUDED


void Configure_Port(void);
uint32_t read_serial_port(uint8_t *data, uint32_t l);
void Close_serial_port(void);
void purge_serial_port(void);
void Write_to_serial_port(uint8_t *data, uint32_t l);


#endif
