#include "kernel.h"

/************************************************************************
**	�������ƣ�OsplcPortInit(void)
**	��������:	��ʼ�����Զ˿�
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcPortInit(void)
{
    OsplcLckOut();   //lck�����
    OsplcClkOut();   //clk�����
    OsplcDataOut();  //���������

    OsplcLck_H();    //lck���1��
    OsplcClk_H();    //clk���1��
    OsplcData_H();   //data���1��
}

/************************************************************************
**	�������ƣ�OsplcLatch(void)
**	��������:	����������������LED��ʾ��ʹ�ô˺����������ݵ�����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcLatch(void)
{
    OsplcLck_L();  //�½�����Ч��Ӳ����·�з���������ȡ����
    _delay_us(5);    //����ʱ��δ��׼���������ʱ����Ϊ���������������ʱ�ܵ�Դ��ѹӰ��ϴ�

    OsplcLck_H();  //�½�����Ч��Ӳ����·�з���������ȡ����
    _delay_us(5);    //����ʱ��δ��׼���������ʱ����Ϊ���������������ʱ�ܵ�Դ��ѹӰ��ϴ�

}

/************************************************************************
**	�������ƣ�OsplcEnclk(void)
**	��������:	����SPLC��һ��ʱ��ʹ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcEnclk(void)
{
    OsplcClk_H();   //����Ϊ�½�����Ч��
    _delay_us(5);     //����ʱ��δ��׼���������ʱ����Ϊ���������������ʱ�ܵ�Դ��ѹӰ��ϴ�

    OsplcClk_L();
    _delay_us(5);     //����ʱ��δ��׼���������ʱ����Ϊ���������������ʱ�ܵ�Դ��ѹӰ��ϴ�

}

/************************************************************************
**	�������ƣ�OsplcDot(uint8_t dcode)
**	��������:	������д��С����
**	�������: dcode С������� �� С����ȫ�Ա���
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcDot(uint8_t dcode)
{
    uint8_t i, temp;

    for(i = 0; i < 6; i++)
    {
        temp = (dcode << i) & 0x20;

        if(temp)
        {
            OsplcData_H();
        }
        else
        {
            OsplcData_L();
        }

        OsplcEnclk();
    }
}

/************************************************************************
**	�������ƣ�OsplcDispB(uint8_t *pdisp, bool nPoint)
**	��������:	������Һ��д��һ֡������
**	�������: pdisp :��д�����ݵĵ�ַ��������������ʾ22�����ݣ����ϵ��µ�һ��8�����ڶ���8����������6�� ADD BY LY��
	          nPoint: С������ʾ����  0x2A С�������   0x00 С����ȫ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcDispB(uint8_t *pdisp, bool nPoint)
{
    uint8_t SEGMAP[16] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x8C, 0x86, 0xFF, 0xC6, 0x00, 0xBF};
    uint8_t i, j, showdata;

    if(nPoint)
    {
        OsplcDot(0x2a);                       //С������롣
    }
    else
    {
        OsplcDot(0x00);                       //С����ȫ�ԡ�
    }

    for(i = 0; i < 22; i++)                    //����ʾ22��λ�ã�����С���㡣
    {
        showdata = SEGMAP[pdisp[i]];

        for(j = 0; j < 7; j++)
        {
            showdata = showdata << 1;

            if(showdata & 0x80)
            {
                OsplcData_H();
            }
            else
            {
                OsplcData_L();
            }

            OsplcEnclk();                    //ʱ��ʹ�ܡ�
        }
    }

    OsplcLatch();                             //�������档
}

/************************************************************************
**	�������ƣ�OsplcShowB(uint32_t Row1, uint32_t Row2, uint16_t Row3, bool bStop, bool bOverFlag)
**	��������:	������Һ��ĳ��д������
**	�������: Row1����һ��Ҫд������ݣ���ע��������ֱ����дҪ��ʾ���������� ADD BY LY��
              Row2���ڶ���Ҫд������ݣ���ע��������ֱ����дҪ��ʾ���������� ADD BY LY��
              Row3��������Ҫд������ݣ���ע��������ֱ����дҪ��ʾ���������� ADD BY LY��
              bStop�����ű�־��          bStop =1��ʾ����;������ʾ��ADD BY LY
              bOverFlag����һ�У��ڶ��е�һ���ַ���ʾ��־��bOverFlag=1,��һ����ʾ C���ڶ�����ʾP;������ʾ ADD BY LY
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28

**  ˵���� ÿ��������ʾʱ���Ǵ��ҵ���������ʾ������Row1 Ϊ12���������ʾһ��һ�������ƣ���Ϊ:1.23 ADD BY LY
*************************************************************************/
void OsplcShowB(uint32_t Row1, uint32_t Row2, uint16_t Row3, bool bStop, bool bOverFlag)
{
    char buf[12] = {0};
    uint8_t dispbuf[22] = {0};       //����Һ������ʾ22�����֣���������Ϊ����У���������ʼ�������������ϵ�����
    uint8_t m = 0, n = 0, len = 0;
    //uint32_t row;
    memset(dispbuf, 12, sizeof(dispbuf));

    dispbuf[5] = sysparas.gunnum / 10;                      //��ʾ���š�
    dispbuf[4] = sysparas.gunnum % 10;

    if(bOverFlag)
    {
        dispbuf[13]  = 10;	               //�ڶ��е�һλ��ʾP��
        dispbuf[21] =  13;	               //��һ�е�һλ��ʾC��
    }

    /************************************************************************
    ** ��һ�С�
    *************************************************************************/
    memset(buf, 13, sizeof(buf));
    //row=(uint32_t)(Row1*100);
    len = sprintf_P(buf, "%03ld", Row1);       //����һ������д�����飬���õ����ȡ�

    len = (7 > len) ? len : 7;
    m = len;

    for(n = 0; n < len; n++)
    {
        dispbuf[13 + m] = buf[n] - 0x30;	 //��Ҫ��ʾ�����ݴ������顣
        m--;
    }

    if(bStop)                              //��һ�и�����־������Ǹ����Ļ�������ʾ������
    {
        dispbuf[len + 13] = 15;
    }

    /************************************************************************
    ** �ڶ��С�
    *************************************************************************/
    memset(buf, 0, sizeof(buf));
    //row=(uint32_t)(Row2*100);
    len = sprintf_P(buf, "%03ld", Row2);      //����һ������д�����飬���õ����ȡ�
    len = (7 > len) ? len : 7;
    m = len;

    for(n = 0; n < len; n++)
    {
        dispbuf[5 + m] = buf[n] - 0x30;	   //��Ҫ��ʾ�����ݴ������顣
        m--;
    }

    if(bStop)
    {
        dispbuf[len + 5] = 15;            //�ڶ��и�����־������Ǹ����Ļ�������ʾ������
    }

    /************************************************************************
    ** �����С�
    *************************************************************************/
    memset(buf, 0, sizeof(buf));
    //row=(uint32_t)(Row3*100);
    len = sprintf_P(buf, "%03d", Row3);        //����һ������д�����飬���õ����ȡ�
    len = (4 > len) ? len : 4;
    m = len;

    for(n = 0; n < len; n++)
    {
        m--;
        dispbuf[m] = buf[n] - 0x30;	    //��Ҫ��ʾ�����ݴ������顣
    }

    OsplcDispB(dispbuf, true);
}

/************************************************************************
**	�������ƣ�OsplcHardwarechk(void)
**	��������:	���Һ����������������sysparas.lcdtype=0�������ʹ�����sysparas.lcdtype=�������������Ƿ�������������
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcHardwarechk(void)
{
    if(sysparas.lcdtype == 0)   //����������
    {
        uint8_t disp[22] = {0};
        uint8_t  m = 0;

        for(m = 0; m <= 9; m++)
        {
            memset(&disp[0], m, 22);
            OsplcDispB(disp, true);   //��ʾ��
            _delay_ms(200);
        }

        OsplcShowB(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price, 0, 1);
    }
    else //���Դ�����
    {
        OsplcDispInit();
        OsplcShowN(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price);
    }
}

/************************************************************************
**	�������ƣ�OsplcInit(void)
**	��������:	��ʼ������Һ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcInit(void)
{
    OsplcPortInit();
    OsplcHardwarechk();
}

/************************************************************************
**	�������ƣ�OsplcDispN(void)
**	��������:	�����Һ��д��һ֡������
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcDispN(void)
{
    uint8_t m, n, showdata;

    for(m = 0; m < 40; m++)
    {
        showdata = DispMem[m];

        for(n = 0; n < 8; n++)
        {
            if(showdata & 0x80)
            {
                OsplcData_H();
            }
            else
            {
                OsplcData_L();
            }

            OsplcEnclk();
            showdata = showdata << 1;
        }
    }

    OsplcLatch();
}

/************************************************************************
**	�������ƣ�OsplcDispInit(void)
**	��������: ��ʼ������Һ����ʼ������Һ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcDispInit(void)
{
    uint32_t temp1[10] = {00000000, 11111111, 22222222, 33333333, 44444444, 55555555, 66666666, 77777777, 88888888, 99999999};
    uint32_t temp2[10] = {0000, 1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999};
    uint32_t temp3[10] = {000, 111, 222, 333, 444, 555, 666, 777, 888, 999};
    uint8_t temp4[10] = {00, 11, 22, 33, 44, 55, 66, 77, 88, 99};
    float temp5[10] = {00.0, 11.1, 22.2, 33.3, 44.4, 55.5, 66.6, 77.7, 88.8, 99.9};
    uint8_t i;

    memset(DispMem, 0xFF, sizeof(DispMem));
    OsplcTitleOn();

    for(i = 0; i < 10; i++)
    {
        OsplcPrice(temp1[i]);
        OsplcNum(temp1[i]);
        OsplcSum(temp1[i]);
        OsplcFlRate(temp2[i]);
        OsplcFlrateBack(temp2[i]);
        OsplcDrvgain(temp3[i]);
        OsplcPressure(temp3[i]);
        OsplcTemperature(-temp5[i]);
        OsplcWarnning(temp4[i]);
        OsplcGunNum(temp4[i]);

        if(i == 0)
            memset(DispMem, 0x0, sizeof(DispMem));

        OsplcDispN();
        _delay_ms(200);
    }
}

/************************************************************************
**	�������ƣ�OsplcTitleOn(void)
**	��������: ����Һ���ϵ����ƣ���λ��λ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcTitleOn(void)
{
    OsplcKGON();       //����
    OsplcSUMON();      //���
    OsplcPRICEON();    //����
    OsplcKGMINON();    //����KG/MIN
    OsplcWARNINGON();  //����
    OsplcGNUMON();     //ǹ��
    OsplcFLNUMON();    //��������S9
    OsplcSTATEON();    //��������S10
    OsplcSTATEUPAYON();//����KG/MIN
    OsplcSTATEIDLEON();//����
    OsplcTEMPON();     //�¶�
    OsplcTEMPUNITON();
    OsplcPRESSUREON(); //ѹ��
    OsplcLOGOON();     //LOGO

}

uint16_t OsplcNumArr[8][12] =
{
    {0x2120, 0xF901, 0x46FE, 0xF6FF, 0x27FE, 0xA6FE, 0x96FF, 0x8EFE, 0x0eFE, 0xE6FF, 0x06FE, 0x86FE},
    {0x20FF, 0xFE00, 0x21FF, 0xF9FF, 0x13FF, 0x51FF, 0xC9FF, 0x45FF, 0x05FF, 0xF1FF, 0x01FF, 0x41FF},
    {0x1F1E, 0x9E03, 0x61FE, 0xF9FF, 0x73FC, 0x71FD, 0xE9FD, 0x65FD, 0x65FC, 0xF1FF, 0x61FC, 0x61FD},
    {0x1F1E, 0x20FC, 0xDF23, 0xDFFB, 0xFF13, 0xDF53, 0xDFCB, 0xDF47, 0xDF07, 0xDFF3, 0xDF03, 0xDF43},
    {0x04FF, 0x7F00, 0x90FF, 0xFCFF, 0x89FF, 0xA8FF, 0xE4FF, 0xA2FF, 0x82FF, 0xF8FF, 0x80FF, 0xA0FF},
    {0x0403, 0x803F, 0x7FC8, 0x7FFB, 0xFFC1, 0x7FD1, 0x7FF2, 0x7FD4, 0x7FC4, 0x7FF9, 0x7FC0, 0x7FD0},
    {0x0302, 0xC01F, 0x3FE4, 0x3FFF, 0x7FE2, 0x3FEA, 0x3FF9, 0xBFE8, 0xBFE0, 0x3FFE, 0x3FE0, 0x3FE8},
    {0x0201, 0xE00F, 0x1FF2, 0x9FFF, 0x3FF1, 0x1FF5, 0x9FFC, 0x5FF4, 0x5FF0, 0x1FFF, 0x1FF0, 0x1FF4},
};

/************************************************************************
**	�������ƣ�OsplcNumFunc(uint8_t id, uint8_t num)
**	��������: 
**	�������: id:
			  num:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcNumFunc(uint8_t id, uint8_t num)
{
    uint8_t hB, lB;
    hB = OsplcNumArr[id][0] / 0x100;
    lB = OsplcNumArr[id][0] % 0x100;

    DispMem[hB] |= OsplcNumArr[id][1] / 0x100;

    if(num <= 9)
    {
        DispMem[hB] &= OsplcNumArr[id][num + 2] / 0x100;
    }

    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcNumArr[id][1] % 0x100;

        if(num <= 9)
        {
            DispMem[lB] &= OsplcNumArr[id][num + 2] % 0x100;
        }
    }
}

/************************************************************************
**	�������ƣ�OsplcNum(uint32_t vnum)
**	��������: ����Һ��������ʾ
**	�������: vnum:
**	����ֵ  	: true: ;false:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
bool OsplcNum(uint32_t vnum)
{
    uint8_t num, n, m, len = 0;
    char buf[33];
    memset(buf, 0, sizeof(buf));

    if(vnum < 100)
    {
        OsplcNumFunc(7, 10);
        OsplcNumFunc(6, 10);
        OsplcNumFunc(5, 10);
        OsplcNumFunc(4, 10);
        OsplcNumFunc(3, 10);
        OsplcNumFunc(2, 0);
        OsplcNumFunc(1, vnum / 10);
        OsplcNumFunc(0, vnum % 10);
    }
    else
    {
        len = sprintf_P(buf, "%ld", vnum);
        len = (9 > len) ? len : 8;

        if(len < 3)
        {
            return false;
        }

        m = len - 1;

        for(n = 0; n < len; n++)
        {
            num = buf[n] - 0x30;
            OsplcNumFunc(m, num);
            m--;
        }
    }

    DispMem[31] &= 0xFE;
    return true;
}

uint16_t OsplcSumArr[8][12] =
{
    {0x1B1A, 0xF007, 0x0FF9, 0xCFFF, 0x9FF8, 0x8FFA, 0x4FFE, 0x2FFA, 0x2FF8, 0x8FFF, 0x0FF8, 0x0FFA}, //NUM16
    {0x1A19, 0xF803, 0x87FC, 0xE7FF, 0x0FFD, 0x47FD, 0x67FE, 0x57FC, 0x17FC, 0xE7FD, 0x07FC, 0x47FC}, //NUM15
    {0x2219, 0x03F8, 0xFC87, 0xFFE7, 0xFC4F, 0xFD47, 0xFF27, 0xFD17, 0xFC17, 0xFFC7, 0xFC07, 0xFD07}, //NUM14
    {0x2221, 0xF806, 0x87F9, 0xE7FF, 0x4FF9, 0x47FD, 0x27FF, 0x17FD, 0x17F9, 0xC7FF, 0x07F9, 0x07FD}, //NUM13
    {0x0100, 0xF007, 0x4FF8, 0xEFFD, 0x9FF8, 0xAFF8, 0x2FFD, 0x2FFA, 0x0FFA, 0xEFFC, 0x0FF8, 0x2FF8}, //NUM12
    {0x1800, 0x03F8, 0xFC87, 0xFFE7, 0xFC4F, 0xFD47, 0xFF27, 0xFD17, 0xFC17, 0xFFC7, 0xFC07, 0xFD07}, //NUM11
    {0x1817, 0xFC04, 0x43FB, 0xF3FF, 0x27FB, 0xA3FB, 0x93FF, 0x8BFB, 0x0BFB, 0xE3FF, 0x03FB, 0x83FB}, //NUM10
    {0x17FF, 0xFBFF, 0x24FF, 0xFCFF, 0x16FF, 0x54FF, 0xCCFF, 0x45FF, 0x05FF, 0xF4FF, 0x04FF, 0x44FF}, //NUM9
};

/************************************************************************
**	�������ƣ�OsplcSumFunc(uint8_t id, uint8_t sum)
**	��������: 
**	�������: id:
			  num:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcSumFunc(uint8_t id, uint8_t sum)
{
    uint8_t hB, lB;

    hB = OsplcSumArr[id][0] / 0x100;
    lB = OsplcSumArr[id][0] % 0x100;

    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcSumArr[id][1] % 0x100;

        if(sum <= 9)
            DispMem[lB] &= OsplcSumArr[id][sum + 2] % 0x100;
    }

    DispMem[hB] |= OsplcSumArr[id][1] / 0x100;

    if(sum <= 9)
        DispMem[hB] &= OsplcSumArr[id][sum + 2] / 0x100;
}

/************************************************************************
**	�������ƣ�OsplcSum(uint32_t fvsum)
**	��������: 
**	�������: fvsum:
**	����ֵ  	: true: ;false:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
bool OsplcSum(uint32_t fvsum)
{
    uint8_t sum, n, m, len = 0;
    char buf[33];
    memset(buf, 0, sizeof(buf));

    if(fvsum < 100)
    {
        OsplcSumFunc(7, 10);
        OsplcSumFunc(6, 10);
        OsplcSumFunc(5, 10);
        OsplcSumFunc(4, 10);
        OsplcSumFunc(3, 10);
        OsplcSumFunc(2, 0);
        OsplcSumFunc(1, fvsum / 10);
        OsplcSumFunc(0, fvsum % 10);
    }
    else
    {
        len = sprintf_P(buf, "%ld", fvsum);
        len = (9 > len) ? len : 8;

        if(len < 3)
        {
            return false;
        }

        m = len - 1;

        for(n = 0; n < len; n++)
        {
            sum = buf[n] - 0x30;
            OsplcSumFunc(m, sum);
            m--;
        }
    }

    DispMem[25] &= 0xFB;
    return true;
}

uint16_t DispPriceArr[5][12] =
{
    {0x1D1C, 0xF806, 0x87F9, 0xE7FF, 0x4FF9, 0x47FD, 0x27FF, 0x17FD, 0x17F9, 0xC7FF, 0x07F9, 0x07FD}, //����NUM21
    {0x1C1B, 0xF901, 0x46FE, 0xF6FF, 0x27FE, 0xA6FE, 0x96FF, 0x8EFE, 0x0EFE, 0xE6FF, 0x06FE, 0x86FE}, //����NUM20
    {0x0B1B, 0x0F0E, 0xF2F1, 0xFFF9, 0xF1F3, 0xF5F1, 0xFCF9, 0xF4F5, 0xF0F5, 0xFFF1, 0xF0F1, 0xF4F1}, //����NUM19
    {0x0CFF, 0xFEFF, 0x09FF, 0x3FFF, 0x45FF, 0x15FF, 0x33FF, 0x91FF, 0x81FF, 0x3DFF, 0x01FF, 0x11FF}, //����NUM18
    {0x0D0C, 0xF901, 0x16FE, 0x7FFE, 0x8EFE, 0x2EFE, 0x67FE, 0x26FF, 0x06FF, 0x7EFE, 0x06FE, 0x26FE}, //����NUM17
};

/************************************************************************
**	�������ƣ�OsplcPriceFunc(uint8_t id, uint8_t pnum)
**	��������: ����Һ�������ʾ
**	�������: id:
			  num:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcPriceFunc(uint8_t id, uint8_t pnum)
{
    uint8_t hB, lB;

    hB = DispPriceArr[id][0] / 0x100;
    lB = DispPriceArr[id][0] % 0x100;

    DispMem[hB] |= DispPriceArr[id][1] / 0x100;

    if(pnum <= 9)
    {
        DispMem[hB] &= DispPriceArr[id][pnum + 2] / 0x100;
    }

    if(lB != 0xFF)
    {
        DispMem[lB] |= DispPriceArr[id][1] % 0x100;

        if(pnum <= 9)
        {
            DispMem[lB] &= DispPriceArr[id][pnum + 2] % 0x100;
        }
    }
}

/************************************************************************
**	�������ƣ�OsplcPrice(uint32_t vprice)
**	��������: ����Һ���۸���ʾ
**	�������: vprice:
**	����ֵ  	: true: ;false:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
bool OsplcPrice(uint32_t vprice)
{
    uint8_t sum, n, m, len = 0;
    char buf[33];
    memset(buf, 0, sizeof(buf));

    if(vprice < 100)
    {
        OsplcPriceFunc(4, 10);
        OsplcPriceFunc(3, 10);
        OsplcPriceFunc(2, 0);
        OsplcPriceFunc(1, vprice / 10);
        OsplcPriceFunc(0, vprice % 10);
    }
    else
    {
        len = sprintf_P(buf, "%ld", vprice);
        len = (6 > len) ? len : 5;

        if(len < 3)
        {
            return false;
        }

        m = len - 1;

        for(n = 0; n < len; n++)
        {
            sum = buf[n] - 0x30;
            OsplcPriceFunc(m, sum);
            m--;
        }
    }

    DispMem[11] &= 0xEF;
    return true;
}

/************************************************************************
**	�������ƣ�OsplcGunNum(uint8_t GunNum)
**	��������: ����Һ��ǹ����ʾ
**	�������: GunNum:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcGunNum(uint8_t GunNum)   //NUM24 NUM25
{
    uint8_t OsplchGarr[10] =  {0x09, 0x3F, 0x45, 0x15, 0x33, 0x91, 0x81, 0x3D, 0x01, 0x11};
    uint8_t OsplclGarr[10] =  {0x81, 0x9F, 0xA2, 0x8A, 0x9C, 0xC8, 0xC0, 0x9B, 0x80, 0x88};

    if(GunNum > 99)
    {
        return;
    }

    DispMem[19] |= 0xFE; // DispMem[19] &= OsplchGarr[GunNum%10];

    if(GunNum >= 10)
    {
        DispMem[19] &= OsplchGarr[GunNum / 10];
    }
    else
    {
        DispMem[19] &= OsplchGarr[0];
    }

    DispMem[18] |= 0x7F;
    DispMem[18] &= OsplclGarr[GunNum % 10];
}

uint16_t OsplcFrateArr[4][12] =
{
    {0x0EFF, 0x7FFF, 0x84FF, 0x9FFF, 0xA2FF, 0x8AFF, 0x99FF, 0xC8FF, 0xC0FF, 0x9EFF, 0x80FF, 0x88FF}, //??NUM29
    {0x100F, 0x20FC, 0xDF0B, 0xFF3F, 0xDF47, 0xDF17, 0xFF33, 0xDF93, 0xDF83, 0xDF3F, 0xDF03, 0xDF13}, //??NUM28
    {0x100F, 0x9E03, 0x69FC, 0x7FFE, 0x65FD, 0x75FC, 0x73FE, 0xF1FC, 0xE1FC, 0x7DFE, 0x61FC, 0x71FC}, //??NUM27
    {0x1110, 0xFC01, 0x13FE, 0x7FFE, 0x8BFE, 0x2BFE, 0x67FE, 0x23FF, 0x03FF, 0x7BFE, 0x03FE, 0x23FE}, //??NUM26
};

/************************************************************************
**	�������ƣ�OsplcFrateFunc(uint8_t id, uint8_t frnum)
**	��������: 
**	�������: id:
			  frnum:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcFrateFunc(uint8_t id, uint8_t frnum)
{
    uint8_t hB, lB;

    hB = OsplcFrateArr[id][0] / 0x100;
    lB = OsplcFrateArr[id][0] % 0x100;

    DispMem[hB] |= OsplcFrateArr[id][1] / 0x100;

    if(frnum <= 9)
    {
        DispMem[hB] &= OsplcFrateArr[id][frnum + 2] / 0x100;
    }

    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcFrateArr[id][1] % 0x100;

        if(frnum <= 9)
            DispMem[lB] &= OsplcFrateArr[id][frnum + 2] % 0x100;
    }

}

/************************************************************************
**	�������ƣ�OsplcFlRate(uint32_t nfrNum)
**	��������: ����Һ������������ʾ
**	�������: nfrNum:
**	����ֵ  	: true: ;false:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
bool OsplcFlRate(uint32_t nfrNum)
{
    uint8_t sum, n, m, len = 0;
    char buf[33];
    memset(buf, 0, sizeof(buf));

    if(nfrNum < 100)
    {

        OsplcFrateFunc(1, nfrNum / 10);
        OsplcFrateFunc(0, nfrNum % 10);
    }
    else
    {
        len = sprintf_P(buf, "%d", nfrNum);
        len = (5 > len) ? len : 4;

        if(len < 3)
        {
            return false;
        }

        m = len - 1;

        for(n = 0; n < len; n++)
        {
            sum = buf[n] - 0x30;
            OsplcFrateFunc(m, sum);
            m--;
        }
    }

    DispMem[16] &= 0xBF;
    return true;
}

/************************************************************************
**	�������ƣ�OsplcPressure(uint32_t npNum)
**	��������: ����Һ��ѹ����ʾ
**	�������: npNum:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcPressure(uint32_t npNum)
{
    uint32_t vpNum, DisphhWarr[10] = {/*0x0807, 0xC01F*/0x3FE1, 0xFFE7, 0xBFE8, 0xBFE2, 0x7FE6, 0x3FF2, 0x3FF0, 0xBFE7, 0x3FE0, 0x3FE2};
    uint32_t DisphWarr[10] = {0xE4FC, 0xFFFC, 0xE2FE, 0xEAFC, 0xF9FC, 0xE8FD, 0xE0FD, 0xFEFC, 0xE0FC, 0xE8FC};
    uint32_t DisplWarr[10] = {0x9F87, 0xFF9F, 0xBFA3, 0xBF8B, 0xDF9B, 0x9FCB, 0x9FC3, 0xBF9F, 0x9F83, 0x9F8B};

    if(npNum > 999)
    {
        return;
    }

    vpNum = npNum / 10;

    if(vpNum >= 10)
    {
        DispMem[8] |= 0xC0;
        DispMem[7] |= 0x1F;
        DispMem[8] &= DisphhWarr[vpNum / 10] / 0x100;
        DispMem[7] &= DisphhWarr[vpNum / 10] % 0x100;
    }
    else
    {
        DispMem[8] |= 0xC0;
        DispMem[7] |= 0x1F;

    }

    DispMem[6] |= 0x1F; //0x0605,0x1F03,
    DispMem[5] |= 0x03;
    DispMem[6] &= DisphWarr[vpNum % 10] / 0x100;
    DispMem[5] &= DisphWarr[vpNum % 10] % 0x100;

    DispMem[6] |= 0x60;  //0x0605,0x607C,
    DispMem[5] |= 0x7C;
    DispMem[6] &= (DisplWarr[npNum % 10] / 0x100) & 0x7F;
    DispMem[5] &= DisplWarr[npNum % 10] % 0x100;
}

uint16_t OsplcTemperatureArr[3][12] =
{
    {0x0908, 0x803F, 0x7FC2, 0xFFCF, 0x7FD4, 0x7FC5, 0xFFC9, 0x7FE1, 0x7FE0, 0x7FCF, 0x7FC0, 0x7FC1}, //NUM32
    {0x09FF, 0x7FFF, 0x84FF, 0x9FFF, 0xA2FF, 0x8AFF, 0x99FF, 0xC8FF, 0xC0FF, 0x9EFF, 0x80FF, 0x88FF}, //31
    {0x0B0A, 0x607C, 0x9F87, 0xFF9F, 0xBFA3, 0xBF8B, 0xDF9B, 0x9FCB, 0x9FC3, 0xBF9F, 0x9F83, 0x9F8B}, //NUM30
};

/************************************************************************
**	�������ƣ�OsplcTemperatureFunc(uint8_t id, uint8_t tnum)
**	��������: 
**	�������: id:
			  frnum:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcTemperatureFunc(uint8_t id, uint8_t tnum)
{
    uint8_t hB, lB;
    hB = OsplcTemperatureArr[id][0] / 0x100;
    lB = OsplcTemperatureArr[id][0] % 0x100;

    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcTemperatureArr[id][1] % 0x100;

        if(tnum <= 9)
        {
            DispMem[lB] &= OsplcTemperatureArr[id][tnum + 2] % 0x100;
        }
    }

    DispMem[hB] |= OsplcTemperatureArr[id][1] / 0x100;

    if(tnum <= 9)
    {
        DispMem[hB] &= OsplcTemperatureArr[id][tnum + 2] / 0x100;
    }
}

/************************************************************************
**	�������ƣ�OsplcTemperatureEx(int nTnum) 
**	��������: 
**	�������: nTnum:
**	����ֵ  	: true: ;false:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
bool OsplcTemperatureEx(int nTnum)   //NUM30/1/2/3  �¶�
{
    uint8_t num, n, m, len = 0; //С����  DispMem[10] |= 0x80
    char buf[33];
    memset(buf, 0, sizeof(buf));
    OsplcTemperatureFunc(3, 10);
    OsplcTemperatureFunc(2, 10);
    OsplcTemperatureFunc(1, 0);
    OsplcTemperatureFunc(0, 0);
    len = sprintf_P(buf, "%d", nTnum);
    len = (4 > len) ? len : 3;
    m = len - 1;

    for(n = 0; n < len; n++)
    {
        num = buf[n] - 0x30;
        OsplcTemperatureFunc(m, num);
        m--;
    }

    return true;
}

/************************************************************************
**	�������ƣ�OsplcTemperature(float vfTemp)
**	��������: ����Һ���¶���ʾ
**	�������: nTnum:
**	����ֵ  	: true: ;false:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
bool OsplcTemperature(float vfTemp)
{
    uint32_t npNum;
    OsplcTEMPNOFF();

    if(vfTemp < 0)   //��ʾ���š�
    {
        OsplcTEMPNON();
    }

    npNum = (uint32_t)(fabs(vfTemp) * 10);
    OsplcTemperatureEx(npNum);
    DispMem[10] |= 0x80;

    if(npNum < 1000)   //��ʾС���㡣
    {
        DispMem[10] &= (~0x80);
    }

    return true;
}

/************************************************************************
**	�������ƣ�OsplcWarnning(uint8_t WNO)
**	��������: ����Һ���쳣ͣ��������ʾ
**	�������: WNO:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcWarnning(uint8_t WNO)   //NUM22 NUM23
{
    uint32_t OsplchWarr[10] =  {0x07FE, 0x9FFF, 0x57FC, 0x17FD, 0x8FFD, 0x27FD, 0x27FC, 0x97FF, 0x07FC, 0x07FD};
    uint32_t OsplclWarr[10] =  {0x13FE, 0x7FFE, 0x8BFE, 0x2BFE, 0x67FE, 0x23FF, 0x03FF, 0x7BFE, 0x03FE, 0x23FE};

    if(WNO > 99) return;

    if(WNO >= 10)
    {
        DispMem[21] |= 0xF8;
        DispMem[20] |= 0x03;
        DispMem[21] &= OsplchWarr[WNO / 10] / 0x100;
        DispMem[20] &= OsplchWarr[WNO / 10] % 0x100;
    }
    else
    {
        DispMem[21] |= 0xF8;
        DispMem[20] |= 0x03;
        DispMem[21] &= OsplchWarr[0] / 0x100;
        DispMem[20] &= OsplchWarr[0] % 0x100;
    }

    DispMem[20] |= 0xFC;
    DispMem[19] |= 0x01;
    DispMem[20] &= OsplclWarr[WNO % 10] / 0x100;
    DispMem[19] &= OsplclWarr[WNO % 10] % 0x100;
}

/************************************************************************
**	�������ƣ�OsplcDrvgainFunc(uint8_t id, uint8_t drvgainum)
**	��������: 				  
**	�������: id:
			  drvgainum:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcDrvgainFunc(uint8_t id, uint8_t drvgainum)
{
    uint8_t hB, lB;
    uint32_t OsplcDrvgainArr[3][12] =  //??
    {
        {0x1615, 0xFA01, 0x25FE, 0xFDFE, 0x15FF, 0x55FE, 0xCDFE, 0x47FE, 0x07FE, 0xF5FE, 0x05FE, 0x45FE}, //NUM42
        {0x2316, 0xF805, 0x27FA, 0xFFFA, 0x17FB, 0x57FA, 0xCFFA, 0x47FE, 0x07FE, 0xF7FA, 0x07FA, 0x47FA}, //NUM41
        {0x2423, 0xF007, 0x4FF8, 0xFFF9, 0x2FFA, 0xAFF8, 0x9FF9, 0x8FFC, 0x0FFC, 0xEFF9, 0x0FF8, 0x8FF8}, //NUM40
    };
    hB = OsplcDrvgainArr[id][0] / 0x100;
    lB = OsplcDrvgainArr[id][0] % 0x100;

    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcDrvgainArr[id][1] % 0x100;

        if(drvgainum <= 9)
        {
            DispMem[lB] &= OsplcDrvgainArr[id][drvgainum + 2] % 0x100;
        }

    }

    DispMem[hB] |= OsplcDrvgainArr[id][1] / 0x100;

    if(drvgainum <= 9)
    {
        DispMem[hB] &= OsplcDrvgainArr[id][drvgainum + 2] / 0x100;
    }
}

/************************************************************************
**	�������ƣ�OsplcDrvgain(int nTnum)
**	��������: 				  
**	�������: nTnum:
**	����ֵ  	: true: ;false:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
bool OsplcDrvgain(int nTnum)
{
    uint8_t num, n, m, len = 0; //С����  DispMem[10] |= 0x80
    char buf[33];
    memset(buf, 0, sizeof(buf));

    OsplcDrvgainFunc(2, 10);
    OsplcDrvgainFunc(1, 0);
    OsplcDrvgainFunc(0, 10);
    len = sprintf_P(buf, "%d", nTnum);
    len = (4 > len) ? len : 3;
    m = len - 1;

    for(n = 0; n < len; n++)
    {
        num = buf[n] - 0x30;
        OsplcDrvgainFunc(m, num);
        m--;
    }

    DispMem[5] &= 0x7F;
    return true;
}

/************************************************************************
**	�������ƣ�OsplcFrateBack(uint8_t id, uint8_t ftnum)
**	��������: 				  
**	�������: id:
			  ftnum:
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcFrateBack(uint8_t id, uint8_t ftnum)
{
    uint8_t hB, lB;
    uint32_t OsplcFrateBack[4][12] =  //�������١�
    {
        {0x2524, 0xE00F, 0x9FF0, 0xFFF3, 0x5FF4, 0x5FF1, 0x3FF3, 0x1FF9, 0x1FF8, 0xDFF3, 0x1FF0, 0x1FF1}, //NUM39
        {0x2625, 0xC01F, 0x3FE1, 0xFFE7, 0xBFE8, 0xBFE2, 0x7FE6, 0x3FF2, 0x3FF0, 0xBFE7, 0x3FE0, 0x3FE2}, //NUM38
        {0x2726, 0x803F, 0x7FC2, 0xFFCF, 0x7FD1, 0x7FC5, 0xFFCC, 0x7FE4, 0x7FE0, 0x7FCF, 0x7FC0, 0x7FC4}, //NUM37
        {0x27FF, 0x7FFF, 0x84FF, 0x9FFF, 0xA2FF, 0x8AFF, 0x99FF, 0xC8FF, 0xC0FF, 0x9EFF, 0x80FF, 0x88FF}, //NUM36
    };
    hB = OsplcFrateBack[id][0] / 0x100;
    lB = OsplcFrateBack[id][0] % 0x100;

    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcFrateBack[id][1] % 0x100;

        if(ftnum <= 9)
        {
            DispMem[lB] &= OsplcFrateBack[id][ftnum + 2] % 0x100;
        }

    }

    DispMem[hB] |= OsplcFrateBack[id][1] / 0x100;

    if(ftnum <= 9)
    {
        DispMem[hB] &= OsplcFrateBack[id][ftnum + 2] / 0x100;
    }
}

/************************************************************************
**	�������ƣ�OsplcFlrateBack(uint32_t nfrNum)
**	��������: 				  
**	�������: nfrNum:
**	����ֵ  	: true: ;false:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
bool OsplcFlrateBack(uint32_t nfrNum)
{
    uint8_t sum, n, m, len = 0;
    char buf[33];
    memset(buf, 0, sizeof(buf));

    if(nfrNum < 100)
    {
        OsplcFrateBack(3, 10);
        OsplcFrateBack(2, 0);
        OsplcFrateBack(1, nfrNum / 10);
        OsplcFrateBack(0, nfrNum % 10);
    }
    else
    {
        len = sprintf_P(buf, "%d", nfrNum);
        len = (5 > len) ? len : 4;

        if(len < 3) return false;

        m = len - 1;

        for(n = 0; n < len; n++)
        {
            sum = buf[n] - 0x30;
            OsplcFrateBack(m, sum);
            m--;
        }
    }

    DispMem[13] &= 0xFD;//P11
    return true;
}

/************************************************************************
**	�������ƣ�GetOnePosition(uint32_t num)
**	��������: ��ȡһ��uint32_t���͵����ֵĵ�һ��1���ֵ�λ��				  
**	�������: num:
**	����ֵ  	: 0: û��1;��һ��1���ֵ�λ��:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
uint8_t GetOnePosition(uint32_t num)
{
    uint8_t i = 0;

    for(i = 1; i < 33; i++)
    {
        if((num >> (i - 1)) & (0x01) == 1) 
			return i;
    }

    return 0;
}

/************************************************************************
**	�������ƣ�OsplcShowN(uint32_t Row1, uint32_t Row2, uint16_t Row3)
**	��������: �����д��Ҫ��ʾ����������				  
**	�������: Row1:
			  Row2:
			  Row3:
**	����ֵ  	: ��:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcShowN(uint32_t Row1, uint32_t Row2, uint16_t Row3)
{
    uint8_t tmperror;
    tmperror = GetOnePosition(syserr);
    memset(DispMem, 0xFF, sizeof(DispMem));
    OsplcTitleOn();         //������
    OsplcNum(Row1);         //������
    OsplcSum(Row2);         //��
    OsplcPrice(Row3);       //���ۡ�
    OsplcGunNum(sysparas.gunnum);      //���ţ��˴�������Ҫ�޸�ֵ��
    OsplcFlRate((uint32_t)(realtime.cur_flowrate_l / 6));       //�������٣��˴�������Ҫ�޸�ֵ��
    OsplcPressure(realtime.cur_pressure / 10);   //ѹ�����˴�������Ҫ�޸�ֵ��
    OsplcTemperature(realtime.cur_temperature_l);   //�¶ȣ��˴�������Ҫ�޸�ֵ��
    OsplcWarnning(tmperror);    //ͣ���쳣�������˴�������Ҫ�޸�ֵ��
    OsplcDrvgain((uint32_t)realtime.cur_gain_l * 10);     //���棬�˴�������Ҫ�޸�ֵ�� ����ǿ������ת�� ADD BY LY
    OsplcFlrateBack((uint32_t)realtime.cur_flowrate_g);     //�������٣��˴�������Ҫ�޸�ֵ������ǿ������ת�� ADD BY LY
    OsplcDispN();           //����������д��DispMem����֮�󣬴˺������͸�Һ����ʾ��
}

/************************************************************************
**	�������ƣ�OsplcShow(uint32_t Row1, uint32_t Row2, uint16_t Row3, bool bStop, bool bOverFlag)
**	��������: ��Һ����д������				  
**	�������: Row1����һ��Ҫд������ݣ�
              Row2���ڶ���Ҫд������ݣ�
              Row3��������Ҫд������ݡ�
              bStop�����ű�־��
              bOverFlag����һ�У��ڶ��е�һ���ַ���ʾ��־��
**	����ֵ  	: ��:

**	�� �� ��:	YCK
**	��������:	2016-03-28
*************************************************************************/
void OsplcShow(uint32_t Row1, uint32_t Row2, uint16_t Row3, bool bStop, bool bOverFlag)
{
    //�����ж��Ƿ�ֹ�����򶨶����ʱ�����䵼����ʾ������������ycx20161201
    if(globalvar.fix == 1)   //����
    {
        if(Row1 > globalvar.fixnum) return;
    }

    if(globalvar.fix == 2)   //����
    {
        if(Row2 > globalvar.fixmoney) return;
    }

    if(sysparas.lcdtype == 0)
    {
        OsplcShowB(Row1, Row2, Row3, bStop, bOverFlag);
    }
    else if(sysparas.lcdtype == 1)
    {
        OsplcShowN(Row1, Row2, Row3);
    }
}

