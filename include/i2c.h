#ifndef __I2C_H
#define __I2C_H

void I2cInit(void);
signed char I2cReadBytesForDs3232(uint8_t DVaddr, uint8_t eeaddr, signed char len, uint8_t *buf);
signed char I2cWriteBytesForDs3232(uint8_t DVaddr, uint8_t eeaddr, signed char len, uint8_t *buf);

int I2cReadBytesForEeprom(uint8_t Dvaddr, uint16_t addr, int len, uint8_t *buf);
int I2cWriteBytesForEeprom(uint8_t Dvaddr, uint16_t addr, int len, uint8_t *buf);

#define  F_CPU    7372800
#define _delay_us(x)\
__delay_cycles( (uint32_t) ( (double)(F_CPU)*((x)/1000000.0) + 0.5))

#define _delay_ms(x)\
__delay_cycles( (uint32_t) ( (double)(F_CPU)*((x)/1000.0) + 0.5))

#define _delay_s(x) \
__delay_cycles( (uint32_t) ( (double)(F_CPU)*((x)/1.0) + 0.5))
#endif