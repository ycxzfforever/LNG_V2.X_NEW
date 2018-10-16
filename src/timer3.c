#include "kernel.h"

/******************************************************************************
** 说明：Timer3 定时 100ms，主要负责流量计数据读取
*******************************************************************************/
//定时参数设置说明1 ADD BY LY
//TIMER3 initialize - prescale:1
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 100mSec
// actual value:  100.000mSec (0.0%)

/************************************************************************
**	函数名称：Timer3Init(void)
**	函数功能: 定时计数器3初始化，定时100ms  
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-03-28
*************************************************************************/
void Timer3Init(void)
{
    TCCR3B = 0x00; //stop
    TCNT3H = 0xD3; //setup
    TCNT3L = 0x01;
    OCR3AH = 0x2C;
    OCR3AL = 0xFF;
    OCR3BH = 0x2C;
    OCR3BL = 0xFF;
    OCR3CH = 0x00;
    OCR3CL = 0x00;
    ICR3H  = 0x2C;
    ICR3L  = 0xFF;
    TCCR3A = 0x00;
    TCCR3C = 0x00;
//  TCCR3B = 0x01; //start Timer
    TCCR3B = 0x00; //stop Timer
//  timer3.ticks=0;
}

/************************************************************************
**	函数名称：Timer3On(void)
**	函数功能: 打开定时计数器3
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-03-28
*************************************************************************/
void Timer3On(void)
{
    TCCR3B = 0x03; //start Timer
    TIMSK3_TOIE3 = 1;
}

/************************************************************************
**	函数名称：Timer3Off(void)
**	函数功能: 关闭定时计数器3
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-03-28
*************************************************************************/
void Timer3Off(void)
{
    TCCR3B = 0x00; //start Timer
    TIMSK3_TOIE3 = 0;
}

/************************************************************************
**	函数名称：ReadModbusAndPressure(void)
**	函数功能: 读流量计和压力传感器
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2017-03-28
*************************************************************************/
void ReadModbusAndPressure(void)
{
    if(sysparas.simulation == 0)
    {
        //读取液相流量计数据，每次必须读
        ModbusReadScanBuffer_L(_MODBUS_L_);
        globalvar.modbus_l_error++;

        if(globalvar.modbus_l_error > 5)
        {
            SetBit(syserr, SYSERR_MODBUS_L_AUTO_ERR);   //液相流量计通讯错误。
            globalvar.stoptimes = 1000; //液象流量计异常直接停机
            stopreason = 2;
        }

        //读取气相流量计数据，回气计量时才读
        if((sysparas.backgascalc == 1) || (GetBit(syserr, SYSERR_MODBUS_G_AUTO_ERR) == 1))         //回气计量或者气象异常时读
        {
            _delay_ms(30);   //必须的延时，否则不能处理气象流量计数据
            ModbusReadScanBuffer_G(_MODBUS_G_);
            globalvar.modbus_g_error++;

            if(globalvar.modbus_g_error > 5)
            {
                SetBit(syserr, SYSERR_MODBUS_G_AUTO_ERR);   //气相流量计通讯错误。
                globalvar.stoptimes = 1000; //气象流量计异常直接停机
                stopreason = 1;
            }
        }

        //读取压力变送器
        if(!PressureGetValue())
        {
            globalvar.pressure_error++;

            if(globalvar.pressure_error > 10)
            {
                SetBit(syserr, SYSERR_PRESSURE);
                globalvar.stoptimes = 1000; //压力传感器异常直接停机
                stopreason = 4;
            }
        }
        else
        {
            globalvar.pressure_error = 0;
            ClrBit(syserr, SYSERR_PRESSURE);
        }
    }

}

/************************************************************************
**	函数名称：TIMER3_OVF_vectFunc(void)
**	函数功能: 定时计数器3溢出中断
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2017-03-28
*************************************************************************/
#pragma vector = TIMER3_OVF_vect
__interrupt void TIMER3_OVF_vectFunc(void)
{
    //TIMER3 has overflowed
    TCNT3H = 0xD3; //setup
    TCNT3L = 0x01;
}
