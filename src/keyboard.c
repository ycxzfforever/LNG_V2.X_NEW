#include "kernel.h"

Screeninput ScreenInput;

/************************************************************************
**	函数名称:	KeyBoardInit(void)
**	函数功能:	按键板初始化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2018-1-31
*************************************************************************/
void KeyBoardInit(void)
{
    KEYBOARD_SH_OUT();
    KEYBOARD_CLK_OUT();
    KEYBOARD_DATA_IN();
    KEYBOARD_SH_H();
    KEYBOARD_CLK_H();
    KEYBOARD_DATA_L();
}

/************************************************************************
**	函数名称:	KeyBoardReadKey(void)
**	函数功能:	读取键值
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2018-1-31
*************************************************************************/
uint32_t KeyBoardReadKey(void)
{
    volatile  uint8_t i;
    volatile  uint8_t rdp = 0;
    volatile  uint32_t rdv = 0;
//  PORTE_PORTE3 = 0;
//  PORTE_PORTE2 = 0;
    KEYBOARD_SH_L();
    KEYBOARD_CLK_L();
    __delay_cycles(100);   //必须的延时，因为触发器输入输出延时受电源电压影响较大
//  PORTE_PORTE2 = 1;
//  PORTE_PORTE3 = 1;
    KEYBOARD_CLK_H();
    KEYBOARD_SH_H();
    __delay_cycles(100);   //必须的延时，因为触发器输入输出延时受电源电压影响较大

    for(i = 0; i < 24; i++)   ////读数据
    {
        rdp = PINE;
//      PORTE_PORTE2 = 0;
        KEYBOARD_CLK_L();
        __delay_cycles(100);   //必须的延时，因为触发器输入输出延时受电源电压影响较大
//      PORTE_PORTE2 = 1;
        KEYBOARD_CLK_H();
        __delay_cycles(100);   //必须的延时，因为触发器输入输出延时受电源电压影响较大

        rdv  = rdv << 1;

        if(rdp & 0x10) rdv |= 0x01;
    }

    rdv = rdv & 0x000FFFFF;

    return rdv;
}

/************************************************************************
**	函数名称:	KeyBoardReadKeyValue(void)
**	函数功能:	防抖，获取键值
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2018-1-31
*************************************************************************/
void KeyBoardReadKeyValue(void)
{
    uint32_t    rdkey, rdkey1;

    rdkey = KeyBoardReadKey();//读取键值

    if(globalvar.KeyPreValue == rdkey)   //如果该次键值与上次相同，表示按键未释放
        return;

    if(rdkey != 0x0000000)   //如果确实有键按下
    {
        _delay_ms(5);   //延时5ms
        rdkey1 = KeyBoardReadKey();//再次读取键值

        if(rdkey == rdkey1)   //如果两次的值相同，则表示确实有键按下
        {
            globalvar.KeyValue = rdkey; //给按键值赋值给全局键值
            BeepOut(1);
        }
    }

    globalvar.KeyPreValue = rdkey; //将当次按键赋值给前一次键值，为下次读取按键做准备
}

/************************************************************************
**	函数名称:	NumKeyHandler(void)
**	函数功能:	对按键值进行数字处理
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	2018-1-31
*************************************************************************/
void NumKeyHandler(void)
{
    if(ScreenInput.inputlen < ScreenInput.inputmaxlen)
    {
        switch(globalvar.KeyValue)
        {
            case KEY_0:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x30; // '0' BY LY    根据按键值，初始化键值存放数组，更新实际输入的长度
                break;

            case KEY_1:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x31; // '1' BY LY
                break;

            case KEY_2:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x32; // '2' BY LY
                break;

            case KEY_3:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x33; // '3' BY LY
                break;

            case KEY_4:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x34; // '4' BY LY
                break;

            case KEY_5:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x35; // '5' BY LY
                break;

            case KEY_6:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x36; // '6' BY LY
                break;

            case KEY_7:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x37; // '7' BY LY
                break;

            case KEY_8:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x38; // '8' BY LY
                break;

            case KEY_9:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x39; // '9' BY LY
                break;

            case KEY_PT:// 小数点 BY LY
                if(ScreenInput.pointnum == 0)
                {
                    ScreenInput.pointnum = 1;
                    globalvar.KeyValue = 0;
                    ScreenInput.array[ScreenInput.inputlen++] = 0x2E; // '.'  小数点的ASCII码值 BY LY
                }

                break;

            case KEY_CLR://如果是删除键 ADD BY LY
                globalvar.KeyValue = 0;

                if(ScreenInput.inputlen > 0)  ScreenInput.inputlen--;   //输入长度不计数 BY LY

                if(ScreenInput.array[ScreenInput.inputlen] == 0x2E) ScreenInput.pointnum = 0;   //如果按删除键前输入的是小数点，清除小数点位数标志 ADD BY LY

                ScreenInput.array[ScreenInput.inputlen] = 0x20;   //执行清除动作，将上一次输入的键值清空 ADD BY LY
                break;
        }
    }
    else //如果输出超出最大输入长度，当做输入的删除键 BY LY
    {
        switch(globalvar.KeyValue)
        {
//            case KEY_RET:
            case KEY_CLR:
                globalvar.KeyValue = 0;

                if(ScreenInput.inputlen > 0)  ScreenInput.inputlen--;

                if(ScreenInput.array[ScreenInput.inputlen] == 0x2E)   //小数点个数变为0 ADD BY LY
                    ScreenInput.pointnum = 0;

                ScreenInput.array[ScreenInput.inputlen] = 0x20;//小数点变空格 ADD BY LY
                break;
        }
    }
}

#if 0

//KeyBoardWait只有在与Psam卡有关的程序中才有调用，ADD BY LY
void KeyBoardWait(uint32_t keyvalue, uint16_t timeout_ms)
{
    Timer1ClcTick(0);

    while(1)
    {
        if(KeyBoardReadKey() == keyvalue)
            break;

        if(Timer1GetTick() >= timeout_ms)
        {
            Timer1Off();
            break;
        }
    }
}

#endif