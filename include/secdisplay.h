/*************************************************************************************************
--------------File Info--------------------------------------------------------------------------
** 文本：SPLC.H
** 说明：本文档用于定义客显液晶屏的驱动函数和驱动端口。
** 端口：1:LCK--GPIO329
         2:CLK--GPIO330
         3:DAT--GPIO331
** 版本：V1.0
** 作者：杨传坤
** 时间：2016/03/29
**************************************************************************************************/
#ifndef _SECDISPLAY_H
#define _SECDISPLAY_H

#include "stdbool.h"//包含bool类型。
#include "math.h"   //包含fabs函数。


//气量
#define OsplcKGON()         DispMem[29] &= 0xFB  //S4与S1共用。
#define OsplcKGOFF()        DispMem[29] |= 0x04
//金额
#define OsplcSUMON()        DispMem[29] &= 0xFD  //S5 (U2/Y2)与S2共用。
#define OsplcSUMOFF()       DispMem[29] |= 0x02
//单价
#define OsplcPRICEON()      DispMem[29] &= 0xFE  //S (U2/Y1)与S18共用。
#define OsplcPRICEOFF()     DispMem[29] |= 0x01
//进气KG/MIN
#define OsplcKGMINON()      DispMem[13] &= 0xFB
#define OsplcKGMINOFF()     DispMem[13] |= 0x04  //SS(U5/Y11)
//报警
#define OsplcWARNINGON()    DispMem[21] &= 0xFD
#define OsplcWARNINGOFF()   DispMem[21] |= 0x02
//枪号
#define OsplcGNUMON()       DispMem[21] &= 0xFB
#define OsplcGNUMOFF()      DispMem[21] |= 0x04
//进气流量S9
#define OsplcFLNUMON()      DispMem[18] &= 0x7F
#define OsplcFLNUMOFF()     DispMem[18] |= 0x80
//回气流量S10
#define OsplcSTATEON()      DispMem[17] &= 0xFE
#define OsplcSTATEOFF()     DispMem[17] |= 0x01
//回气KG/MIN
#define OsplcSTATEUPAYON()  DispMem[14] &= 0x7F  //S12 (U5/Y8)
#define OsplcSTATEUPAYOFF() DispMem[14] |= 0x80
//增益
#define OsplcSTATEIDLEON()  DispMem[11] &= 0x7F  //S14 (U5/Y10)
#define OsplcSTATEIDLEOFF() DispMem[11] |= 0x80
//温度
#define OsplcTEMPNON()     DispMem[17] &= 0xFD   //S20 
#define OsplcTEMPNOFF()    DispMem[17] |= (~0xFD)

#define OsplcTEMPON()       DispMem[10]  &= 0xFE //S16 (U5/Y10)
#define OsplcTEMPOFF()      DispMem[10] |= 0x01
#define OsplcTEMPUNITON()   DispMem[7]  &= 0xBF  //S17 (U6/Y23)
#define OsplcTEMPUNITOFF()  DispMem[7] |= 0x40
//压力
#define OsplcPRESSUREON()   DispMem[7]  &= 0x7F  //S18 (U6/Y24)与S8共用。
#define OsplcPRESSUREOFF()  DispMem[7] |= 0x80
//LOGO
#define OsplcLOGOON()       DispMem[7]  &= 0xDF  //S15 (U6/Y22)
#define OsplcLOGOOFF()      DispMem[7] |= 0x20


static uint8_t DispMem[40] = {0};            //液晶大屏数据存储数组。

void OsplcPortInit(void);

void OsplcLatch(void);

void OsplcEnclk(void);

void OsplcDot(uint8_t dcode);

void OsplcDispB(uint8_t *pdisp, bool nPoint);

void OsplcShowB(uint32_t Row1, uint32_t Row2, uint16_t Row3, bool bStop, bool bOverFlag);

void OsplcHardwarechk(void);

void OsplcInit(void);

void OsplcDispN(void);

void OsplcShowN(uint32_t Row1, uint32_t Row2, uint16_t Row3);

void OsplcTitleOn(void);

bool OsplcNum(uint32_t vnum);

bool OsplcSum(uint32_t vsum);

bool OsplcPrice(uint32_t vprice);
void OsplcGunNum(uint8_t GunNum);   //NUM24 NUM25

bool OsplcFlRate(uint32_t nfrNum);

bool OsplcTemperature(float vfTemp);

void OsplcWarnning(uint8_t WNO);   //NUM22 NUM23

bool OsplcDrvgain(int nTnum);

bool OsplcFlrateBack(uint32_t nfrNum);

void OsplcPressure(uint32_t npNum);

void OsplcDispInit(void);
void OsplcShow(uint32_t Row1, uint32_t Row2, uint16_t Row3, bool bStop, bool bOverFlag);
uint8_t GetOnePosition(uint32_t num);

#endif
