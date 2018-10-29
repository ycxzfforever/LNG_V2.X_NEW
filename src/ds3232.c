#include "kernel.h"

Time    time;

/************************************************************************
**	函数名称:	Ds3232ReadTime(void)
**	函数功能:	从DS3232读出时间
**	输入参数: 无
**	返回值		：true	：读取时间成功
			  false ：读取时间失败

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
bool  Ds3232ReadTime(void)
{
    uint8_t tmpbuffer[7];

	//读取时间和日期失败
    if(I2cReadBytesForDs3232(0xd0, 0x00, 7, tmpbuffer) != 7)   
    {	
        return false;
    }

	//读取时间和日期成功，赋值给time
    time.year   = tmpbuffer[6];
    time.month  = tmpbuffer[5];
    time.day    = tmpbuffer[4];
    time.hour   = tmpbuffer[2];
    time.minute = tmpbuffer[1];
    time.second = tmpbuffer[0];

    return true;

}

/************************************************************************
**	函数名称:	Ds3232SetTime(uint8_t *buffer)
**	函数功能:	设置DS3232时间
**	输入参数: buffer：输入时间（顺序为：年月日时分秒，年的范围为00~99，格式为BCD）
**	返回值		：true	：时间设置成功
			  false ：时间设置失败

**	创 建 者:	HMM
**	创建日期:	2016-04-25
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
    //I2cWriteBytesForDs3232() 返回的是有符号数，而Ds3232SetTime()返回的是无符号数，转换一下 ADD BY LY
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
**	函数名称:	DispDateTime(void)
**	函数功能:	显示时间
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void DispDateTime(void)
{
    if(globalvar.U32Temp < 5)
    {
        globalvar.U32Temp++;
		//显示格式1
        FYD12864DispPrintfFlash(4, 1, "20%02X-%02X-%02X %02X:%02X",
                                time.year, time.month, time.day, time.hour, time.minute);
    }
    else if(globalvar.U32Temp < 10)
    {
        globalvar.U32Temp++;
		//显示格式2
        FYD12864DispPrintfFlash(4, 1, "20%02X-%02X-%02X %02X %02X",
                                time.year, time.month, time.day, time.hour, time.minute);
    }
    else globalvar.U32Temp = 0;
}
