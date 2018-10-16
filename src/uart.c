
#include "kernel.h"

struct UART 		uart0, uart2;
struct UART_MODBUS 	uart1, uart3;


/************************************************************************
**	函数名称：Uart0Init(void)
**	函数功能: 串口 0 初始化  打印机/读卡器使用此端口（波特率9600，8数据位，无奇偶校验位，1停止位）
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART0 RS232 打印机  读卡器
*************************************************************************/
void Uart0Init(void)
{
    UCSR0B = 0x00; //disable while setting baud rate
    UCSR0A = 0x00;
    UCSR0C = 0x06;
    UBRR0L = 0x2F; //set baud rate lo  //9600 bps
    UBRR0H = 0x00; //set baud rate hi
    //UCSR0B = (1<<RXCIE0)|(1<<TXEN0) |(1<<RXEN0);//受和发送 ,接受中断使能 <中断方式>
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);     //发送 ,接受中断使能 <中断方式>

    UART0S0_OUT();//4052输出控制端配置
    UART0S1_OUT();

    SELECT_TFT();//启动时，uart0默认选择一个没有使用的功能  ADD BY LY
}

/************************************************************************
**	函数名称：Uart0SendByte(uint8_t ch)
**	函数功能: 串口 0 发送一个字节的数据
**	输入参数: ch 待发送的数据
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART0 RS232 打印机  读卡器
*************************************************************************/
void Uart0SendByte(uint8_t ch)
//Explain:Send a char from usart.
{
    while(UCSR0A_UDRE0 == 0);

    UDR0 = ch;

    while(UCSR0A_UDRE0 == 0);
}

/************************************************************************
**	函数名称：Uart0SendStr(uint8_t *buf, uint32_t len)
**	函数功能: 串口 0 发送一串长度为len的数据
**	输入参数: buf 待发送的数据存储的地址
			  len 待发送数据的长度
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART0 RS232 打印机  读卡器
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
**	函数名称：Uart0PutStringflash(char __farflash* string)
**	函数功能: 串口 0 发送一个任意长度的字符串，该字符串存储于flash中
**	输入参数: string 待发送的字符串存储的地址
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART0 RS232 打印机  读卡器
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
**	函数名称：Uart0PutString(char * string)
**	函数功能: 串口 0 发送一个任意长度的字符串
**	输入参数: string 待发送的字符串存储的地址
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART0 RS232 打印机  读卡器
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
**	函数名称：Uart0PutString(char * string)
**	函数功能: 串口 0 通过查询方式读取一个字节
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART0 RS232 打印机  读卡器
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
**	函数名称：USART0_RX_ISR(void)
**	函数功能: 串口 0 接收数据中断程序
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART0 RS232 打印机  读卡器
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
**	函数名称：Uart1Init(uint16_t baudrate)
**	函数功能: 串口 1 初始化 液相流量计端口 （波特率baudrate，8数据位，偶校验，1停止位）
**	输入参数: baudrate：波特率
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART1 RS485 液相流量计
*************************************************************************/
void Uart1Init(uint16_t baudrate)
{
    uint16_t UBRR;

    MODBUS_L_EN_LOW();
    MODBUS_L_EN_OUT();
    //PORTB_PORTB6 = 1;//输出 //485收发控制引脚 方向 输出
    UCSR1B = 0x00; //disable while setting baud rate
    UBRR = ((F_CPU >> 4) / baudrate - 1);         //计算UBRR
    UBRR1L = (uint8_t)(UBRR & 0xFF);        //set baud rate lo
    UBRR1H = (uint8_t)((UBRR >> 8) & 0xFF);       //set baud rate hi
    UCSR1B_TXEN1 = 1;//_BV(TXEN3) | _BV(RXEN3);//| _BV(RXCIE3);//| _BV(TXCIE3);
    UCSR1B_RXEN1 = 1;//发送接收使能
    UCSR1C = (1 << UCSZ10) | (1 << UCSZ11) | (1 << UPM11);       //0x06 8位数据，1位停止位，偶校验
}

/************************************************************************
**	函数名称：Uart1SendByte(uint8_t ch)
**	函数功能: 串口 1 发送一个字节的数据
**	输入参数: ch 待发送的数据
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART1 RS485 液相流量计
*************************************************************************/
void Uart1SendByte(uint8_t ch)
//Explain:Send a char from usart.
{
    while(UCSR1A_UDRE1 == 0);

    UDR1 = ch;

    while(UCSR1A_UDRE1 == 0);
}

/************************************************************************
**	函数名称：Uart1SendStr(uint8_t *buf, uint32_t len)
**	函数功能: 串口 1 发送一个字节的数据
**	输入参数: buf 待发送的数据存储的地址
			  len 待发送数据的长度
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART1 RS485 液相流量计
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
**	函数名称：USART1_RX_ISR(void)
**	函数功能: 串口 1 接收数据中断程序
**	输入参数: 无 
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART1 RS485 液相流量计
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
        if(uart1.recbuf[0] == _MODBUS_L_)   //液相
        {
            ModbusRecDataCheck_L();
        }
        else if(uart1.recbuf[0] == _MODBUS_G_)  //气液相数据线合并时处理
        {
            memcpy(uart3.recbuf, uart1.recbuf, uart1.reclen);
            uart3.reclen = uart1.reclen;
            ModbusRecDataCheck_G();
        }
    }

    LED2_OFF();
}

/************************************************************************
**	函数名称：Uart2Init(uint32_t baudrate)
**	函数功能: 串口 2 初始化 后台通讯接口 （波特率baudrate，8数据位，无奇偶校验，1停止位）
**	输入参数: baudrate：波特率
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART2 RS485 后台
*************************************************************************/
void Uart2Init(uint32_t baudrate)
{

    RS485_EN_LOW();
    RS485_EN_OUT();	    //设置使能控制口PL4

    UCSR2B = 0x00;      //disable while setting baud rate
    UCSR2A = 0x00;
    UCSR2C = (1 << 2) | (1 << 1);     //8位数据位，无奇偶校验，1位停止位UCSZn1UCSZn0

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

    UCSR2B = (1 << 3) | (1 << 4);     //使能接收和发送

    uart2.flag = 0;
    uart2.reclen = 0;
    memset(uart2.recbuf, 0, BUFSIZE);   //发送数组清零。

    OpenUart2RcvIRQ();//开uart2 接收数据中断

}

/************************************************************************
**	函数名称：Uart2SendByte(uint8_t ch)
**	函数功能: 串口 2 发送一个字节的数据
**	输入参数: ch 待发送的数据
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART2 RS485 后台
*************************************************************************/
void Uart2SendByte(uint8_t ch)
{
    while(UCSR2A_UDRE2 == 0);

    UDR2 = ch;

    while(UCSR2A_UDRE2 == 0);
}

/************************************************************************
**	函数名称：Uart2SendStr(uint8_t *buf, uint32_t len)
**	函数功能: 串口 2 发送一串长度为len的数据
**	输入参数: buf 待发送的数据存储的地址
			  len 待发送数据的长度
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART2 RS485 后台
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

    for(temp = 0; temp < 600;  temp++)   //利用NOP()延时
        asm("nop");

#endif

    RS485_EN_LOW();
}

/************************************************************************
**	函数名称：USART2_RX_ISR(void)
**	函数功能: 串口 2 接收数据中断程序
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说明：UART2 RS485 后台
*************************************************************************/
#pragma vector = USART2_RX_vect
__interrupt void USART2_RX_ISR(void)
{
    uint8_t recbyte;

    recbyte = UDR2;

    uart2.recbuf[uart2.reclen++] = recbyte;
}

/************************************************************************
**	函数名称：Uart3Init(uint32_t baudrate)
**	函数功能: 串口 3 初始化 气相流量计通讯接口 （波特率baudrate，8数据位，偶校验，1停止位）
**	输入参数: baudrate：波特率
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说	明：UART3 RS485 气相流量计
*************************************************************************/
void Uart3Init(uint32_t baudrate)
{
    uint16_t UBRR;
    MODBUS_G_EN_LOW();
    MODBUS_G_EN_OUT();
    //memset(&massflowmodbus.sendbuf[0],0,sizeof(struct MassFlowModbus));
	// massflowmodbus.sendbuf[0]=DEFAULT_ADDR;

    //DDRB |= 0x20;  //485收发控制引脚 方向 输出
    UCSR3B = 0x00; //disable while setting baud rate
    UBRR = ((F_CPU >> 4) / baudrate - 1);         //计算UBRR
    UBRR3L = (uint8_t)(UBRR & 0xFF);        //set baud rate lo
    UBRR3H = (uint8_t)((UBRR >> 8) & 0xFF);       //set baud rate hi
    UCSR3B_TXEN3 = 1;//_BV(TXEN3) | _BV(RXEN3);//| _BV(RXCIE3);//| _BV(TXCIE3);
    UCSR3B_RXEN3 = 1;//发送接收使能

    UCSR3C = (1 << UCSZ30) | (1 << UCSZ31) | (1 << UPM31);       //0x06 8位数据，1位停止位，偶校验
    CloseUart3RcvIRQ();//disable the interrupt

}

/************************************************************************
**	函数名称：Uart3SendByte(uint8_t ch)
**	函数功能: 串口 3 发送一个字节的数据
**	输入参数: ch 待发送的数据
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说	明：UART3 RS485 气相流量计
*************************************************************************/
void Uart3SendByte(uint8_t ch)
//Explain:Send a char from usart.
{
    while(UCSR3A_UDRE3 == 0);

    UDR3 = ch;

    while(UCSR3A_UDRE3 == 0);
}

/************************************************************************
**	函数名称：Uart3SendStr(uint8_t *buf, uint32_t len)
**	函数功能: 串口 3 发送一串长度为len的数据
**	输入参数: buf 待发送的数据存储的地址
			  len 待发送数据的长度
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说	明：UART3 RS485 气相流量计
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
    else //气液相合并时，使用液相数据线
    {
        Uart1SendStr(buf, len);
    }
}

/************************************************************************
**	函数名称：USART3_RX_ISR(void)
**	函数功能: 串口 3 接收数据中断程序
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说	明：UART3 RS485 气相流量计
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
**	函数名称：CloseUart3RcvIRQ(void)
**	函数功能: 关Uart3的接收中断	气相流量计 
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说	明：UART3 RS485 气相流量计
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
**	函数名称：OpenUart3RcvIRQ(void)
**	函数功能: 开Uart3的接收中断	气相流量计 
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2017-03-28

**	说	明：UART3 RS485 气相流量计
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





