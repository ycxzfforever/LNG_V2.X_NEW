#include "kernel.h"

/************************************************************************
**	��������:	EepromInterReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
**	��������:	���ڲ�EERPROM��ָ����ַ��ȡָ�����������ݣ�һ��һ���ض�
**	�������: nID:		 ��ȡ���ݵ��׵�ַ
			  rdbuf�����������ݴ�����׵�ַ
			  cnt��	 ��ȡ���ݵĸ���
**	����ֵ		��true:	 ���ɹ�
			  false: ��ʧ��

**	�� �� ��:	HMM
**	��������:	2016-04-25
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
**	��������:	EepromInterWriteBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
**	��������:	���ڲ�EERPOM��ָ����ַд��ָ�����������ݣ�һ��һ����д
**	�������: nID:		 ��д�����ݵ��׵�ַ
			  wtbuf�������д�����ݵ��׵�ַ
			  cnt��	 ��д�����ݵĸ���
**	����ֵ		��true:	 д�ɹ�
			  false: дʧ��

**	�� �� ��:	HMM
**	��������:	2016-04-25
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
**	��������:	EepromExternalWriteBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
**	��������:	���ⲿEERPOM��ָ����ַд��ָ�����������ݣ�һ��һ����д
**	�������: nID:		 ��д�����ݵ��׵�ַ
			  wtbuf�������д�����ݵ��׵�ַ
			  cnt��	 ��д�����ݵĸ���
**	����ֵ		��true:	 д�ɹ�
			  false: дʧ��

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
bool EepromExternalWriteBytes(uint16_t nID,  uint8_t* wtbuf, uint16_t cnt)
//���ܣ�ֻдֵ���ⲿEEPROM
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
**	��������:	EepromExternalPageWriteBytes(uint16_t nID, uint8_t* wtbuf, uint16_t cnt)
**	��������:	���ⲿEERPROM��ָ����ַд��ָ�����������ݣ���ҳ�жϣ�������һ��һ����д�ģ������Ż�
**	�������: nID:		 ��д�����ݵ��׵�ַ
			  wtbuf�������д�����ݵ��׵�ַ
			  cnt��	 ��д�����ݵĸ���
**	����ֵ		��true:	 д�ɹ�
			  false: дʧ��

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
bool EepromExternalPageWriteBytes(uint16_t nID, uint8_t* wtbuf, uint16_t cnt)
{
    uint8_t pagesize = 0x3F; //AT24C128 ÿҳ 64�ֽڣ�ҳ��ַ��(0-0x3F)
    uint16_t pageoffset, pagenum; //pageoffset ҳƫ�Ƶ�ַ  pagenum ҳ��
    uint32_t startaddr;//д�����ݵ�ַ
    uint32_t addr;
    uint32_t count = 0; //�Ѿ�д�����ݸ���

    addr = nID;
    startaddr = nID;
    pageoffset = (uint8_t)(addr & pagesize);     //���д���ַ��ĳҳ�е�λ��

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
**	��������:	EepromExternalReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
**	��������:	���ⲿEERPROM��ָ����ַ��ȡָ�����������ݣ�һ��һ���ض�
**	�������: nID:		 ��ȡ���ݵ��׵�ַ
			  rdbuf�����������ݴ�����׵�ַ
			  cnt��	 ��ȡ���ݵĸ���
**	����ֵ		��true:	 ���ɹ�
			  false: ��ʧ��

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
bool EepromExternalReadBytes(uint16_t nID,  uint8_t* rdbuf, uint16_t cnt)
//���ܣ�ֻдֵ���ⲿEEPROM
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
**	��������:	SaveEeprom(uint16_t addr, uint8_t *data, uint16_t num)
**	��������:	�����ݱ��浽�ڲ�EEPROM���ⲿEEPROM
**	�������: addr:	д��EEPROM����ʼ��ַ
			  data:	����ָ��,ָ��д�������
			  cnt��	num :д�����ݸ���
**	����ֵ  	: false:����ʧ��
          	  true :����ɹ�

**	�� �� ��:	YCX
**	��������:	2016-12-9
*************************************************************************/
uint8_t SaveEeprom(uint16_t addr, uint8_t *data, uint16_t num)
{
    uint8_t     *t1, *t2;
    uint16_t    i;
    uint8_t 	ret = false;
    Sysparas    tmp1, tmp2;

    t1 = (uint8_t*)&tmp1;
    t2 = (uint8_t*)&tmp2;

    if(EepromInterWriteBytes(addr, data, num))     //д�ڲ�eeprom��ȷ
    {
        ClrBit(syserr, SYSERR_W_IN_EEPROM);

        if(EepromExternalWriteBytes(addr, data, num))     //д�ⲿeeprom��ȷ
        {
            ClrBit(syserr, SYSERR_W_EX_EEPROM);

            if(EepromInterReadBytes(addr, t1, num))     //���ڲ�eeprom��ȷ
            {

                if(EepromExternalReadBytes(addr, t2, num))     //���ⲿeeprom��ȷ
                {

                    for(i = 0; i < num; i++)  //���ڲ�EEPROM���ⲿEEPROMд������ݶ����ȶ�
                    {
                        if(t1[i] != t2[i])  //���κ�һ�������˳�
                            break;
                    }

                    if(i == num)  //ȫ����ȷ ����1
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
            SetBit(syserr, SYSERR_W_EX_EEPROM);   //д�ⲿeeprom����ϵͳ����0x40��
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
**	��������:	SaveSysparas(uint16_t addr, uint8_t *data, uint16_t num)
**	��������:	��ϵͳ�������浽�ڲ�EEPROM���ⲿEEPROM
**	�������: addr:	д��EEPROM����ʼ��ַ
			  data:	����ָ��,ָ��д�������
			  cnt��	num :д�����ݸ���
**	����ֵ  	: 0:����ʧ��
          	  1 :����ɹ�

**	�� �� ��:	YCX
**	��������:	2016-12-9
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
    else//��eepromд�������ڵ������д����ʱ�����ڱ���ַ����һ�Σ�Ȼ���ַ����ƫ�ƣ�����ٱ���һ��
    {
        ret = SaveEeprom(addr, data, num);   //���ر���(����90000��д�룬�´ζ���ʱ�򣬾ͳ���90000��)ϵͳ����ADD BY LY

        if(addr == SYSPARA_ADDR1)
        {
            SYSPARA_ADDR = SYSPARA_ADDR2; //��������ǰ�洢��ַ��ֵ��SYSPARA_ADDR
            sysparas.writeE2num = 1;
        }
        else if(addr == SYSPARA_ADDR2)
        {
            SYSPARA_ADDR = SYSPARA_ADDR3; //��������ǰ�洢��ַ��ֵ��SYSPARA_ADDR
            sysparas.writeE2num = 1;
        }
        else if(addr == SYSPARA_ADDR3)
        {
            SYSPARA_ADDR = SYSPARA_ADDR4; //��������ǰ�洢��ַ��ֵ��SYSPARA_ADDR
            sysparas.writeE2num = 1;
        }
        else if(addr == SYSPARA_ADDR4)
        {
            globalvar.eepromflag = 1; //��ϵͳ�����Ѿ�д��ϵͳ����������ַ�Ժ���ʾ����
        }

        crc = ModbusCrc16(data, num - 2);
        sysparas.crc = crc;

        ret = SaveEeprom(addr, data, num);  //д���µĲ����洢����
    }

    return ret;
}

/************************************************************************
**	��������:	 SaveSignalSysparas(uint8_t paraindex, uint8_t crcwrflag)
**	��������:	������ϵͳ�������浽�ڲ�EEPROM���ⲿEEPROM
**	�������: paraindex�����������������
			  crcwrflag���Ƿ����CRC��־��1������CRC��0��������CRC
**	����ֵ  	: 0:����ʧ��
          	  1 :����ɹ�

**	�� �� ��:	LY
**	��������:	2018-9-19
*************************************************************************/
uint8_t SaveSignalSysparas(uint8_t paraindex, uint8_t crcwrflag)
{
    uint8_t     ret = false, *data, len;
    uint16_t	addroffset, addr;

    /****************************** EEPROM д���������ж� *************************/
    if(crcwrflag == 1)
    {
        sysparas.writeE2num++;
		addroffset = ((uint16_t)&sysparas.writeE2num - (uint16_t)&sysparas.head);
		data = (uint8_t*)&sysparas.writeE2num;
		len = sizeof(sysparas.writeE2num);
		addr = SYSPARA_ADDR + addroffset;
        if(sysparas.writeE2num < EEPROM_W_TIMES)
        {            
            //���EEPOMд����û�г��ޣ��ͽ���ǰEEPROMд����д�� ADD BY LY
            SaveEeprom(addr, data, len);
        }
        else
        {
            //���ԭ���Ĳ����洢�������ޣ�����ǰEEPROM ��д����д�뵱ǰ�� ADD BY LY
            if(SaveEeprom(addr, data, len))
            {
                SaveCrc();
            }

            if(SYSPARA_ADDR == SYSPARA_ADDR1)
            {
                SYSPARA_ADDR = SYSPARA_ADDR2; //��������ǰ�洢��ַ��ֵ��SYSPARA_ADDR
                sysparas.writeE2num = 1;
            }
            else if(SYSPARA_ADDR == SYSPARA_ADDR2)
            {
                SYSPARA_ADDR = SYSPARA_ADDR3; //��������ǰ�洢��ַ��ֵ��SYSPARA_ADDR
                sysparas.writeE2num = 1;
            }
            else if(SYSPARA_ADDR == SYSPARA_ADDR3)
            {
                SYSPARA_ADDR = SYSPARA_ADDR4; //��������ǰ�洢��ַ��ֵ��SYSPARA_ADDR
                sysparas.writeE2num = 1;
            }
            else if(SYSPARA_ADDR == SYSPARA_ADDR4)
            {
                globalvar.eepromflag = 1; //��ϵͳ�����Ѿ�д��ϵͳ����������ַ�Ժ���ʾ����
            }

            addr = SYSPARA_ADDR ;

            //�����ǰEERPOM��д�������ޣ�������һ���������в���ȫ��дһ�� ADD BY LY
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

        /******************************* EEPROM д���������ж� *******************************/
    }

    //ֻ���ֶ��������õĲ������Ų�ȡ�˷����������� ADD BY LY
    switch(paraindex)
    {
        case 2://ǹ��
            addroffset = (uint16_t)&sysparas.gunnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.gunnum;
            len = sizeof(sysparas.gunnum);
            break;

        case 3://������ˮ��
            addroffset = (uint16_t)&sysparas.transnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.transnum;
            len = sizeof(sysparas.transnum);
            break;

        case 4://�Ѵ�������ˮ��
            addroffset = (uint16_t)&sysparas.uptransnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.uptransnum;
            len = sizeof(sysparas.uptransnum);
            break;

        case 5://ǹ������
            addroffset = (uint16_t)&sysparas.totvolume - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.totvolume;
            len = sizeof(sysparas.totvolume);
            break;

        case 6://ǹ�ܽ��
            addroffset = (uint16_t)&sysparas.totmoney - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.totmoney;
            len = sizeof(sysparas.totmoney);
            break;

        case 7://����0
            addroffset = (uint16_t)&sysparas.price[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[0];
            len = sizeof(sysparas.price[0]);
            break;

        case 8://����1
            addroffset = (uint16_t)&sysparas.price[1] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[1];
            len = sizeof(sysparas.price[1]);
            break;

        case 9://����2
            addroffset = (uint16_t)&sysparas.price[2] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[2];
            len = sizeof(sysparas.price[2]);
            break;

        case 10://����3
            addroffset = (uint16_t)&sysparas.price[3] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[3];
            len = sizeof(sysparas.price[3]);
            break;

        case 11://����4
            addroffset = (uint16_t)&sysparas.price[4] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[4];
            len = sizeof(sysparas.price[4]);
            break;

        case 12://����5
            addroffset = (uint16_t)&sysparas.price[5] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[5];
            len = sizeof(sysparas.price[5]);
            break;

        case 13://����6
            addroffset = (uint16_t)&sysparas.price[6] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[6];
            len = sizeof(sysparas.price[6]);
            break;

        case 14://����7
            addroffset = (uint16_t)&sysparas.price[7] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[7];
            len = sizeof(sysparas.price[7]);
            break;

        case 15://����8
            addroffset = (uint16_t)&sysparas.price[8] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[8];
            len = sizeof(sysparas.price[8]);
            break;

        case 16://����9
            addroffset = (uint16_t)&sysparas.price[9] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[9];
            len = sizeof(sysparas.price[9]);
            break;

        case 17://����10
            addroffset = (uint16_t)&sysparas.price[10] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[10];
            len = sizeof(sysparas.price[10]);
            break;

        case 18://����11
            addroffset = (uint16_t)&sysparas.price[11] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[11];
            len = sizeof(sysparas.price[11]);
            break;

        case 19://����12
            addroffset = (uint16_t)&sysparas.price[12] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[12];
            len = sizeof(sysparas.price[12]);
            break;

        case 20://����13
            addroffset = (uint16_t)&sysparas.price[13] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[13];
            len = sizeof(sysparas.price[13]);
            break;

        case 21://����14
            addroffset = (uint16_t)&sysparas.price[14] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[14];
            len = sizeof(sysparas.price[14]);
            break;

        case 22://����15
            addroffset = (uint16_t)&sysparas.price[15] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[15];
            len = sizeof(sysparas.price[15]);
            break;

        case 23://����16
            addroffset = (uint16_t)&sysparas.price[16] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[16];
            len = sizeof(sysparas.price[16]);
            break;

        case 24://����17
            addroffset = (uint16_t)&sysparas.price[17] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[17];
            len = sizeof(sysparas.price[17]);
            break;

        case 25://����18
            addroffset = (uint16_t)&sysparas.price[18] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[18];
            len = sizeof(sysparas.price[18]);
            break;

        case 26://����19
            addroffset = (uint16_t)&sysparas.price[19] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.price[19];
            len = sizeof(sysparas.price[19]);
            break;

        case 27://����������
            addroffset = (uint16_t)&sysparas.modbustype - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbustype;
            len = sizeof(sysparas.modbustype);
            break;

        case 28://������λ
            addroffset = (uint16_t)&sysparas.unit - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.unit;
            len = sizeof(sysparas.unit);
            break;

        case 29://�ʼ�ģʽ
            addroffset = (uint16_t)&sysparas.qcmode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.qcmode;
            len = sizeof(sysparas.qcmode);
            break;

        case 30://�ܶ�
            addroffset = (uint16_t)&sysparas.densty - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.densty;
            len = sizeof(sysparas.densty);
            break;

        case 31://����ϵ��
            addroffset = (uint16_t)&sysparas.coeff - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.coeff;
            len = sizeof(sysparas.coeff);
            break;

        case 32://ѹ�����
            addroffset = (uint16_t)&sysparas.pzero - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pzero;
            len = sizeof(sysparas.pzero);
            break;

        case 33://ѹ��ϵ��
            addroffset = (uint16_t)&sysparas.pcoeff - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pcoeff;
            len = sizeof(sysparas.pcoeff);
            break;

        case 34://�޸ļ�¼��ˮ��
            addroffset = (uint16_t)&sysparas.modnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modnum;
            len = sizeof(sysparas.modnum);
            break;

        case 35://�Ѵ��޸ļ�¼��ˮ��
            addroffset = (uint16_t)&sysparas.upmodnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.upmodnum;
            len = sizeof(sysparas.upmodnum);
            break;

        case 36://�����
            addroffset = (uint16_t)&sysparas.shiftmaxnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftmaxnum;
            len = sizeof(sysparas.shiftmaxnum);
            break;

        case 37://���
            addroffset = (uint16_t)&sysparas.shiftnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftnum;
            len = sizeof(sysparas.shiftnum);
            /************************�ֶ����� ��ʼ****************************/
            //��һ��Ŀ�ʼʱ��
            memcpy(&shiftrecordinfo.classstarttime[0], &sysparas.shiftstarttime[0], 6);
            //��һ�����ʱ��
            Ds3232ReadTime();
            memcpy(&sysparas.shiftendtime[0], &time, 6);
            memcpy(&shiftrecordinfo.classendtime[0], &sysparas.shiftendtime, 6);
            //��һ�࿪ʼʱ��
            memcpy(&sysparas.shiftstarttime[0], &sysparas.shiftendtime[0], 6);
            Exshiftid(tmp_sysparas.shiftnum);
            /************************�ֶ����� ����****************************/
            break;

        case 38://����ˮ��
            addroffset = (uint16_t)&sysparas.shiftransnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftransnum;
            len = sizeof(sysparas.shiftransnum);
            break;

        case 39://�Ѵ�����ˮ��
            addroffset = (uint16_t)&sysparas.shiftuptransnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftuptransnum;
            len = sizeof(sysparas.shiftuptransnum);
            break;

        case 40://�࿪ʼʱ��
            addroffset = (uint16_t)&sysparas.shiftstarttime[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftstarttime[0];
            len = 6;
            break;

        case 41://�����ʱ��
            addroffset = (uint16_t)&sysparas.shiftendtime[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftendtime[0];
            len = 6;
            break;

        case 42://�࿪ʼǹ��
            addroffset = (uint16_t)&sysparas.shiftstartvol - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftstartvol;
            len = sizeof(sysparas.shiftstartvol);
            break;

        case 43://�����ǹ��
            addroffset = (uint16_t)&sysparas.shiftendvol - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftendvol;
            len = sizeof(sysparas.shiftendvol);
            break;

        case 44://�࿪ʼ���
            addroffset = (uint16_t)&sysparas.shiftstartmon - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftstartmon;
            len = sizeof(sysparas.shiftstartmon);
            break;

        case 45://��ϼƼ�����
            addroffset = (uint16_t)&sysparas.shiftvolsum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftvolsum;
            len = sizeof(sysparas.shiftvolsum);
            break;

        case 46://��ϼ�����
            addroffset = (uint16_t)&sysparas.shiftotvol - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftotvol;
            len = sizeof(sysparas.shiftotvol);
            break;

        case 47://��ϼƽ��
            addroffset = (uint16_t)&sysparas.shiftotmon - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftotmon;
            len = sizeof(sysparas.shiftotmon);
            break;

        case 48://��ʵ���ֽ�
            addroffset = (uint16_t)&sysparas.shiftrealmoney - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftrealmoney;
            len = sizeof(sysparas.shiftrealmoney);
            break;

        case 49://���������
            addroffset = (uint16_t)&sysparas.shiftfueltimes - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shiftfueltimes;
            len = sizeof(sysparas.shiftfueltimes);
            break;

        case 50://ʡ����
            addroffset = (uint16_t)&sysparas.shengcode[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.shengcode[0];
            len = 2;
            break;

        case 51://վ����1
            addroffset = (uint16_t)&sysparas.stationcodearray[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.stationcodearray[0];
            len = 4;
            break;

        case 52://վ����2
            addroffset = (uint16_t)&sysparas.stationcode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.stationcode;
            len = sizeof(sysparas.stationcode);
            break;

        case 53://���д���
            addroffset = (uint16_t)&sysparas.citycode[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.citycode[0];
            len = 2;
            break;

        case 54://�ϼ���λ����
            addroffset = (uint16_t)&sysparas.parentcode[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.parentcode[0];
            len = 4;
            break;

        case 55://����������
            addroffset = (uint16_t)&sysparas.cardmaxyue - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardmaxyue;
            len = sizeof(sysparas.cardmaxyue);
            break;

        case 56://����������
            addroffset = (uint16_t)&sysparas.cardminyue - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardminyue;
            len = sizeof(sysparas.cardminyue);
            break;

        case 57://���˿��������
            addroffset = (uint16_t)&sysparas.accountcardmaxyue - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.accountcardmaxyue;
            len = sizeof(sysparas.accountcardmaxyue);
            break;

        case 58://PSAM������ѡ��
            addroffset = (uint16_t)&sysparas.cardpsamnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardpsamnum;
            len = sizeof(sysparas.cardpsamnum);
            break;

        case 59://��δʹ�����������
            addroffset = (uint16_t)&sysparas.cardmaxday - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardmaxday;
            len = sizeof(sysparas.cardmaxday);
            break;

        case 60://�Ƿ�����ƿ
            addroffset = (uint16_t)&sysparas.cardcheck - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.cardcheck;
            len = sizeof(sysparas.cardcheck);
            break;

        case 61://�Ƿ��ÿ�
            addroffset = (uint16_t)&sysparas.usecard - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.usecard;
            len = sizeof(sysparas.usecard);
            break;

        case 62://�����Ʋ�����
            addroffset = (uint16_t)&sysparas.modbusbaud - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbusbaud;
            len = sizeof(sysparas.modbusbaud);
            break;

        case 63://��̨������
            addroffset = (uint16_t)&sysparas.pcbaud - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pcbaud;
            len = sizeof(sysparas.pcbaud);
            break;

        case 64://����ģʽ
            addroffset = (uint16_t)&sysparas.fuelmode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.fuelmode;
            len = sizeof(sysparas.fuelmode);
            break;

        case 65://СƱ��ӡ��ʽ
            addroffset = (uint16_t)&sysparas.printmode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.printmode;
            len = sizeof(sysparas.printmode);
            break;

        case 66://���۰汾��
            addroffset = (uint16_t)&sysparas.pricever - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pricever;
            len = sizeof(sysparas.pricever);
            break;

        case 68://ͨ����Ϣ�汾��
            addroffset = (uint16_t)&sysparas.comminfover - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.comminfover;
            len = sizeof(sysparas.comminfover);
            break;

        case 69://��Կ�汾��
            addroffset = (uint16_t)&sysparas.lockver - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.lockver;
            len = sizeof(sysparas.lockver);
            break;

        case 70://�˵���������
            addroffset = (uint16_t)&sysparas.menupwd - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.menupwd;
            len = sizeof(sysparas.menupwd);
            break;

        case 71://�ʼ�����
            addroffset = (uint16_t)&sysparas.qcpwd - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.qcpwd;
            len = sizeof(sysparas.qcpwd);
            break;

        case 72://��������
            addroffset = (uint16_t)&sysparas.testpwd - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.testpwd;
            len = sizeof(sysparas.testpwd);
            break;

        case 73://�ָ���������
            addroffset = (uint16_t)&sysparas.resetpwd - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.resetpwd;
            len = sizeof(sysparas.resetpwd);
            break;

        case 74://���ܰ�����
            addroffset = (uint16_t)&sysparas.pwdboard[0] - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pwdboard[0];
            len = 6;
            break;

        case 75://�Ƿ�ʹ�ü��ܰ�
            addroffset = (uint16_t)&sysparas.usepwdboard - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.usepwdboard;
            len = sizeof(sysparas.usepwdboard);
            break;

        case 76://����ģʽ
            addroffset = (uint16_t)&sysparas.offlinemode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.offlinemode;
            len = sizeof(sysparas.offlinemode);
            break;

        case 77://��Ļѡ��
            addroffset = (uint16_t)&sysparas.lcdtype - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.lcdtype;
            len = sizeof(sysparas.lcdtype);
            break;

        case 78://����ģʽ
            addroffset = (uint16_t)&sysparas.simulation - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.simulation;
            len = sizeof(sysparas.simulation);
            break;

        case 79://�Ƿ�ȡ��
            addroffset = (uint16_t)&sysparas.intnumflag - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.intnumflag;
            len = sizeof(sysparas.intnumflag);
            break;

        case 80://������������
            addroffset = (uint16_t)&sysparas.maxvolumeonce - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.maxvolumeonce;
            len = sizeof(sysparas.maxvolumeonce);
            break;

        case 81://�������������
            addroffset = (uint16_t)&sysparas.maxmoneyonce - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.maxmoneyonce;
            len = sizeof(sysparas.maxmoneyonce);
            break;

        case 82://�豸����
            addroffset = (uint16_t)&sysparas.devicetype - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.devicetype;
            len = sizeof(sysparas.devicetype);
            break;

        case 83://�쳣��ˮ��
            addroffset = (uint16_t)&sysparas.err_id - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.err_id;
            len = sizeof(sysparas.err_id);
            break;

        case 84://�Ѵ��쳣��ˮ��
            addroffset = (uint16_t)&sysparas.uperr_id - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.uperr_id;
            len = sizeof(sysparas.uperr_id);
            break;
            
        case 85://�Ƿ��ӡ���ƺ�
            addroffset = (uint16_t)&sysparas.IsPrintCarnum - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.IsPrintCarnum;
            len = sizeof(sysparas.IsPrintCarnum);
            break;

        case 201://�Ƿ��������
            addroffset = (uint16_t)&sysparas.backgascalc - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.backgascalc;
            len = sizeof(sysparas.backgascalc);
            break;

        case 202://Ԥ�䷽ʽ
            addroffset = (uint16_t)&sysparas.precoolmode - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precoolmode;
            len = sizeof(sysparas.precoolmode);
            break;

        case 203://Ԥ���ٽ��¶�
            addroffset = (uint16_t)&sysparas.precooltemp - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precooltemp;
            len = sizeof(sysparas.precooltemp);
            break;

        case 204://Ԥ���ٽ�����
            addroffset = (uint16_t)&sysparas.precoolgain - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precoolgain;
            len = sizeof(sysparas.precoolgain);
            break;

        case 205://Ԥ���ٽ��ܶ�
            addroffset = (uint16_t)&sysparas.precooldensty - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precooldensty;
            len = sizeof(sysparas.precooldensty);
            break;

        case 206://Ԥ�䳬ʱʱ��
            addroffset = (uint16_t)&sysparas.precooltimeout - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precooltimeout;
            len = sizeof(sysparas.precooltimeout);
            break;

        case 207://ͣ����ʱʱ��
            addroffset = (uint16_t)&sysparas.delaystoptime - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.delaystoptime;
            len = sizeof(sysparas.delaystoptime);
            break;

        case 208://������ͣ��
            addroffset = (uint16_t)&sysparas.lowratestop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.lowratestop;
            len = sizeof(sysparas.lowratestop);
            break;

        case 209://������ͣ��
            addroffset = (uint16_t)&sysparas.highratestop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.highratestop;
            len = sizeof(sysparas.highratestop);
            break;

        case 210://�����ܶ�ͣ��
            addroffset = (uint16_t)&sysparas.backdenstystop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.backdenstystop;
            len = sizeof(sysparas.backdenstystop);
            break;

        case 211://��������ͣ��
            addroffset = (uint16_t)&sysparas.backgainstop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.backgainstop;
            len = sizeof(sysparas.backgainstop);
            break;

        case 212://ѹ��ͣ��
            addroffset = (uint16_t)&sysparas.pressurestop - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.pressurestop;
            len = sizeof(sysparas.pressurestop);
            break;

        case 213://��Һ����ʱͣ��
            addroffset = (uint16_t)&sysparas.stoppumptime - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.stoppumptime;
            len = sizeof(sysparas.stoppumptime);
            break;

        case 214://����Ԥ����ʱ
            addroffset = (uint16_t)&sysparas.precoolfinishedtime - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.precoolfinishedtime;
            len = sizeof(sysparas.precoolfinishedtime);
            break;

        case 215://���������Ƶ�ַ
            addroffset = (uint16_t)&sysparas.modbusaddr_g - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbusaddr_g;
            len = sizeof(sysparas.modbusaddr_g);
            break;

        case 216://Һ�������Ƶ�ַ
            addroffset = (uint16_t)&sysparas.modbusaddr_l - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbusaddr_l;
            len = sizeof(sysparas.modbusaddr_l);
            break;

        case 217://��������������
            addroffset = (uint16_t)&sysparas.modbusRS485 - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.modbusRS485;
            len = sizeof(sysparas.modbusRS485);
            break;
			
        case 218://СƱ��ӡ����
            addroffset = (uint16_t)&sysparas.printtimes - (uint16_t)&sysparas.head;
            data = (uint8_t*)&sysparas.printtimes;
            len = sizeof(sysparas.printtimes);
            break;

        default://��paraindex���������κβ���ʱ��Ԥ��д������Ӧ������ʼ��Ϊhead�Ĳ�����ַ ADD BY LY
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
**	��������: SaveCrc(void)
**	��������:	����ϵͳ�����е�CRC
**	�������: ��
**	����ֵ  	: 0: ����ʧ��
          	  1 :����ɹ�

**	�� �� ��:	LY
**	��������:	2018-9-19
*************************************************************************/
uint8_t SaveCrc(void)
{
    uint16_t crc, addroffset;
	//����sysparas��CRC
    crc = ModbusCrc16((uint8_t*)&sysparas, sizeof(sysparas) - 2);
    sysparas.crc = crc;
    addroffset = (uint32_t)&sysparas.crc - (uint32_t)&sysparas.head;
	//����CRC
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
**	��������: ExshiftidSaveParas(void)
**	��������:	���滻��ʱ��Ҫ����Ĳ���
**	�������: ��
**	����ֵ  	: 0: ����ʧ��
          	  1 :����ɹ�

**	�� �� ��:	LY
**	��������:	2018-9-19
*************************************************************************/
uint8_t ExshiftidSaveParas(void)
{
    uint16_t addr, len;
    //����EEPROM����ĵ�ַ����sysparas.shiftnum��ʼ���棬��ʱ�����ǵ�ַ��ҳ
    addr = SYSPARA_ADDR + abs((uint16_t)&sysparas.shiftnum - (uint16_t)&sysparas.head);

    //������Ҫ����ĳ���
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
**	��������: StopFuelingSaveParas(void)
**	��������:	��Һ���������汣����ˮʱ��Ҫ���µ�����
**	�������: ��
**	����ֵ  	: 0: ����ʧ��
          	  1 :����ɹ�

**	�� �� ��:	YCX
**	��������:	2018-9-19
*************************************************************************/
uint8_t StopFuelingSaveParas(void)
{	
	//������ˮ��
    SaveSignalSysparas(3, 0);			//sysparas.transnum
    //����ǹ������
    SaveSignalSysparas(5, 0);			//sysparas.totvolume
    //����ǹ�ܽ��
    SaveSignalSysparas(6, 0);			//sysparas.totmoney
    //�����ϼ�����
    SaveSignalSysparas(46, 0);			//sysparas.shiftotvol
    //�����ϼƽ��
    SaveSignalSysparas(47, 0); 			//sysparas.shiftotmon
    //��������������ͬʱ����CRC
    return SaveSignalSysparas(49, 1); 	//sysparas.shiftfueltimes
}

/************************************************************************
**	��������: ReadSysparas(uint16_t addr, uint8_t *data, uint16_t num)
**	��������:	��EEPROM�����⣩�ж�ȡϵͳ����
**	�������: addr:	��ʼ������ʼ��ַ
			  data:	��ȡ����ָ��
			  num : ��ȡ���ݸ���
**	����ֵ  	: 0: ��ȡʧ��
          	  1: ��ȡ�ɹ�

**	�� �� ��:	YCX
**	��������:	2016-4-19
*************************************************************************/
uint8_t ReadSysparas(uint16_t addr, uint8_t *data, uint16_t num)
{
    uint16_t    i;
    uint16_t crc;
    uint8_t     *t1, *t2, ret = false;
    Sysparas    tmp1, tmp2;

    t1 = (uint8_t*)&tmp1;
    t2 = (uint8_t*)&tmp2;

    if(EepromInterReadBytes(addr, t1, num))     //���ڲ�eeprom��ȷ
    {
        crc = ModbusCrc16(t1, num - 2);

        if(tmp1.crc == crc)	//CRCУ��ͨ��
        {
            ClrBit(syserr, SYSERR_R_IN_EEPROM);
            ret = true; //�ڲ�eeprom������ȷ
        }
        else	//CRCУ��ʧ��
        {
            if(tmp1.writeE2num > 100000)
            {
                tmp1.writeE2num = 0; //��ֹ�״ζ�һ���°��ӣ�����ȫΪ0xFF
            }

            SetBit(syserr, SYSERR_R_IN_EEPROM);
            ret = false; //�ڲ�eeprom���ݴ���

        }

        //��ʹCRC����Ҳ��ֵ
        for(i = 0; i < num; i++)
        {
            data[i] = t1[i]; //���ڲ�EEPROM�еĲ�����������data��ָ��Ļ����� BY LY
        }
    }

    if(ret == false)   //����ڲ�eeprom�����ٶ��ⲿeeprom
    {
        if(EepromExternalReadBytes(addr, t2, num))
        {
            crc = ModbusCrc16(t2, num - 2);

            if(tmp2.crc == crc)  //���ⲿeeprom��ȷ CRCУ��ͨ��
            {
                ClrBit(syserr, SYSERR_R_EX_EEPROM);
                ret = true; //�ⲿeeprom������ȷ
            }
            else //CRCУ��ʧ��
            {
                if(tmp2.writeE2num > 100000)
                    tmp2.writeE2num = 0; //��ֹ�״ζ�һ���°��ӣ�����ȫΪ0xFF

                SetBit(syserr, SYSERR_R_EX_EEPROM);
                ret = false; //�ⲿeeprom���ݴ���

            }
            //��ʹCRC����Ҳ��ֵ
            for(i = 0; i < num; i++)
            {
                data[i] = t2[i]; //���ⲿEEPROM�еĲ�����������t2�� BY LY
            }
        }
    }

    return ret;
}

/************************************************************************
**	��������:	EepromDeviceTest(void)
**	��������:	�ⲿEEPROM��д����
**	�������: ��
**	����ֵ  	: 0: ��ȡʧ��
          	  1: ��ȡ�ɹ�

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
bool EepromDeviceTest(void)
{
    uint8_t i = 0;
    bool result = false; //���ڴ洢���Է��ؽ��
    uint8_t WriteBuff[200], ReadBuff[200];

    for(i = 0; i < 100; i++)   //��ʼ�����ݻ�����
    {
        WriteBuff[i] = i;
        ReadBuff[i]  = 0;
    }

    EepromExternalWriteBytes(EEPROM_TESTADDR, WriteBuff, 200);
    _delay_ms(5);   //�ȴ�д�������
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

