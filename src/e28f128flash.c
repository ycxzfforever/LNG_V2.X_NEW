#include "kernel.h"

/*
������������Numonyx��˾��JS28F128 FLASH
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
**	��������:	OS_ENTER_CRITICAL(void)
**	��������:	��ȫ��ȫ���ж�
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void OS_ENTER_CRITICAL(void)
{
    __disable_interrupt();
    ///Os_Enter_Sum++;
}

/************************************************************************
**	��������:	OS_EXIT_CRITICAL(void)
**	��������:	��ȫ��ȫ���ж�
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void OS_EXIT_CRITICAL(void)
{
    //if(--Os_Enter_Sum==0)
    __enable_interrupt();
}

/************************************************************************
**	��������:	JS28F128FlashInit(void)
**	��������:	JS28F128Flash ��ʼ��
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void JS28F128FlashInit(void)
{
    DDRJ  |= 0xBC;//AT14
    //DDRJ  |= 0xFC; //J0,J0 Ϊ����3
    DDRL  |= 0x40;
    DDRD  |= 0xF0;
    XMCRA |= 0x80;
    XMCRB  = 0x00;
    PORTJ |= 0x80; //��Flash��Ϊ��������ģʽ
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
    offsetaddr = ulAddr & 0x7FFF; //ȡ�õ�ַ�ĸ�λ
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
**	��������:	JS28F128FlashInit(void)
**	��������:	����JS28F128Flash ��ĳ����
**	�������: ulAddr����Ҫ�����Ŀ��ַ
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
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
    IODATA(offsetaddr) = 0xD0;   //ȷ�ϲ���

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
**	��������: JS28F128FlashWriteBuf(uint32_t wtAddr, uint32_t Cnt, uint8_t *buf)
**	��������:	������λ�ã���flashд�����ⳤ�ȵ�����
**	�������: wtAddr��д��������ʼ��ַ
			  Cnt��д�����ݵĳ���
			  buf������ָ�룬ָ��Ҫд��������׵�ַ
**	����ֵ  	: 0: д��ʧ��
          	  1 :д��ɹ�

**	�� �� ��:	HMM
**	��������:	2016-4-19
*************************************************************************/
bool JS28F128FlashWriteBuf(uint32_t wtAddr, uint32_t Cnt, uint8_t *buf)
{
    uint32_t i, j;
    bool ret;
    OS_ENTER_CRITICAL();

	//JS28F128 ��һ��bolck Ϊ131072 �ֽڼ�128K Byte����ַ��Χ��0x000000-0x01FFFF ADD BY LY
	//�ж�д���ַ�Ƿ񳬳�һ��block,�����һ���µ�block����Ҫ�Ȳ���	ADD BY LY
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
**	��������: JS28F128FlashEraseAll(void)
**	��������:	����flash�����п飨JS28F128 ֻ��һ��һ��ز� ADD BY LY��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	HMM
**	��������:	2016-4-19
*************************************************************************/
void JS28F128FlashEraseAll(void)
{
    uint8_t i, blocks;

    blocks = 128;

    FYD12864ClrScreen();
    FYD12864DispPrintfFlash(1, 1, "�������FLASH", 1);
    FYD12864DispPrintfFlash(2, 1, "���Ժ�...", 1);

    for(i = 0; i < blocks; i++)
    {
        if(!JS28F128FlshEraseBlock(i * 0x20000))
        {
            FYD12864DispPrintfFlash(2, 1, "error");
            return ;
        }

        FYD12864ClearLine(4);
        FYD12864DispPrintfFlash(4, 1, "  �������%02d", i);

        if(i == 128)
        {
            FYD12864DispPrintfFlash(4, 1, "FLASH�������");
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
**�������ƣ�SaveFuelRecord
**�������ܣ����������ˮ��¼
**��ڲ�����0-����ˮ��1-�����ˮ��
**�� �� ֵ��true:�ɹ���false:ʧ��
**�� �� �ߣ����
**�������ڣ�2016-4-26 14:12:09

**�޸��ߣ�
**�޸����ڣ�
**�޸����ݣ�
***************************************************************************/
uint8_t SaveFuelRecord(uint8_t select)
{
    uint8_t 	ret = false;
    uint8_t 	*pt, *pt1;
    uint32_t	addr, i, num, id;
    FuelRecordInfo	tmp1;
    ShiftRecordInfo tmp2;

    if(select == 0) 			//select=0 ,������ˮ��¼
    {
        num = sizeof(FuelRecordInfo);
        id = sysparas.transnum - 1; //��¼��ˮ�Ŵ�0��ʼ��
        pt = (uint8_t *)(&fuelrecordinfo.transnum);
        pt1 = (uint8_t *)(&tmp1.transnum);
        id = id % FlashRecMaxID;
        addr = FlashRecSADDR + id * FlashRecLen;
    }

    if(select == 1)		//select=1,�������ˮ��¼
    {
        num = sizeof(shiftrecordinfo);
        id = sysparas.shiftransnum - 1; //��¼�� ��ˮ�Ŵ�0��ʼ��
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
                break; //����������д�����ݲ�һ��
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
        SetBit(syserr, SYSERR_W_FLASH);   //дflash����
        ret = false;
    }

    return ret;
}

/***************************************************************************
**�������ƣ�ReadFuelRecord
**�������ܣ���������ˮ��¼
**��ڲ�����id:��ˮ��
**			select��0:������ˮ��1:����ˮ
**�� �� ֵ��true:�ɹ���false:ʧ��
**�� �� �ߣ����
**�������ڣ�2016-4-26 15:37:37

**�޸��ߣ�
**�޸����ڣ�
**�޸����ݣ�
***************************************************************************/
uint8_t ReadFuelRecord(uint32_t id, uint8_t select)
{
    uint8_t         ret = false;
    uint8_t			*pt1;
    uint16_t        crc;
    uint32_t        addr, num;

    FuelRecordInfo  tmp1;/*tmp1���ڶ�ȡ������ˮ ADD BY LY*/
    ShiftRecordInfo tmp2;/*tmp2���ڶ�ȡ����ˮ          ADD BY LY*/
	
	//��ȡ������ˮ��¼ ADD BY LY
    if(select == 0)   												
    {
        num = sizeof(FuelRecordInfo);
        pt1 = (uint8_t *)(&tmp1.transnum);
		//��ˮ����20000��������20000�����ִ�ͷ��ʼ����д�� ADD BY LY
        id = id % FlashRecMaxID; 		
		//������ˮ�ż��������ˮ����ʼ��ַ ADD BY LY
        addr = FlashRecSADDR + id * FlashRecLen; 					
    }
	
	//��ȡ���¼��ˮ BY LY
    if(select == 1)   												
    {
        num = sizeof(shiftrecordinfo);
        pt1 = (uint8_t *)(&tmp2.classid);
		//����ˮ����3072��������3072�����ִ�ͷ��ʼ����д�� ADD BY LY
        id = id % FlashShiftMaxID;
		//���ݰ���ˮ�ż����������ˮ����ʼ��ַ ADD BY LY
        addr = FlashShiftSADDR + id * FlashShiftLen;
    }

    JS28F128FlashReadBuf(addr, num, pt1);
    _delay_ms(10);
	//���������ˮ��CRC BY LY
    crc = ModbusCrc16(&pt1[0], num - 2);   							
	//CRCУ�� BY LY
    if((tmp1.crc == crc) || (tmp2.crc == crc))       				
    {
        ClrBit(syserr, SYSERR_R_FLASH);
        ret = true;
    }
    else
    {
    	//�����»������壬��û����ˮ��¼���ʶ�����������Ч��ֱ������ ADD BY LY
        if((tmp1.transnum == 0xFFFFFFFF) && (select == 0))     
		{
			//������ˮ�ṹ������
			memset(pt1, 0, sizeof(FuelRecordInfo));
		}
        //�����»������壬��û�а���ˮ��¼���ʶ�����������Ч��ֱ������ ADD BY LY      			
        else if((tmp2.classid == 0xFFFFFFFF) && (select == 1)) 
		{
			//����ˮ�ṹ������
			memset(pt1, 0, sizeof(ShiftRecordInfo)); 
		}     			
        else
        {
        	//�������»����壬������CRC�޷�ͨ��У�飬���ö�FLASH�����־ ADD BY LY
        	SetBit(syserr, SYSERR_R_FLASH); 
        }
              						
        ret = false;
    }

    //�����������ݸ�ֵ����ˮ�ṹ��
    if(select == 0)
    {
        fuelrecordinfo = tmp1;
    }
	//�����������ݸ�ֵ������ˮ�ṹ��
    else if(select == 1)
    {
        shiftrecordinfo = tmp2;
    }

    return ret;
}
