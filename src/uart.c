
#include "kernel.h"

struct UART 		uart0, uart2;
struct UART_MODBUS 	uart1, uart3;


/************************************************************************
**	�������ƣ�Uart0Init(void)
**	��������: ���� 0 ��ʼ��  ��ӡ��/������ʹ�ô˶˿ڣ�������9600��8����λ������żУ��λ��1ֹͣλ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART0 RS232 ��ӡ��  ������
*************************************************************************/
void Uart0Init(void)
{
    UCSR0B = 0x00; //disable while setting baud rate
    UCSR0A = 0x00;
    UCSR0C = 0x06;
    UBRR0L = 0x2F; //set baud rate lo  //9600 bps
    UBRR0H = 0x00; //set baud rate hi
    //UCSR0B = (1<<RXCIE0)|(1<<TXEN0) |(1<<RXEN0);//�ܺͷ��� ,�����ж�ʹ�� <�жϷ�ʽ>
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);     //���� ,�����ж�ʹ�� <�жϷ�ʽ>

    UART0S0_OUT();//4052������ƶ�����
    UART0S1_OUT();

    SELECT_TFT();//����ʱ��uart0Ĭ��ѡ��һ��û��ʹ�õĹ���  ADD BY LY
}

/************************************************************************
**	�������ƣ�Uart0SendByte(uint8_t ch)
**	��������: ���� 0 ����һ���ֽڵ�����
**	�������: ch �����͵�����
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART0 RS232 ��ӡ��  ������
*************************************************************************/
void Uart0SendByte(uint8_t ch)
//Explain:Send a char from usart.
{
    while(UCSR0A_UDRE0 == 0);

    UDR0 = ch;

    while(UCSR0A_UDRE0 == 0);
}

/************************************************************************
**	�������ƣ�Uart0SendStr(uint8_t *buf, uint32_t len)
**	��������: ���� 0 ����һ������Ϊlen������
**	�������: buf �����͵����ݴ洢�ĵ�ַ
			  len ���������ݵĳ���
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART0 RS232 ��ӡ��  ������
*************************************************************************/
void Uart0SendStr(uint8_t *buf, uint32_t len)
{
    uint32_t i;

    for(i = 0; i < len; i++)
    {
        Uart0SendByte(buf[i]);
    }
}

/************************************************************************
**	�������ƣ�Uart0PutStringflash(char __farflash* string)
**	��������: ���� 0 ����һ�����ⳤ�ȵ��ַ��������ַ����洢��flash��
**	�������: string �����͵��ַ����洢�ĵ�ַ
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART0 RS232 ��ӡ��  ������
*************************************************************************/
void Uart0PutStringflash(char __farflash* string)
{
    while(*string != '\0')
    {
        Uart0SendByte(*string);
        string++;
    }
}

/************************************************************************
**	�������ƣ�Uart0PutString(char * string)
**	��������: ���� 0 ����һ�����ⳤ�ȵ��ַ���
**	�������: string �����͵��ַ����洢�ĵ�ַ
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART0 RS232 ��ӡ��  ������
*************************************************************************/
void Uart0PutString(char * string)
{
    while(*string != '\0')
    {
        Uart0SendByte(*string);
        string++;
    }
}

/************************************************************************
**	�������ƣ�Uart0PutString(char * string)
**	��������: ���� 0 ͨ����ѯ��ʽ��ȡһ���ֽ�
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART0 RS232 ��ӡ��  ������
*************************************************************************/
uint8_t Uart0GetByte(void)
{
    uint8_t     ucRcvData = 0xFF;
    uint32_t    tick485dly = 0;

    while((UCSR0A_RXC0 == 0) && (++tick485dly < 50000));       //RXCn

    if(tick485dly < 50000)
        ucRcvData = UDR0;

    return ucRcvData;
}

/************************************************************************
**	�������ƣ�USART0_RX_ISR(void)
**	��������: ���� 0 ���������жϳ���
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART0 RS232 ��ӡ��  ������
*************************************************************************/
#pragma vector = USART0_RX_vect

__interrupt void USART0_RX_ISR(void)
{
    uint8_t recbyte;

    recbyte = UDR0;
    uart0.recbuf[uart0.reclen++] = recbyte;

    if(uart0.reclen > BUFSIZE)
    {
        uart0.reclen = 0;
    }

    Mt318RecDataCheck();

}

/************************************************************************
**	�������ƣ�Uart1Init(uint16_t baudrate)
**	��������: ���� 1 ��ʼ�� Һ�������ƶ˿� ��������baudrate��8����λ��żУ�飬1ֹͣλ��
**	�������: baudrate��������
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART1 RS485 Һ��������
*************************************************************************/
void Uart1Init(uint16_t baudrate)
{
    uint16_t UBRR;

    MODBUS_L_EN_LOW();
    MODBUS_L_EN_OUT();
    //PORTB_PORTB6 = 1;//��� //485�շ��������� ���� ���
    UCSR1B = 0x00; //disable while setting baud rate
    UBRR = ((F_CPU >> 4) / baudrate - 1);         //����UBRR
    UBRR1L = (uint8_t)(UBRR & 0xFF);        //set baud rate lo
    UBRR1H = (uint8_t)((UBRR >> 8) & 0xFF);       //set baud rate hi
    UCSR1B_TXEN1 = 1;//_BV(TXEN3) | _BV(RXEN3);//| _BV(RXCIE3);//| _BV(TXCIE3);
    UCSR1B_RXEN1 = 1;//���ͽ���ʹ��
    UCSR1C = (1 << UCSZ10) | (1 << UCSZ11) | (1 << UPM11);       //0x06 8λ���ݣ�1λֹͣλ��żУ��
}

/************************************************************************
**	�������ƣ�Uart1SendByte(uint8_t ch)
**	��������: ���� 1 ����һ���ֽڵ�����
**	�������: ch �����͵�����
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART1 RS485 Һ��������
*************************************************************************/
void Uart1SendByte(uint8_t ch)
//Explain:Send a char from usart.
{
    while(UCSR1A_UDRE1 == 0);

    UDR1 = ch;

    while(UCSR1A_UDRE1 == 0);
}

/************************************************************************
**	�������ƣ�Uart1SendStr(uint8_t *buf, uint32_t len)
**	��������: ���� 1 ����һ���ֽڵ�����
**	�������: buf �����͵����ݴ洢�ĵ�ַ
			  len ���������ݵĳ���
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART1 RS485 Һ��������
*************************************************************************/
void Uart1SendStr(uint8_t *buf, uint32_t len)
{
    uint32_t i;
    MODBUS_L_EN_HIGH();

    for(i = 0; i < len; i++)
    {
        LED3_ON();
        Uart1SendByte(buf[i]);
        LED3_OFF();
    }

    _delay_ms(1);
    MODBUS_L_EN_LOW();
}

/************************************************************************
**	�������ƣ�USART1_RX_ISR(void)
**	��������: ���� 1 ���������жϳ���
**	�������: �� 
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART1 RS485 Һ��������
*************************************************************************/
#pragma vector = USART1_RX_vect
__interrupt void USART1_RX_ISR(void)
{
    uint8_t recbyte;

    recbyte = UDR1;
    uart1.recbuf[uart1.reclen++] = recbyte;
    LED2_ON();

    if((uart1.recbuf[0] != _MODBUS_G_) && (uart1.recbuf[0] != _MODBUS_L_))
    {
        uart1.reclen = 0;
    }

    if(uart1.reclen > MODBUS_BUFSIZE)
    {
        uart1.reclen = 0;
    }

    if(uart1.reclen >= 7)
    {
        if(uart1.recbuf[0] == _MODBUS_L_)   //Һ��
        {
            ModbusRecDataCheck_L();
        }
        else if(uart1.recbuf[0] == _MODBUS_G_)  //��Һ�������ߺϲ�ʱ����
        {
            memcpy(uart3.recbuf, uart1.recbuf, uart1.reclen);
            uart3.reclen = uart1.reclen;
            ModbusRecDataCheck_G();
        }
    }

    LED2_OFF();
}

/************************************************************************
**	�������ƣ�Uart2Init(uint32_t baudrate)
**	��������: ���� 2 ��ʼ�� ��̨ͨѶ�ӿ� ��������baudrate��8����λ������żУ�飬1ֹͣλ��
**	�������: baudrate��������
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART2 RS485 ��̨
*************************************************************************/
void Uart2Init(uint32_t baudrate)
{

    RS485_EN_LOW();
    RS485_EN_OUT();	    //����ʹ�ܿ��ƿ�PL4

    UCSR2B = 0x00;      //disable while setting baud rate
    UCSR2A = 0x00;
    UCSR2C = (1 << 2) | (1 << 1);     //8λ����λ������żУ�飬1λֹͣλUCSZn1UCSZn0

    switch(baudrate)
    {
        case 4800:
            UBRR2L = 0x5f; //set baud rate lo
            UBRR2H = 0x00; //set baud rate hi
            break;

        case 9600:
            UBRR2L = 0x2f; 	//set baud rate lo
            UBRR2H = 0x00; //set baud rate hi
            break;

        case 19200:
            UBRR2L = 0x17; 	//set baud rate lo
            UBRR2H = 0x00; //set baud rate hi
            break;

        case 38400:
            UBRR2L = 0x0B; 	//set baud rate lo
            UBRR2H = 0x00; //set baud rate hi
            break;

        default:
            break;
    }

    UCSR2B = (1 << 3) | (1 << 4);     //ʹ�ܽ��պͷ���

    uart2.flag = 0;
    uart2.reclen = 0;
    memset(uart2.recbuf, 0, BUFSIZE);   //�����������㡣

    OpenUart2RcvIRQ();//��uart2 ���������ж�

}

/************************************************************************
**	�������ƣ�Uart2SendByte(uint8_t ch)
**	��������: ���� 2 ����һ���ֽڵ�����
**	�������: ch �����͵�����
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART2 RS485 ��̨
*************************************************************************/
void Uart2SendByte(uint8_t ch)
{
    while(UCSR2A_UDRE2 == 0);

    UDR2 = ch;

    while(UCSR2A_UDRE2 == 0);
}

/************************************************************************
**	�������ƣ�Uart2SendStr(uint8_t *buf, uint32_t len)
**	��������: ���� 2 ����һ������Ϊlen������
**	�������: buf �����͵����ݴ洢�ĵ�ַ
			  len ���������ݵĳ���
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART2 RS485 ��̨
*************************************************************************/
void Uart2SendStr(uint8_t *buf, uint32_t len)
{
    uint32_t i;
    uint32_t  temp = 0;

    RS485_EN_HIGH();

    for(i = 0; i < len; i++)
    {
        Uart2SendByte(buf[i]);
    }

#if 1

    for(temp = 0; temp < 600;  temp++)   //����NOP()��ʱ
        asm("nop");

#endif

    RS485_EN_LOW();
}

/************************************************************************
**	�������ƣ�USART2_RX_ISR(void)
**	��������: ���� 2 ���������жϳ���
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵����UART2 RS485 ��̨
*************************************************************************/
#pragma vector = USART2_RX_vect
__interrupt void USART2_RX_ISR(void)
{
    uint8_t recbyte;

    recbyte = UDR2;

    uart2.recbuf[uart2.reclen++] = recbyte;
}

/************************************************************************
**	�������ƣ�Uart3Init(uint32_t baudrate)
**	��������: ���� 3 ��ʼ�� ����������ͨѶ�ӿ� ��������baudrate��8����λ��żУ�飬1ֹͣλ��
**	�������: baudrate��������
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵	����UART3 RS485 ����������
*************************************************************************/
void Uart3Init(uint32_t baudrate)
{
    uint16_t UBRR;
    MODBUS_G_EN_LOW();
    MODBUS_G_EN_OUT();
    //memset(&massflowmodbus.sendbuf[0],0,sizeof(struct MassFlowModbus));
	// massflowmodbus.sendbuf[0]=DEFAULT_ADDR;

    //DDRB |= 0x20;  //485�շ��������� ���� ���
    UCSR3B = 0x00; //disable while setting baud rate
    UBRR = ((F_CPU >> 4) / baudrate - 1);         //����UBRR
    UBRR3L = (uint8_t)(UBRR & 0xFF);        //set baud rate lo
    UBRR3H = (uint8_t)((UBRR >> 8) & 0xFF);       //set baud rate hi
    UCSR3B_TXEN3 = 1;//_BV(TXEN3) | _BV(RXEN3);//| _BV(RXCIE3);//| _BV(TXCIE3);
    UCSR3B_RXEN3 = 1;//���ͽ���ʹ��

    UCSR3C = (1 << UCSZ30) | (1 << UCSZ31) | (1 << UPM31);       //0x06 8λ���ݣ�1λֹͣλ��żУ��
    CloseUart3RcvIRQ();//disable the interrupt

}

/************************************************************************
**	�������ƣ�Uart3SendByte(uint8_t ch)
**	��������: ���� 3 ����һ���ֽڵ�����
**	�������: ch �����͵�����
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵	����UART3 RS485 ����������
*************************************************************************/
void Uart3SendByte(uint8_t ch)
//Explain:Send a char from usart.
{
    while(UCSR3A_UDRE3 == 0);

    UDR3 = ch;

    while(UCSR3A_UDRE3 == 0);
}

/************************************************************************
**	�������ƣ�Uart3SendStr(uint8_t *buf, uint32_t len)
**	��������: ���� 3 ����һ������Ϊlen������
**	�������: buf �����͵����ݴ洢�ĵ�ַ
			  len ���������ݵĳ���
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵	����UART3 RS485 ����������
*************************************************************************/
void Uart3SendStr(uint8_t *buf, uint32_t len)
{
    uint32_t i;

    if(sysparas.modbusRS485 == 2)
    {
        MODBUS_G_EN_HIGH();

        for(i = 0; i < len; i++)
        {
            Uart3SendByte(buf[i]);
        }

        _delay_ms(1);
        MODBUS_G_EN_LOW();
    }
    else //��Һ��ϲ�ʱ��ʹ��Һ��������
    {
        Uart1SendStr(buf, len);
    }
}

/************************************************************************
**	�������ƣ�USART3_RX_ISR(void)
**	��������: ���� 3 ���������жϳ���
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵	����UART3 RS485 ����������
*************************************************************************/
#pragma vector = USART3_RX_vect
__interrupt void USART3_RX_ISR(void)
{
    uint8_t recbyte;

    recbyte = UDR3;
    uart3.recbuf[uart3.reclen++] = recbyte;

    if((uart3.recbuf[0] != _MODBUS_G_) && (uart3.recbuf[0] != _MODBUS_L_))
    {
        uart3.reclen = 0;
    }

    if(uart3.reclen > MODBUS_BUFSIZE)
    {
        uart3.reclen = 0;
    }

    if(uart3.reclen >= 7)
    {
        ModbusRecDataCheck_G();
    }
}

/************************************************************************
**	�������ƣ�CloseUart3RcvIRQ(void)
**	��������: ��Uart3�Ľ����ж�	���������� 
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵	����UART3 RS485 ����������
*************************************************************************/
void  CloseUart3RcvIRQ(void)
{
    if(sysparas.modbusRS485 == 1)
    {
        UCSR1B_RXCIE1 = 0;
    }
    else
    {
        UCSR3B_RXCIE3 = 0;
    }
}

/************************************************************************
**	�������ƣ�OpenUart3RcvIRQ(void)
**	��������: ��Uart3�Ľ����ж�	���������� 
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2017-03-28

**	˵	����UART3 RS485 ����������
*************************************************************************/
void OpenUart3RcvIRQ(void)
{
    if(sysparas.modbusRS485 == 1)
    {
        UCSR1B_RXCIE1 = 1;
    }
    else
    {
        UCSR3B_RXCIE3 = 1;
    }
}





