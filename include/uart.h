#ifndef __UART_H
#define __UART_H

#define BUFSIZE     		200	//�������ͺ�̨���ڽ���buffer��С
#define MODBUS_BUFSIZE		40	//�����ƴ��ڽ���buffer��С
#define BAUD_BPS    		9600

#define CardPort       0
#define PrintPort      0

#define CloseUart0RcvIRQ()      UCSR0B_RXCIE0 = 0     //�ش���0�Ľ����ж�  ��ӡ��/������
#define OpenUart0RcvIRQ()       UCSR0B_RXCIE0 = 1     //������0�Ľ����ж�

#define CloseUart1RcvIRQ()      UCSR1B_RXCIE1 = 0     //�ش���1�Ľ����ж�  Һ�������� 
#define OpenUart1RcvIRQ()       UCSR1B_RXCIE1 = 1     //������1�Ľ����ж�

#define CloseUart2RcvIRQ()      UCSR2B_RXCIE2 = 0     //�ش���2�Ľ����ж�  RS485 ��̨
#define OpenUart2RcvIRQ()       UCSR2B_RXCIE2 = 1     //������2�Ľ����ж� 

void CloseUart3RcvIRQ(void)		;	//�ش���3�Ľ����ж�  ����������
void OpenUart3RcvIRQ(void);       	//������3�Ľ����ж�

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

/*******************  UART0 RS232 ��ӡ��/������ *******************************/
void Uart0Init(void);
void Uart0SendByte(uint8_t ch);
void Uart0SendStr(uint8_t *puiStr, uint32_t len);
void Uart0PutString(char * string);
void Uart0PutStringflash(char __farflash* string);
uint8_t Uart0GetByte(void);

/******************  UART1 RS485 Һ��������  **********************************/
void Uart1Init(uint16_t baudrate);
void Uart1SendByte(uint8_t ch);
void Uart1SendStr(uint8_t *puiStr, uint32_t len);

/*****************  UART2 RS485 ��̨  *****************************************/
void Uart2Init(uint32_t baudrate);
void Uart2SendByte(uint8_t ch);
void Uart2SendStr(uint8_t *puiStr, uint32_t len);

/*****************  UART3 RS485 ���������� ************************************/
void Uart3Init(uint32_t baudrate);
void Uart3SendByte(uint8_t ch);
void Uart3SendStr(uint8_t *buf, uint32_t len);

#endif