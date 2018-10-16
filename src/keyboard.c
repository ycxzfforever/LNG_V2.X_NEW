#include "kernel.h"

Screeninput ScreenInput;

/************************************************************************
**	��������:	KeyBoardInit(void)
**	��������:	�������ʼ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2018-1-31
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
**	��������:	KeyBoardReadKey(void)
**	��������:	��ȡ��ֵ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2018-1-31
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
    __delay_cycles(100);   //�������ʱ����Ϊ���������������ʱ�ܵ�Դ��ѹӰ��ϴ�
//  PORTE_PORTE2 = 1;
//  PORTE_PORTE3 = 1;
    KEYBOARD_CLK_H();
    KEYBOARD_SH_H();
    __delay_cycles(100);   //�������ʱ����Ϊ���������������ʱ�ܵ�Դ��ѹӰ��ϴ�

    for(i = 0; i < 24; i++)   ////������
    {
        rdp = PINE;
//      PORTE_PORTE2 = 0;
        KEYBOARD_CLK_L();
        __delay_cycles(100);   //�������ʱ����Ϊ���������������ʱ�ܵ�Դ��ѹӰ��ϴ�
//      PORTE_PORTE2 = 1;
        KEYBOARD_CLK_H();
        __delay_cycles(100);   //�������ʱ����Ϊ���������������ʱ�ܵ�Դ��ѹӰ��ϴ�

        rdv  = rdv << 1;

        if(rdp & 0x10) rdv |= 0x01;
    }

    rdv = rdv & 0x000FFFFF;

    return rdv;
}

/************************************************************************
**	��������:	KeyBoardReadKeyValue(void)
**	��������:	��������ȡ��ֵ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2018-1-31
*************************************************************************/
void KeyBoardReadKeyValue(void)
{
    uint32_t    rdkey, rdkey1;

    rdkey = KeyBoardReadKey();//��ȡ��ֵ

    if(globalvar.KeyPreValue == rdkey)   //����ôμ�ֵ���ϴ���ͬ����ʾ����δ�ͷ�
        return;

    if(rdkey != 0x0000000)   //���ȷʵ�м�����
    {
        _delay_ms(5);   //��ʱ5ms
        rdkey1 = KeyBoardReadKey();//�ٴζ�ȡ��ֵ

        if(rdkey == rdkey1)   //������ε�ֵ��ͬ�����ʾȷʵ�м�����
        {
            globalvar.KeyValue = rdkey; //������ֵ��ֵ��ȫ�ּ�ֵ
            BeepOut(1);
        }
    }

    globalvar.KeyPreValue = rdkey; //�����ΰ�����ֵ��ǰһ�μ�ֵ��Ϊ�´ζ�ȡ������׼��
}

/************************************************************************
**	��������:	NumKeyHandler(void)
**	��������:	�԰���ֵ�������ִ���
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	2018-1-31
*************************************************************************/
void NumKeyHandler(void)
{
    if(ScreenInput.inputlen < ScreenInput.inputmaxlen)
    {
        switch(globalvar.KeyValue)
        {
            case KEY_0:
                globalvar.KeyValue = 0;
                ScreenInput.array[ScreenInput.inputlen++] = 0x30; // '0' BY LY    ���ݰ���ֵ����ʼ����ֵ������飬����ʵ������ĳ���
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

            case KEY_PT:// С���� BY LY
                if(ScreenInput.pointnum == 0)
                {
                    ScreenInput.pointnum = 1;
                    globalvar.KeyValue = 0;
                    ScreenInput.array[ScreenInput.inputlen++] = 0x2E; // '.'  С�����ASCII��ֵ BY LY
                }

                break;

            case KEY_CLR://�����ɾ���� ADD BY LY
                globalvar.KeyValue = 0;

                if(ScreenInput.inputlen > 0)  ScreenInput.inputlen--;   //���볤�Ȳ����� BY LY

                if(ScreenInput.array[ScreenInput.inputlen] == 0x2E) ScreenInput.pointnum = 0;   //�����ɾ����ǰ�������С���㣬���С����λ����־ ADD BY LY

                ScreenInput.array[ScreenInput.inputlen] = 0x20;   //ִ���������������һ������ļ�ֵ��� ADD BY LY
                break;
        }
    }
    else //����������������볤�ȣ����������ɾ���� BY LY
    {
        switch(globalvar.KeyValue)
        {
//            case KEY_RET:
            case KEY_CLR:
                globalvar.KeyValue = 0;

                if(ScreenInput.inputlen > 0)  ScreenInput.inputlen--;

                if(ScreenInput.array[ScreenInput.inputlen] == 0x2E)   //С���������Ϊ0 ADD BY LY
                    ScreenInput.pointnum = 0;

                ScreenInput.array[ScreenInput.inputlen] = 0x20;//С�����ո� ADD BY LY
                break;
        }
    }
}

#if 0

//KeyBoardWaitֻ������Psam���йصĳ����в��е��ã�ADD BY LY
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