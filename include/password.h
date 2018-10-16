/*
 * GetPassword.h
 *
 * Created: 2016-5-11 15:40:13
 *  Author: Administrator
 */

#ifndef _PASSWORD_H_
#define _PASSWORD_H_

#define 	delay 			5
#define 	SDA_STATE  		SDA		

#define 	ACK 			0
#define 	NACK			1
#define 	ADDRESS_ERROR 	0
#define  	PAGESIZE  		8 //256字节分成32页，每页8个字节
#define 	OK				1

#define  AT24C128W  0xA0
#define  AT24C128R  0xA1

#define 	I2C_ADDRESS		0xA0
#define 	I2C_ADDRMAX		0x4000



extern uint8_t Password[6];

uint8_t At24C128WriteNByte(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt);
uint8_t At24C128ReadNByte(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt);

void LockPortInit(void);
unsigned char LockDataCompare(void);
void LockDataStore(void);
void GetLockdata(void);

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_AckCheck(void);
uint8_t I2C_SendByte(uint8_t data);
uint8_t I2C_ReadByte(void);
uint8_t I2C_SendNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Wtbuf, uint8_t Num);
uint8_t I2C_ReadNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Rdbuf, uint8_t Num);
#endif
