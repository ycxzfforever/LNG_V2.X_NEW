/************��Ա���쿻ʹ�ӡ������*********/
#include "kernel.h"

uint8_t stopreasonbuf[][16] =
{
    "�ֶ�ͣ��",         // 0
    "�����������쳣",   // 1
    "Һ���������쳣",   // 2
    "˲ʱ���ݴ���",     // 3
    "ѹ���������쳣",   // 4
    "�����λͣ��",   // 5
    "������ͣ��",       // 6
    "�����ͣ��",       // 7
    "������ͣ��",       // 8
    "������ͣ��",       // 9
    "�����ܶ�ͣ��",     // 10
    "��������ͣ��",     // 11
    "��ѹͣ��",         // 12
    "�ӿ�ͣ��",         // 13
    "���˳���",         // 14
    "�������",//15
    "���γ������",   // 16
};
	
/************************************************************************
**	��������:	WHPrinterInit(void)
**	��������:	��ӡ����ʼ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHPrinterInit(void)
{
    SELECT_PRINT();
    Uart0SendByte(0x1B);
    Uart0SendByte(0x40);
    Uart0SendByte(0x0D);
    //SELECT_CARD();
}

/************************************************************************
**	��������:	WHPrinterNormalCmd(void)
**	��������:	���ô�ӡģʽ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHPrinterNormalCmd(void)
{
    uint8_t cmd[14] = {0x1b, 0x40, 0x1c, 0x26, 0x1b, 0x31, 0x08,
                       0x1b, 0x70, 0x02, 0x1b, 0x63, 0x00, 0x0D};
    //0x1c 26:���뺺�ִ�ӡ��ʽ
    //0x1b 56 02:�����߶�������ӡ
    //0x1b 63 00:�ַ���������ķ����ӡ
    Uart0SendStr(cmd, 14);
}

/************************************************************************
**	��������:	WHPrinterFreeRow(uint8_t rows)
**	��������:	��ӡ����
**	�������: rows���к�
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHPrinterFreeRow(uint8_t rows)
{
    while(rows--)
    {
        Uart0SendByte(0x0A);   //����
        Uart0SendByte(0x0D);   //�س���ȷ�ϴ�ӡ����(��֮ǰ��)����
    }
}

/************************************************************************
**	��������:	WHPrinterHead(void)
**	��������:	���ô�ӡͷ
**	�������: rows���к�
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHPrinterHead(void)
{
    uint8_t buff[8] = {0x1c, 0x26, 0x1b, 0x56, 0x02, 0x1b, 0x63, 0x00};
    //0x1c 26:���뺺�ִ�ӡ��ʽ
    //0x1b 56 02:�����߶�������ӡ
    //0x1b 63 00:�ַ���������ķ����ӡ
    Uart0SendStr(buff, 8);
}

/************************************************************************
**	��������:	WHPrinterCheck(void)
**	��������:	��ӡ������ȷ���ź�
**	�������: ��
**	����ֵ  	: true:��ӡ��ȷ�ϳɹ�����true����ӡ��ȷ��ʧ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
uint8_t WHPrinterCheck(void)
{
    uint8_t  val;
	
    Uart0SendByte(0x1c);
    Uart0SendByte(0x76);
    Uart0SendByte(0x0D);
    val = Uart0GetByte();

    if(val == 0xFF)    return(0xFF);

    if(val == 0x04)    return(0x04);

    return true;
}

/************************************************************************
**	��������:	WHPrintReady(void)
**	��������:	��ӡ��׼����
**	�������: ��
**	����ֵ  	: true:��ӡ��׼���ã�false����ӡ��δ׼����

**	�� �� ��:	
**	��������:	
*************************************************************************/
uint8_t WHPrintReady(void)
{
    if(WHPrinterCheck() != true)    
		return false;

    return true;
}

/************************************************************************
**	��������:	WHPrintReady(void)
**	��������:	��ӡ����ָ����ʽ��ӡ�ַ���
**	�������: fmt:�ַ�ָ�룬ִ��ָ����ʽ���ַ���
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHPrintf(char *fmt, ...)
{
    va_list ap;
    char str[40];
    va_start(ap, fmt);
    vsprintf(str, fmt, ap);
    Uart0PutString(str);
    va_end(ap);
}

/************************************************************************
**	��������:	WHPrintfFlash(char __farflash*fmt, ...)
**	��������:	��ӡ����ָ����ʽ��ӡ�ַ�������ͬ���ǣ��ַ����洢��flash��
**	�������: fmt:�ַ�ָ�룬ִ��ָ����ʽ���ַ���
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHPrintfFlash(char __farflash*fmt, ...)
{
    va_list ap;
    char str[60], tmp[60];
    strcpy_P(tmp, fmt);
    va_start(ap, tmp);
    vsprintf(str, tmp, ap);
    Uart0PutString(str);
    va_end(ap);
}

/************************************************************************
**	��������:	WHPrintfString(char __farflash* str)
**	��������:	��ӡ����ӡһ���洢��flash�е��ַ���
**	�������: str:�ַ�ָ�룬ָ��flash�е��ַ���
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHPrintfString(char __farflash* str)
{
    Uart0PutStringflash(str);
}

/************************************************************************
**	��������:	WHFuelRecord(void)
**	��������:	������ˮ��ӡ��ʽ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHFuelRecord(void)

{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("          LCNG����ƾ��\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintfFlash("ǹ��:%02d #\r\n", sysparas.gunnum);
    WHPrintfFlash("���:%02d #\r\n", fuelrecordinfo.classnum);
    WHPrintfFlash("��ˮ��:%ld\r\n", fuelrecordinfo.transnum);
    if(sysparas.unit==2)
    {
	    WHPrintfFlash("����:%d.%02d Ԫ/��\r\n", fuelrecordinfo.price / 100, fuelrecordinfo.price % 100);
	    WHPrintfFlash("����:%ld.%02ld ��\r\n", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);
	}
	else
	{
		WHPrintfFlash("����:%d.%02d Ԫ/kg\r\n", fuelrecordinfo.price / 100, fuelrecordinfo.price % 100);
	    WHPrintfFlash("����:%ld.%02ld kg\r\n", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);	
	}
    WHPrintfFlash("���:%ld.%02ld Ԫ\r\n", fuelrecordinfo.money / 100, fuelrecordinfo.money % 100);

    if(fuelrecordinfo.usernum != 0)
    {
        WHPrintfFlash("����:%08ld\r\n", fuelrecordinfo.usernum);
        WHPrintfFlash("���:%ld.%02ld Ԫ\r\n", fuelrecordinfo.afterbalance / 100, fuelrecordinfo.afterbalance % 100);

    }

    if((sysparas.backgascalc == 1) && (fuelrecordinfo.stopreason != 0xFF))       //������ݲ���ӡ��Һ���ͻ�����
    {
        WHPrintfFlash("��Һ��:%ld.%02d kg\r\n", (fuelrecordinfo.volume + fuelrecordinfo.backgas) / 100, (fuelrecordinfo.volume + fuelrecordinfo.backgas) % 100);
        WHPrintfFlash("������:%ld.%02d kg\r\n", fuelrecordinfo.backgas / 100, fuelrecordinfo.backgas % 100);
    }
    if(sysparas.IsPrintCarnum == 1)
		WHPrintfFlash("���ƺ�:%04ld\r\n", fuelrecordinfo.carnum_another);
	WHPrintfFlash("վ����:%ld\r\n", sysparas.stationcode);
    WHPrintfFlash("ͣ��ԭ��:%s\r\n", stopreasonbuf[fuelrecordinfo.stopreason]);
    WHPrintfFlash("��ʼʱ��:20%02X-%02X-%02X %02X:%02X:%02X\r\n", fuelrecordinfo.starttime[0], fuelrecordinfo.starttime[1], fuelrecordinfo.starttime[2],
                  fuelrecordinfo.starttime[3], fuelrecordinfo.starttime[4], fuelrecordinfo.starttime[5]);
    WHPrintfFlash("����ʱ��:20%02X-%02X-%02X %02X:%02X:%02X\r\n", fuelrecordinfo.endtime[0], fuelrecordinfo.endtime[1], fuelrecordinfo.endtime[2],
                  fuelrecordinfo.endtime[3], fuelrecordinfo.endtime[4], fuelrecordinfo.endtime[5]);
    WHPrinterFreeRow(1);
    WHPrintfString("     ��ӭ����  ����ƽ��\r\n");   //�·ָ���
    WHPrintfString("     <�����Ʊ����ƾ��>\r\n");   //��ע
    WHPrinterFreeRow(6);
    SELECT_CARD();
}

/************************************************************************
**	��������:	 Printid(void)
**	��������:	��ӡ��ˮ��¼
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void Printid(void)
{
    WHPrinterInit();	//��ʼ����

    if(WHPrintReady())	//��ӡ����׼����
    {
        WHFuelRecord();	//��ӡ��ˮ
    }
}

/************************************************************************
**	�������ƣ�StopPrint(void)
**	��������:	ֹͣ��ӡ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void StopPrint(void)
{
    Printid();
    DispUI = DispFirst;
}

/************************************************************************
**	�������ƣ�WHFuelShifRecord(void)
**	��������:	����ˮ��ӡ��ʽ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHFuelShifRecord(void)
//��ӡ����ˮ��¼
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("         LCNG���ۼ�¼\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintfFlash("ǹ��:%d\r\n", sysparas.gunnum);
    WHPrintfFlash("���:%d\r\n", shiftrecordinfo.classnum);
    WHPrintfFlash("����ˮ��:%ld\r\n", shiftrecordinfo.classid);
    WHPrintfFlash("���������:%ld\r\n", shiftrecordinfo.classtimes);
    WHPrintfFlash("�������ۻ�:%ld.%02d\r\n", shiftrecordinfo.classgassum / 100, shiftrecordinfo.classgassum % 100);
    WHPrintfFlash("�����ۼ�:%ld.%02d\r\n", shiftrecordinfo.classgasmoney / 100, shiftrecordinfo.classgasmoney % 100);
    WHPrintfFlash("ǹ�����ۼ�:%ld.%02d\r\n", shiftrecordinfo.endallgunvolume / 100, shiftrecordinfo.endallgunvolume % 100);
    WHPrintfFlash("ǹ����ۼ�:%ld.%02d\r\n", shiftrecordinfo.endagunmoney / 100, shiftrecordinfo.endagunmoney % 100);

    if(shiftrecordinfo.devicetype == 0)
    {
        WHPrintfString("��Ʒ����:LNG\r\n");
    }
    else if(shiftrecordinfo.devicetype == 1)
    {
        WHPrintfString("��Ʒ����:CNG\r\n");
    }

    if(shiftrecordinfo.measuretype == 1)
    {
        WHPrintfString("������λ:����\r\n");
    }

    else if(shiftrecordinfo.measuretype == 2)
    {
        WHPrintfString("������λ:�귽\r\n");
    }

    WHPrintfFlash("��ʼʱ��:%02X-%02X-%02X %02X:%02X:%02X\r\n", shiftrecordinfo.classstarttime[0], shiftrecordinfo.classstarttime[1], shiftrecordinfo.classstarttime[2],
                  shiftrecordinfo.classstarttime[3], shiftrecordinfo.classstarttime[4], shiftrecordinfo.classstarttime[5]);
    WHPrintfFlash("����ʱ��:%02X-%02X-%02X %02X:%02X:%02X\r\n", shiftrecordinfo.classendtime[0], shiftrecordinfo.classendtime[1], shiftrecordinfo.classendtime[2],
                  shiftrecordinfo.classendtime[3], shiftrecordinfo.classendtime[4], shiftrecordinfo.classendtime[5]);
    WHPrinterFreeRow(1);
    WHPrintfString("     ��ӭ����  ����ƽ��\r\n");   //�·ָ���
    WHPrintfString("     <�����Ʊ����ƾ��>\r\n");   //��ע
    WHPrinterFreeRow(6);
    SELECT_CARD();
}

/************************************************************************
**	�������ƣ�PrintSum(void)
**	��������:	��ӡ����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void PrintSum(void)
{
    WHPrinterInit();//��ʼ����

    if(WHPrintReady())
    {
        WHFuelSumRecord();
    }
}

/************************************************************************
**	�������ƣ�WHFuelSumRecord(void)
**	��������:	���۴�ӡ��ʽ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHFuelSumRecord(void)
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("         LCNG���ۼ�¼\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintfFlash("ǹ��:%d\r\n", sysparas.gunnum);
    WHPrintfFlash("������ˮ��:%ld\r\n", sysparas.transnum);
    WHPrintfFlash("�������ۼ�:%ld.%02d\r\n", sysparas.totvolume / 100, sysparas.totvolume % 100);
    WHPrintfFlash("�ܽ���ۼ�:%ld.%02d\r\n", sysparas.totmoney / 100, sysparas.totmoney % 100);

    if(fuelrecordinfo.devicetype == 1)
    {
        WHPrintfString("��Ʒ����:LNG\r\n");
    }
    else if(fuelrecordinfo.devicetype == 2)
    {
        WHPrintfString("��Ʒ����:CNG\r\n");
    }

    if(fuelrecordinfo.measuretype == 1)
    {
        WHPrintfString("������λ:����\r\n");
    }
    else if(fuelrecordinfo.measuretype == 2)
    {
        WHPrintfString("������λ:�귽\r\n");
    }

    WHPrinterFreeRow(1);
    WHPrintfString("     <�����Ʊ����ƾ��>\r\n");   //��ע
    WHPrinterFreeRow(6);
    SELECT_CARD();
}

/************************************************************************
**	�������ƣ�PrintSysInfo(void)
**	��������:	��ӡϵͳ����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void PrintSysInfo(void)
{
    WHPrinterInit();//��ʼ����

    if(WHPrintReady())
    {
        WHFuelSysInfo();
    }
}

/************************************************************************
**	�������ƣ�WHFuelSysInfo(void)
**	��������:	ϵͳ������ӡ��ʽ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void WHFuelSysInfo(void)
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("         LCNGϵͳ����\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();

    WHPrintfFlash("ǹ��:%d\r\n", sysparas.gunnum);
    WHPrintfFlash("����:%d.%02d\r\n", sysparas.price[0] / 100, sysparas.price[0] % 100);
    WHPrintfFlash("�޸���ˮ��:%ld\r\n", sysparas.modnum);
    WHPrintfFlash("�Ѵ��޸ĺ�:%ld\r\n", sysparas.upmodnum);
    WHPrintfFlash("������:%ld.%02d\r\n", sysparas.totvolume / 100, sysparas.totvolume % 100);
    WHPrintfFlash("�ܽ��:%ld.%02d\r\n", sysparas.totmoney / 100, sysparas.totmoney % 100);
    WHPrintfFlash("���:%d\r\n", sysparas.shiftnum);
    WHPrintfFlash("�����:%d\r\n", sysparas.shiftmaxnum);
    WHPrintfFlash("����������:%d\r\n", sysparas.modbustype);
    WHPrintfFlash("������λ:%d\r\n", sysparas.unit);
    WHPrintfFlash("�ʼ�ģʽ:%d\r\n", sysparas.qcmode);
    WHPrintfFlash("EEд����:%ld\r\n", (SYSPARA_ADDR / sizeof(Sysparas))*EEPROM_W_TIMES + sysparas.writeE2num);
    WHPrintfFlash("�ܶ�:%d.%04d\r\n", sysparas.densty / 10000, sysparas.densty % 10000);
    WHPrintfFlash("����ϵ��:%d.%04d\r\n", sysparas.coeff / 10000, sysparas.coeff % 10000);
    WHPrintfFlash("ѹ�����:%d\r\n", sysparas.pzero);
    WHPrintfFlash("ѹ��ϵ��:%d\r\n", sysparas.pcoeff);
    WHPrintfFlash("�������:%ld.%02d\r\n", sysparas.cardmaxyue / 100, sysparas.cardmaxyue % 100);
    WHPrintfFlash("�������:%ld.%02d\r\n", sysparas.cardminyue / 100, sysparas.cardminyue % 100);
    WHPrintfFlash("���˿�����:%ld.%02d\r\n", sysparas.accountcardmaxyue / 100, sysparas.accountcardmaxyue % 100);
    WHPrintfFlash("PSAM����:%d\r\n", sysparas.cardpsamnum);
    WHPrintfFlash("��ƿ�������:%d\r\n", sysparas.cardmaxday);
    WHPrintfFlash("�Ƿ�����ƿ:%d\r\n", sysparas.cardcheck);
    WHPrintfFlash("�����Ʋ���:%ld\r\n", sysparas.modbusbaud);
    WHPrintfFlash("��̨������:%ld\r\n", sysparas.pcbaud);
    WHPrintfFlash("����ģʽ:%d\r\n", sysparas.fuelmode);
    WHPrintfFlash("��ӡ��ʽ:%d\r\n", sysparas.printmode);
    WHPrintfFlash("���۰汾��:%d\r\n", sysparas.pricever);
    WHPrintfFlash("ͨ����Ϣ�汾��:%d\r\n", sysparas.comminfover);
    WHPrintfFlash("��Կ�汾��:%d\r\n", sysparas.lockver);
    WHPrintfFlash("�Ƿ�ʹ�ü��ܰ�:%d\r\n", sysparas.usepwdboard);
    WHPrintfFlash("�࿪ʼ����:%ld.%02d\r\n", sysparas.shiftstartvol / 100, sysparas.shiftstartvol % 100);
    WHPrintfFlash("�࿪ʼ���:%ld.%02d\r\n", sysparas.shiftstartmon / 100, sysparas.shiftstartmon % 100);
    WHPrintfFlash("���������:%ld\r\n", sysparas.shiftfueltimes);
    WHPrintfFlash("��������:%ld.%02d\r\n", sysparas.shiftotvol / 100, sysparas.shiftotvol % 100);
    WHPrintfFlash("���ܽ��:%ld.%02d\r\n", sysparas.shiftotmon / 100, sysparas.shiftotmon % 100);
    WHPrintfFlash("��ˮ��:%ld\r\n", sysparas.transnum);
    WHPrintfFlash("�Ѵ���ˮ��:%ld\r\n", sysparas.uptransnum);
    WHPrintfFlash("����ˮ��:%ld\r\n", sysparas.shiftransnum);
    WHPrintfFlash("�Ѵ�����ˮ��:%ld\r\n", sysparas.shiftuptransnum);
    WHPrintfString("ϵͳ�汾��:");
    WHPrintfString(VERSION);
    WHPrintfString("\r\n");
    WHPrinterFreeRow(1);
    WHPrintfString("     <�����Ʊ����ƾ��>\r\n");   //��ע
    WHPrinterFreeRow(6);
    SELECT_CARD();
}

/************************************************************************
**	�������ƣ�PrintShifid(void)
**	��������:	��ӡ����ˮ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void PrintShifid(void)
{
    WHPrinterInit();//��ʼ����

    if(WHPrintReady())
    {
        WHFuelShifRecord();
    }
}

/************************************************************************
**	�������ƣ�QueryPrintContInfo(void)
**	��������:	��ӡ���Ʋ���
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void QueryPrintContInfo(void)
{
    PrintContInfo();
    DispUI = DispCtrlParas1;
}

/************************************************************************
**	�������ƣ�PrintContInfo(void)
**	��������:	���Ʋ�����ӡ��ʽ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void PrintContInfo(void)
{
    SELECT_PRINT();

    WHPrinterHead();
    WHPrintfString("         LNG���Ʋ���\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintfFlash("Ԥ�䳬ʱʱ��:%d\r\n", sysparas.precooltimeout);
    WHPrintfFlash("Ԥ���ٽ��¶�:-%d\r\n", sysparas.precooltemp);
    WHPrintfFlash("Ԥ���ٽ�����:%d\r\n", sysparas.precoolgain);
    WHPrintfFlash("Ԥ���ٽ��ܶ�:%d.%04d\r\n", sysparas.precooldensty / 10000, sysparas.precooldensty % 10000);
    WHPrintfFlash("ͣ����ʱ:%d\r\n", sysparas.delaystoptime);
    WHPrintfFlash("��ѹͣ��:%d.%02d\r\n", sysparas.pressurestop / 100, sysparas.pressurestop % 100);
    WHPrintfFlash("������ͣ��:%d\r\n", sysparas.lowratestop);
    WHPrintfFlash("������ͣ��:%d\r\n", sysparas.highratestop);
    WHPrintfFlash("�����ܶ�ͣ��:%d.%04d\r\n", sysparas.backdenstystop / 10000, sysparas.backdenstystop % 10000);
    WHPrintfFlash("��������ͣ��:%d\r\n", sysparas.backgainstop);
    WHPrintfFlash("�Ƿ��������:%d\r\n", sysparas.backgascalc);
    WHPrintfFlash("Ԥ�䷽ʽѡ��:%d\r\n", sysparas.precoolmode);
    WHPrintfFlash("ͣ����ʱ:%d\r\n", sysparas.stoppumptime);
    WHPrintfFlash("����Ԥ����ʱ:%d\r\n", sysparas.precoolfinishedtime);

    WHPrinterFreeRow(1);
    WHPrintfString("     <�����Ʊ����ƾ��>\r\n");   //��ע
    WHPrinterFreeRow(6);
    SELECT_CARD();
}


