#include "kernel.h"

/************************************************************************
**	��������:	CheckDate(uint8_t * str)
**	��������:	�������������Ƿ�Ϸ�
**	�������: str:��������
**	����ֵ		��true	:��������ںϷ�
			  false	:��������ڲ��Ϸ�

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
uint8_t CheckDate(uint8_t * str)
{
    uint8_t     DayOfMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t     m, d;
    uint16_t    y;
    y = 2000 + BcdToHex(str[0]);
    m = BcdToHex(str[1]);
    d = BcdToHex(str[2]);

    if(((y % 4) == 0 && (y % 100 != 0)) || (y % 400 == 0)) DayOfMon[1] += 1;           //��

    if((m > 12) || (m < 1))  return false;       //��

    if(d > DayOfMon[m - 1])  return false;   //��

    return true;
}

/************************************************************************
**	��������:	 CheckTime(uint8_t * str)
**	��������:	��������ʱ���Ƿ�Ϸ�
**	�������: str:ʱ������
**	����ֵ		��true	:�����ʱ��Ϸ�
			  false	:�����ʱ�䲻�Ϸ�

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
uint8_t CheckTime(uint8_t * str)
{
    if(BcdToHex(str[0]) > 23 || BcdToHex(str[1]) >= 60 || BcdToHex(str[2]) >= 60)
        return false;

    return true;
}
uint16_t BcdToDec(uint8_t  h)
{
    return h / 0x10 * 10 + h % 0x10;
}

uint32_t BcdToUint(uint8_t *pbstr)
{
    return (BcdToDec(pbstr[0]) * 100) + (BcdToDec(pbstr[1]));
}

uint32_t  BcdToUlong(uint8_t  *pbstr)
{
    return ((BcdToUint(pbstr)) * 10000) + ((BcdToUint(&pbstr[2])));
}

/************************************************************************
**	��������:	 AscToBcd(uint8_t* asc)
**	��������:	�������ֽڵ�asc[0]��asc[1] ASCII��תΪ1���ֽڵ�BCD��
**	�������: asc:��Ҫת����asc���飨2���ֽڣ�
**	����ֵ		��ת�����BCD�룬һ���ֽ�

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
uint8_t AscToBcd(uint8_t* asc)
{
    if((asc[1] > 0x30))
        return (asc[0] - 0x30) * 0x10 + (asc[1] - 0x30);
    else
        return (asc[0] - 0x30) * 0x10;
}

/************************************************************************
**	��������:	  AscToBcd(uint8_t* asc)
**	��������:	��һ���ֽڵ�BCD��ת����2���ֽڵ�ASCII��
**	�������: bcd:��Ҫת����BCD��
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
void BcdToAsc(uint8_t* dest, uint8_t bcd)
{
    *dest     = bcd / 10 + 0x30;
    *(dest + 1) = bcd % 10 + 0x30;
}

/************************************************************************
**	��������:	ltostr(char *str, uint32_t val, uint8_t base)
**	��������:	��һ����������ָ����ʽ����ʽΪ��Ӧ���ַ���
**	�������: str:	 	��ʽ����ַ���
			  val:	��Ҫ��ʽ������������
			  base��ָ����ת����ʽ
**	����ֵ		����ʽ���Ժ���ַ���

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
char* ltostr(char *str, uint32_t val, uint8_t base)
{
    ldiv_t r;
    r = ldiv(labs(val), base);

    if(r.quot > 0)
        str = ltostr(str, r.quot, base);

    *str++ = "0123456789abcdefghijklmnopqrstuvwxyz"[(int)r.rem];
    *str   = '\0';
    return str;
}

/************************************************************************
**	��������:	HexToBcd(uint32_t sdt)
**	��������:	��һ��������ת����BCD��
**	�������: sdt:	��Ҫת����������
**	����ֵ		��ת���Ժ��BCD��

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
uint32_t HexToBcd(uint32_t sdt)
{
    char buf[12];
    memset(buf, 0, sizeof(buf));
    ltostr(buf, sdt, 10);
    return strtoul((char*)&buf[0], NULL, 16);
}

/************************************************************************
**	��������:	BcdToHex(uint32_t sdt)
**	��������:	��һ��BCD��תΪ������
**	�������: sdt:	��Ҫת����BCD������
**	����ֵ		��ת���Ժ����������

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
uint32_t BcdToHex(uint32_t sdt)
{
    uint32_t rd_h = 0;
    uint32_t rd_l = 0;
    char buf[12];
    memset(buf, 0, sizeof(buf));
    rd_h = sdt / 0x10000;
    rd_l = sdt % 0x10000;
    memset(buf, 0, sizeof(buf));
    ltostr(buf, rd_h, 16);
    rd_h = strtoul((char*)&buf[0], NULL, 10) * 10000;
    memset(buf, 0, sizeof(buf));
    ltostr(buf, rd_l, 16);
    return rd_h + strtoul((char*)&buf[0], NULL, 10);
}

/************************************************************************
**	��������: BcdbufToHex(uint8_t* buf, uint8_t len)
**	��������:	��һ��BCD����תΪ������
**	�������: buf:��Ҫת����BCD������
			  len:buf����ĳ��ȣ�һ��Ϊ2��4��6
**	����ֵ		��ת���Ժ����������

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
uint32_t  BcdbufToHex(uint8_t* buf, uint8_t len)
{
    UnionU32 tmp;
    UnionU16 tmp1;
    uint8_t n;
    uint32_t ull = 0;

    tmp.data = 0;
    tmp1.data = 0;

    if(len <= 4)
    {
        for(n = 0; n < len; n++)
        {
            tmp.rdcv[n] = buf[len - 1 - n];
        }

        return BcdToHex(tmp.data);
    }

    if(len == 6)
    {
        tmp1.data = 0;
        tmp1.rdcv[0] = buf[1];
        tmp1.rdcv[1] = buf[0];
        ull  = BcdToHex(tmp1.data);
        ull *= 100000000;
        tmp.data = 0;
        tmp.rdcv[0] = buf[5];
        tmp.rdcv[1] = buf[4];
        tmp.rdcv[2] = buf[3];
        tmp.rdcv[3] = buf[2];
        ull += BcdToHex(tmp.data);

        return ull;

    }

    return 0;
}

/************************************************************************
**	��������:	  Cmp2float(double vcmp1, double vcmp2)
**	��������:	�Ƚ������������Ƿ����
**	�������: vcmp1:������1
			  vcmp2:������2
**	����ֵ		��true��	 ����ֵ���
			  false������ֵ����

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
uint8_t Cmp2float(double vcmp1, double vcmp2)
{
    char   buf1[12] = {0};
    char   buf2[12] = {0};
    uint8_t len1, len2 = 0;
    sprintf_P((char*)&buf1[0], "%.02f", vcmp1);
    sprintf_P((char*)&buf2[0], "%.02f", vcmp2);
    len1 = strlen(buf1);
    len2 = strlen(buf2);
    len1 = (len1 >= len2) ? len1 : len2;

    if(memcmp(buf1, buf2, len1) == 0) return true;

    return false;
}


/************************************************************************
**	��������:	HexToBcdbuf(uint32_t sdt, uint8_t* buf, uint8_t len)
**	��������:	��һ��������תΪBCD������
**	�������: sdt:��Ҫת����������
			  buf:ת�����BCD������
			  len:buf����ĳ���
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
void HexToBcdbuf(uint32_t sdt, uint8_t* buf, uint8_t len)
{
    char str[16] = {0};
    uint8_t g, n, m, l = 0;

    if(len == 0) return;

    g = len - 1;
    l = 11;
    m = 6;
    sprintf_P(str, "%012ld", sdt);

    for(n = 0; n < m; n++)
    {
        buf[g] = ((str[l - 1] - '0') << 4) + (str[l] - '0');

        if(g == 0) break;

        l -= 2;
        g -= 1;
    }
}

/************************************************************************
**	��������:	FloatToBcdbuf(double fdt, char* buf, uint8_t len)
**	��������:	��һ��������תΪBCD������
**	�������: fdt:��Ҫת���ĸ�����
			  buf:ת�����BCD������
			  len:buf����ĳ���
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
void FloatToBcdbuf(double fdt, char* buf, uint8_t len)
{
    char str[16] = {0};
    uint8_t g, n, m, l = 0;

    if(len == 0) return;

    sprintf_P(str, "%013.02f", fdt);
    buf[len - 1] = ((str[11] - '0') << 4) + (str[12] - '0');
    g = len - 2;
    l = 9;
    m = 5;

    for(n = 0; n < m; n++)
    {
        buf[g] = ((str[l - 1] - '0') << 4) + (str[l] - '0');

        if(g == 0) break;

        l -= 2;
        g -= 1;
    }
}

/************************************************************************
**	��������:	  FloatToAsciibuf(double fdt, char* buf, uint8_t len)
**	��������:	��һ��������תΪASC������
**	�������: fdt:��Ҫת���ĸ�����
			  buf:ת�����ASC������
			  len:buf����ĳ���
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
void FloatToAsciibuf(double fdt, char* buf, uint8_t len)
{
    char str[16] = {0};
    uint8_t g, n = 0;

    if(len < 3)   return;

    sprintf_P(str, "%013.02f", fdt);
    buf[len - 1] = str[12];
    buf[len - 2] = str[11];
    g = len - 3;

    for(n = 0; n < len - 2; n++)
    {
        buf[g] = str[9 - n];

        if(g == 0) break;

        g--;
    }
}

/************************************************************************
**	��������:	  HexToAsciibuf(uint32_t fdt, char *buf, uint8_t len)
**	��������:	��һ��������תΪASC������
**	�������: fdt:��Ҫת����������
			  buf:ת�����ASC������
			  len:buf����ĳ���
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2016-4-13
*************************************************************************/
void HexToAsciibuf(uint32_t fdt, char *buf, uint8_t len)
{
    char str[16] = {0};
    uint8_t n = 0;

    if(len < 3)   return;

    memset(str, 0, 16);
    sprintf_P(str, "%012ld", fdt);

    for(n = 0; n < len; n ++)
    {
        buf[len - 1 - n] = str[11 - n];
    }
}


/************************************************************************
**	��������:	RemoveStringChar(uint8_t *buf, uint8_t ch, uint16_t len)
**	��������:	ɾ���ַ���ǰ��������ַ�
**	�������: buf:��Ҫɾ�������ַ����ַ�������
			  ch:��Ҫɾ���������ַ�
			  len:�ַ�������ĳ���
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-11-30
*************************************************************************/
void RemoveStringChar(uint8_t *buf, uint8_t ch, uint16_t len)
{
    uint8_t *t1, *t2;
    uint16_t i = 0, j = 0;

    if((len == 0) || (buf == NULL)) return;

    if(buf[0] != ch)  return;   //��һ���Ͳ����㣬ֱ�ӷ���

    t1 = buf;               //���ڱ仯����buf
    t2 = buf;               //������ӽ�����

    while(*buf == ch)       //�ҳ�����Ҫɾ�����ַ���һ���ĵ�һ���ַ�
    {
        buf++;
        i++;
    }

    len -= i;               //����buf�����������ݳ���
    j = len;                //����˳��ȣ���������ַ���������

    while(len--)
    {
        *t1++ = *buf++;
    }

    t2[j] = '\0';
}

/************************************************************************
**	��������:	BcdToStr(uint8_t bcd[], uint8_t str[])
**	��������:	��BCD��ת��Ϊ�ַ���
**	�������: bcd:��Ҫת����BCD����
			  str:ת���Ժ���ַ�������
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-11-30
*************************************************************************/
void BcdToStr(uint8_t bcd[], uint8_t str[])
{
    str[0] = ((bcd[0] >> 4) + 0x30);
    str[1] = ((bcd[0] & 0x0f) + 0x30);
    str[2] = ((bcd[1] >> 4) + 0x30);
    str[3] = ((bcd[1] & 0x0f) + 0x30);
    str[4] = ((bcd[2] >> 4) + 0x30);
    str[5] = ((bcd[2] & 0x0f) + 0x30);
    str[6] = ((bcd[3] >> 4) + 0x30);
    str[7] = ((bcd[3] & 0x0f) + 0x30);
}


/************************************************************************
**	��������:	BufReverse2(uint8_t *buf, uint8_t num)
**	��������:	��buf�е�num��Ԫ�أ��ߵ�˳���ٴ���buf��
**	���������buf��Ŀ��buf
			  num: buf��Ԫ�صĸ���
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2016-06-3
*************************************************************************/
void BufReverse2(uint8_t *buf, uint8_t num)
{
    uint8_t  temp;

    if(num == 2)
    {
        temp   = buf[0];
        buf[0] = buf[1];
        buf[1] = temp;
    }

    if(num == 4)
    {
        temp   = buf[0];
        buf[0] = buf[3];
        buf[3] = temp;
        temp   = buf[1];
        buf[1] = buf[2];
        buf[2] = temp;
    }
}
/************************************************************************
**	��������:	StrReverse(uint8_t *buf,uint8_t num,uint8_t n)
**	��������:	��һ������Ϊn����Ϊnum�����ݵߵ�λ�ú��ٴ���buf��
**	���������buf����ת���������׵�ַ
			  num: ��ת�������ݸ���
			  n	 : ת�����ݵ��ֽ���
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2016-06-3
*************************************************************************/
void StrReverse(uint8_t *buf, uint8_t num, uint8_t n)
{
    uint8_t i = 0;

    if(n == 2)
    {
        for(i = 0; i < num; i++)
        {
            BufReverse2(buf, 2);
            buf += 2;
        }
    }

    if(n == 4)
    {
        for(i = 0; i < num; i++)
        {
            BufReverse2(buf, 4);
            buf += 4;
        }
    }
}

