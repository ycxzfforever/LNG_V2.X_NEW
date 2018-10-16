#ifndef __EEPROM_H
#define __EEPROM_H

extern uint32_t    SYSPARA_ADDR;

#define SLA_W_24CXXX  0xA0
#define SLA_R_24CXXX  0xA1


#define  SYSINFO_EEP_ROLLLEN     200//可以自己定义50个参数
#define  SYSINFO_EEP_ROLLMAX     8
/************************************************************************************
** 说明： 内部 EEPROM 读写数据
************************************************************************************/
bool EepromInterReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt);
bool EepromInterWriteBytes(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt);

/************************************************************************************
** 说明： 外部 EEPROM (AT24C128) 读写数据
************************************************************************************/
bool EepromExternalReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt);
bool EepromExternalWriteBytes(uint16_t nID, uint8_t* wtbuf, uint16_t cnt);
bool EepromExternalPageWriteBytes(uint16_t nID, uint8_t* wtbuf, uint16_t cnt);

uint8_t SaveEeprom(uint16_t addr, uint8_t *data, uint16_t num);
uint8_t SaveSysparas(uint16_t addr, uint8_t *data, uint16_t num);
uint8_t ReadSysparas(uint16_t addr, uint8_t *data, uint16_t num);
uint8_t SaveSignalSysparas(uint8_t paraindex, uint8_t crcwrflag);
uint8_t SaveCrc(void);
uint8_t ExshiftidSaveParas(void);
uint8_t StopFuelingSaveParas(void);

bool EepromDeviceTest(void);
#endif