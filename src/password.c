/*
 * password.c
 * ˵�������ĵ��������ڼ��ܰ幦��ʹ�á�
 * Created: 2016-5-11 13:51:16
 *  Author: yck
 */
 
#include "kernel.h"
uint8_t Password[6];
/************************************************************************
**	��������:	GetGunNum(void)
**	��������:	��ǹ��ת��ΪBCD��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void GetGunNum(void)
{
    Password[0] = sysparas.gunnum;
}

/************************************************************************
**	��������:	GetTimeSec(void)
**	��������:	��ʱ����ת��ΪBCD��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void GetTimeSec(void)
{
    Password[1] = time.second;
}

/************************************************************************
**	��������:	GetGunSum(void)
**	��������:	ȡǹ�ۼ����ѽ������ֽ�
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void GetGunSum(void)
{
    Password[2] = (uint8_t)sysparas.totmoney;   // ǹ�ۼ����ѽ������ֽڡ�
}

/************************************************************************
**	��������:	GetGunGas(void)
**	��������:	ȡǹ�ۼ�������������ֽ�
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void GetGunGas(void)
{
    Password[3] = (uint8_t)sysparas.totvolume;   // ǹ�ۼ�������������ֽڡ�
}

/************************************************************************
**	��������:	GetTimeMin(void)
**	��������:	��ʱ���ת��ΪBCD��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void GetTimeMin(void)
{
    Password[4] = time.minute;        // ʱ��֡�
}

/************************************************************************
**	��������:	GetDataCrc(void)
**	��������:	�õ���������CRCУ����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void GetDataCrc(void)
{
    Password[5] = (uint8_t)ModbusCrc16(&Password[0], 5);     // CRCУ�顣
}

/************************************************************************
**	��������:	LockPortInit(void)
**	��������:	���ܰ�˿ڳ�ʼ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void LockPortInit(void)
{
    SI_INDATE_IN();         //S1��������Ϊ���롣
}

/************************************************************************
**	��������:	GetLockdata(void)
**	��������:	�õ���������
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void GetLockdata(void)
{
    GetGunNum(); // ǹ��
    GetTimeSec();// ��
    GetGunSum(); // ǹ�������С�ֽ�
    GetGunGas(); // ǹ��������С�ֽ�
    GetTimeMin();// ��
    GetDataCrc();// CRC
}

/************************************************************************
**	��������:	At24C128WriteNByte(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt)
**	��������:	����ܰ��EEPROM����洢��������
**	�������: nID:	д�������׵�ַ
			  wtbuf������ָ�룬ָ���д�������
			  cnt��	 ��д�����ݵĸ���
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-09-25
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
**	��������:	At24C128ReadNByte(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt)
**	��������:	�Ӽ��ܰ��EEPROM�����ȡ��������
**	�������: nID:	��ȡ�����׵�ַ
			  rdbuf������ָ�룬���ڴ洢����������
			  cnt��	 ��Ҫ��ȡ�����ݸ���
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-09-25
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
**	��������:	LockDataStore(void)
**	��������:	���ⲿEEPROM�ͼ��ܰ�����洢��������
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
void LockDataStore(void)
{
    uint8_t i1=0, i2=0;
    uint8_t buf[6];
    uint8_t tmp[6];

	//��ȡ��������
    GetLockdata(); 
	//����������д���ⲿEEPROM��
	EepromExternalWriteBytes(LOCK_ADDR,	&Password[0], 6);    
	_delay_ms(100);
	//����������д����ܰ�EERPOM��
    At24C128WriteNByte(0,&Password[0], 6); 
    //�ж�д���ⲿEEPROM�еļ��������Ƿ���ȷ ADD BY LY
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
	//�ж�д����ܰ��EEPROM�еļ��������Ƿ���ȷ ADD BY LY
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
    	//������ܰ����� ADD BY LY
        if(SaveSignalSysparas(74,1))
        {
        	//�����Ƿ�ʹ�ü��ܰ��־ ADD BY LY
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
**	��������:	LockDataCompare(void)
**	��������:	����ȶ�
**	�������: ��
**	����ֵ  	: 1���ȶԳɹ���0���ȶ�ʧ��

**	�� �� ��:	YCX
**	��������:	2016-05-12
*************************************************************************/
uint8_t LockDataCompare(void)
{
    uint8_t temp_lock0[6];
    uint8_t temp_lock1[6];
    uint8_t i;

    //��ȡ�ⲿEEPROM�еİ�����	
    //At24C512ReadNByte(LOCK_ADDR, temp_lock0, 6);
    EepromExternalReadBytes(LOCK_ADDR, temp_lock0, 6);
    _delay_ms(100);
	
    //��ȡ���ܰ�EEPROM�е�����		
    At24C128ReadNByte(0,temp_lock1, 6);

	//�ȶԼ������� ADD BY LY
    for(i = 0; i < 6; i++)                
    {
        if(temp_lock0[i] != temp_lock1[i])
        {
            return 0;                     //�ȶ�ʧ�ܡ�
        }
    }

    return 1;                             //�ȶԳɹ���
}

/************************************************************************
**	��������:	I2C_Init(void)
**	��������:	I2C���ų�ʼ��
**	�������: ��
**	����ֵ  	: 1���ȶԳɹ���0���ȶ�ʧ��

**	�� �� ��:	LJ
**	��������:	2016-05-12
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
**	��������:	Send_Ask(uint8_t data)
**	��������:	��������ACK��NACK
**	�������: data��1��NACK��0��ACK
**	����ֵ  	: ��

**	�� �� ��:	LJ
**	��������:	2016-05-12
*************************************************************************/
void Send_Ask(uint8_t data)
{
    SCL_OUT();
    SDA_OUT();//����SDLΪ���
    if (data) 
    {
        SDA_CLR();    /*����SDA���ACK�ź�*/
    } 
    else 
    {
        SDA_SET();     /* ����SDA���NACK�ź�,CPU�����ݵ����һ���ֽڷ���*/
    }
    _delay_us(5);    
    SCL_SET();         
    _delay_us(5);        
    SCL_CLR();            
    SDA_SET();
    _delay_us(5);    
}

/************************************************************************
**	��������:	I2C_Start(void)
**	��������:	I2C��ʼ����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-09-26
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
**	��������:	I2C_Stop(void)
**	��������:	I2Cֹͣ����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-09-26
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
**	��������:	I2C_AckCheck(void)
**	��������:	I2C ACK��NACK�źŶ�ȡ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-09-26
*************************************************************************/
uint8_t I2C_AckCheck(void)
{
    uint16_t count=1000;
    uint8_t result;
    result=false;
    SCL_OUT();
    SDA_OUT();//����SDLΪ���
    
    SDA_SET();
    _delay_us(5);
    SDA_IN();//����SDL����Ϊ����
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
**	��������:	I2C_SendByte(uint8_t data)
**	��������:	ģ��I2C����һ���ֽ�����
**	�������: data:�����͵�����
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-09-26
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
**	��������:	I2C_ReadByte(void)
**	��������:	ģ��I2C��ȡһ���ֽ�����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-09-26
*************************************************************************/
uint8_t I2C_ReadByte(void)
{
    unsigned char i;
    unsigned char data;
	
    data=0;
    SDA_OUT();//����SDLΪ���
    SDA_SET();
    _delay_us(2);
    SDA_IN();//����SDL����Ϊ����
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
**	��������:	I2C_SendNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Wtbuf, uint8_t Num)
**	��������:	ģ��I2C��ȡһ���ֽ�����
**	�������: SubAddr1��д��ַ1
			  SubAddr2��д��ַ2
			  Wtbuf������ָ�룬ָ���д�������
			  Num��д�����ݸ���
**	����ֵ  	: true:д�ɹ���false��дʧ��

**	�� �� ��:	LY
**	��������:	2018-09-26
*************************************************************************/

uint8_t  I2C_SendNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Wtbuf, uint8_t Num) // SUBA �����ӵ�ַ  *s����  N ���ݸ���
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
**	��������:	I2C_ReadNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Rdbuf, uint8_t Num)
**	��������:	ģ��I2C��ȡһ���ֽ�����
**	�������: SubAddr1��д��ַ1
			  SubAddr2��д��ַ2
			  Rdbuf����Ŷ������ݵ��׵�ַ
			  Num��д�����ݸ���
**	����ֵ  	: true:д�ɹ���false��дʧ��

**	�� �� ��:	LY
**	��������:	2018-09-26
*************************************************************************/

uint8_t I2C_ReadNBytes(uint8_t SubAddr1,uint8_t SubAddr2,uint8_t *Rdbuf, uint8_t Num)// SUBA �����ӵ�ַ  *s����  N ���ݸ���
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

