#include "kernel.h"

/************************************************************************
**	函数名称:	CheckDate(uint8_t * str)
**	函数功能:	检测输入的日期是否合法
**	输入参数: str:日期数组
**	返回值		：true	:输入的日期合法
			  false	:输入的日期不合法

**	创 建 者:	YCX
**	创建日期:	2016-4-13
*************************************************************************/
uint8_t CheckDate(uint8_t * str)
{
    uint8_t     DayOfMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t     m, d;
    uint16_t    y;
    y = 2000 + BcdToHex(str[0]);
    m = BcdToHex(str[1]);
    d = BcdToHex(str[2]);

    if(((y % 4) == 0 && (y % 100 != 0)) || (y % 400 == 0)) DayOfMon[1] += 1;           //年

    if((m > 12) || (m < 1))  return false;       //月

    if(d > DayOfMon[m - 1])  return false;   //日

    return true;
}

/************************************************************************
**	函数名称:	 CheckTime(uint8_t * str)
**	函数功能:	检测输入的时间是否合法
**	输入参数: str:时间数组
**	返回值		：true	:输入的时间合法
			  false	:输入的时间不合法

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称:	 AscToBcd(uint8_t* asc)
**	函数功能:	将两个字节的asc[0]和asc[1] ASCII码转为1个字节的BCD码
**	输入参数: asc:需要转换的asc数组（2个字节）
**	返回值		：转换后的BCD码，一个字节

**	创 建 者:	YCX
**	创建日期:	2016-4-13
*************************************************************************/
uint8_t AscToBcd(uint8_t* asc)
{
    if((asc[1] > 0x30))
        return (asc[0] - 0x30) * 0x10 + (asc[1] - 0x30);
    else
        return (asc[0] - 0x30) * 0x10;
}

/************************************************************************
**	函数名称:	  AscToBcd(uint8_t* asc)
**	函数功能:	将一个字节的BCD码转换成2个字节的ASCII码
**	输入参数: bcd:需要转换的BCD码
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2016-4-13
*************************************************************************/
void BcdToAsc(uint8_t* dest, uint8_t bcd)
{
    *dest     = bcd / 10 + 0x30;
    *(dest + 1) = bcd % 10 + 0x30;
}

/************************************************************************
**	函数名称:	ltostr(char *str, uint32_t val, uint8_t base)
**	函数功能:	将一个整型数按指定格式，格式为相应的字符串
**	输入参数: str:	 	格式后的字符串
			  val:	需要格式化的整型数据
			  base：指定的转换格式
**	返回值		：格式化以后的字符串

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称:	HexToBcd(uint32_t sdt)
**	函数功能:	将一个整型数转换成BCD码
**	输入参数: sdt:	需要转换的整型数
**	返回值		：转换以后的BCD码

**	创 建 者:	YCX
**	创建日期:	2016-4-13
*************************************************************************/
uint32_t HexToBcd(uint32_t sdt)
{
    char buf[12];
    memset(buf, 0, sizeof(buf));
    ltostr(buf, sdt, 10);
    return strtoul((char*)&buf[0], NULL, 16);
}

/************************************************************************
**	函数名称:	BcdToHex(uint32_t sdt)
**	函数功能:	将一个BCD码转为整型数
**	输入参数: sdt:	需要转换的BCD码数据
**	返回值		：转换以后的整型数据

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称: BcdbufToHex(uint8_t* buf, uint8_t len)
**	函数功能:	将一个BCD数组转为整型数
**	输入参数: buf:需要转化的BCD码数组
			  len:buf数组的长度，一般为2或4或6
**	返回值		：转换以后的整型数据

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称:	  Cmp2float(double vcmp1, double vcmp2)
**	函数功能:	比较两个浮点数是否相等
**	输入参数: vcmp1:浮点数1
			  vcmp2:浮点数2
**	返回值		：true：	 两个值相等
			  false：两个值不等

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称:	HexToBcdbuf(uint32_t sdt, uint8_t* buf, uint8_t len)
**	函数功能:	将一个整型数转为BCD码数组
**	输入参数: sdt:需要转换的整型数
			  buf:转化后的BCD码数组
			  len:buf数组的长度
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称:	FloatToBcdbuf(double fdt, char* buf, uint8_t len)
**	函数功能:	将一个浮点数转为BCD码数组
**	输入参数: fdt:需要转换的浮点数
			  buf:转化后的BCD码数组
			  len:buf数组的长度
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称:	  FloatToAsciibuf(double fdt, char* buf, uint8_t len)
**	函数功能:	将一个浮点数转为ASC码数组
**	输入参数: fdt:需要转换的浮点数
			  buf:转化后的ASC码数组
			  len:buf数组的长度
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称:	  HexToAsciibuf(uint32_t fdt, char *buf, uint8_t len)
**	函数功能:	将一个整型数转为ASC码数组
**	输入参数: fdt:需要转换的整型数
			  buf:转化后的ASC码数组
			  len:buf数组的长度
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2016-4-13
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
**	函数名称:	RemoveStringChar(uint8_t *buf, uint8_t ch, uint16_t len)
**	函数功能:	删除字符串前面的特殊字符
**	输入参数: buf:需要删除特殊字符的字符串数组
			  ch:需要删除的特殊字符
			  len:字符串数组的长度
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-11-30
*************************************************************************/
void RemoveStringChar(uint8_t *buf, uint8_t ch, uint16_t len)
{
    uint8_t *t1, *t2;
    uint16_t i = 0, j = 0;

    if((len == 0) || (buf == NULL)) return;

    if(buf[0] != ch)  return;   //第一个就不满足，直接返回

    t1 = buf;               //用于变化处理buf
    t2 = buf;               //用于添加结束符

    while(*buf == ch)       //找出与需要删除的字符不一样的第一个字符
    {
        buf++;
        i++;
    }

    len -= i;               //计算buf后面有用数据长度
    j = len;                //保存此长度，用以添加字符串结束符

    while(len--)
    {
        *t1++ = *buf++;
    }

    t2[j] = '\0';
}

/************************************************************************
**	函数名称:	BcdToStr(uint8_t bcd[], uint8_t str[])
**	函数功能:	将BCD码转化为字符串
**	输入参数: bcd:需要转换的BCD数组
			  str:转换以后的字符串数组
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-11-30
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
**	函数名称:	BufReverse2(uint8_t *buf, uint8_t num)
**	函数功能:	将buf中的num个元素，颠倒顺序，再存入buf中
**	输入参数：buf：目标buf
			  num: buf中元素的个数
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2016-06-3
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
**	函数名称:	StrReverse(uint8_t *buf,uint8_t num,uint8_t n)
**	函数功能:	将一串长度为n个数为num的数据颠倒位置后，再存入buf中
**	输入参数：buf：待转换的数据首地址
			  num: 待转换的数据个数
			  n	 : 转换数据的字节数
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2016-06-3
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

