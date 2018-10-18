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

#include "datatypetransform.h"  //�������͵�ת���й�
#include "cpupinpredefine.h"    //CPU IO��ʼ���й�
#include "uart.h"               //����uart �й�
#include "fyd12864.h"           //����12864�й�
#include "mt318.h"              //M1���й�
#include "password.h"			//���ܰ��й�

#include "query.h"              //��ѯ�˵�
#include "controlparas.h"
#include "eeprom.h"             //�ڲ����ⲿEEPROM�й�  
#include "menuset.h"            //���й��ܲ˵�
#include "keyboard.h"           //�����й�
#include "i2c.h"
#include "modbus.h"             //�������й�
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
#include "app.h" //��̨
#include "lngctrl.h" //�����������й�


#define VERSION "LTY.V2.X20181018"
#define EX_EEPROM       1
#define FLASH (char __farflash*)
#define Debug 0  // ���ڵ���ʹ�� 
#define CLI() __disable_interrupt();
#define SEI() __enable_interrupt();

#define GetBit(dat,i) (((dat)&(1ul<<(i)))?1:0)  //�ж�dat��iλ�Ƿ�Ϊ1
#define SetBit(dat,i) ((dat)|=(1ul<<(i)))        //��dat��iλ��1
#define ClrBit(dat,i) ((dat)&=(~(1ul<<(i))))    //��dat��iλ��0  

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