/*
 * password.c
 * 说明：本文档函数用于加密板功能使用。
 * Created: 2016-5-11 13:51:16
 *  Author: yck
 */
 
#include "kernel.h"
uint8_t Password[6];
/************************************************************************
**	函数名称:	GetGunNum(void)
**	函数功能:	将枪号转化为BCD码
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void GetGunNum(void)
{
    Password[0] = sysparas.gunnum;
}

/************************************************************************
**	函数名称:	GetTimeSec(void)
**	函数功能:	将时间秒转化为BCD码
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void GetTimeSec(void)
{
    Password[1] = time.second;
}

/************************************************************************
**	函数名称:	GetGunSum(void)
**	函数功能:	取枪累计消费金额最低字节
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void GetGunSum(void)
{
    Password[2] = (uint8_t)sysparas.totmoney;   // 枪累计消费金额最低字节。
}

/************************************************************************
**	函数名称:	GetGunGas(void)
**	函数功能:	取枪累计消费气量最低字节
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void GetGunGas(void)
{
    Password[3] = (uint8_t)sysparas.totvolume;   // 枪累计消费气量最低字节。
}

/************************************************************************
**	函数名称:	GetTimeMin(void)
**	函数功能:	将时间分转化为BCD码
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void GetTimeMin(void)
{
    Password[4] = time.minute;        // 时间分。
}

/************************************************************************
**	函数名称:	GetDataCrc(void)
**	函数功能:	得到加密数据CRC校验码
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void GetDataCrc(void)
{
    Password[5] = (uint8_t)ModbusCrc16(&Password[0], 5);     // CRC校验。
}

/************************************************************************
**	函数名称:	LockPortInit(void)
**	函数功能:	加密版端口初始化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void LockPortInit(void)
{
    SI_INDATE_IN();         //S1键，设置为输入。
}

/************************************************************************
**	函数名称:	GetLockdata(void)
**	函数功能:	得到加密数据
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void GetLockdata(void)
{
    GetGunNum(); // 枪号
    GetTimeSec();// 秒
    GetGunSum(); // 枪金额累最小字节
    GetGunGas(); // 枪气量累最小字节
    GetTimeMin();// 分
    GetDataCrc();// CRC
}

/************************************************************************
**	函数名称:	At24C128WriteNByte(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt)
**	函数功能:	向加密板的EEPROM里面存储加密数据
**	输入参数: nID:	写入数据首地址
			  wtbuf：数据指针，指向待写入的数据
			  cnt：	 待写入数据的个数
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-09-25
*************************************************************************/
uint8_t At24C128WriteNByte(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt)
{
  uint8_t subaddr1;
  uint8_t subaddr2;	
  if(nID > I2C_ADDRMAX)
		return ADDRESS_ERROR;
	subaddr1 = (uint8_t)nID;
	subaddr2 = (uint8_t)(nID>>8);

	if(!I2C_SendNBytes(subaddr1,subaddr2,wtbuf,cnt))
	{
		return false;
	}

	return true;
}

/************************************************************************
**	函数名称:	At24C128ReadNByte(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt)
**	函数功能:	从加密板的EEPROM里面读取加密数据
**	输入参数: nID:	读取数据首地址
			  rdbuf：数据指针，用于存储读出的数据
			  cnt：	 需要读取的数据个数
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-09-25
*************************************************************************/
uint8_t At24C128ReadNByte(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
{
	uint8_t subaddr1;
	uint8_t subaddr2;

	if(nID > I2C_ADDRMAX)
		return ADDRESS_ERROR;
	subaddr1 = (uint8_t)nID;
	subaddr2 = (uint8_t)(nID>>8);

	if(!I2C_ReadNBytes(subaddr1,subaddr2,rdbuf,cnt))
	{
		return false;
	}
	
	return true;
}


/************************************************************************
**	函数名称:	LockDataStore(void)
**	函数功能:	在外部EEPROM和加密版里面存储加密数据
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
void LockDataStore(void)
{
    uint8_t i1=0, i2=0;
    uint8_t buf[6];
    uint8_t tmp[6];

	//获取加密数据
    GetLockdata(); 
	//将加密数据写入外部EEPROM中
	EepromExternalWriteBytes(LOCK_ADDR,	&Password[0], 6);    
	_delay_ms(100);
	//将加密数据写入加密板EERPOM中
    At24C128WriteNByte(0,&Password[0], 6); 
    //判断写入外部EEPROM中的加密数据是否正确 ADD BY LY
    if(At24C128ReadNByte(0, buf, 6))
    {
        for(i1 = 0; i1 < 6; i1++)
        {
            if(Password[i1] != buf[i1])
            {
                break;
            }
        }

    }
	//判断写入加密板的EEPROM中的加密数据是否正确 ADD BY LY
	//if(At24C512ReadNByte(LOCK_ADDR, tmp, 6))
    if(EepromExternalReadBytes(LOCK_ADDR, tmp, 6))
    {
        for(i2 = 0; i2 < 6; i2++)
        {
            if(Password[i2] != tmp[i2])
            {
                break;
            }
        }

    }

    if((i1 == 6) && (i2 == 6))
    {
    	//保存加密板数据 ADD BY LY
        if(SaveSignalSysparas(74,1))
        {
        	//保存是否使用加密板标志 ADD BY LY
        	if(SaveSignalSysparas(globalvar.paraindex,1))
        	{
             	SetSuccess();       		
        	}
        }
        else
        {
            sysparas.usepwdboard = 0;
            SetFault();
        }
    }
    else
    {
        sysparas.usepwdboard = 0;
        SetFault();
    }
}

/************************************************************************
**	函数名称:	LockDataCompare(void)
**	函数功能:	密码比对
**	输入参数: 无
**	返回值  	: 1：比对成功；0：比对失败

**	创 建 者:	YCX
**	创建日期:	2016-05-12
*************************************************************************/
uint8_t LockDataCompare(void)
{
    uint8_t temp_lock0[6];
    uint8_t temp_lock1[6];
    uint8_t i;

    //读取外部EEPROM中的板数据	
    //At24C512ReadNByte(LOCK_ADDR, temp_lock0, 6);
    EepromExternalReadBytes(LOCK_ADDR, temp_lock0, 6);
    _delay_ms(100);
	
    //读取加密板EEPROM中的数据		
    At24C128ReadNByte(0,temp_lock1, 6);

	//比对加密数据 ADD BY LY
    for(i = 0; i < 6; i++)                
    {
        if(temp_lock0[i] != temp_lock1[i])
        {
            return 0;                     //比对失败。
        }
    }

    return 1;                             //比对成功。
}

/************************************************************************
**	函数名称:	I2C_Init(void)
**	函数功能:	I2C引脚初始化
**	输入参数: 无
**	返回值  	: 1：比对成功；0：比对失败

**	创 建 者:	LJ
**	创建日期:	2016-05-12
*************************************************************************/
void I2C_Init(void)
{
	SCL_OUT();
	_delay_us(5);
	SDA_OUT();
	_delay_us(5);

	 SDA_SET();
	_delay_us(5);
	 SCL_SET();
	_delay_us(5);	

}

/************************************************************************
**	函数名称:	Send_Ask(uint8_t data)
**	函数功能:	主机发送ACK或NACK
**	输入参数: data：1：NACK；0：ACK
**	返回值  	: 无

**	创 建 者:	LJ
**	创建日期:	2016-05-12
*************************************************************************/
void Send_Ask(uint8_t data)
{
    SCL_OUT();
    SDA_OUT();//设置SDL为输出
    if (data) 
    {
        SDA_CLR();    /*设置SDA输出ACK信号*/
    } 
    else 
    {
        SDA_SET();     /* 设置SDA输出NACK信号,CPU读数据的最后一个字节发出*/
    }
    _delay_us(5);    
    SCL_SET();         
    _delay_us(5);        
    SCL_CLR();            
    SDA_SET();
    _delay_us(5);    
}

/************************************************************************
**	函数名称:	I2C_Start(void)
**	函数功能:	I2C起始条件
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-09-26
*************************************************************************/
void I2C_Start(void)
{
    SDA_OUT();
    SCL_OUT();
    _delay_us(5);

    SCL_CLR();        
    _delay_us(5);

    SDA_SET();
    _delay_us(2); 
    SCL_SET();
    _delay_us(5);
    SDA_CLR();
    _delay_us(5);
    SCL_CLR();
    _delay_us(5);
}

/************************************************************************
**	函数名称:	I2C_Stop(void)
**	函数功能:	I2C停止条件
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-09-26
*************************************************************************/
void I2C_Stop(void)
{
    SDA_OUT();
    SCL_OUT();
    _delay_us(5);
    SDA_CLR();
    _delay_us(2);
    SCL_SET();
    _delay_us(5);
    SDA_SET();
    _delay_us(5);
    SCL_CLR();
    _delay_us(2);
}

/************************************************************************
**	函数名称:	I2C_AckCheck(void)
**	函数功能:	I2C ACK或NACK信号读取
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-09-26
*************************************************************************/
uint8_t I2C_AckCheck(void)
{
    uint16_t count=1000;
    uint8_t result;
    result=false;
    SCL_OUT();
    SDA_OUT();//设置SDL为输出
    
    SDA_SET();
    _delay_us(5);
    SDA_IN();//设置SDL方向为输入
    SCL_SET();
    _delay_us(5);
    while(count)
    {
        count--;
        if(SDA==0)
        {
          break;
        }
    }
    if(count==0)
    {
      I2C_Stop();
      
    }
    else
    {
      result=true;
    }
    SCL_CLR();
    SDA_OUT();
    SDA_SET();
    _delay_us(5);
    return(result);
}

/************************************************************************
**	函数名称:	I2C_SendByte(uint8_t data)
**	函数功能:	模拟I2C发送一个字节数据
**	输入参数: data:待发送的数据
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-09-26
*************************************************************************/
uint8_t I2C_SendByte(uint8_t data)
{
    uint8_t i;
    uint8_t result;
	
    SDA_OUT();
	
    for(i=0; i<8; i++)
    {
        if(data&0x80)
        {
            SDA_SET();
        }
        else
        {
            SDA_CLR();
        }
        _delay_us(5);
        SCL_SET();
        _delay_us(5);
        SCL_CLR();
        data=data<<1;
    }
    _delay_us(5);
    result=I2C_AckCheck();
    _delay_us(3);
    return(result);
}

/************************************************************************
**	函数名称:	I2C_ReadByte(void)
**	函数功能:	模拟I2C读取一个字节数据
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-09-26
*************************************************************************/
uint8_t I2C_ReadByte(void)
{
    unsigned char i;
    unsigned char data;
	
    data=0;
    SDA_OUT();//设置SDL为输出
    SDA_SET();
    _delay_us(2);
    SDA_IN();//设置SDL方向为输入
    SCL_CLR();
    _delay_us(5);
    for(i=0; i<8; i++)
    {
        SCL_SET();
        _delay_us(5);
        data=data<<1;
        if(SDA)
        {
            data=data |0x01;
        }
        SCL_CLR();
        _delay_us(5);
    }   
    return(data);

}

/************************************************************************
**	函数名称:	I2C_SendNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Wtbuf, uint8_t Num)
**	函数功能:	模拟I2C读取一个字节数据
**	输入参数: SubAddr1：写地址1
			  SubAddr2：写地址2
			  Wtbuf：数据指针，指向待写入的数据
			  Num：写入数据个数
**	返回值  	: true:写成功；false：写失败

**	创 建 者:	LY
**	创建日期:	2018-09-26
*************************************************************************/

uint8_t  I2C_SendNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Wtbuf, uint8_t Num) // SUBA 器件子地址  *s数据  N 数据个数
{
    uint8_t i=0;
    uint8_t result;
	
    I2C_Start();
    result=I2C_SendByte(AT24C128W);
    if(result==false)
    {
      return false;
    }
    I2C_SendByte(SubAddr1);
    I2C_SendByte(SubAddr2);
    for(i=0; i<Num; i++)
    {
        I2C_SendByte(*Wtbuf);
        Wtbuf++;
    }
    I2C_Stop();
    _delay_us(10000);
    return true;
}

/************************************************************************
**	函数名称:	I2C_ReadNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Rdbuf, uint8_t Num)
**	函数功能:	模拟I2C读取一个字节数据
**	输入参数: SubAddr1：写地址1
			  SubAddr2：写地址2
			  Rdbuf：存放读出数据的首地址
			  Num：写入数据个数
**	返回值  	: true:写成功；false：写失败

**	创 建 者:	LY
**	创建日期:	2018-09-26
*************************************************************************/

uint8_t I2C_ReadNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Rdbuf, uint8_t Num)// SUBA 器件子地址  *s数据  N 数据个数
{
    uint8_t i=0;
    uint8_t result;
	
    I2C_Start();
    result=I2C_SendByte(AT24C128W);
    if(result==false)
    {
      return false;
    }
    I2C_SendByte(SubAddr1);
    I2C_SendByte(SubAddr2);
    I2C_Start();
    I2C_SendByte(AT24C128R);

    for(i=0; i<Num; i++)
    {
        *Rdbuf=I2C_ReadByte();
        Rdbuf++;
        if(i!=Num-1)
            Send_Ask(1);
        else
            Send_Ask(0);
    }
    I2C_Stop();
    return true;
}

