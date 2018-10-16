#include "kernel.h"
/*
注：这个源文件主要是建立驱动FYD12864-0402C液晶的函数
*/
#define LCMCMD	   true
#define LCMDATA    false

/************************************************************************
**	函数名称:	FYD12864SendByte(uint8_t B_yte)
**	函数功能:	发送一个字节
**	输入参数: B_yte：待发送的字节
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void FYD12864SendByte(uint8_t B_yte)
{
    uint8_t i;
    OLCD_SID_OUT(); 	 //设置SID为输出

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
**	函数名称:	FYD12864WrtData(char Data, char bCmd)
**	函数功能:	写一个字节的数据或命令
**	输入参数: Data：待写入的数据或命令
			  bCmd：写入的是数据还是命令标识，1:Data是命令；0：Data是数据
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void FYD12864WrtData(char Data, char bCmd)
{
    OLCD_CS_H();

    if(bCmd)
    {
        FYD12864SendByte(0xF8);      //写命
    }
    else
    {
        FYD12864SendByte(0xFA);      //写数据
    }

    FYD12864SendByte(0xF0 & Data);		    //高4位
    FYD12864SendByte(0xF0 & (Data << 4));	//低4位
    OLCD_CS_L();
// Delay(5);
}

/************************************************************************
**	函数名称:	FYD12864ClrScreen(void)
**	函数功能:	清屏
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void FYD12864ClrScreen(void)
{
    FYD12864WrtData(0, 0x01);
    _delay_ms(2);
    FYD12864ShowGDRAM(NULL);
    _delay_ms(2);
}

/************************************************************************
**	函数名称:	FYD12864Locate(uint8_t row, uint8_t col)
**	函数功能:	将坐标定位到指定行和列的首地址
**	输入参数: row：行坐标1<=row<=4
			  col：列坐标1<=col<=8
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void FYD12864Locate(uint8_t row, uint8_t col)
{
    uint8_t addr[4] = {0x80, 0x90, 0x88, 0x98};
    FYD12864WrtData(addr[row - 1] + col - 1, LCMCMD);
}

/************************************************************************
**	函数名称:	FYD12864WrtStr(uint8_t row, uint8_t col, char *str, char bClrLeft)
**	函数功能:	从指定坐标开始显示数据
**	输入参数: row：			行坐标
			  col：		列坐标
			  str：		要显示的内容
			  bClrleft：剩余没有写入数据的位置是否清空标志，1:清空；0：不清空
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void FYD12864WrtStr(uint8_t row, uint8_t col, char *str, char bClrLeft)
{
    uint8_t i, length;
	//待显示的数据长度
    length = strlen(str);
	//进行显示长度处理，如果超过单行，则截断不显示
    length = ((9 - col) * 2 < length) ? (9 - col) * 2 : length;       
    FYD12864Locate(row, col);

    for(i = 0; i < length; i++)
    {
    	FYD12864WrtData(*str++, LCMDATA);   //OLCD_WrtStr(str,length);
    }
        
	//下面的程序主要作用是用在一行所显示内容没有满，以免出现乱码，就让它显示空白内容，0x20--空白
    if(bClrLeft)   
    {
        for(i = (col - 1) * 2 + length; i < 16; i++)
        {
            FYD12864WrtData(0x20, LCMDATA);   //0x20=空格
        }
    }
}

/************************************************************************
**	函数名称:	FYD12864Inputchar(uint8_t row, uint8_t   col, char tmp)
**	函数功能:	从指定坐标显示一个字节的数据
**	输入参数: row：			行坐标
			  col：		列坐标
			  tmp：		要显示的内容
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void FYD12864Inputchar(uint8_t row, uint8_t  col, char tmp)
{
    FYD12864Locate(row, col);
    FYD12864WrtData(tmp, LCMDATA);
}

/************************************************************************
**	函数名称:	FYD12864DispPrintf(uint8_t posx, unsigned posy, char *fmt, ...)
**	函数功能:	显示函数，用法与printf函数用法一样
**	输入参数: posx：			行坐标，1<=posx<=4
			  posy：		列坐标，1<=posy<=8
			  fmt：		指针，指向指定格式的数据
			  ...：		
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void FYD12864DispPrintf(uint8_t posx, unsigned posy, char *fmt, ...)
{
    va_list ap;
    char str[50];
    va_start(ap, fmt);
    vsprintf(str, fmt, ap);
    FYD12864WrtData(0x03, 0x01);   //设定DDRAM的地址计数器到"00"
    FYD12864WrtData(0x0C, 0x01);   //整体显示
    FYD12864WrtStr(posx, posy, str, 1);
    va_end(ap);
}

/************************************************************************
**	函数名称:	FYD12864DispPrintfFlash(uint8_t posx, unsigned posy, char __farflash*fmt, ...)
**	函数功能:	显示函数，用法与printf函数用法一样，不同的是其显示的内容存储于flash中
**	输入参数: posx：			行坐标，1<=posx<=4
			  posy：		列坐标，1<=posy<=8
			  fmt：		指向flash的指针，指向flash中指定格式的数据
			  ...：		
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2018-05-25
*************************************************************************/
void FYD12864DispPrintfFlash(uint8_t posx, unsigned posy, char __farflash*fmt, ...)

{
    va_list ap;
    char str[30], tmp[30];
    strcpy_P(tmp, fmt);
    va_start(ap, tmp);
    vsprintf(str, tmp, ap);
    FYD12864WrtData(0x03, 0x01);   //设定DDRAM的地址计数器到"00"
    FYD12864WrtData(0x0C, 0x01);   //整体显示
    FYD12864WrtStr(posx, posy, str, 1);
    va_end(ap);
}

/************************************************************************
**	函数名称:	FYD12864DispPrintfFlash(uint8_t posx, unsigned posy, char __farflash*fmt, ...)
**	函数功能:	显示函数，用法与printf函数用法一样，不同的是其显示的内容存储于flash中
**	输入参数: row：			行坐标，1<=posx<=4
			  col:		列坐标，1<=posy<=8
			  str：		指向flash的指针，指向flash中指定格式的数据
			  bClrleft：剩余未显示的位置是否清空标志，1:清空剩余位置；0：不清空剩余位置		
**	返回值  	: 无

**	创 建 者:	HHM
**	创建日期:	2016-04-25
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
**	函数名称:	FYD12864ClearLine(uint8_t line)
**	函数功能:	清除某行
**	输入参数: line：		行坐标，1<=line<=4
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-04-25
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
**	函数名称:	FYD12864NarrowUp(uint8_t px, uint8_t py)
**	函数功能:	显示上翻图形
**	输入参数: px：行坐标，1<=px<=4
			  py：列坐标，1<=py<=8
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-04-25
*************************************************************************/
void FYD12864NarrowUp(uint8_t px, uint8_t py)
{
    FYD12864Locate(px, py);
    FYD12864WrtData(0x1E, LCMDATA);
//  FYD12864WrtData(0x1E,LCMDATA);
}

/************************************************************************
**	函数名称:	FYD12864NarrowDown(uint8_t px, uint8_t py)
**	函数功能:	显示下翻图形
**	输入参数: px：行坐标，1<=px<=4
			  py：列坐标，1<=py<=8
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-04-25
*************************************************************************/
void FYD12864NarrowDown(uint8_t px, uint8_t py)
{
    FYD12864Locate(px, py);
    FYD12864WrtData(0x1F, LCMDATA);
//  FYD12864WrtData(0x1F,LCMDATA);
}

/************************************************************************
**	函数名称:	FYD12864SelectFlag(uint8_t px, uint8_t py)
**	函数功能:	选择图标
**	输入参数: px：行坐标，1<=px<=4
			  py：列坐标，1<=py<=8
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-04-25
*************************************************************************/
void FYD12864SelectFlag(uint8_t px, uint8_t py)
{
    FYD12864Locate(px, py);
    FYD12864WrtData(0x3C, LCMDATA);
}

/************************************************************************
**	函数名称:	FYD12864ClearSelectFlag(uint8_t px, uint8_t py)
**	函数功能:	清除选择图标
**	输入参数: px：行坐标，1<=px<=4
			  py：列坐标，1<=py<=8
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-04-25
*************************************************************************/
void FYD12864ClearSelectFlag(uint8_t px, uint8_t py)
//功能：清除选择图标
{
    FYD12864Locate(px, py);
    FYD12864WrtData(0x20, LCMDATA);
}

/************************************************************************
**	函数名称:	FYD12864Init(void)
**	函数功能:	FYD12864硬件初始化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2016-04-25
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
    FYD12864WrtData(0x03, LCMCMD);   //设定DDRAM的地址计数器到"00"
    FYD12864WrtData(0x0C, LCMCMD);   //整体显示
    FYD12864WrtData(0x06, LCMCMD);   //光标右移
    FYD12864WrtData(0x01, LCMCMD);   //清除显示
    __delay_cycles(73728);   //_delay_ms(10);
    FYD12864ClrScreen();
}

/************************************************************************
**	函数名称:	FYD12864ShowGDRAM(uint8_t* address)
**	函数功能:	清屏(address为NULL,将GDRAM填满0)或显示其他信息(显示address所指向的数据1024个字节
**	输入参数: address：NULL:清屏；数组:address是指向数组的指针,满屏显示
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2018-1-31
*************************************************************************/
void FYD12864ShowGDRAM(uint8_t* address)
{
    uint8_t i, j;
    FYD12864WrtData(0x36, 0x01);   //进入扩展指令，并打开图形显示

    for(i = 0; i < 32; i++)   //上半屏的32排依次先写满
    {
        FYD12864WrtData(0x80 + i, 1);   //先送垂直地址
        FYD12864WrtData(0x80, 1);   //再送水平地址，水平地址可自加1

        for(j = 0; j < 16; j++)   //每排128个点，所以一共要16个两位16进制数（也就是8位二进制数）才能全部控制
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

    for(i = 0; i < 32; i++)   //下半屏的32排操作原理和上半屏一样
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

    FYD12864WrtData(0x30, 0x01);   //进入基本指令
}

/************************************************************************
**	函数名称:	  Fyd12864CurrsorBlink(uint8_t row, uint8_t col, uint8_t flag)
**	函数功能:	光标闪烁控制
**	输入参数: row: 行坐标(注：针对FYD12864,范围为1<=row<=4)
			  col: 列坐标(注：针对FYD12864,范围为1<=col<=8))
			  flag:1:闪烁，0:不闪
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2018-2-1
*************************************************************************/
void Fyd12864CurrsorBlink(uint8_t row, uint8_t col, uint8_t flag)
{
    FYD12864Locate(row, col);

    if(flag)
    {
        FYD12864WrtData(0x0F, 1);    //0x0E不反白，0x0F反白
    }
    else
    {
        FYD12864WrtData(0x0C, 1);
    }
}

