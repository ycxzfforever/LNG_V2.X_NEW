#ifndef  __KERNEL_H
#define  __KERNEL_H

#ifdef __cplusplus
extern "C" {

#endif
#pragma pack(1)

#include "iom1280.h"
#include "avr_macros.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "intrinsics.h"
#include "stdbool.h"
#include "pgmspace.h"
#include "math.h"
#include "stddef.h"

#include "datatypetransform.h"  //数据类型的转换有关
#include "cpupinpredefine.h"    //CPU IO初始化有关
#include "uart.h"               //所有uart 有关
#include "fyd12864.h"           //主显12864有关
#include "mt318.h"              //M1卡有关
#include "password.h"			//加密板有关

#include "query.h"              //查询菜单
#include "controlparas.h"
#include "eeprom.h"             //内部、外部EEPROM有关  
#include "menuset.h"            //所有功能菜单
#include "keyboard.h"           //键盘有关
#include "i2c.h"
#include "modbus.h"             //流量计有关
#include "store.h"
#include "printer.h"
#include "e28f128flash.h"
#include "timer1.h"
#include "timer3.h"
#include "ds3232.h"
#include "query.h"
#include "devicetest.h"
#include "adc.h"
#include "modbus.h"
#include "secdisplay.h"
#include "ctrl.h"
#include "app.h" //后台
#include "lngctrl.h" //加气机控制有关


#define VERSION "LTY.V2.X20181018"
#define EX_EEPROM       1
#define FLASH (char __farflash*)
#define Debug 0  // 用于调试使用 
#define CLI() __disable_interrupt();
#define SEI() __enable_interrupt();

#define GetBit(dat,i) (((dat)&(1ul<<(i)))?1:0)  //判断dat第i位是否为1
#define SetBit(dat,i) ((dat)|=(1ul<<(i)))        //将dat第i位置1
#define ClrBit(dat,i) ((dat)&=(~(1ul<<(i))))    //将dat第i位置0  

#define  F_CPU    7372800
#define _delay_us(x)\
__delay_cycles( (uint32_t) ( (double)(F_CPU)*((x)/1000000.0) + 0.5))

#define _delay_ms(x)\
__delay_cycles( (uint32_t) ( (double)(F_CPU)*((x)/1000.0) + 0.5))

#define _delay_s(x) \
__delay_cycles( (uint32_t) ( (double)(F_CPU)*((x)/1.0) + 0.5))

#ifdef __cplusplus
}
#endif

#endif