#include "kernel.h"

/************************************************************************
**	函数名称:	EepromInterReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
**	函数功能:	从内部EERPROM的指定地址读取指定个数的数据，一个一个地读
**	输入参数: nID:		 读取数据的首地址
			  rdbuf：读出的数据存入的首地址
			  cnt：	 读取数据的个数
**	返回值		：true:	 读成功
			  false: 读失败

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
bool EepromInterReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
{
    uint16_t i = 0;
    uint32_t eep_addr;

    eep_addr = nID;

    for(i = 0; i < cnt; i++)
        __EEGET(*rdbuf++, eep_addr + i);

    return true;
}

/************************************************************************
**	函数名称:	EepromInterWriteBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
**	函数功能:	向内部EERPOM的指定地址写入指定个数的数据，一个一个地写
**	输入参数: nID:		 待写入数据的首地址
			  wtbuf：保存待写入数据的首地址
			  cnt：	 待写入数据的个数
**	返回值		：true:	 写成功
			  false: 写失败

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
bool EepromInterWriteBytes(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt)
{
    uint16_t i = 0;
    uint32_t eep_addr;
    eep_addr = nID;

    for(i = 0; i < cnt; i++)
    {
        __EEPUT(eep_addr + i, wtbuf[i]);
        _delay_ms(1);
    }

    return true;
}
/************************************************************************
**	函数名称:	EepromExternalWriteBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
**	函数功能:	向外部EERPOM的指定地址写入指定个数的数据，一个一个地写
**	输入参数: nID:		 待写入数据的首地址
			  wtbuf：保存待写入数据的首地址
			  cnt：	 待写入数据的个数
**	返回值		：true:	 写成功
			  false: 写失败

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
bool EepromExternalWriteBytes(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt)
//功能：只写值到外部EEPROM
{
    uint16_t i = 0;
    uint32_t eep_addr;

    eep_addr = nID;

//  uint32_t eep_addr = cur_SysinfoAddr+nID;
    for(i = 0; i < cnt; i++)
        I2cWriteBytesForEeprom(SLA_W_24CXXX, eep_addr + i, 1, &wtbuf[i]);

    return true;
}

/************************************************************************
**	函数名称:	EepromExternalPageWriteBytes(uint16_t nID, uint8_t* wtbuf, uint16_t cnt)
**	函数功能:	向外部EERPROM的指定地址写入指定个数的数据，有页判断，但还是一个一个地写的，后续优化
**	输入参数: nID:		 待写入数据的首地址
			  wtbuf：保存待写入数据的首地址
			  cnt：	 待写入数据的个数
**	返回值		：true:	 写成功
			  false: 写失败

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
bool EepromExternalPageWriteBytes(uint16_t nID, uint8_t* wtbuf, uint16_t cnt)
{
    uint8_t pagesize = 0x3F; //AT24C128 每页 64字节，页地址从(0-0x3F)
    uint16_t pageoffset, pagenum; //pageoffset 页偏移地址  pagenum 页数
    uint32_t startaddr;//写入数据地址
    uint32_t addr;
    uint32_t count = 0; //已经写入数据个数

    addr = nID;
    startaddr = nID;
    pageoffset = (uint8_t)(addr & pagesize);     //求得写入地址在某页中的位置

    while(count < cnt)
    {
        if((startaddr & pagesize) == 0)
        {
            if((cnt - count) > pagesize)
            {
                pagenum = pagesize;
            }
            else
            {
                pagenum = cnt - count;
            }
        }
        else
        {
            pagenum = pagesize - pageoffset;

            if(cnt <= pagenum)
            {
                pagenum = cnt;
            }
        }

        if(EepromExternalWriteBytes(startaddr, wtbuf + count, pagenum))
        {
            count += pagenum;
            startaddr += pagenum;
        }
    }

    return true;
}

/************************************************************************
**	函数名称:	EepromExternalReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
**	函数功能:	从外部EERPROM的指定地址读取指定个数的数据，一个一个地读
**	输入参数: nID:		 读取数据的首地址
			  rdbuf：读出的数据存入的首地址
			  cnt：	 读取数据的个数
**	返回值		：true:	 读成功
			  false: 读失败

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
bool EepromExternalReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
//功能：只写值到外部EEPROM
{
    uint16_t i = 0;
    uint32_t eep_addr;

    eep_addr = nID;
//  uint32_t eep_addr = cur_SysinfoAddr+nID;

    for(i = 0; i < cnt; i++)
        I2cReadBytesForEeprom(SLA_W_24CXXX, eep_addr + i, 1, &rdbuf[i]);

    return true;
}
/************************************************************************
**	函数名称:	SaveEeprom(uint16_t addr, uint8_t *data, uint16_t num)
**	函数功能:	将数据保存到内部EEPROM和外部EEPROM
**	输入参数: addr:	写入EEPROM的起始地址
			  data:	数据指针,指向写入的数据
			  cnt：	num :写入数据个数
**	返回值  	: false:保存失败
          	  true :保存成功

**	创 建 者:	YCX
**	创建日期:	2016-12-9
*************************************************************************/
uint8_t SaveEeprom(uint16_t addr, uint8_t *data, uint16_t num)
{
    uint8_t     *t1, *t2;
    uint16_t    i;
    uint8_t 	ret = false;
    Sysparas    tmp1, tmp2;

    t1 = (uint8_t*)&tmp1;
    t2 = (uint8_t*)&tmp2;

    if(EepromInterWriteBytes(addr, data, num))     //写内部eeprom正确
    {
        ClrBit(syserr, SYSERR_W_IN_EEPROM);

        if(EepromExternalWriteBytes(addr, data, num))     //写外部eeprom正确
        {
            ClrBit(syserr, SYSERR_W_EX_EEPROM);

            if(EepromInterReadBytes(addr, t1, num))     //读内部eeprom正确
            {

                if(EepromExternalReadBytes(addr, t2, num))     //读外部eeprom正确
                {

                    for(i = 0; i < num; i++)  //将内部EEPROM与外部EEPROM写入的数据读出比对
                    {
                        if(t1[i] != t2[i])  //有任何一个错误，退出
                            break;
                    }

                    if(i == num)  //全部正确 返回1
                    {
                        ret = true;
                    }
                    else if(i < num)
                    {
                        ret = false;
                    }
                }
                else
                {
                    SetBit(syserr, SYSERR_R_EX_EEPROM);
                    ret = false;
                }
            }
            else
            {
                SetBit(syserr, SYSERR_R_IN_EEPROM);
                ret = false;
            }
        }
        else
        {
            SetBit(syserr, SYSERR_W_EX_EEPROM);   //写外部eeprom错误。系统错误0x40。
            ret = false;
        }
    }
    else
    {
        SetBit(syserr, SYSERR_W_IN_EEPROM);
        ret = false;
    }

    return ret;
}
/************************************************************************
**	函数名称:	SaveSysparas(uint16_t addr, uint8_t *data, uint16_t num)
**	函数功能:	将系统参数保存到内部EEPROM和外部EEPROM
**	输入参数: addr:	写入EEPROM的起始地址
			  data:	数据指针,指向写入的数据
			  cnt：	num :写入数据个数
**	返回值  	: 0:保存失败
          	  1 :保存成功

**	创 建 者:	YCX
**	创建日期:	2016-12-9
*************************************************************************/
uint8_t SaveSysparas(uint16_t addr, uint8_t *data, uint16_t num)
{
    uint8_t     ret = 0;
    uint16_t    crc;

    sysparas.writeE2num++;

    crc = ModbusCrc16(data, num - 2);
    sysparas.crc = crc;

    if(sysparas.writeE2num < EEPROM_W_TIMES)
    {
        ret = SaveEeprom(addr, data, num);
    }
    else//当eeprom写次数大于等于最大写次数时，先在本地址保存一次，然后地址进行偏移，随后再保存一次
    {
        ret = SaveEeprom(addr, data, num);   //本地保存(将第90000次写入，下次读的时候，就超过90000了)系统参数ADD BY LY

        if(addr == SYSPARA_ADDR1)
        {
            SYSPARA_ADDR = SYSPARA_ADDR2; //将参数当前存储地址赋值给SYSPARA_ADDR
            sysparas.writeE2num = 1;
        }
        else if(addr == SYSPARA_ADDR2)
        {
            SYSPARA_ADDR = SYSPARA_ADDR3; //将参数当前存储地址赋值给SYSPARA_ADDR
            sysparas.writeE2num = 1;
        }
        else if(addr == SYSPARA_ADDR3)
        {
            SYSPARA_ADDR = SYSPARA_ADDR4; //将参数当前存储地址赋值给SYSPARA_ADDR
            sysparas.writeE2num = 1;
        }
        else if(addr == SYSPARA_ADDR4)
        {
            globalvar.eepromflag = 1; //当系统参数已经写到系统参数的最大地址以后，提示更换
        }

        crc = ModbusCrc16(data, num - 2);
        sysparas.crc = crc;

        ret = SaveEeprom(addr, data, num);  //写入新的参数存储区域
    }

    return ret;
}

/************************************************************************
**	函数名称:	 SaveSignalSysparas(uint8_t paraindex, uint8_t crcwrflag)
**	函数功能:	将单个系统参数保存到内部EEPROM和外部EEPROM
**	输入参数: paraindex：保存参数的索引号
			  crcwrflag：是否更新CRC标志，1：更新CRC；0：不更新CRC
**	返回值  	: 0:保存失败
          	  1 :保存成功

**	创 建 者:	LY
**	创建日期:	2018-9-19
*************************************************************************/
uint8_t SaveSignalSysparas(uint8_t paraindex, uint8_t crcwrflag)
{
    uint8_t     ret = false, *data, len;
    uint16_t	addroffset, addr;

    /****************************** EEPROM 写次数超限判断 *************************/
    if(crcwrflag == 1)
    {
        sysparas.writeE2num++;
		addroffset = ((uint16_t)&sysparas.writeE2num - (uint16_t)&sysparas.head);
		data = (uint8_t*)&sysparas.writeE2num;
		len = sizeof(sysparas.writeE2num);
		addr = SYSPARA_ADDR + addroffset;
        if(sysparas.writeE2num < EEPROM_W_TIMES)
        {            
            //如果EEPOM写次数没有超限，就将当前EEPROM写次数写入 ADD BY LY
            SaveEeprom(addr, data, len);
        }
        else
        {
            //如果原来的参数存储次数超限，将当前EEPROM 读写次数写入当前块 ADD BY LY
            if(SaveEeprom(addr, data, len))
            {
                SaveCrc();
            }

            if(SYSPARA_ADDR == SYSPARA_ADDR1)
            {
                SYSPARA_ADDR = SYSPARA_ADDR2; //将参数当前存储地址赋值给SYSPARA_ADDR
                sysparas.writeE2num = 1;
            }
            else if(SYSPARA_ADDR == SYSPARA_ADDR2)
            {
                SYSPARA_ADDR = SYSPARA_ADDR3; //将参数当前存储地址赋值给SYSPARA_ADDR
                sysparas.writeE2num = 1;
            }
            else if(SYSPARA_ADDR == SYSPARA_ADDR3)
            {
                SYSPARA_ADDR = SYSPARA_ADDR4; //将参数当前存储地址赋值给SYSPARA_ADDR
                sysparas.writeE2num = 1;
            }
            else if(SYSPARA_ADDR == SYSPARA_ADDR4)
            {
                globalvar.eepromflag = 1; //当系统参数已经写到系统参数的最大地址以后，提示更换
            }

            addr = SYSPARA_ADDR ;

            //如果当前EERPOM读写次数超限，就在下一块区域将所有参数全部写一遍 ADD BY LY
            if(SaveSysparas(addr, (uint8_t*)&sysparas.head, sizeof(sysparas)))
            {
                ret = true;
            }
            else
            {
                ret = false;
            }

            return ret;
        }

        /******************************* EEPROM 写次数超限判断 *******************************/
    }

    //只有手动可以设置的参数，才采取此方法单独保存 ADD BY LY
    switch(paraindex)
    {
        case 2://枪号
            addroffset = (uint16_t)&sysparas.gunnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.gunnum;
            len = sizeof(sysparas.gunnum);
            break;

        case 3://交易流水号
            addroffset = (uint16_t)&sysparas.transnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.transnum;
            len = sizeof(sysparas.transnum);
            break;

        case 4://已传交易流水号
            addroffset = (uint16_t)&sysparas.uptransnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.uptransnum;
            len = sizeof(sysparas.uptransnum);
            break;

        case 5://枪总气量
            addroffset = (uint16_t)&sysparas.totvolume - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.totvolume;
            len = sizeof(sysparas.totvolume);
            break;

        case 6://枪总金额
            addroffset = (uint16_t)&sysparas.totmoney - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.totmoney;
            len = sizeof(sysparas.totmoney);
            break;

        case 7://单价0
            addroffset = (uint16_t)&sysparas.price[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[0];
            len = sizeof(sysparas.price[0]);
            break;

        case 8://单价1
            addroffset = (uint16_t)&sysparas.price[1] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[1];
            len = sizeof(sysparas.price[1]);
            break;

        case 9://单价2
            addroffset = (uint16_t)&sysparas.price[2] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[2];
            len = sizeof(sysparas.price[2]);
            break;

        case 10://单价3
            addroffset = (uint16_t)&sysparas.price[3] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[3];
            len = sizeof(sysparas.price[3]);
            break;

        case 11://单价4
            addroffset = (uint16_t)&sysparas.price[4] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[4];
            len = sizeof(sysparas.price[4]);
            break;

        case 12://单价5
            addroffset = (uint16_t)&sysparas.price[5] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[5];
            len = sizeof(sysparas.price[5]);
            break;

        case 13://单价6
            addroffset = (uint16_t)&sysparas.price[6] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[6];
            len = sizeof(sysparas.price[6]);
            break;

        case 14://单价7
            addroffset = (uint16_t)&sysparas.price[7] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[7];
            len = sizeof(sysparas.price[7]);
            break;

        case 15://单价8
            addroffset = (uint16_t)&sysparas.price[8] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[8];
            len = sizeof(sysparas.price[8]);
            break;

        case 16://单价9
            addroffset = (uint16_t)&sysparas.price[9] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[9];
            len = sizeof(sysparas.price[9]);
            break;

        case 17://单价10
            addroffset = (uint16_t)&sysparas.price[10] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[10];
            len = sizeof(sysparas.price[10]);
            break;

        case 18://单价11
            addroffset = (uint16_t)&sysparas.price[11] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[11];
            len = sizeof(sysparas.price[11]);
            break;

        case 19://单价12
            addroffset = (uint16_t)&sysparas.price[12] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[12];
            len = sizeof(sysparas.price[12]);
            break;

        case 20://单价13
            addroffset = (uint16_t)&sysparas.price[13] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[13];
            len = sizeof(sysparas.price[13]);
            break;

        case 21://单价14
            addroffset = (uint16_t)&sysparas.price[14] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[14];
            len = sizeof(sysparas.price[14]);
            break;

        case 22://单价15
            addroffset = (uint16_t)&sysparas.price[15] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[15];
            len = sizeof(sysparas.price[15]);
            break;

        case 23://单价16
            addroffset = (uint16_t)&sysparas.price[16] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[16];
            len = sizeof(sysparas.price[16]);
            break;

        case 24://单价17
            addroffset = (uint16_t)&sysparas.price[17] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[17];
            len = sizeof(sysparas.price[17]);
            break;

        case 25://单价18
            addroffset = (uint16_t)&sysparas.price[18] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[18];
            len = sizeof(sysparas.price[18]);
            break;

        case 26://单价19
            addroffset = (uint16_t)&sysparas.price[19] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[19];
            len = sizeof(sysparas.price[19]);
            break;

        case 27://流量计类型
            addroffset = (uint16_t)&sysparas.modbustype - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbustype;
            len = sizeof(sysparas.modbustype);
            break;

        case 28://计量单位
            addroffset = (uint16_t)&sysparas.unit - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.unit;
            len = sizeof(sysparas.unit);
            break;

        case 29://质检模式
            addroffset = (uint16_t)&sysparas.qcmode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.qcmode;
            len = sizeof(sysparas.qcmode);
            break;

        case 30://密度
            addroffset = (uint16_t)&sysparas.densty - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.densty;
            len = sizeof(sysparas.densty);
            break;

        case 31://修正系数
            addroffset = (uint16_t)&sysparas.coeff - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.coeff;
            len = sizeof(sysparas.coeff);
            break;

        case 32://压力零点
            addroffset = (uint16_t)&sysparas.pzero - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pzero;
            len = sizeof(sysparas.pzero);
            break;

        case 33://压力系数
            addroffset = (uint16_t)&sysparas.pcoeff - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pcoeff;
            len = sizeof(sysparas.pcoeff);
            break;

        case 34://修改记录流水号
            addroffset = (uint16_t)&sysparas.modnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modnum;
            len = sizeof(sysparas.modnum);
            break;

        case 35://已传修改记录流水号
            addroffset = (uint16_t)&sysparas.upmodnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.upmodnum;
            len = sizeof(sysparas.upmodnum);
            break;

        case 36://最大班号
            addroffset = (uint16_t)&sysparas.shiftmaxnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftmaxnum;
            len = sizeof(sysparas.shiftmaxnum);
            break;

        case 37://班号
            addroffset = (uint16_t)&sysparas.shiftnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftnum;
            len = sizeof(sysparas.shiftnum);
            /************************手动换班 开始****************************/
            //上一班的开始时间
            memcpy(&shiftrecordinfo.classstarttime[0], &sysparas.shiftstarttime[0], 6);
            //上一班结束时间
            Ds3232ReadTime();
            memcpy(&sysparas.shiftendtime[0], &time, 6);
            memcpy(&shiftrecordinfo.classendtime[0], &sysparas.shiftendtime, 6);
            //下一班开始时间
            memcpy(&sysparas.shiftstarttime[0], &sysparas.shiftendtime[0], 6);
            Exshiftid(tmp_sysparas.shiftnum);
            /************************手动换班 结束****************************/
            break;

        case 38://班流水号
            addroffset = (uint16_t)&sysparas.shiftransnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftransnum;
            len = sizeof(sysparas.shiftransnum);
            break;

        case 39://已传班流水号
            addroffset = (uint16_t)&sysparas.shiftuptransnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftuptransnum;
            len = sizeof(sysparas.shiftuptransnum);
            break;

        case 40://班开始时间
            addroffset = (uint16_t)&sysparas.shiftstarttime[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftstarttime[0];
            len = 6;
            break;

        case 41://班结束时间
            addroffset = (uint16_t)&sysparas.shiftendtime[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftendtime[0];
            len = 6;
            break;

        case 42://班开始枪累
            addroffset = (uint16_t)&sysparas.shiftstartvol - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftstartvol;
            len = sizeof(sysparas.shiftstartvol);
            break;

        case 43://班结束枪累
            addroffset = (uint16_t)&sysparas.shiftendvol - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftendvol;
            len = sizeof(sysparas.shiftendvol);
            break;

        case 44://班开始金额
            addroffset = (uint16_t)&sysparas.shiftstartmon - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftstartmon;
            len = sizeof(sysparas.shiftstartmon);
            break;

        case 45://班合计加气量
            addroffset = (uint16_t)&sysparas.shiftvolsum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftvolsum;
            len = sizeof(sysparas.shiftvolsum);
            break;

        case 46://班合计气量
            addroffset = (uint16_t)&sysparas.shiftotvol - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftotvol;
            len = sizeof(sysparas.shiftotvol);
            break;

        case 47://班合计金额
            addroffset = (uint16_t)&sysparas.shiftotmon - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftotmon;
            len = sizeof(sysparas.shiftotmon);
            break;

        case 48://班实收现金
            addroffset = (uint16_t)&sysparas.shiftrealmoney - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftrealmoney;
            len = sizeof(sysparas.shiftrealmoney);
            break;

        case 49://班加气次数
            addroffset = (uint16_t)&sysparas.shiftfueltimes - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftfueltimes;
            len = sizeof(sysparas.shiftfueltimes);
            break;

        case 50://省编码
            addroffset = (uint16_t)&sysparas.shengcode[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shengcode[0];
            len = 2;
            break;

        case 51://站编码1
            addroffset = (uint16_t)&sysparas.stationcodearray[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.stationcodearray[0];
            len = 4;
            break;

        case 52://站编码2
            addroffset = (uint16_t)&sysparas.stationcode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.stationcode;
            len = sizeof(sysparas.stationcode);
            break;

        case 53://地市代码
            addroffset = (uint16_t)&sysparas.citycode[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.citycode[0];
            len = 2;
            break;

        case 54://上级单位代码
            addroffset = (uint16_t)&sysparas.parentcode[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.parentcode[0];
            len = 4;
            break;

        case 55://储蓄卡余额上限
            addroffset = (uint16_t)&sysparas.cardmaxyue - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardmaxyue;
            len = sizeof(sysparas.cardmaxyue);
            break;

        case 56://储蓄卡余额下限
            addroffset = (uint16_t)&sysparas.cardminyue - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardminyue;
            len = sizeof(sysparas.cardminyue);
            break;

        case 57://记账卡余额下限
            addroffset = (uint16_t)&sysparas.accountcardmaxyue - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.accountcardmaxyue;
            len = sizeof(sysparas.accountcardmaxyue);
            break;

        case 58://PSAM卡卡座选择
            addroffset = (uint16_t)&sysparas.cardpsamnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardpsamnum;
            len = sizeof(sysparas.cardpsamnum);
            break;

        case 59://卡未使用最大间隔天数
            addroffset = (uint16_t)&sysparas.cardmaxday - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardmaxday;
            len = sizeof(sysparas.cardmaxday);
            break;

        case 60://是否检验钢瓶
            addroffset = (uint16_t)&sysparas.cardcheck - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardcheck;
            len = sizeof(sysparas.cardcheck);
            break;

        case 61://是否用卡
            addroffset = (uint16_t)&sysparas.usecard - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.usecard;
            len = sizeof(sysparas.usecard);
            break;

        case 62://流量计波特率
            addroffset = (uint16_t)&sysparas.modbusbaud - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbusbaud;
            len = sizeof(sysparas.modbusbaud);
            break;

        case 63://后台波特率
            addroffset = (uint16_t)&sysparas.pcbaud - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pcbaud;
            len = sizeof(sysparas.pcbaud);
            break;

        case 64://加气模式
            addroffset = (uint16_t)&sysparas.fuelmode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.fuelmode;
            len = sizeof(sysparas.fuelmode);
            break;

        case 65://小票打印方式
            addroffset = (uint16_t)&sysparas.printmode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.printmode;
            len = sizeof(sysparas.printmode);
            break;

        case 66://单价版本号
            addroffset = (uint16_t)&sysparas.pricever - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pricever;
            len = sizeof(sysparas.pricever);
            break;

        case 68://通用信息版本号
            addroffset = (uint16_t)&sysparas.comminfover - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.comminfover;
            len = sizeof(sysparas.comminfover);
            break;

        case 69://密钥版本号
            addroffset = (uint16_t)&sysparas.lockver - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.lockver;
            len = sizeof(sysparas.lockver);
            break;

        case 70://菜单进入密码
            addroffset = (uint16_t)&sysparas.menupwd - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.menupwd;
            len = sizeof(sysparas.menupwd);
            break;

        case 71://质检密码
            addroffset = (uint16_t)&sysparas.qcpwd - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.qcpwd;
            len = sizeof(sysparas.qcpwd);
            break;

        case 72://测试密码
            addroffset = (uint16_t)&sysparas.testpwd - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.testpwd;
            len = sizeof(sysparas.testpwd);
            break;

        case 73://恢复出厂密码
            addroffset = (uint16_t)&sysparas.resetpwd - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.resetpwd;
            len = sizeof(sysparas.resetpwd);
            break;

        case 74://加密板数据
            addroffset = (uint16_t)&sysparas.pwdboard[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pwdboard[0];
            len = 6;
            break;

        case 75://是否使用加密板
            addroffset = (uint16_t)&sysparas.usepwdboard - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.usepwdboard;
            len = sizeof(sysparas.usepwdboard);
            break;

        case 76://联机模式
            addroffset = (uint16_t)&sysparas.offlinemode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.offlinemode;
            len = sizeof(sysparas.offlinemode);
            break;

        case 77://屏幕选择
            addroffset = (uint16_t)&sysparas.lcdtype - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.lcdtype;
            len = sizeof(sysparas.lcdtype);
            break;

        case 78://加气模式
            addroffset = (uint16_t)&sysparas.simulation - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.simulation;
            len = sizeof(sysparas.simulation);
            break;

        case 79://是否取整
            addroffset = (uint16_t)&sysparas.intnumflag - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.intnumflag;
            len = sizeof(sysparas.intnumflag);
            break;

        case 80://单次最大加气量
            addroffset = (uint16_t)&sysparas.maxvolumeonce - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.maxvolumeonce;
            len = sizeof(sysparas.maxvolumeonce);
            break;

        case 81://单次最大加气金额
            addroffset = (uint16_t)&sysparas.maxmoneyonce - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.maxmoneyonce;
            len = sizeof(sysparas.maxmoneyonce);
            break;

        case 82://设备类型
            addroffset = (uint16_t)&sysparas.devicetype - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.devicetype;
            len = sizeof(sysparas.devicetype);
            break;

        case 83://异常流水号
            addroffset = (uint16_t)&sysparas.err_id - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.err_id;
            len = sizeof(sysparas.err_id);
            break;

        case 84://已传异常流水号
            addroffset = (uint16_t)&sysparas.uperr_id - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.uperr_id;
            len = sizeof(sysparas.uperr_id);
            break;
            
        case 85://是否打印车牌号
            addroffset = (uint16_t)&sysparas.IsPrintCarnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.IsPrintCarnum;
            len = sizeof(sysparas.IsPrintCarnum);
            break;

        case 201://是否回气计量
            addroffset = (uint16_t)&sysparas.backgascalc - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.backgascalc;
            len = sizeof(sysparas.backgascalc);
            break;

        case 202://预冷方式
            addroffset = (uint16_t)&sysparas.precoolmode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precoolmode;
            len = sizeof(sysparas.precoolmode);
            break;

        case 203://预冷临界温度
            addroffset = (uint16_t)&sysparas.precooltemp - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precooltemp;
            len = sizeof(sysparas.precooltemp);
            break;

        case 204://预冷临界增益
            addroffset = (uint16_t)&sysparas.precoolgain - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precoolgain;
            len = sizeof(sysparas.precoolgain);
            break;

        case 205://预冷临界密度
            addroffset = (uint16_t)&sysparas.precooldensty - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precooldensty;
            len = sizeof(sysparas.precooldensty);
            break;

        case 206://预冷超时时间
            addroffset = (uint16_t)&sysparas.precooltimeout - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precooltimeout;
            len = sizeof(sysparas.precooltimeout);
            break;

        case 207://停机延时时间
            addroffset = (uint16_t)&sysparas.delaystoptime - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.delaystoptime;
            len = sizeof(sysparas.delaystoptime);
            break;

        case 208://低流速停机
            addroffset = (uint16_t)&sysparas.lowratestop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.lowratestop;
            len = sizeof(sysparas.lowratestop);
            break;

        case 209://高流速停机
            addroffset = (uint16_t)&sysparas.highratestop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.highratestop;
            len = sizeof(sysparas.highratestop);
            break;

        case 210://回气密度停机
            addroffset = (uint16_t)&sysparas.backdenstystop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.backdenstystop;
            len = sizeof(sysparas.backdenstystop);
            break;

        case 211://回气增益停机
            addroffset = (uint16_t)&sysparas.backgainstop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.backgainstop;
            len = sizeof(sysparas.backgainstop);
            break;

        case 212://压力停机
            addroffset = (uint16_t)&sysparas.pressurestop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pressurestop;
            len = sizeof(sysparas.pressurestop);
            break;

        case 213://加液后延时停泵
            addroffset = (uint16_t)&sysparas.stoppumptime - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.stoppumptime;
            len = sizeof(sysparas.stoppumptime);
            break;

        case 214://重新预冷延时
            addroffset = (uint16_t)&sysparas.precoolfinishedtime - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precoolfinishedtime;
            len = sizeof(sysparas.precoolfinishedtime);
            break;

        case 215://气相流量计地址
            addroffset = (uint16_t)&sysparas.modbusaddr_g - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbusaddr_g;
            len = sizeof(sysparas.modbusaddr_g);
            break;

        case 216://液相流量计地址
            addroffset = (uint16_t)&sysparas.modbusaddr_l - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbusaddr_l;
            len = sizeof(sysparas.modbusaddr_l);
            break;

        case 217://流量计总线设置
            addroffset = (uint16_t)&sysparas.modbusRS485 - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbusRS485;
            len = sizeof(sysparas.modbusRS485);
            break;
			
        case 218://小票打印次数
            addroffset = (uint16_t)&sysparas.printtimes - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.printtimes;
            len = sizeof(sysparas.printtimes);
            break;

        default://当paraindex不是以上任何参数时，预防写错，将对应参数初始化为head的参数地址 ADD BY LY
            addroffset = 0;
            data = (uint8_t*)&sysparas.head;
            len = 1;
            break;
    }

    addr = SYSPARA_ADDR + addroffset;

    if(SaveEeprom(addr, data, len))
    {
        if(crcwrflag == 1)
        {
            if(SaveCrc())
            {
                ret = true;
            }
            else
            {
                ret = false;
            }
        }
        else
        {
            ret = true;
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

/************************************************************************
**	函数名称: SaveCrc(void)
**	函数功能:	更新系统参数中的CRC
**	输入参数: 无
**	返回值  	: 0: 保存失败
          	  1 :保存成功

**	创 建 者:	LY
**	创建日期:	2018-9-19
*************************************************************************/
uint8_t SaveCrc(void)
{
    uint16_t crc, addroffset;
	//计算sysparas的CRC
    crc = ModbusCrc16((uint8_t*)&sysparas, sizeof(sysparas) - 2);
    sysparas.crc = crc;
    addroffset = (uint32_t)&sysparas.crc - (uint32_t)&sysparas.head;
	//保存CRC
    if(SaveEeprom(SYSPARA_ADDR + addroffset, (uint8_t*)&sysparas.crc, sizeof(sysparas.crc)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/************************************************************************
**	函数名称: ExshiftidSaveParas(void)
**	函数功能:	保存换班时需要保存的参数
**	输入参数: 无
**	返回值  	: 0: 保存失败
          	  1 :保存成功

**	创 建 者:	LY
**	创建日期:	2018-9-19
*************************************************************************/
uint8_t ExshiftidSaveParas(void)
{
    uint16_t addr, len;
    //计算EEPROM保存的地址，从sysparas.shiftnum开始保存，此时不考虑地址翻页
    addr = SYSPARA_ADDR + abs((uint16_t)&sysparas.shiftnum - (uint16_t)&sysparas.head);

    //计算需要保存的长度
    len = abs((uint16_t)&sysparas.shengcode - (uint16_t)&sysparas.shiftnum);

    if(SaveEeprom(addr, (uint8_t *)&sysparas.shiftnum, len))
    {
        return SaveCrc();
    }
    else
    {
        return false;
    }
}

/************************************************************************
**	函数名称: StopFuelingSaveParas(void)
**	函数功能:	加液结束，保存保存流水时需要更新的数据
**	输入参数: 无
**	返回值  	: 0: 保存失败
          	  1 :保存成功

**	创 建 者:	YCX
**	创建日期:	2018-9-19
*************************************************************************/
uint8_t StopFuelingSaveParas(void)
{	
	//保存流水号
    SaveSignalSysparas(3, 0);			//sysparas.transnum
    //保存枪总气量
    SaveSignalSysparas(5, 0);			//sysparas.totvolume
    //保存枪总金额
    SaveSignalSysparas(6, 0);			//sysparas.totmoney
    //保存班合计气量
    SaveSignalSysparas(46, 0);			//sysparas.shiftotvol
    //保存班合计金额
    SaveSignalSysparas(47, 0); 			//sysparas.shiftotmon
    //保存班加气次数，同时更新CRC
    return SaveSignalSysparas(49, 1); 	//sysparas.shiftfueltimes
}

/************************************************************************
**	函数名称: ReadSysparas(uint16_t addr, uint8_t *data, uint16_t num)
**	函数功能:	从EEPROM（内外）中读取系统参数
**	输入参数: addr:	开始读的起始地址
			  data:	读取数据指针
			  num : 读取数据个数
**	返回值  	: 0: 读取失败
          	  1: 读取成功

**	创 建 者:	YCX
**	创建日期:	2016-4-19
*************************************************************************/
uint8_t ReadSysparas(uint16_t addr, uint8_t *data, uint16_t num)
{
    uint16_t    i;
    uint16_t crc;
    uint8_t     *t1, *t2, ret = false;
    Sysparas    tmp1, tmp2;

    t1 = (uint8_t*)&tmp1;
    t2 = (uint8_t*)&tmp2;

    if(EepromInterReadBytes(addr, t1, num))     //读内部eeprom正确
    {
        crc = ModbusCrc16(t1, num - 2);

        if(tmp1.crc == crc)	//CRC校验通过
        {
            ClrBit(syserr, SYSERR_R_IN_EEPROM);
            ret = true; //内部eeprom数据正确
        }
        else	//CRC校验失败
        {
            if(tmp1.writeE2num > 100000)
            {
                tmp1.writeE2num = 0; //防止首次读一块新板子，数据全为0xFF
            }

            SetBit(syserr, SYSERR_R_IN_EEPROM);
            ret = false; //内部eeprom数据错误

        }

        //即使CRC错误也赋值
        for(i = 0; i < num; i++)
        {
            data[i] = t1[i]; //将内部EEPROM中的参数读出存入data所指向的缓存中 BY LY
        }
    }

    if(ret == false)   //如果内部eeprom读错，再读外部eeprom
    {
        if(EepromExternalReadBytes(addr, t2, num))
        {
            crc = ModbusCrc16(t2, num - 2);

            if(tmp2.crc == crc)  //读外部eeprom正确 CRC校验通过
            {
                ClrBit(syserr, SYSERR_R_EX_EEPROM);
                ret = true; //外部eeprom数据正确
            }
            else //CRC校验失败
            {
                if(tmp2.writeE2num > 100000)
                    tmp2.writeE2num = 0; //防止首次读一块新板子，数据全为0xFF

                SetBit(syserr, SYSERR_R_EX_EEPROM);
                ret = false; //外部eeprom数据错误

            }
            //即使CRC错误也赋值
            for(i = 0; i < num; i++)
            {
                data[i] = t2[i]; //将外部EEPROM中的参数读出存入t2中 BY LY
            }
        }
    }

    return ret;
}

/************************************************************************
**	函数名称:	EepromDeviceTest(void)
**	函数功能:	外部EEPROM读写测试
**	输入参数: 无
**	返回值  	: 0: 读取失败
          	  1: 读取成功

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
bool EepromDeviceTest(void)
{
    uint8_t i = 0;
    bool result = false; //用于存储测试返回结果
    uint8_t WriteBuff[200], ReadBuff[200];

    for(i = 0; i < 100; i++)   //初始化数据缓冲区
    {
        WriteBuff[i] = i;
        ReadBuff[i]  = 0;
    }

    EepromExternalWriteBytes(EEPROM_TESTADDR, WriteBuff, 200);
    _delay_ms(5);   //等待写操作完成
    EepromExternalReadBytes(EEPROM_TESTADDR, ReadBuff, 200);
    _delay_ms(5);

    for(i = 0; i < 200; i++)
    {
        if(WriteBuff[i] != ReadBuff[i])
        {
            result = false;
            break;
        }
        else
        {
            result = true;
        }
    }


    return result;
}

