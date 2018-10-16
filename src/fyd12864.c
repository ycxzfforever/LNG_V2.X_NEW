#include "kernel.h"
/*
ע�����Դ�ļ���Ҫ�ǽ�������FYD12864-0402CҺ���ĺ���
*/
#define LCMCMD	   true
#define LCMDATA    false

/************************************************************************
**	��������:	FYD12864SendByte(uint8_t B_yte)
**	��������:	����һ���ֽ�
**	�������: B_yte�������͵��ֽ�
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void FYD12864SendByte(uint8_t B_yte)
{
    uint8_t i;
    OLCD_SID_OUT(); 	 //����SIDΪ���

    for(i = 0; i < 8; i++)
    {
        OLCD_CLK_L();

        if(B_yte & 0x80)
        {
            OLCD_SID_H();
        }
        else
        {
            OLCD_SID_L();
        }

        B_yte = B_yte << 1;
        OLCD_CLK_H();
        __delay_cycles(2);
    }

    OLCD_SID_IN();
}

/************************************************************************
**	��������:	FYD12864WrtData(char Data, char bCmd)
**	��������:	дһ���ֽڵ����ݻ�����
**	�������: Data����д������ݻ�����
			  bCmd��д��������ݻ��������ʶ��1:Data�����0��Data������
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void FYD12864WrtData(char Data, char bCmd)
{
    OLCD_CS_H();

    if(bCmd)
    {
        FYD12864SendByte(0xF8);      //д��
    }
    else
    {
        FYD12864SendByte(0xFA);      //д����
    }

    FYD12864SendByte(0xF0 & Data);		    //��4λ
    FYD12864SendByte(0xF0 & (Data << 4));	//��4λ
    OLCD_CS_L();
// Delay(5);
}

/************************************************************************
**	��������:	FYD12864ClrScreen(void)
**	��������:	����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void FYD12864ClrScreen(void)
{
    FYD12864WrtData(0, 0x01);
    _delay_ms(2);
    FYD12864ShowGDRAM(NULL);
    _delay_ms(2);
}

/************************************************************************
**	��������:	FYD12864Locate(uint8_t row, uint8_t col)
**	��������:	�����궨λ��ָ���к��е��׵�ַ
**	�������: row��������1<=row<=4
			  col��������1<=col<=8
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void FYD12864Locate(uint8_t row, uint8_t col)
{
    uint8_t addr[4] = {0x80, 0x90, 0x88, 0x98};
    FYD12864WrtData(addr[row - 1] + col - 1, LCMCMD);
}

/************************************************************************
**	��������:	FYD12864WrtStr(uint8_t row, uint8_t col, char *str, char bClrLeft)
**	��������:	��ָ�����꿪ʼ��ʾ����
**	�������: row��			������
			  col��		������
			  str��		Ҫ��ʾ������
			  bClrleft��ʣ��û��д�����ݵ�λ���Ƿ���ձ�־��1:��գ�0�������
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void FYD12864WrtStr(uint8_t row, uint8_t col, char *str, char bClrLeft)
{
    uint8_t i, length;
	//����ʾ�����ݳ���
    length = strlen(str);
	//������ʾ���ȴ�������������У���ضϲ���ʾ
    length = ((9 - col) * 2 < length) ? (9 - col) * 2 : length;       
    FYD12864Locate(row, col);

    for(i = 0; i < length; i++)
    {
    	FYD12864WrtData(*str++, LCMDATA);   //OLCD_WrtStr(str,length);
    }
        
	//����ĳ�����Ҫ����������һ������ʾ����û����������������룬��������ʾ�հ����ݣ�0x20--�հ�
    if(bClrLeft)   
    {
        for(i = (col - 1) * 2 + length; i < 16; i++)
        {
            FYD12864WrtData(0x20, LCMDATA);   //0x20=�ո�
        }
    }
}

/************************************************************************
**	��������:	FYD12864Inputchar(uint8_t row, uint8_t   col, char tmp)
**	��������:	��ָ��������ʾһ���ֽڵ�����
**	�������: row��			������
			  col��		������
			  tmp��		Ҫ��ʾ������
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void FYD12864Inputchar(uint8_t row, uint8_t  col, char tmp)
{
    FYD12864Locate(row, col);
    FYD12864WrtData(tmp, LCMDATA);
}

/************************************************************************
**	��������:	FYD12864DispPrintf(uint8_t posx, unsigned posy, char *fmt, ...)
**	��������:	��ʾ�������÷���printf�����÷�һ��
**	�������: posx��			�����꣬1<=posx<=4
			  posy��		�����꣬1<=posy<=8
			  fmt��		ָ�룬ָ��ָ����ʽ������
			  ...��		
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void FYD12864DispPrintf(uint8_t posx, unsigned posy, char *fmt, ...)
{
    va_list ap;
    char str[50];
    va_start(ap, fmt);
    vsprintf(str, fmt, ap);
    FYD12864WrtData(0x03, 0x01);   //�趨DDRAM�ĵ�ַ��������"00"
    FYD12864WrtData(0x0C, 0x01);   //������ʾ
    FYD12864WrtStr(posx, posy, str, 1);
    va_end(ap);
}

/************************************************************************
**	��������:	FYD12864DispPrintfFlash(uint8_t posx, unsigned posy, char __farflash*fmt, ...)
**	��������:	��ʾ�������÷���printf�����÷�һ������ͬ��������ʾ�����ݴ洢��flash��
**	�������: posx��			�����꣬1<=posx<=4
			  posy��		�����꣬1<=posy<=8
			  fmt��		ָ��flash��ָ�룬ָ��flash��ָ����ʽ������
			  ...��		
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2018-05-25
*************************************************************************/
void FYD12864DispPrintfFlash(uint8_t posx, unsigned posy, char __farflash*fmt, ...)

{
    va_list ap;
    char str[30], tmp[30];
    strcpy_P(tmp, fmt);
    va_start(ap, tmp);
    vsprintf(str, tmp, ap);
    FYD12864WrtData(0x03, 0x01);   //�趨DDRAM�ĵ�ַ��������"00"
    FYD12864WrtData(0x0C, 0x01);   //������ʾ
    FYD12864WrtStr(posx, posy, str, 1);
    va_end(ap);
}

/************************************************************************
**	��������:	FYD12864DispPrintfFlash(uint8_t posx, unsigned posy, char __farflash*fmt, ...)
**	��������:	��ʾ�������÷���printf�����÷�һ������ͬ��������ʾ�����ݴ洢��flash��
**	�������: row��			�����꣬1<=posx<=4
			  col:		�����꣬1<=posy<=8
			  str��		ָ��flash��ָ�룬ָ��flash��ָ����ʽ������
			  bClrleft��ʣ��δ��ʾ��λ���Ƿ���ձ�־��1:���ʣ��λ�ã�0�������ʣ��λ��		
**	����ֵ  	: ��

**	�� �� ��:	HHM
**	��������:	2016-04-25
*************************************************************************/
void FYD12864DispString(uint8_t row, uint8_t col, char __farflash* str, bool bClrLeft)
{
    uint8_t i, length;
    length = strlen_P(str);
    length = ((9 - col) * 2 < length) ? (9 - col) * 2 : length;
    FYD12864Locate(row, col);

    for(i = 0; i < length; i++)
        FYD12864WrtData(*str++, LCMDATA);

    if(bClrLeft)
    {
        for(i = (col - 1) * 2 + length; i < 16; i++)
        {
            FYD12864WrtData(0x20, LCMDATA);
        }
    }
}

/************************************************************************
**	��������:	FYD12864ClearLine(uint8_t line)
**	��������:	���ĳ��
**	�������: line��		�����꣬1<=line<=4
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-04-25
*************************************************************************/
void FYD12864ClearLine(uint8_t line)
//1<=line<=4
{
    uint8_t i;
    FYD12864Locate(line, 1);

    for(i = 0; i < 16; i++)
    {
        FYD12864WrtData(0x20, LCMDATA);
    }
}

/************************************************************************
**	��������:	FYD12864NarrowUp(uint8_t px, uint8_t py)
**	��������:	��ʾ�Ϸ�ͼ��
**	�������: px�������꣬1<=px<=4
			  py�������꣬1<=py<=8
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-04-25
*************************************************************************/
void FYD12864NarrowUp(uint8_t px, uint8_t py)
{
    FYD12864Locate(px, py);
    FYD12864WrtData(0x1E, LCMDATA);
//  FYD12864WrtData(0x1E,LCMDATA);
}

/************************************************************************
**	��������:	FYD12864NarrowDown(uint8_t px, uint8_t py)
**	��������:	��ʾ�·�ͼ��
**	�������: px�������꣬1<=px<=4
			  py�������꣬1<=py<=8
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-04-25
*************************************************************************/
void FYD12864NarrowDown(uint8_t px, uint8_t py)
{
    FYD12864Locate(px, py);
    FYD12864WrtData(0x1F, LCMDATA);
//  FYD12864WrtData(0x1F,LCMDATA);
}

/************************************************************************
**	��������:	FYD12864SelectFlag(uint8_t px, uint8_t py)
**	��������:	ѡ��ͼ��
**	�������: px�������꣬1<=px<=4
			  py�������꣬1<=py<=8
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-04-25
*************************************************************************/
void FYD12864SelectFlag(uint8_t px, uint8_t py)
{
    FYD12864Locate(px, py);
    FYD12864WrtData(0x3C, LCMDATA);
}

/************************************************************************
**	��������:	FYD12864ClearSelectFlag(uint8_t px, uint8_t py)
**	��������:	���ѡ��ͼ��
**	�������: px�������꣬1<=px<=4
			  py�������꣬1<=py<=8
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-04-25
*************************************************************************/
void FYD12864ClearSelectFlag(uint8_t px, uint8_t py)
//���ܣ����ѡ��ͼ��
{
    FYD12864Locate(px, py);
    FYD12864WrtData(0x20, LCMDATA);
}

/************************************************************************
**	��������:	FYD12864Init(void)
**	��������:	FYD12864Ӳ����ʼ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2016-04-25
*************************************************************************/
void FYD12864Init(void)
{
    OLCD_CS_OUT();
    __delay_cycles(2);
    OLCD_SID_OUT();
    __delay_cycles(2);
    OLCD_CLK_OUT();
    __delay_cycles(2);
    OLCD_CS_H();
    __delay_cycles(2);
    OLCD_SID_H();
    __delay_cycles(2);
    OLCD_CLK_H();
    __delay_cycles(2);
    FYD12864WrtData(0x03, LCMCMD);   //�趨DDRAM�ĵ�ַ��������"00"
    FYD12864WrtData(0x0C, LCMCMD);   //������ʾ
    FYD12864WrtData(0x06, LCMCMD);   //�������
    FYD12864WrtData(0x01, LCMCMD);   //�����ʾ
    __delay_cycles(73728);   //_delay_ms(10);
    FYD12864ClrScreen();
}

/************************************************************************
**	��������:	FYD12864ShowGDRAM(uint8_t* address)
**	��������:	����(addressΪNULL,��GDRAM����0)����ʾ������Ϣ(��ʾaddress��ָ�������1024���ֽ�
**	�������: address��NULL:����������:address��ָ�������ָ��,������ʾ
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2018-1-31
*************************************************************************/
void FYD12864ShowGDRAM(uint8_t* address)
{
    uint8_t i, j;
    FYD12864WrtData(0x36, 0x01);   //������չָ�����ͼ����ʾ

    for(i = 0; i < 32; i++)   //�ϰ�����32��������д��
    {
        FYD12864WrtData(0x80 + i, 1);   //���ʹ�ֱ��ַ
        FYD12864WrtData(0x80, 1);   //����ˮƽ��ַ��ˮƽ��ַ���Լ�1

        for(j = 0; j < 16; j++)   //ÿ��128���㣬����һ��Ҫ16����λ16��������Ҳ����8λ��������������ȫ������
        {
            if(address != NULL)
            {
                FYD12864WrtData(*address, 0);
                address++;
            }
            else
            {
                FYD12864WrtData(0, 0);
            }
        }
    }

    for(i = 0; i < 32; i++)   //�°�����32�Ų���ԭ����ϰ���һ��
    {
        FYD12864WrtData(0x80 + i, 1);
        FYD12864WrtData(0x88, 1);

        for(j = 0; j < 16; j++)
        {
            if(address != NULL)
            {
                FYD12864WrtData(*address, 0);
                address++;
            }
            else
            {
                FYD12864WrtData(0, 0);
            }
        }
    }

    FYD12864WrtData(0x30, 0x01);   //�������ָ��
}

/************************************************************************
**	��������:	  Fyd12864CurrsorBlink(uint8_t row, uint8_t col, uint8_t flag)
**	��������:	�����˸����
**	�������: row: ������(ע�����FYD12864,��ΧΪ1<=row<=4)
			  col: ������(ע�����FYD12864,��ΧΪ1<=col<=8))
			  flag:1:��˸��0:����
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2018-2-1
*************************************************************************/
void Fyd12864CurrsorBlink(uint8_t row, uint8_t col, uint8_t flag)
{
    FYD12864Locate(row, col);

    if(flag)
    {
        FYD12864WrtData(0x0F, 1);    //0x0E�����ף�0x0F����
    }
    else
    {
        FYD12864WrtData(0x0C, 1);
    }
}

