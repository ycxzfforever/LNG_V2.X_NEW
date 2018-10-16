#include "kernel.h"

/******************************************************************************
** 说明：Timer1 定时 100ms，主要各task计时
*******************************************************************************/

struct TIMER1 timer1;

//定时参数设置说明1 ADD BY LY
// Target : m1280
// Crystal: 7.3728Mhz
//TIMER1 initialize - prescale:256
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1Sec
// actual value:  1.000Sec (0.0%)

//定时参数设置说明2 ADD BY LY
//TIMER1 initialize - prescale:64
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 100mSec
// actual value: 99.991mSec (0.0%)
/************************************************************************
**	函数名称：Timer1Init(void)
**	函数功能: 定时计数器1初始化，定时100ms  
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-03-28
*************************************************************************/
void Timer1Init(void)
{
    TCCR1B = 0x00; //stop
    TCNT1H = 0xD3; //定时计数器1的定时计数值的高8位
    TCNT1L = 0x01;//定时计数器1的定时计数值的低8位
    OCR1AH = 0x2C;
    OCR1AL = 0xFF;
    OCR1BH = 0x2C;
    OCR1BL = 0xFF;
    OCR1CH = 0x00;
    OCR1CL = 0x00;
    ICR1H  = 0x2C;
    ICR1L  = 0xFF;
    TCCR1A = 0x00;
    TCCR1C = 0x00;
    //TCCR1B = 0x03; //start Timer
    TCCR1B = 0; //stop timer
    memset(&timer1.tick_100ms, 0, sizeof(struct TIMER1));
}

/************************************************************************
**	函数名称：Timer1On(void)
**	函数功能: 打开定时计数器1
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-03-28
*************************************************************************/
void Timer1On(void)
{
    TCCR1B = 0x03; //start Timer
    TIMSK1_TOIE1 = 1;
}

/************************************************************************
**	函数名称：Timer1Off(void)
**	函数功能: 关闭定时计数器1
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-03-28
*************************************************************************/
void Timer1Off(void)
{
    TCCR1B = 0x00; //stop Timer
    TIMSK1_TOIE1 = 0;
}

/************************************************************************
**	函数名称：TIMER1_OVF_vectFunc(void)
**	函数功能: 关闭定时计数器1溢出中断
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-03-28
*************************************************************************/
#pragma vector = TIMER1_OVF_vect
__interrupt void TIMER1_OVF_vectFunc(void)
{
    TCNT1H = 0xD3; //reload counter high value
    TCNT1L = 0x01; //reload counter low value

    timer1.tick_100ms++;
    timer1.flag_100ms = 1;

    if(timer1.tick_100ms % 5 == 0)   //定时500ms
    {
        timer1.flag_500ms = 1;

        if(m1card.debittimes > 0)
        {
            m1card.debittimes--;
        }
    }

    if(timer1.tick_100ms % 10 == 0)   //定时1s
    {
        timer1.flag_1s = 1;
        globalvar.comfailcount++;  //后台与主板通信是否正常标志。每1秒计数1次

        if(globalvar.comfailcount > 200)
        {
            globalvar.comfailcount = 200;
        }

        globalvar.timeout++;//计算超时时间 每隔1s钟累加1次

        if(globalvar.precoolfinished == 1)
        {
            globalvar.finishedtimeout++;

            if(globalvar.finishedtimeout > sysparas.precoolfinishedtime)   //预冷完成后10s都不起泵则需重新预冷
            {
                globalvar.finishedtimeout = 0;
                globalvar.precoolfinished = 0;
            }
        }
    }

    if(timer1.tick_100ms > 1000000)
        timer1.tick_100ms = 0;
}