#include "kernel.h"

/************************************************************************
**	函数名称：ReadLossCard(uint32_t cid)
**	函数功能: 根据卡号，查询此卡是否为挂失卡			  
**	输入参数: cid： 需要查询的IC 卡卡号（HEX）
**	返回值  	: true:正常卡；false:挂失卡（或卡号超过范围）ADD BY LY

**	创 建 者:	HMM
**	创建日期:	2016-03-28
*************************************************************************/
uint8_t ReadLossCard(uint32_t cid)
{
    uint8_t index  = cid / 10000000; //取最高字节的值 ADD BY LY
#if 0
    uint16_t  offset = cid % 10000000;
#endif
    //8字节ASCII码IC卡卡号的组成
    //最高1字节是表示卡类型
    //次高字节是站编号
    //剩余6字节为卡号
    uint32_t  offset = cid % 1000000; //新后台 取后6位  //第7位表示站编号
    /*if(index == 2)//(山东)
    {
      if(offset>2000)  offset -= 2000;
      if(offset<40)    offset  = 160 - offset;
      else             return false;
    }*/
    uint32_t  offx   = offset / 8;
    uint32_t  offy   = offset % 8;
    uint32_t  addr;
    uint8_t eepuc;
    //最高位为 1 ：表示是用户卡
    //最高位为 2 ：表示是员工卡
    //最高位为 3 ：表示是记账卡
    //最高位为 4 ：表示是维修卡
    //最高位为 5 ：表示是内部卡
    //最高位为 6 ：表示是几分卡
    //最高位为 7 ：表示是VIP卡

    switch(index)
    {
        case 1: //用户卡
            if(offx >= CARD_USRER_LEN) return false;

            addr = CARD_USRER_ADDR + offx;
            break;

        case 2: //员工卡
            if(offx >= CARD_WORKER_LEN) return false;

            addr = CARD_WORKER_ADDR + offx;
            break;

        case 3:  //20130117 记账卡
            if(offx >= CARD_MEM_LEN) return false;

            addr = CARD_MEM_ADDR + offx;
            break;

        case 4://维修卡
            if(offx >= CARD_SERV_LEN) return false;

            addr = CARD_SERV_ADDR + offx;
            break;

        case 5://内部卡
            if(offx >= CARD_OWN_LEN) return false;

            addr = CARD_OWN_ADDR + offx;
            break;

        case 6: //积分卡
            if(offx >= CARD_CAL_LEN) return false;

            addr = CARD_CAL_ADDR + offx;
            break;

        case 7: //VIP卡201610
            if(offx >= CARD_VIP_LEN) return false;

            addr = CARD_VIP_ADDR + offx;
            break;

        default:
            return false;
    }

    if(I2cReadBytesForEeprom(SLA_W_24CXXX, (uint16_t)addr, 1, &eepuc) == -1)
    {

        return false;
    }

    eepuc >>= offy;
    eepuc &= 0x01;

    if(eepuc) return false;

    return true;
}

/************************************************************************
**	函数名称：SetLossCard(uint32_t cid, bool bset)
**	函数功能: 设置卡号为cid的IC卡在内存中的挂失标志	  
**	输入参数: cid： 需要查询的IC 卡卡号（HEX）
			  bset：挂失标志，1：挂失；0：解挂
**	返回值  	: true:写成功 ； false:卡号超过范围 ADD BY LY

**	创 建 者:	HMM
**	创建日期:	2016-03-28
*************************************************************************/
bool SetLossCard(uint32_t cid, bool bset)
{
    uint32_t 	addr;
    uint8_t  	eepuc;

    uint8_t 	index  = cid / 10000000; //卡号最高位，卡类型索引 7：VIP卡；6：积分卡；5：内部卡；4：维修卡；3：记账卡；2：员工卡；1：用户卡 ADD BY LY
    uint32_t  offset = cid % 1000000; //新后台 取后6位
    uint32_t  offx   = offset / 8; //根据卡号后6位长度判断，是否已经超出存储的范围，即哪个字节 ADD BY LY
    uint32_t  offy   = offset % 8; //根据卡号后6位判断卡号挂失卡对应的标记位 ADD BY LY

    switch(index)
    {
        case 1: //用户卡
            if(offx >= CARD_USRER_LEN) return false;

            addr = CARD_USRER_ADDR + offx;//计算出该卡号挂失卡对应标记的地址 ADD BY LY
            break;

        case 2: //员工卡
            if(offx >= CARD_WORKER_LEN) return false;

            addr = CARD_WORKER_ADDR + offx;
            break;

        case 3: //20130117 记账卡
            if(offx >= CARD_MEM_LEN) return false;

            addr = CARD_MEM_ADDR + offx;
            break;

        case 4://维修卡
            if(offx >= CARD_SERV_LEN) return false;

            addr = CARD_SERV_ADDR + offx;
            break;

        case 5://内部卡
            if(offx >= CARD_OWN_LEN) return false;

            addr = CARD_OWN_ADDR + offx;
            break;

        case 6: //积分卡
            if(offx >= CARD_CAL_LEN) return false;

            addr = CARD_CAL_ADDR + offx;
            break;

        case 7: //VIP卡201610
            if(offx >= CARD_VIP_LEN) return false;

            addr = CARD_VIP_ADDR + offx;
            break;

        default:
            return false;
    }

    if(I2cReadBytesForEeprom(SLA_W_24CXXX, (uint16_t)addr, 1, &eepuc) == -1)
    {
        return false;
    }

    if(bset)
        eepuc |= (1 << offy);   //取出该卡挂失标志所在位置的数据，然后将offy对应的位置1表示是挂失 //ADD BY LY
    else
        eepuc &= ~(1 << offy);   //取出该卡挂失标志所在位置的数据，然后将offy对应的位置0表示取消挂失 //ADD BY LY

    if(I2cWriteBytesForEeprom(SLA_W_24CXXX, (uint16_t)addr, 1, &eepuc) == -1)
    {
        return false;
    }

    return true;
}

/************************************************************************
**	函数名称：ClearLossCard(void)
**	函数功能: 清除所有挂失卡标记 ADD BY LY 
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	HMM
**	创建日期:	2016-03-28
*************************************************************************/
void ClearLossCard(void)
{
    uint32_t 	addr;
    uint8_t  	eepuc;

    eepuc = 0;

    FYD12864ClrScreen();

    FYD12864DispPrintfFlash(2, 1, "清除挂失卡中");
    FYD12864DispPrintfFlash(3, 1, "请稍后..");

    for(addr = CARD_USRER_ADDR; addr < CARD_VIP_ADDR + 100; addr++)
    {
        I2cWriteBytesForEeprom(SLA_W_24CXXX, (uint16_t)addr, 1, &eepuc);
    }

    FYD12864ClrScreen();
    FYD12864DispPrintfFlash(2, 1, "清除成功");
    DispUI = DispSetICInfo2;
}

/************************************************************************
**	函数名称：WriteLossCardFlag(void)
**	函数功能: 根据卡号，查询系统中该卡是否为挂失卡，如果是挂失卡，写卡的挂失标记 ADD BY LY
**	输入参数: 无
**	返回值  	: true:置挂失标志成功 ； false:置挂失标志失败

**	创 建 者:	HMM
**	创建日期:	2016-03-28
*************************************************************************/
uint8_t WriteLossCardFlag(void)
{
    uint8_t buf[16];
    uint8_t bufbak[16];

    if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, buf))     //读块2
    {
        return false;
    }

    //置挂失卡标志
    buf[8] = 0x30;
    buf[9] = 0x32;

    if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 2, buf))     //写挂失标志
    {
        return false;
    }

    _delay_ms(10);

    if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, bufbak))     //读出刚刚写入的数据
    {
        return false;
    }

    if(memcmp(buf, bufbak, 16) != 0)	//比对写入是否正确 ADD BY LY
    {
        return false;
    }

    return true;
}


