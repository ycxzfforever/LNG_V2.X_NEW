#ifndef __LNGCTRL_H__
#define __LNGCTRL_H__

#include "kernel.h"

extern  double          lngvolume;//½øÒºÁ¿

void    PumpSwitch(uint8_t state);
void    StartFueling(void);
void    StopFueling(void);
void    StartPrecooling(void);
void    DispStartFueling(void);
void 	FuelEndDisplay(void);

void    GetCardData(void);
void    DispCardInfo(void);
void 	GreyLock(void);
void 	GreyDataDeal(void);

uint8_t Card_WriteGunandTran(uint8_t gunnumber, uint32_t transum, uint8_t stationid);

void 	GreyUnLock(void);
void 	CardMessageCheck(void);
void 	CardDataError(void);
void 	ComparePasswd(void);
void 	InputIccardPasswd(void);
void 	IcCardPasswd(void);
void 	DispDateOver(void);
void 	IcCardOperaError(void);

uint8_t Card_UpdateSec1_67(void);
uint8_t Card_UpdateSec6(uint8_t optype);
uint8_t Card_UpdateSec45(void);
uint8_t IcCardPayMoney(void);

void 	SaveRealtime(void);
void 	PowerDown(void);
void 	PowerDownRealDate(void);
void 	FixGas(void);
void 	FixNum(void);
void 	FixMoney(void);
void 	FixError(uint8_t FixType);
void 	ReFixNum(void);
void 	ReFixMoney(void);

void 	GetIntNum(void);

#endif
