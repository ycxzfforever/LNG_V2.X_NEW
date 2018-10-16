#include "kernel.h"

/******************************************************************************
** ˵����Timer3 ��ʱ 100ms����Ҫ�������������ݶ�ȡ
*******************************************************************************/
//��ʱ��������˵��1 ADD BY LY
//TIMER3 initialize - prescale:1
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 100mSec
// actual value:  100.000mSec (0.0%)

/************************************************************************
**	�������ƣ�Timer3Init(void)
**	��������: ��ʱ������3��ʼ������ʱ100ms  
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-03-28
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
**	�������ƣ�Timer3On(void)
**	��������: �򿪶�ʱ������3
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-03-28
*************************************************************************/
void Timer3On(void)
{
    TCCR3B = 0x03; //start Timer
    TIMSK3_TOIE3 = 1;
}

/************************************************************************
**	�������ƣ�Timer3Off(void)
**	��������: �رն�ʱ������3
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-03-28
*************************************************************************/
void Timer3Off(void)
{
    TCCR3B = 0x00; //start Timer
    TIMSK3_TOIE3 = 0;
}

/************************************************************************
**	�������ƣ�ReadModbusAndPressure(void)
**	��������: �������ƺ�ѹ��������
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2017-03-28
*************************************************************************/
void ReadModbusAndPressure(void)
{
    if(sysparas.simulation == 0)
    {
        //��ȡҺ�����������ݣ�ÿ�α����
        ModbusReadScanBuffer_L(_MODBUS_L_);
        globalvar.modbus_l_error++;

        if(globalvar.modbus_l_error > 5)
        {
            SetBit(syserr, SYSERR_MODBUS_L_AUTO_ERR);   //Һ��������ͨѶ����
            globalvar.stoptimes = 1000; //Һ���������쳣ֱ��ͣ��
            stopreason = 2;
        }

        //��ȡ�������������ݣ���������ʱ�Ŷ�
        if((sysparas.backgascalc == 1) || (GetBit(syserr, SYSERR_MODBUS_G_AUTO_ERR) == 1))         //�����������������쳣ʱ��
        {
            _delay_ms(30);   //�������ʱ�������ܴ�����������������
            ModbusReadScanBuffer_G(_MODBUS_G_);
            globalvar.modbus_g_error++;

            if(globalvar.modbus_g_error > 5)
            {
                SetBit(syserr, SYSERR_MODBUS_G_AUTO_ERR);   //����������ͨѶ����
                globalvar.stoptimes = 1000; //�����������쳣ֱ��ͣ��
                stopreason = 1;
            }
        }

        //��ȡѹ��������
        if(!PressureGetValue())
        {
            globalvar.pressure_error++;

            if(globalvar.pressure_error > 10)
            {
                SetBit(syserr, SYSERR_PRESSURE);
                globalvar.stoptimes = 1000; //ѹ���������쳣ֱ��ͣ��
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
**	�������ƣ�TIMER3_OVF_vectFunc(void)
**	��������: ��ʱ������3����ж�
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2017-03-28
*************************************************************************/
#pragma vector = TIMER3_OVF_vect
__interrupt void TIMER3_OVF_vectFunc(void)
{
    //TIMER3 has overflowed
    TCNT3H = 0xD3; //setup
    TCNT3L = 0x01;
}
