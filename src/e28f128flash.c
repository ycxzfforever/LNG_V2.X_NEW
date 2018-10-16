#include "kernel.h"

/*
下面的驱动针对Numonyx公司的JS28F128 FLASH
*/
typedef union
{
    uint16_t TVar;
    struct
    {
        uint8_t TVar_Bit0: 1,
                 TVar_Bit1: 1,
                 TVar_Bit2: 1,
                 TVar_Bit3: 1,
                 TVar_Bit4: 1,
                 TVar_Bit5: 1,
                 TVar_Bit6: 1,
                 TVar_Bit7: 1,

                 TVar_Bit8: 1,
                 TVar_Bit9: 1,
                 TVar_Bit10: 1,
                 TVar_Bit11: 1,
                 TVar_Bit12: 1,
                 TVar_Bit13: 1,
                 TVar_Bit14: 1,
                 TVar_Bit15: 1;
    };
} var_Bitctrl;

#define OFlash_Enable()
#define OFlash_Disable()

#define FLASHCHIPADDR                 0x8000
#define INTEL_MANUFACT 		      0x89
#define INTEL_ID_28F128J3A 	      0x18
#define CMD_RDID                      0x90
#define ERR_PROTECT                   0x01
#define ERR_BUSY                      0x02
#define OFLASH_DLY_CYCLES             20
#define OFLASH_BLOCKS                 128

#define FLASH_BUSY                    false
#define FLASH_READY                   true

#define IODATA  *(uint8_t *)

/************************************************************************
**	函数名称:	OS_ENTER_CRITICAL(void)
**	函数功能:	关全部全局中断
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void OS_ENTER_CRITICAL(void)
{
    __disable_interrupt();
    ///Os_Enter_Sum++;
}

/************************************************************************
**	函数名称:	OS_EXIT_CRITICAL(void)
**	函数功能:	开全部全局中断
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void OS_EXIT_CRITICAL(void)
{
    //if(--Os_Enter_Sum==0)
    __enable_interrupt();
}

/************************************************************************
**	函数名称:	JS28F128FlashInit(void)
**	函数功能:	JS28F128Flash 初始化
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void JS28F128FlashInit(void)
{
    DDRJ  |= 0xBC;//AT14
    //DDRJ  |= 0xFC; //J0,J0 为串口3
    DDRL  |= 0x40;
    DDRD  |= 0xF0;
    XMCRA |= 0x80;
    XMCRB  = 0x00;
    PORTJ |= 0x80; //将Flash置为正常工作模式
}

bool JS28F128FlashCheckId(void)
{
    volatile uint8_t mCode = 0, dCode = 0;

    OS_ENTER_CRITICAL();

    __delay_cycles(OFLASH_DLY_CYCLES);
    IODATA(FLASHCHIPADDR) = CMD_RDID;
    __delay_cycles(OFLASH_DLY_CYCLES);
    mCode = (uint8_t)IODATA(FLASHCHIPADDR);
    dCode = (uint8_t)IODATA(FLASHCHIPADDR + 2);

    if(INTEL_ID_28F128J3A == dCode && INTEL_MANUFACT == mCode)
    {
        IODATA(FLASHCHIPADDR) = 0xFF;
        __delay_cycles(OFLASH_DLY_CYCLES);
        OS_EXIT_CRITICAL();
        return true;
    }

    IODATA(FLASHCHIPADDR) = 0xFF;
    __delay_cycles(OFLASH_DLY_CYCLES);
    OS_EXIT_CRITICAL();
    OFlash_Disable();
    return false;
}

bool JS28F128FlashCheckStatus(void)
{
    volatile uint8_t rd;
    volatile uint16_t  nCnt = 1000;

    while(nCnt--)
    {
        IODATA(FLASHCHIPADDR) = 0x70;
        __delay_cycles(OFLASH_DLY_CYCLES);
        rd = (uint8_t)IODATA(FLASHCHIPADDR);

        if(rd & (1 << 7))     //bit7: busy/ready
        {
            IODATA(FLASHCHIPADDR) = 0x70;
            __delay_cycles(OFLASH_DLY_CYCLES);
            rd = (uint8_t)IODATA(FLASHCHIPADDR);

            if(rd == 0x80) return true;

            return false;
        }

        __delay_cycles(8000);   //_delay_ms(1);
    }

    return false;
}

void JS28F128FlashSetPage(uint32_t ulAddr)
{
    var_Bitctrl vBctrl ;

    vBctrl.TVar = (ulAddr & 0xFF8000) >> 15;
    PORTJ &= 0xC3; //PORTJ_BIT2/3/4/5 =>AT15/16/17/18

    if(vBctrl.TVar_Bit0 == 1)  PORTJ_PORTJ5 = 1;

    if(vBctrl.TVar_Bit1 == 1)  PORTJ_PORTJ4 = 1;

    if(vBctrl.TVar_Bit2 == 1)  PORTJ_PORTJ3 = 1;

    if(vBctrl.TVar_Bit3 == 1)  PORTJ_PORTJ2 = 1;

    if(vBctrl.TVar_Bit4 == 1)  PORTL_PORTL6 = 1;
    else                       PORTL_PORTL6 = 0;

    PORTD &= 0x0F; //PORTD_BIT4/5/6/7 => AT20/21/22/23

    if(vBctrl.TVar_Bit5 == 1)  PORTD_PORTD4 = 1;

    if(vBctrl.TVar_Bit6 == 1)  PORTD_PORTD5 = 1;

    if(vBctrl.TVar_Bit7 == 1)  PORTD_PORTD6 = 1;

    if(vBctrl.TVar_Bit8 == 1)  PORTD_PORTD7 = 1;
}

bool JS28F128FlashUnProtect(uint32_t ulAddr)
{
    volatile uint16_t offsetaddr;

    if(ulAddr > 0xFFFFFF)  return false;

    OFlash_Enable();
    JS28F128FlashSetPage(ulAddr);
    offsetaddr = ulAddr & 0x7FFF; //取得地址的高位
    offsetaddr |= FLASHCHIPADDR;
    IODATA(offsetaddr) = 0x50;   //Clear Status Register
    __delay_cycles(OFLASH_DLY_CYCLES);
    IODATA(offsetaddr) = 0x60;
    __delay_cycles(OFLASH_DLY_CYCLES);
    IODATA(offsetaddr) = 0xD0;
    __delay_cycles(OFLASH_DLY_CYCLES);

    if(JS28F128FlashCheckStatus() == false)
    {
        IODATA(FLASHCHIPADDR) = 0xB0;
        __delay_cycles(OFLASH_DLY_CYCLES);
        return false;
    }

    IODATA(FLASHCHIPADDR) = 0xFF;
    __delay_cycles(OFLASH_DLY_CYCLES);
    return true;
}

/************************************************************************
**	函数名称:	JS28F128FlashInit(void)
**	函数功能:	擦除JS28F128Flash 的某个块
**	输入参数: ulAddr：需要擦除的块地址
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
bool JS28F128FlshEraseBlock(uint32_t ulAddr)
{
    volatile uint16_t offsetaddr;
    uint32_t i;

//  OS_ENTER_CRITICAL();
    if(!JS28F128FlashUnProtect(ulAddr))
    {
//    OS_EXIT_CRITICAL();
        return false;
    }

    JS28F128FlashSetPage(ulAddr);
    offsetaddr = ulAddr & 0x7FFF;
    offsetaddr |= FLASHCHIPADDR;

    IODATA(offsetaddr) = 0x50;   //clear status reg
    __delay_cycles(OFLASH_DLY_CYCLES);
    IODATA(offsetaddr) = 0x20;
    __delay_cycles(OFLASH_DLY_CYCLES);
    IODATA(offsetaddr) = 0xD0;   //确认擦除

    for(i = 0; i < 5000; i++)
    {
//    JXing485Net();
    }

//  __delay_cycles(8000000);
    if(!JS28F128FlashCheckStatus())
    {
        IODATA(offsetaddr) = 0xB0;
        __delay_cycles(OFLASH_DLY_CYCLES);
//    OS_EXIT_CRITICAL();
        return false;
    }

    IODATA(offsetaddr) = 0xFF;
    __delay_cycles(OFLASH_DLY_CYCLES);
//  OS_EXIT_CRITICAL();
    return true;
}

bool JS28F128FlashWriteChar(uint32_t ulAddr, uint8_t ucData)
{
    volatile uint16_t offsetaddr;
    volatile bool ret;

    // OFlash_Enable();
    JS28F128FlashSetPage(ulAddr);
    offsetaddr = ulAddr & 0x7FFF;
    offsetaddr |= FLASHCHIPADDR;

    IODATA(offsetaddr) = 0x40;
    __delay_cycles(OFLASH_DLY_CYCLES);
    IODATA(offsetaddr) = ucData;
    __delay_cycles(OFLASH_DLY_CYCLES);
    ret = JS28F128FlashCheckStatus();
    IODATA(offsetaddr) = 0xFF;
    __delay_cycles(OFLASH_DLY_CYCLES);
    return ret;
}

uint8_t JS28F128FlashReadChar(uint32_t ulAddr)
{
    uint8_t rd;
    uint16_t offsetaddr;
    OFlash_Enable();
    JS28F128FlashSetPage(ulAddr);
    offsetaddr = ulAddr & 0x7FFF;
    offsetaddr |= FLASHCHIPADDR;
    IODATA(offsetaddr) = 0xFF;
    __delay_cycles(OFLASH_DLY_CYCLES);
    rd = (uint8_t)IODATA(offsetaddr);
    OFlash_Disable();
    return rd;
}

void JS28F128FlashReadBuf(uint32_t rdAddr, uint32_t Cnt, uint8_t *buf)
{
    uint16_t i;
    OS_ENTER_CRITICAL();

    for(i = 0; i < Cnt; i++)
        *(buf + i) = JS28F128FlashReadChar(rdAddr + i);

    OS_EXIT_CRITICAL();
}

/************************************************************************
**	函数名称: JS28F128FlashWriteBuf(uint32_t wtAddr, uint32_t Cnt, uint8_t *buf)
**	函数功能:	从任意位置，向flash写入任意长度的数据
**	输入参数: wtAddr：写入数据起始地址
			  Cnt：写入数据的长度
			  buf：数据指针，指向要写入的数据首地址
**	返回值  	: 0: 写入失败
          	  1 :写入成功

**	创 建 者:	HMM
**	创建日期:	2016-4-19
*************************************************************************/
bool JS28F128FlashWriteBuf(uint32_t wtAddr, uint32_t Cnt, uint8_t *buf)
{
    uint32_t i, j;
    bool ret;
    OS_ENTER_CRITICAL();

	//JS28F128 的一个bolck 为131072 字节即128K Byte，地址范围：0x000000-0x01FFFF ADD BY LY
	//判断写入地址是否超出一个block,如果是一个新的block则需要先擦除	ADD BY LY
    if((wtAddr & 0x01FFFF) == 0)
    {
        if(!JS28F128FlshEraseBlock(wtAddr))
        {
            OS_EXIT_CRITICAL();
            return false;
        }
    }

    ret = true;

    for(i = 0; i < Cnt; i++)
    {
        for(j = 0; j < 3; j++)
        {
            if(JS28F128FlashWriteChar(wtAddr + i, *(buf + i)));

            if(JS28F128FlashReadChar(wtAddr + i) == *(buf + i))
            {
                ret = true;
                break;
            }
            else
            {
                ret = false;
            }
        }

        if(ret == false)
        {
            OS_EXIT_CRITICAL();
            return false;
        }
    }

    OS_EXIT_CRITICAL();
    return ret;
}

/************************************************************************
**	函数名称: JS28F128FlashEraseAll(void)
**	函数功能:	擦除flash的所有块（JS28F128 只能一块一块地擦 ADD BY LY）
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	HMM
**	创建日期:	2016-4-19
*************************************************************************/
void JS28F128FlashEraseAll(void)
{
    uint8_t i, blocks;

    blocks = 128;

    FYD12864ClrScreen();
    FYD12864DispPrintfFlash(1, 1, "正在清除FLASH", 1);
    FYD12864DispPrintfFlash(2, 1, "请稍后...", 1);

    for(i = 0; i < blocks; i++)
    {
        if(!JS28F128FlshEraseBlock(i * 0x20000))
        {
            FYD12864DispPrintfFlash(2, 1, "error");
            return ;
        }

        FYD12864ClearLine(4);
        FYD12864DispPrintfFlash(4, 1, "  正在清除%02d", i);

        if(i == 128)
        {
            FYD12864DispPrintfFlash(4, 1, "FLASH擦除完成");
        }

//   SecDisplayNotice(1,i);
    }

    __delay_cycles(800000);

    DispUI = DispUnitQcReset;
}

bool FlashWriteBuffer(uint32_t ulAddr, uint32_t Cnt, uint8_t *buf)
//cnt<=64
{
    volatile uint16_t offsetaddr;
    volatile bool ret;
    uint8_t i;
    uint8_t tmp;


    JS28F128FlashSetPage(ulAddr);
    offsetaddr = ulAddr & 0x7FFF;
    offsetaddr |= FLASHCHIPADDR;

    IODATA(offsetaddr) = 0xe8;   //write buffer command
    __delay_cycles(10);
    IODATA(offsetaddr) = 2 * (Cnt >> 1) - 1;
    __delay_cycles(10);

    for(i = 0; i < Cnt; i++)
    {
        IODATA(offsetaddr + i) = *(buf + i);
        __delay_cycles(10);
    }

    IODATA(offsetaddr) = 0xD0;   //confirm

    if(JS28F128FlashCheckStatus())
    {
        IODATA(FLASHCHIPADDR) = 0x70;
        __delay_cycles(10);
        tmp = (uint8_t)IODATA(FLASHCHIPADDR);

//   PrinterPrintf("the value tmp is:%x\r\n",tmp);
        if((tmp & 0x30) == 0)
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
        ret = false;
    }

    return ret;
}


/***************************************************************************
**函数名称：SaveFuelRecord
**函数功能：保存加气流水记录
**入口参数：0-存流水；1-存班流水。
**返 回 值：true:成功，false:失败
**创 建 者：杨朝旭
**创建日期：2016-4-26 14:12:09

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SaveFuelRecord(uint8_t select)
{
    uint8_t 	ret = false;
    uint8_t 	*pt, *pt1;
    uint32_t	addr, i, num, id;
    FuelRecordInfo	tmp1;
    ShiftRecordInfo tmp2;

    if(select == 0) 			//select=0 ,保存流水记录
    {
        num = sizeof(FuelRecordInfo);
        id = sysparas.transnum - 1; //记录流水号从0开始的
        pt = (uint8_t *)(&fuelrecordinfo.transnum);
        pt1 = (uint8_t *)(&tmp1.transnum);
        id = id % FlashRecMaxID;
        addr = FlashRecSADDR + id * FlashRecLen;
    }

    if(select == 1)		//select=1,保存班流水记录
    {
        num = sizeof(shiftrecordinfo);
        id = sysparas.shiftransnum - 1; //记录班 流水号从0开始的
        pt = (uint8_t *)(&shiftrecordinfo.classid);
        pt1 = (uint8_t *)(&tmp2.classid);
        id = id % FlashShiftMaxID;
        addr = FlashShiftSADDR + id * FlashShiftLen;
    }

    if(JS28F128FlashWriteBuf(addr, num, pt))
    {
        _delay_ms(5);
        JS28F128FlashReadBuf(addr, num, pt1);

        for(i = 0; i < num; i++)
        {
            if(pt1[i] != pt[i])
                break; //读出数据与写入数据不一致
        }

        if(i == num)
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
        SetBit(syserr, SYSERR_W_FLASH);   //写flash错误
        ret = false;
    }

    return ret;
}

/***************************************************************************
**函数名称：ReadFuelRecord
**函数功能：读加气流水记录
**入口参数：id:流水号
**			select：0:加气流水；1:班流水
**返 回 值：true:成功，false:失败
**创 建 者：杨朝旭
**创建日期：2016-4-26 15:37:37

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ReadFuelRecord(uint32_t id, uint8_t select)
{
    uint8_t         ret = false;
    uint8_t			*pt1;
    uint16_t        crc;
    uint32_t        addr, num;

    FuelRecordInfo  tmp1;/*tmp1用于读取加气流水 ADD BY LY*/
    ShiftRecordInfo tmp2;/*tmp2用于读取班流水          ADD BY LY*/
	
	//读取加气流水记录 ADD BY LY
    if(select == 0)   												
    {
        num = sizeof(FuelRecordInfo);
        pt1 = (uint8_t *)(&tmp1.transnum);
		//流水最大存20000条，超过20000条，又从头开始覆盖写入 ADD BY LY
        id = id % FlashRecMaxID; 		
		//根据流水号计算该条流水的起始地址 ADD BY LY
        addr = FlashRecSADDR + id * FlashRecLen; 					
    }
	
	//读取班记录流水 BY LY
    if(select == 1)   												
    {
        num = sizeof(shiftrecordinfo);
        pt1 = (uint8_t *)(&tmp2.classid);
		//班流水最大存3072条，超过3072条，又从头开始覆盖写入 ADD BY LY
        id = id % FlashShiftMaxID;
		//根据班流水号计算该条班流水的起始地址 ADD BY LY
        addr = FlashShiftSADDR + id * FlashShiftLen;
    }

    JS28F128FlashReadBuf(addr, num, pt1);
    _delay_ms(10);
	//计算读出流水的CRC BY LY
    crc = ModbusCrc16(&pt1[0], num - 2);   							
	//CRC校验 BY LY
    if((tmp1.crc == crc) || (tmp2.crc == crc))       				
    {
        ClrBit(syserr, SYSERR_R_FLASH);
        ret = true;
    }
    else
    {
    	//若是新换的主板，因没有流水记录，故读出的数据无效，直接清零 ADD BY LY
        if((tmp1.transnum == 0xFFFFFFFF) && (select == 0))     
		{
			//加气流水结构体清零
			memset(pt1, 0, sizeof(FuelRecordInfo));
		}
        //若是新换的主板，因没有班流水记录，故读出的数据无效，直接清零 ADD BY LY      			
        else if((tmp2.classid == 0xFFFFFFFF) && (select == 1)) 
		{
			//班流水结构体清零
			memset(pt1, 0, sizeof(ShiftRecordInfo)); 
		}     			
        else
        {
        	//若不是新换主板，读出的CRC无法通过校验，设置读FLASH错误标志 ADD BY LY
        	SetBit(syserr, SYSERR_R_FLASH); 
        }
              						
        ret = false;
    }

    //将读出的数据赋值给流水结构体
    if(select == 0)
    {
        fuelrecordinfo = tmp1;
    }
	//将读出的数据赋值给班流水结构体
    else if(select == 1)
    {
        shiftrecordinfo = tmp2;
    }

    return ret;
}
