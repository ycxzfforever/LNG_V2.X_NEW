#ifndef _DATATYPETRANSFORM_H
#define _DATATYPETRANSFORM_H
/********************************************************
** ˵���������������ݵĳ��ȣ���Atemga 1280�еĳ���
**  char   :  1 �ֽ�
**  int    :  2 �ֽ�
**  long   :  4 �ֽ�
**  float  :  4 �ֽ�
**  double :  8 �ֽ�
********************************************************/

#define uint8_t     unsigned char
#define uint16_t    unsigned int
#define uint32_t    unsigned long
#define uint64_t    double

typedef union       //������������
{
    float data;
    uint8_t rdcv[4];
} UnionFloat;

typedef union       // 16λ������������
{
    uint16_t data;
    uint8_t rdcv[2];
} UnionU16;

typedef union       // 32λ������������
{
    uint32_t  data;
    uint8_t   rdcv[4];
} UnionU32;

typedef union       // 64λ������������
{
    uint64_t  data;
    uint8_t   rdcv[8];
} UnionU64;


uint8_t     CheckDate(uint8_t * str);
uint8_t     CheckTime(uint8_t * str);

uint16_t 	BcdToDec(uint8_t  h);
uint32_t 	BcdToUint(uint8_t *pbstr);
uint32_t  	BcdToUlong(uint8_t  *pbstr);

uint8_t     AscToBcd(uint8_t* asc);
void        BcdToAsc(uint8_t* dest, uint8_t bcd);
char*       ltostr(char *str, uint32_t val, uint8_t base) ;
uint32_t    HexToBcd(uint32_t sdt);
uint32_t    BcdToHex(uint32_t sdt);
uint32_t    BcdbufToHex(uint8_t* buf, uint8_t len);
uint8_t     Cmp2float(double vcmp1, double vcmp2);
void        HexToBcdbuf(uint32_t sdt, uint8_t* buf, uint8_t len);
void        FloatToBcdbuf(double fdt, char* buf, uint8_t len);
void        FloatToAsciibuf(double fdt, char* buf, uint8_t len);
void        HexToAsciibuf(uint32_t fdt, char *buf, uint8_t len);

void 		RemoveStringChar(uint8_t *buf, uint8_t ch, uint16_t len);
void 		BcdToStr(uint8_t bcd[], uint8_t str[]);
uint8_t 	DateCheck(uint8_t * str);
uint8_t 	TimeCheck(uint8_t * str);

void 		BufReverse2(uint8_t *buf, uint8_t num);
void 		StrReverse(uint8_t *buf, uint8_t num, uint8_t n);
void 		ClearBuf(volatile uint8_t *buf, uint8_t num, uint16_t len);

#endif