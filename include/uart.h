#ifndef __UART_H
#define __UART_H

#define BUFSIZE     		200	//读卡器和后台串口接收buffer大小
#define MODBUS_BUFSIZE		40	//流量计串口接收buffer大小
#define BAUD_BPS    		9600

#define CardPort       0
#define PrintPort      0

#define CloseUart0RcvIRQ()      UCSR0B_RXCIE0 = 0     //关串口0的接收中断  打印机/读卡器
#define OpenUart0RcvIRQ()       UCSR0B_RXCIE0 = 1     //开串口0的接收中断

#define CloseUart1RcvIRQ()      UCSR1B_RXCIE1 = 0     //关串口1的接收中断  液相流量计 
#define OpenUart1RcvIRQ()       UCSR1B_RXCIE1 = 1     //开串口1的接收中断

#define CloseUart2RcvIRQ()      UCSR2B_RXCIE2 = 0     //关串口2的接收中断  RS485 后台
#define OpenUart2RcvIRQ()       UCSR2B_RXCIE2 = 1     //开串口2的接收中断 

void CloseUart3RcvIRQ(void)		;	//关串口3的接收中断  气相流量计
void OpenUart3RcvIRQ(void);       	//开串口3的接收中断

enum
{
    USART0_BANK = 0,
    USART0_TFT,
    USART0_PRINT,
    USART0_CARD
};

struct UART
{
    uint8_t reclen;
    uint8_t recbuf[BUFSIZE];
    uint8_t flag;
};

struct UART_MODBUS
{
    uint8_t reclen;
    uint8_t recbuf[MODBUS_BUFSIZE];
    uint8_t flag;
};

extern  struct UART uart0;
extern  struct UART_MODBUS uart1;
extern  struct UART uart2;
extern  struct UART_MODBUS uart3;

/*******************  UART0 RS232 打印机/读卡器 *******************************/
void Uart0Init(void);
void Uart0SendByte(uint8_t ch);
void Uart0SendStr(uint8_t *puiStr, uint32_t len);
void Uart0PutString(char * string);
void Uart0PutStringflash(char __farflash* string);
uint8_t Uart0GetByte(void);

/******************  UART1 RS485 液相流量计  **********************************/
void Uart1Init(uint16_t baudrate);
void Uart1SendByte(uint8_t ch);
void Uart1SendStr(uint8_t *puiStr, uint32_t len);

/*****************  UART2 RS485 后台  *****************************************/
void Uart2Init(uint32_t baudrate);
void Uart2SendByte(uint8_t ch);
void Uart2SendStr(uint8_t *puiStr, uint32_t len);

/*****************  UART3 RS485 气相流量计 ************************************/
void Uart3Init(uint32_t baudrate);
void Uart3SendByte(uint8_t ch);
void Uart3SendStr(uint8_t *buf, uint32_t len);

#endif