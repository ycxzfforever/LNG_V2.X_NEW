
#include "kernel.h"

#define TW_START		0x08
#define TW_REP_START		0x10
#define TW_MT_SLA_ACK		0x18
#define TW_MT_SLA_NACK		0x20
#define TW_MT_DATA_ACK		0x28
#define TW_MR_SLA_ACK		0x40
#define TW_MR_DATA_ACK		0x50
#define TW_MR_DATA_NACK		0x58
#define TW_MT_DATA_NACK		0x30
#define TW_MT_ARB_LOST		0x38
#define TW_MR_ARB_LOST          0x38
#define TW_MR_SLA_NACK		0x48
#define TW_READ                 1
#define TW_WRITE                0
//#define TWI_SLA_DS              0xD0 //20110812
#define MAX_ITER                20
#define TW_STATUS		(TWSR & 0xF8)
#define TWI_ACT_START()         TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)
#define TWI_ACT_STOP()          TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)
#define TWI_ACT_WAIT()          while ((TWCR & (1<<TWINT)) == 0)

uint8_t twst;

/************************************************************************
**	��������:	I2cInit(void)
**	��������:	I2C��ʼ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2018-1-31
*************************************************************************/
void I2cInit(void)
{
//  DDRD  |= 0x03;
//  PORTD |= 0x03;
    DDRD_Bit0 = 1; //SCL ����Ϊ���
    DDRD_Bit1 = 1;  //SDA ����Ϊ���
    PORTD_Bit0 = 1; //SCL ʹ����������
    PORTD_Bit1 = 1; //SDA ʹ����������
    TWBR = 0x20;//λ���ʣ�CLKcpu/(16+2TWBR*(4��TWPS��))��
}

/************************************************************************
**	��������:	I2cReadBytesForDs3232(uint8_t DVaddr, uint8_t eeaddr, signed char len, uint8_t *buf)
**	��������:	��DS3232ָ����ַ��ȡָ�����ȵ�����
**	�������: DVaddr��DS3232 I2C������ַ
			  eeaddr����������ʼ��ַ
			  len	: ��ȡ���ݵĸ���
			  buf	���������ݴ�����׵�ַ
**	����ֵ  	: -1:	��ȡʧ��
			  ��-1����ȡ�ɹ�

**	�� �� ��:	
**	��������:	2018-1-31
*************************************************************************/
signed char I2cReadBytesForDs3232(uint8_t DVaddr, uint8_t eeaddr, signed char len, uint8_t *buf)
{
    uint8_t sla, twcr, n = 0;
    int   rv = 0;
    sla = DVaddr;//20110812 TWI_SLA_DS;//0xD0

restart:

    if(n++ >= MAX_ITER) return -1;   //20

begin:
    TWI_ACT_START();//TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
    TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)

    switch((twst = TW_STATUS))     //(TWSR & 0xF8)
    {
        case TW_REP_START://0x10
        case TW_START:
            break;//0x08

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            return -1;
    }

    /* send SLA+W */
    TWDR = sla | TW_WRITE;//0
    TWCR = (1 << TWINT) | (1 << TWEN);
    TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)

    switch((twst = TW_STATUS))     //(TWSR & 0xF8)
    {
        case TW_MT_SLA_ACK:
            break;//0x18

        case TW_MT_SLA_NACK:
            goto restart;//0x20

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    TWDR = eeaddr;//��ַ
    TWCR = (1 << TWINT) | (1 << TWEN);
    TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)

    switch((twst = TW_STATUS))     //(TWSR & 0xF8)
    {
        case TW_MT_DATA_ACK:
            break;//0x28

        case TW_MT_DATA_NACK:
            goto quit;//0x30

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    /*
     * Next cycle(s): master receiver mode
     */
    TWI_ACT_START();//TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
    TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)

    switch((twst = TW_STATUS))     //(TWSR & 0xF8)
    {
        case TW_REP_START://0x10
        case TW_START:
            break;//0x08                                                  /* OK, but should not happen */

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    /* send SLA+R */
    TWDR = sla | TW_READ;//1
    TWCR = (1 << TWINT) | (1 << TWEN);                                            /* clear interrupt to start transmission */
    TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)

    switch((twst = TW_STATUS))     //(TWSR & 0xF8)
    {
        case TW_MR_SLA_ACK:
            break;//0x40

        case TW_MR_SLA_NACK:
            goto quit;//0x48

        case TW_MR_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    for(twcr = (1 << TWINT) | (1 << TWEN) | (1 << TWEA) ; len > 0; len--)
    {
        if(len == 1) twcr = (1 << TWINT) | (1 << TWEN);

        TWCR = twcr;

        while((TWCR & (1 << TWINT)) == 0) ;

        switch((twst = TW_STATUS))
        {
            case TW_MR_DATA_NACK:
                len = 0;

            case TW_MR_DATA_ACK:
                *buf++ = TWDR;
                rv++;
                break;

            default:
                goto error;
        }
    }

quit:
    TWI_ACT_STOP();//TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)
    return rv;
error:
    rv = -1;
    goto quit;
}

/************************************************************************
**	��������:	I2cReadBytesForDs3232(uint8_t DVaddr, uint8_t eeaddr, signed char len, uint8_t *buf)
**	��������:	��DS3232ָ����ַд��ָ�����ȵ�����
**	�������: DVaddr��DS3232 I2C������ַ
			  eeaddr��д�����ݵ���ʼ��ַ
			  len	: д�����ݵĸ���
			  buf	������ָ�룬ָ���д��������׵�ַ
**	����ֵ  	: -1:	д��ʧ��
			  ��-1��д��ɹ�

**	�� �� ��:	
**	��������:	2018-1-31
*************************************************************************/
signed char I2cWriteBytesForDs3232(uint8_t DVaddr, uint8_t eeaddr, signed char len, uint8_t *buf)
{
    uint8_t sla, n = 0;
    int   rv = 0;
    sla = DVaddr;//TWI_SLA_DS;

restart:

    if(n++ >= MAX_ITER) return -1;

begin:
    TWI_ACT_START();
    TWI_ACT_WAIT();

    switch((twst = TW_STATUS))
    {
        case TW_REP_START:                                                          /* OK, but should not happen */
        case TW_START:
            break;

        case TW_MT_ARB_LOST:
            goto begin;

        default:
            return -1;
    }

    /* send SLA+W */
    TWDR = sla | TW_WRITE;
    TWCR = (1 << TWINT) | (1 << TWEN);
    TWI_ACT_WAIT();

    switch((twst = TW_STATUS))
    {
        case TW_MT_SLA_ACK:
            break;

        case TW_MT_SLA_NACK:
            goto restart;

        case TW_MT_ARB_LOST:
            goto begin;

        default:
            goto error;
    }

    TWDR = eeaddr;
    TWCR = (1 << TWINT) | (1 << TWEN);                                            /* clear interrupt to start transmission */
    TWI_ACT_WAIT();

    switch((twst = TW_STATUS))
    {
        case TW_MT_DATA_ACK:
            break;

        case TW_MT_DATA_NACK:
            goto quit;

        case TW_MT_ARB_LOST:
            goto begin;

        default:
            goto error;
    }

    for(; len > 0; len--)
    {
        TWDR = *buf++;
        TWCR = (1 << TWINT) | (1 << TWEN);

        while((TWCR & (1 << TWINT)) == 0) ;

        switch((twst = TW_STATUS))
        {
            case TW_MT_DATA_NACK:
                goto error;

            case TW_MT_DATA_ACK:
                rv++;
                break;

            default:
                goto error;
        }
    }

quit:
    TWI_ACT_STOP();
    return rv;
error:
    rv = -1;
    goto quit;
}

/************************************************************************
**	��������:	I2cReadBytesForEeprom(uint8_t Dvaddr, uint16_t addr, int len, uint8_t *buf)
**	��������:	���ⲿEEPROMָ����ַ��ȡָ�����ȵ�����
**	�������: DVaddr���ⲿEEPROM I2C������ַ
			  eeaddr����������ʼ��ַ
			  len	: ��ȡ���ݵĸ���
			  buf	���������ݴ�����׵�ַ
**	����ֵ  	: -1:	��ȡʧ��
			  ��-1����ȡ�ɹ�

**	�� �� ��:	
**	��������:	2018-1-31
*************************************************************************/
int I2cReadBytesForEeprom(uint8_t Dvaddr, uint16_t addr, int len, uint8_t *buf)
{
    uint8_t sla, twcr, n = 0;
    int   rv = 0;
    sla = Dvaddr;

restart:

    if(n++ >= MAX_ITER) return -1;

begin:
    TWI_ACT_START();
    TWI_ACT_WAIT();
#if 0//DEBUG
    ors485_sendchar(0xe1);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)
    {
        case TW_REP_START://0x10
        case TW_START:
            break;//0x08

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            return -1;
    }

    /* send SLA+W */
    TWDR = sla | TW_WRITE;//0
    TWCR = (1 << TWINT) | (1 << TWEN);
    TWI_ACT_WAIT();
#if 0//DEBUG
    ors485_sendchar(0xe2);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)
    {
        case TW_MT_SLA_ACK:
            break;//0x18

        case TW_MT_SLA_NACK:
            goto restart;//0x20

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    TWDR = addr >> 8;
    TWCR = (1 << TWINT) | (1 << TWEN);
    TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)
#if 0//DEBUG
    ors485_sendchar(0xe3);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)   //(TWSR & 0xF8)
    {
        case TW_MT_DATA_ACK:
            break;//0x28

        case TW_MT_DATA_NACK:
            goto quit;//0x30

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    TWDR = addr & 0x00ff;
    TWCR = (1 << TWINT) | (1 << TWEN);
    TWI_ACT_WAIT();
#if 0//DEBUG
    ors485_sendchar(0xe4);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)   //(TWSR & 0xF8)
    {
        case TW_MT_DATA_ACK:
            break;//0x28

        case TW_MT_DATA_NACK:
            goto quit;//0x30

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    /*
     * Next cycle(s): master receiver mode
     */
    TWI_ACT_START();//TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
    TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)
#if 0//DEBUG
    ors485_sendchar(0xe5);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)   //(TWSR & 0xF8)
    {
        case TW_REP_START://0x10
        case TW_START:
            break;//0x08                                                  /* OK, but should not happen */

        case TW_MT_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    /* send SLA+R */
    TWDR = sla | TW_READ;//1
    TWCR = (1 << TWINT) | (1 << TWEN);                                            /* clear interrupt to start transmission */
    TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)
#if 0//DEBUG
    ors485_sendchar(0xe6);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)   //(TWSR & 0xF8)
    {
        case TW_MR_SLA_ACK:
            break;//0x40

        case TW_MR_SLA_NACK:
            goto quit;//0x48

        case TW_MR_ARB_LOST:
            goto begin;//0x38

        default:
            goto error;
    }

    for(twcr = (1 << TWINT) | (1 << TWEN) | (1 << TWEA) ; len > 0; len--)
    {
        if(len == 1) twcr = (1 << TWINT) | (1 << TWEN);

        TWCR = twcr;

        while((TWCR & (1 << TWINT)) == 0) ;

#if 0//DEBUG
        ors485_sendchar(0xe7);
        ors485_sendchar(TW_STATUS);
#endif

        switch(TW_STATUS)
        {
            case TW_MR_DATA_NACK:
                len = 0;//0x58

            case TW_MR_DATA_ACK:
                *buf++ = TWDR;//0x50
                rv++;
                break;

            default:
                goto error;
        }
    }

quit:
    TWI_ACT_STOP();//TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)
    return rv;
error:
    rv = -1;
    goto quit;
}

/************************************************************************
**	��������:	I2cWriteBytesForEeprom(uint8_t Dvaddr, uint16_t addr, int len, uint8_t *buf)
**	��������:	���ⲿEEPROMָ����ַд��ָ�����ȵ�����
**	�������: DVaddr���ⲿEEPROM I2C������ַ
			  eeaddr��д�����ݵ���ʼ��ַ
			  len	: д�����ݵĸ���
			  buf	������ָ�룬ָ���д��������׵�ַ
**	����ֵ  	: -1:	д��ʧ��
			  ��-1��д��ɹ�

**	�� �� ��:	
**	��������:	2018-1-31
*************************************************************************/
int I2cWriteBytesForEeprom(uint8_t Dvaddr, uint16_t addr, int len, uint8_t *buf)
{
    uint8_t sla, n = 0;
    int   rv = 0;
    sla = Dvaddr;

restart:

    if(n++ >= MAX_ITER) return -1;

begin:
    TWI_ACT_START();
    TWI_ACT_WAIT();
#if 0//DEBUG
    ors485_sendchar(0xd1);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)
    {
        case TW_REP_START:                                                          /* OK, but should not happen */
        case TW_START:
            break;

        case TW_MT_ARB_LOST:
            goto begin;

        default:
            return -1;
    }

    /* send SLA+W */
    TWDR = sla | TW_WRITE;
    TWCR = (1 << TWINT) | (1 << TWEN);
    TWI_ACT_WAIT();
#if 0//DEBUG
    ors485_sendchar(0xd2);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)
    {
        case TW_MT_SLA_ACK:
            break;

        case TW_MT_SLA_NACK:
            goto restart;

        case TW_MT_ARB_LOST:
            goto begin;

        default:
            goto error;
    }

    TWDR = addr >> 8;
    TWCR = (1 << TWINT) | (1 << TWEN);                                            /* clear interrupt to start transmission */
    TWI_ACT_WAIT();
#if 0//DEBUG
    ors485_sendchar(0xd3);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)
    {
        case TW_MT_DATA_ACK:
            break;

        case TW_MT_DATA_NACK:
            goto quit;

        case TW_MT_ARB_LOST:
            goto begin;

        default:
            goto error;
    }

    TWDR = addr & 0x00ff;
    TWCR = (1 << TWINT) | (1 << TWEN);                                            /* clear interrupt to start transmission */
    TWI_ACT_WAIT();
#if 0//DEBUG
    ors485_sendchar(0xd4);
    ors485_sendchar(TW_STATUS);
#endif

    switch(TW_STATUS)
    {
        case TW_MT_DATA_ACK:
            break;

        case TW_MT_DATA_NACK:
            goto quit;

        case TW_MT_ARB_LOST:
            goto begin;

        default:
            goto error;
    }

    for(; len > 0; len--)
    {
        TWDR = *buf++;
        TWCR = (1 << TWINT) | (1 << TWEN);

        while((TWCR & (1 << TWINT)) == 0) ;

#if 0//DEBUG
        ors485_sendchar(0xd5);
        ors485_sendchar(TW_STATUS);
#endif

        switch(TW_STATUS)
        {
            case TW_MT_DATA_NACK:
                goto error;

            case TW_MT_DATA_ACK:
                rv++;
                break;

            default:
                goto error;
        }
    }

quit:
    TWI_ACT_STOP();
    _delay_ms(5);
    return rv;
error:
    rv = -1;
    goto quit;
}