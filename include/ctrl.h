#ifndef _CTRL_H
#define _CTRL_H

//加气机状态定义
//#define StaOffDuty      0xA0    // 下班	LNG2.7 无下班状态 ADD BY LY
#define StaIdle         0xA1    // 空闲
#define StaSPrecool     0xA2    // 小循环
#define StaBPrecool     0xA3    // 大循环
#define StaFuel         0xA4    // 加气中
#define StaPay          0xA5    // 待扣费
#define StaClose        0xA6    //  关闭
#define StaOtherFuel    0xA7    //对方加液
#define StaFault        0xF1    // 设备故障

enum    Sys_Err                //系统异常原因
{

    SYSERR_FLASH_CHK,			//FLASH检测失败 0
    SYSERR_W_FLASH,             //FLASH 写失败 1
    SYSERR_R_FLASH,             //FLASH 读失败 2

    SYSERR_W_IN_EEPROM,         //写内部eeprom错误 3
    SYSERR_R_IN_EEPROM,         //读内部eeprom错误 4
    SYSERR_W_EX_EEPROM,         //写外部eeprom错误 5
    SYSERR_R_EX_EEPROM,         //读外部eeprom错误 6
    SYSERR_INANDEX_EEPROM,      //内部eeprom和外部eeprom读出数据不一致 7

    SYSERR_MODBUS_G_AUTO_ERR,   //气相流量计通讯错误 8
    SYSERR_MODBUS_L_AUTO_ERR,   //液相流量计通讯错误 9
    SYSERR_PRESSURE,            //压力传感器异常 10
};



#endif