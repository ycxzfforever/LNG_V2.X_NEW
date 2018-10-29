
#ifndef  __CPUPINPREDEFINE_H
#define  __CPUPINPREDEFINE_H

/********************************************************************
说明：主显
*********************************************************************/
#define OLCD_CS_H()       PORTH_Bit2 = 1
#define OLCD_CS_L()       PORTH_Bit2 = 0
#define OLCD_CS_IN()      DDRH_Bit2  = 0
#define OLCD_CS_OUT()     DDRH_Bit2  = 1

#define OLCD_SID_H()      PORTE_Bit6 = 1
#define OLCD_SID_L()      PORTE_Bit6 = 0
#define OLCD_SID_IN()     DDRE_Bit6  = 0
#define OLCD_SID_OUT()    DDRE_Bit6  = 1

#define OLCD_CLK_H()      PORTE_Bit5 = 1
#define OLCD_CLK_L()      PORTE_Bit5 = 0
#define OLCD_CLK_IN()     DDRE_Bit5  = 0
#define OLCD_CLK_OUT()    DDRE_Bit5  = 1

/********************************************************************
说明：串口 uart0 输出选择(4052 选择)
*********************************************************************/
#define UART0S0_OUT()     DDRB_Bit7 = 1
#define UART0S0_H()       PORTB_Bit7 = 1
#define UART0S0_L()       PORTB_Bit7 = 0

#define UART0S1_OUT()     DDRH_Bit7 = 1
#define UART0S1_H()       PORTH_Bit7 = 1
#define UART0S1_L()       PORTH_Bit7 = 0

#define SELECT_BANK()     UART0S1_L(); UART0S0_L(); __delay_cycles( 10 )
#define SELECT_TFT()	  UART0S1_L(); UART0S0_H(); __delay_cycles( 10 )
#define SELECT_PRINT()	  UART0S1_H(); UART0S0_L(); __delay_cycles( 10 )
#define SELECT_CARD()	  UART0S1_H(); UART0S0_H(); __delay_cycles( 10 )


/********************************************************************
说明：485 与 PC 485后台
*********************************************************************/
#define RS485_EN_OUT()       	DDRB_Bit4 = 1
#define RS485_EN_IN()        	DDRB_Bit4 = 0
#define RS485_EN_HIGH()      	PORTB_Bit4 = 1
#define RS485_EN_LOW()       	PORTB_Bit4 = 0

#define MODBUS_G_EN_OUT()       DDRB_Bit5 = 1
#define MODBUS_G_EN_IN()        DDRB_Bit5 = 0
#define MODBUS_G_EN_HIGH()      PORTB_Bit5 = 1
#define MODBUS_G_EN_LOW()       PORTB_Bit5 = 0

#define MODBUS_L_EN_OUT()       DDRB_Bit6 = 1
#define MODBUS_L_EN_IN()        DDRB_Bit6 = 0
#define MODBUS_L_EN_HIGH()      PORTB_Bit6 = 1
#define MODBUS_L_EN_LOW()       PORTB_Bit6 = 0




/********************************************************************
说明：键盘
*********************************************************************/
#define KEYBOARD_CLK_H()     PORTE_Bit2 = 1
#define KEYBOARD_CLK_L()     PORTE_Bit2 = 0
#define KEYBOARD_CLK_OUT()   DDRE_Bit2  = 1
#define KEYBOARD_CLK_IN()    DDRE_Bit2  = 0

#define KEYBOARD_SH_H()      PORTE_Bit3 = 1
#define KEYBOARD_SH_L()      PORTE_Bit3 = 0
#define KEYBOARD_SH_OUT()    DDRE_Bit3  = 1
#define KEYBOARD_SH_IN()     DDRE_Bit3  = 0

#define KEYBOARD_DATA_H()    PORTE_Bit4 = 1
#define KEYBOARD_DATA_L()    PORTE_Bit4 = 0
#define KEYBOARD_DATA_OUT()  DDRE_Bit4  = 1
#define KEYBOARD_DATA_IN()   DDRE_Bit4  = 0

//底层驱动调试到此 ADD BY LY 2018/05/18
/********************************************************************
说明：电磁阀控制
*********************************************************************/
//电磁阀电源控制 任何阀门要打开，VCTRL_OPENKEY必须要执行 ADD BY LY
#define   VCTRL_OPENKEY()       DDRH_Bit3 = 1;PORTH_Bit3 = 0
#define   VCTRL_CLOSEKEY()      DDRH_Bit3 = 1;PORTH_Bit3 = 1

//低、中、高压阀门没开关控制
#define   VCTRL_LOW_OPEN()      DDRH_Bit4 = 1;PORTH_Bit4 = 0
#define   VCTRL_LOW_CLOSE()     DDRH_Bit4 = 1;PORTH_Bit4 = 1
#define   VCTRL_MID_OPEN()      DDRH_Bit5 = 1;PORTH_Bit5 = 0
#define   VCTRL_MID_CLOSE()     DDRH_Bit5 = 1;PORTH_Bit5 = 1
#define   VCTRL_HIGH_OPEN()     DDRH_Bit6 = 1;PORTH_Bit6 = 0
#define   VCTRL_HIGH_CLOSE()    DDRH_Bit6 = 1;PORTH_Bit6 = 1

/********************************************************************
说明：PLC端口方向控制。
*********************************************************************/
#define   PUMPCTRL_HIGH_OPEN()  DDRK_Bit2 = 1;PORTK_Bit2 = 0
#define   PUMPCTRL_HIGH_CLOSE() DDRK_Bit2 = 1;PORTK_Bit2 = 1
#define   PUMPCTRL_LOW_OPEN()   DDRK_Bit3 = 1;PORTK_Bit3 = 0
#define   PUMPCTRL_LOW_CLOSE()  DDRK_Bit3 = 1;PORTK_Bit3 = 1

/********************************************************************
说明：掉电检测管脚
*********************************************************************/
#define PowerCheck()  DDRG_Bit5 = 0
#define PowerCheckIn  PING_PING5	//掉电管脚输入电平检测 1：掉电；0：未掉电

/********************************************************************
说明：EEPROM
*********************************************************************/
#define EEPROM_OUT()  DDRL_Bit5 = 1
#define EEPROM_IN()   DDRL_Bit5 = 0
#define EEPROM_H()    PORTL_Bit5 = 1
#define EEPROM_L()    PORTL_Bit5 = 0

/********************************************************************
说明：压力传感器
*********************************************************************/
#define ADC_G_IN()  DDRK_Bit1 = 0
#define ADC_G_H()   PORTK_Bit1 = 1
#define ADC_G_L()   PORTK_Bit1 = 0

#define ADC_L_IN()  DDRK_Bit0 = 0
#define ADC_L_H()   PORTK_Bit0 = 1
#define ADC_L_L()   PORTK_Bit0 = 0


/********************************************************************
说明：客显屏
*********************************************************************/
#define OsplcLckOut()   DDRL_Bit2 = 1
#define OsplcLckIn()    DDRL_Bit2 = 0
#define OsplcLck_H()    PORTL_Bit2 = 0
#define OsplcLck_L()    PORTL_Bit2 = 1

#define OsplcClkOut()   DDRL_Bit3 = 1
#define OsplcClkIn()    PORTL_Bit3 = 0
#define OsplcClk_L()    PORTL_Bit3 = 0
#define OsplcClk_H()    PORTL_Bit3 = 1

#define OsplcDataOut()  DDRL_Bit4 = 1
#define OsplcDataIn()   DDRL_Bit4 = 0
#define OsplcData_L()   PORTL_Bit4 = 0
#define OsplcData_H()   PORTL_Bit4 = 1

/********************************************************************
说明：客显屏
*********************************************************************/
#define BEEP_ON()     	DDRK_Bit7 = 1;PORTK_Bit7 = 1 //PK7
#define BEEP_OFF()      DDRK_Bit7 = 1;PORTK_Bit7 = 0

/********************************************************************
说明：LED 显示
*********************************************************************/
#define LED1_OUT()    	DDRF_Bit0 = 1//PF0 运行指示灯
#define LED2_OUT()    	DDRF_Bit1 = 1//PF1
#define LED3_OUT()    	DDRF_Bit2 = 1//PF2

#define LED1_ON()    	PORTF_Bit0 = 1
#define LED2_ON()    	PORTF_Bit1 = 1
#define LED3_ON()    	PORTF_Bit2 = 1

#define LED1_OFF()    	PORTF_Bit0 = 0
#define LED2_OFF()    	PORTF_Bit1 = 0
#define LED3_OFF()    	PORTF_Bit2 = 0

#define LED1_RUN         PORTF_Bit0

/********************************************************************
说明：加密板管脚
*********************************************************************/
#define SI_INDATE_IN()  DDRK_Bit5 	= 0
#define SI_INDATE_OUT() DDRK_Bit5 	= 1
#define SI_INDATE_H()  	PORTK_Bit5 	= 1

#define SI_INDATE		PINK_PINK5  //1：正常模式；0：设置模式

#define SCL_IN()		DDRF_Bit3 	= 0
#define SCL_OUT()		DDRF_Bit3 	= 1
#define SCL_CLR()		PORTF_Bit3 	= 0
#define SCL_SET()		PORTF_Bit3 	= 1
#define SCL				PINF_PINF3

#define SDA_IN()		DDRK_Bit4 	= 0
#define SDA_OUT()		DDRK_Bit4 	= 1
#define SDA_CLR()		PORTK_Bit4 	= 0
#define SDA_SET()		PORTK_Bit4 	= 1	
#define SDA				PINK_PINK4


/********************************************************************
说明：一泵双机互锁功能
*********************************************************************/
#define BA_OUT()      DDRL_Bit0 = 1
#define BA_IN()       DDRL_Bit0 = 0
#define BA_OUT_HIGH() PORTL_Bit0 = 1
#define BA_OUT_LOW()  PORTL_Bit0 = 0

#define AB_OUT()      DDRL_Bit1 = 1
#define AB_IN()       DDRL_Bit1 = 0
#define AB_OUT_HIGH() PORTL_Bit1 = 1
#define AB_OUT_LOW()  PORTL_Bit1 = 0

#define BA       PINL_PINL0
#define AB       PINL_PINL1



void BeepOut(uint8_t N_100ms);
void ValveInit(void);
void LedInit(void);

#endif
