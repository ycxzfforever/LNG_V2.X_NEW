#include "kernel.h"

/******************************************************************************
** ˵����Timer1 ��ʱ 100ms����Ҫ��task��ʱ
*******************************************************************************/

struct TIMER1 timer1;

//��ʱ��������˵��1 ADD BY LY
// Target : m1280
// Crystal: 7.3728Mhz
//TIMER1 initialize - prescale:256
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1Sec
// actual value:  1.000Sec (0.0%)

//��ʱ��������˵��2 ADD BY LY
//TIMER1 initialize - prescale:64
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 100mSec
// actual value: 99.991mSec (0.0%)
/************************************************************************
**	�������ƣ�Timer1Init(void)
**	��������: ��ʱ������1��ʼ������ʱ100ms  
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-03-28
*************************************************************************/
void Timer1Init(void)
{
    TCCR1B = 0x00; //stop
    TCNT1H = 0xD3; //��ʱ������1�Ķ�ʱ����ֵ�ĸ�8λ
    TCNT1L = 0x01;//��ʱ������1�Ķ�ʱ����ֵ�ĵ�8λ
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
**	�������ƣ�Timer1On(void)
**	��������: �򿪶�ʱ������1
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-03-28
*************************************************************************/
void Timer1On(void)
{
    TCCR1B = 0x03; //start Timer
    TIMSK1_TOIE1 = 1;
}

/************************************************************************
**	�������ƣ�Timer1Off(void)
**	��������: �رն�ʱ������1
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-03-28
*************************************************************************/
void Timer1Off(void)
{
    TCCR1B = 0x00; //stop Timer
    TIMSK1_TOIE1 = 0;
}

/************************************************************************
**	�������ƣ�TIMER1_OVF_vectFunc(void)
**	��������: �رն�ʱ������1����ж�
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-03-28
*************************************************************************/
#pragma vector = TIMER1_OVF_vect
__interrupt void TIMER1_OVF_vectFunc(void)
{
    TCNT1H = 0xD3; //reload counter high value
    TCNT1L = 0x01; //reload counter low value

    timer1.tick_100ms++;
    timer1.flag_100ms = 1;

    if(timer1.tick_100ms % 5 == 0)   //��ʱ500ms
    {
        timer1.flag_500ms = 1;

        if(m1card.debittimes > 0)
        {
            m1card.debittimes--;
        }
    }

    if(timer1.tick_100ms % 10 == 0)   //��ʱ1s
    {
        timer1.flag_1s = 1;
        globalvar.comfailcount++;  //��̨������ͨ���Ƿ�������־��ÿ1�����1��

        if(globalvar.comfailcount > 200)
        {
            globalvar.comfailcount = 200;
        }

        globalvar.timeout++;//���㳬ʱʱ�� ÿ��1s���ۼ�1��

        if(globalvar.precoolfinished == 1)
        {
            globalvar.finishedtimeout++;

            if(globalvar.finishedtimeout > sysparas.precoolfinishedtime)   //Ԥ����ɺ�10s���������������Ԥ��
            {
                globalvar.finishedtimeout = 0;
                globalvar.precoolfinished = 0;
            }
        }
    }

    if(timer1.tick_100ms > 1000000)
        timer1.tick_100ms = 0;
}