#include "kernel.h"

Time    time;

/************************************************************************
**	��������:	Ds3232ReadTime(void)
**	��������:	��DS3232����ʱ��
**	�������: ��
**	����ֵ		��true	����ȡʱ��ɹ�
			  false ����ȡʱ��ʧ��

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
bool  Ds3232ReadTime(void)
{
    uint8_t tmpbuffer[7];

	//��ȡʱ�������ʧ��
    if(I2cReadBytesForDs3232(0xd0, 0x00, 7, tmpbuffer) != 7)   
    {	
        return false;
    }

	//��ȡʱ������ڳɹ�����ֵ��time
    time.year   = tmpbuffer[6];
    time.month  = tmpbuffer[5];
    time.day    = tmpbuffer[4];
    time.hour   = tmpbuffer[2];
    time.minute = tmpbuffer[1];
    time.second = tmpbuffer[0];

    return true;

}

/************************************************************************
**	��������:	Ds3232SetTime(uint8_t *buffer)
**	��������:	����DS3232ʱ��
**	�������: buffer������ʱ�䣨˳��Ϊ��������ʱ���룬��ķ�ΧΪ00~99����ʽΪBCD��
**	����ֵ		��true	��ʱ�����óɹ�
			  false ��ʱ������ʧ��

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
uint8_t Ds3232SetTime(uint8_t *buffer)
{
    uint8_t tmpbuffer[7];

    tmpbuffer[0] = buffer[5];
    tmpbuffer[1] = buffer[4];
    tmpbuffer[2] = buffer[3];
    tmpbuffer[3] = 0;
    tmpbuffer[4] = buffer[2];
    tmpbuffer[5] = buffer[1];
    tmpbuffer[6] = buffer[0];

    //changed by ly
    //I2cWriteBytesForDs3232() ���ص����з���������Ds3232SetTime()���ص����޷�������ת��һ�� ADD BY LY
    if(I2cWriteBytesForDs3232(0xd0, 0x00, 7, tmpbuffer) != -1)
    {
        return true;
    }
    else
    {
        return false;
    }

}

/************************************************************************
**	��������:	DispDateTime(void)
**	��������:	��ʾʱ��
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void DispDateTime(void)
{
    if(globalvar.U32Temp < 5)
    {
        globalvar.U32Temp++;
		//��ʾ��ʽ1
        FYD12864DispPrintfFlash(4, 1, "20%02X-%02X-%02X %02X:%02X",
                                time.year, time.month, time.day, time.hour, time.minute);
    }
    else if(globalvar.U32Temp < 10)
    {
        globalvar.U32Temp++;
		//��ʾ��ʽ2
        FYD12864DispPrintfFlash(4, 1, "20%02X-%02X-%02X %02X %02X",
                                time.year, time.month, time.day, time.hour, time.minute);
    }
    else globalvar.U32Temp = 0;
}
