
/***************************************************************************
**�ı���query.c
**˵�������ڲ�ѯ��ʾ�����ں����϶࣬�Ҵ�ͬС�죬
                        ����ֻ��ÿ������������
**������ �����
**�������ڣ�2016-4-25 10:46:36

**�޸��ߣ�
**�޸����ڣ�
**�޸����ݣ�
***************************************************************************/

#include "kernel.h"

//Reply5B  tmp;//�����޸ļ�¼��ѯ����ʹ�á�

/************************************************************************
**	�������ƣ�PrinterSum(void)
**	��������:	��ӡ����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void PrinterSum(void)
{
    PrintSum();
    DispUI = DispSum;
}

/************************************************************************
**	�������ƣ�DispSum(void)
**	��������:	������ʾ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSum(void)
{
    FYD12864DispPrintfFlash(1, 1, "��ˮ��%ld", sysparas.transnum);
    FYD12864DispPrintfFlash(2, 1, "�Ѵ���ˮ��%ld", sysparas.uptransnum);
    FYD12864DispPrintfFlash(3, 1, "������%ld.%02d", sysparas.totvolume / 100, sysparas.totvolume % 100);
    FYD12864DispPrintfFlash(4, 1, "�ܽ��%ld.%02d", sysparas.totmoney / 100, sysparas.totmoney % 100);

    TaskAdd(KEY_RET, DispQuery1, NULL);
    TaskAdd(KEY_OK, PrinterSum, NULL);
}

/************************************************************************
**	�������ƣ�DispShiftSum(void)
**	��������:	����������ʾ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispShiftSum(void)
{
    FYD12864DispPrintfFlash(1, 1, "���%d", sysparas.shiftnum);
    FYD12864DispPrintfFlash(2, 1, "���������%ld", sysparas.shiftfueltimes);
    FYD12864DispPrintfFlash(3, 1, "������%ld.%02d", sysparas.shiftotvol / 100, sysparas.shiftotvol % 100);
    FYD12864DispPrintfFlash(4, 1, "����%ld.%02d", sysparas.shiftotmon / 100, sysparas.shiftotmon % 100);

    TaskAdd(KEY_RET, DispQuery1, NULL);
}

/************************************************************************
**	�������ƣ�DispInputTransNum(void)
**	��������:	������ˮ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispInputTransNum(void)
{
    if(sysparas.transnum > 0)
    {

        FYD12864DispPrintfFlash(1, 1, "          %05ld", sysparas.transnum);
        FYD12864DispPrintfFlash(2, 1, "��������ˮ��");
        FYD12864DispPrintfFlash(3, 1, "%s", ScreenInput.array);
        ScreenInput.inputmaxlen = 16;
        ScreenInput.pointnum = 0;
        fuelrecordinfo.transnum = atol((char *)ScreenInput.array);
        NumKeyHandler();
        TaskAdd(KEY_RET, DispQuery1, NULL);

        if(fuelrecordinfo.transnum <= sysparas.transnum)
        {
            FYD12864ClearLine(4);

            if(fuelrecordinfo.transnum == 0)
            {
                fuelrecordinfo.transnum = sysparas.transnum;
            }

            TaskAdd(KEY_OK, DispRecord, NULL);
        }
        else
        {
            FYD12864DispPrintfFlash(4, 1, "��ˮ�ų���");
        }
    }
    else
    {
        FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(2, 1, "  ����ˮ��¼");
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
        TaskMessage(20, DispQuery1);
    }
}

/************************************************************************
**	�������ƣ�DispInputShifTransNum(void)
**	��������:	�������ˮ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispInputShifTransNum(void)
{
    //FYD12864ClearLine(1);

    FYD12864DispPrintfFlash(1, 1, "          %05ld", sysparas.shiftransnum);
    FYD12864DispPrintfFlash(2, 1, "���������ˮ��");
    FYD12864DispPrintfFlash(3, 1, "%s", ScreenInput.array);
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    shiftrecordinfo.classid = atol((char *)ScreenInput.array);
    NumKeyHandler();
    TaskAdd(KEY_RET, DispQuery2, NULL);

    if(shiftrecordinfo.classid <= sysparas.shiftransnum)
    {
        FYD12864ClearLine(4);

        if(shiftrecordinfo.classid == 0)
        {
            shiftrecordinfo.classid = sysparas.shiftransnum;
        }

        TaskAdd(KEY_OK, DispshifRecord, NULL);
    }
    else
    {
        FYD12864DispPrintfFlash(4, 1, "��ˮ�ų���");
    }
}

/************************************************************************
**	�������ƣ�DispshifRecord(void)
**	��������:	����ˮ��¼��ʾ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispshifRecord(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        ReadFuelRecord(shiftrecordinfo.classid - 1, 1);   //�˺���������Ҫ����Ƿ���Ҫ�޸� ADD BY LY
    }

    FYD12864DispPrintfFlash(1, 1, "����ˮ��:%ld",  shiftrecordinfo.classid);
    FYD12864DispPrintfFlash(2, 1, "���������:%ld",  shiftrecordinfo.classtimes);
    FYD12864DispPrintfFlash(3, 1, "������:%ld.%02d", shiftrecordinfo.classgassum / 100, shiftrecordinfo.classgassum % 100);
    FYD12864DispPrintfFlash(4, 1, "����:%ld.%02d", shiftrecordinfo.classgasmoney / 100, shiftrecordinfo.classgasmoney % 100);

    TaskAdd(KEY_UP, RecordShifUp, NULL);
    TaskAdd(KEY_DOWN, RecordShifDown, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, Printershifid, NULL);
}

/************************************************************************
**	�������ƣ�Printershifid(void)
**	��������:	��ӡ����ˮ��¼
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void Printershifid(void)
{
    PrintShifid();
    DispUI = DispshifRecord;
}

/************************************************************************
**	�������ƣ�RecordShifUp(void)
**	��������:	����ˮ���ϼ���ѯ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void RecordShifUp(void)
{
    if(shiftrecordinfo.classid > 1)
    {
        shiftrecordinfo.classid--;
        globalvar.displayonce = 1;
    }

    DispUI = DispshifRecord;
}

/************************************************************************
**	�������ƣ�RecordShifDown(void)
**	��������:	����ˮ���¼���ѯ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void RecordShifDown(void)
{
    if(shiftrecordinfo.classid < sysparas.shiftransnum)
    {
        shiftrecordinfo.classid++;
        globalvar.displayonce = 1;
    }

    DispUI = DispshifRecord;
}

/************************************************************************
**	�������ƣ�RecordUp(void)
**	��������:	��ˮ���ϼ���ѯ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void RecordUp(void)
{
    if(fuelrecordinfo.transnum > 1)
    {
        fuelrecordinfo.transnum--;
        globalvar.displayonce = 1;
    }

    DispUI = DispRecord;
}

/************************************************************************
**	�������ƣ�RecordDown(void)
**	��������:	��ˮ���¼���ѯ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void RecordDown(void)
{
    if(fuelrecordinfo.transnum < sysparas.transnum)
    {
        fuelrecordinfo.transnum++;
        globalvar.displayonce = 1;
    }

    DispUI = DispRecord;
}

/************************************************************************
**	�������ƣ�DispRecord(void)
**	��������:	��ˮ��¼��ѯ��ʾ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispRecord(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        ReadFuelRecord(fuelrecordinfo.transnum - 1, 0);   //�˺���������Ҫ����Ƿ���Ҫ�޸� ADD BY LY        
    }

	FYD12864DispPrintfFlash(1, 1, "��ˮ��:%ld", fuelrecordinfo.transnum);
    FYD12864DispPrintfFlash(2, 1, "����:%ld.%02d", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);
    FYD12864DispPrintfFlash(3, 1, "����:%d.%02d", fuelrecordinfo.price / 100, fuelrecordinfo.price % 100);
    FYD12864DispPrintfFlash(4, 1, "���:%ld.%02d", fuelrecordinfo.money / 100, fuelrecordinfo.money % 100);
    
    TaskAdd(KEY_UP, RecordUp, NULL);
    TaskAdd(KEY_DOWN, RecordDown, NULL);
    TaskAdd(KEY_RET, DispQuery1, NULL);
    TaskAdd(KEY_OK, Printerid, NULL);
}

/************************************************************************
**	�������ƣ�Printerid(void)
**	��������:	��ӡ��ˮ��¼
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void Printerid(void)
{
    Printid();
    DispUI = DispRecord;
}

/************************************************************************
**	�������ƣ�PrinterSysInfo(void)
**	��������:	��ӡϵͳ����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void PrinterSysInfo(void)
{
    PrintSysInfo();
    DispUI = DispSysParas1;
}

/************************************************************************
**	�������ƣ�DispSysParas1(void)
**	��������:	ϵͳ��Ϣ��ʾ1(ǹ�š����ۡ��޸���ˮ�š��Ѵ��޸���ˮ��)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSysParas1(void)
{
    FYD12864DispPrintfFlash(1, 1, "ǹ��%02d", sysparas.gunnum);
    FYD12864DispPrintfFlash(2, 1, "����%d.%02d", sysparas.price[0] / 100, sysparas.price[0] % 100);
    FYD12864DispPrintfFlash(3, 1, "�޸���ˮ��%ld", sysparas.modnum);
    FYD12864DispPrintfFlash(4, 1, "�Ѵ��޸ĺ�%ld", sysparas.upmodnum);

//    TaskAdd( KEY_UP, DispSysParas8, NULL );
    TaskAdd(KEY_DOWN, DispSysParas2, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	�������ƣ�DispSysParas2(void)
**	��������:	ϵͳ��Ϣ��ʾ2(���������ܽ���š������)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSysParas2(void)
{
    FYD12864DispPrintfFlash(1, 1, "������%ld.%02ld", sysparas.totvolume / 100, sysparas.totvolume % 100);
    FYD12864DispPrintfFlash(2, 1, "�ܽ��%ld.%02ld", sysparas.totmoney / 100, sysparas.totmoney % 100);
    FYD12864DispPrintfFlash(3, 1, "���%d", sysparas.shiftnum);
    FYD12864DispPrintfFlash(4, 1, "�����%d", sysparas.shiftmaxnum);

    TaskAdd(KEY_UP, DispSysParas1, NULL);
    TaskAdd(KEY_DOWN, DispSysParas3, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	�������ƣ�DispSysParas3(void)
**	��������:	ϵͳ��Ϣ��ʾ3(���������͡�������λ���ʼ�ģʽ��EEд����)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSysParas3(void)
{
    FYD12864DispPrintfFlash(1, 1, "����������%d", sysparas.modbustype);
    FYD12864DispPrintfFlash(2, 1, "������λ%d", sysparas.unit);
    FYD12864DispPrintfFlash(3, 1, "�ʼ�ģʽ%d", sysparas.qcmode);
    FYD12864DispPrintfFlash(4, 1, "EEд����%ld", (SYSPARA_ADDR / sizeof(Sysparas))*EEPROM_W_TIMES + sysparas.writeE2num);

    TaskAdd(KEY_UP, DispSysParas2, NULL);
    TaskAdd(KEY_DOWN, DispSysParas4, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	�������ƣ�DispSysParas4(void)
**	��������:	ϵͳ��Ϣ��ʾ4�ܶȡ�ѹ��ϵ����ѹ����㡢ѹ��ϵ��)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSysParas4(void)
{
    FYD12864DispPrintfFlash(1, 1, "�ܶ�%ld.%04ld", sysparas.densty / 10000, sysparas.densty % 10000);
    FYD12864DispPrintfFlash(2, 1, "����ϵ��%ld.%04ld", sysparas.coeff / 10000, sysparas.coeff % 10000);
    FYD12864DispPrintfFlash(3, 1, "ѹ�����%ld", sysparas.pzero);
    FYD12864DispPrintfFlash(4, 1, "ѹ��ϵ��%ld", sysparas.pcoeff);

    TaskAdd(KEY_UP, DispSysParas3, NULL);
    TaskAdd(KEY_DOWN, DispSysParas5, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	�������ƣ�DispSysParas5(void)
**	��������:	ϵͳ��Ϣ��ʾ5(���������ޡ����������ޡ�PASAM����ѡ�񡢸�ƿ����������)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSysParas5(void)
{

    FYD12864DispPrintfFlash(1, 1, "������%ld.%02ld", sysparas.cardmaxyue / 100, sysparas.cardmaxyue % 100);
    FYD12864DispPrintfFlash(2, 1, "������%ld.%02ld", sysparas.cardminyue / 100, sysparas.cardminyue % 100);
    FYD12864DispPrintfFlash(3, 1, "PSAM����%d", sysparas.cardpsamnum);
    FYD12864DispPrintfFlash(4, 1, "�������%d", sysparas.cardmaxday);

    TaskAdd(KEY_UP, DispSysParas4, NULL);
    TaskAdd(KEY_DOWN, DispSysParas6, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	�������ƣ�DispSysParas6(void)
**	��������:	ϵͳ��Ϣ��ʾ6(�Ƿ�����ƿ�������Ʋ����ʡ���̨�����ʡ�����ģʽ)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSysParas6(void)
{
    FYD12864DispPrintfFlash(1, 1, "�Ƿ�����ƿ%d", sysparas.cardcheck);
    FYD12864DispPrintfFlash(2, 1, "�����Ʋ���%ld", sysparas.modbusbaud);
    FYD12864DispPrintfFlash(3, 1, "��̨������%ld", sysparas.pcbaud);
    FYD12864DispPrintfFlash(4, 1, "����ģʽ%d", sysparas.fuelmode);

    TaskAdd(KEY_UP, DispSysParas5, NULL);
    TaskAdd(KEY_DOWN, DispSysParas7, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	�������ƣ�DispSysParas7(void)
**	��������:	ϵͳ��Ϣ��ʾ7(��ӡ��ʽ�����۰汾�š�ͨ����Ϣ�汾�š���Կ�汾��)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSysParas7(void)
{
    FYD12864DispPrintfFlash(1, 1, "��ӡ��ʽ%d", sysparas.printmode);
    FYD12864DispPrintfFlash(2, 1, "���۰��%d", sysparas.pricever);
    FYD12864DispPrintfFlash(3, 1, "ͨ����Ϣ���%d", sysparas.comminfover);
    FYD12864DispPrintfFlash(4, 1, "��Կ���%d", sysparas.lockver);

    TaskAdd(KEY_UP, DispSysParas6, NULL);
    TaskAdd(KEY_DOWN, DispSysParas8, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	�������ƣ�DispSysParas8(void)
**	��������:	ϵͳ��Ϣ��ʾ8(�Ƿ�ʹ�ü��ܰ�)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSysParas8(void)
{

    FYD12864DispPrintfFlash(1, 1, "�Ƿ�ʹ�ü��ܰ�%d", sysparas.usepwdboard);
    FYD12864ClearLine(2);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);

    TaskAdd(KEY_UP, DispSysParas7, NULL);
//    TaskAdd( KEY_DOWN, DispSysParas1, NULL );
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	�������ƣ�DispSystemVer(void)
**	��������:	��ѯϵͳ����汾��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispSystemVer(void)
{
    FYD12864ClearLine(1);
    FYD12864DispPrintfFlash(2, 1, "ϵͳ�汾��");
    FYD12864DispPrintfFlash(3, 1, VERSION);
    FYD12864ClearLine(4);

    TaskAdd(KEY_RET, DispQuery1, NULL);
}

/************************************************************************
**	�������ƣ�DispQuery1(void)
**	��������:	��ѯ�˵���ʾ1(�������ݡ��������ݡ���ˮ��¼�����Ʋ�����ѯ)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispQuery1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.�������ݲ�ѯ");
    FYD12864DispPrintfFlash(2, 1, "2.�������ݲ�ѯ");
    FYD12864DispPrintfFlash(3, 1, "3.��ˮ��¼��ѯ");
    FYD12864DispPrintfFlash(4, 1, "4.���Ʋ�����ѯ");

    TaskAdd(KEY_1, DispSum, NULL);
    TaskAdd(KEY_2, DispShiftSum, NULL);
    TaskAdd(KEY_3, DispInputTransNum, NULL);
    TaskAdd(KEY_4, DispCtrlParas1, NULL);
    TaskAdd(KEY_5, DispSystemVer, NULL);
    TaskAdd(KEY_DOWN, DispQuery2, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/************************************************************************
**	�������ƣ�DispQuery2(void)
**	��������:	��ѯ�˵���ʾ2(ϵͳ��Ϣ������ˮ�š��޸���ˮ�Ų�ѯ)
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispQuery2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.ϵͳ��Ϣ��ѯ");
    FYD12864DispPrintfFlash(2, 1, "2.����ˮ�Ų�ѯ");
    FYD12864DispPrintfFlash(3, 1, "3.�޸���ˮ��ѯ");
//    FYD12864DispPrintfFlash( 4, 1, "4.��������ѯ");
    FYD12864ClearLine(4);

    TaskAdd(KEY_1, DispSysParas1, NULL);
    TaskAdd(KEY_2, DispInputShifTransNum, NULL);
    TaskAdd(KEY_3, DispInputXGNum, NULL);
//    TaskAdd( KEY_4, DispHei, NULL );
    TaskAdd(KEY_UP, DispQuery1, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/************************************************************************
**	�������ƣ�DispInputXGNum(void)
**	��������:	�����޸���ˮ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void DispInputXGNum(void)
{
    if(sysparas.modnum > 0)
    {
        //FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(1, 1, "          %05ld", sysparas.modnum);
        FYD12864DispPrintfFlash(2, 1, "�������޸���ˮ��", 1);
        FYD12864DispPrintfFlash(3, 1, "%s", ScreenInput.array);


        ScreenInput.inputmaxlen = 16;
        ScreenInput.pointnum = 0;
        globalvar.xgtransnum = atol((char *)ScreenInput.array);
        NumKeyHandler();
        TaskAdd(KEY_RET, DispQuery2, NULL);

        if(globalvar.xgtransnum <= sysparas.modnum)
        {
            FYD12864ClearLine(4);

            if(globalvar.xgtransnum == 0)
            {
                globalvar.xgtransnum = 1;
            }

            TaskAdd(KEY_OK, DispXGRecord, NULL);
        }
        else
        {
            FYD12864DispPrintfFlash(4, 1, "��ˮ�ų���");
        }
    }
    else
    {
        FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(2, 1, "  ���޸ļ�¼");
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
        TaskMessage(20, DispQuery2);
    }
}
//�޸���ˮ��¼��ѯ��ʾ
void DispXGRecord(void)
{
#if 0
    uint32_t addr, id;
    uint8_t *pt;

    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        id = globalvar.xgtransnum % FlashModMaxID;
        addr = FlashModSADDR + (id - 1) * FlashModLen;
        pt = (uint8_t *)(&tmp.modifyid);
        ReadFlash(1, addr, pt, sizeof(Reply5B));
    }

    if((tmp.paraindex >= 73) && (tmp.paraindex <= 77))
    {
        FYD12864DispPrintf(1, 1, "�޸���ˮ��%ld",  tmp.modifyid);
        FYD12864DispPrintf(2, 1, "����������%d",  tmp.paraindex);
        FYD12864DispPrintf(3, 1, "��ֵ��******", 1);
        FYD12864DispPrintf(4, 1, "��ֵ��******", 1);
    }
    else if(tmp.paraindex == 30 || tmp.paraindex == 31)
    {
        if(tmp.paraindex == 30)
        {
            FYD12864DispPrintf(1, 1, "�ܶ�  ��ˮ��%ld",  tmp.modifyid);
        }
        else if(tmp.paraindex == 31)
        {
            FYD12864DispPrintf(1, 1, "����  ��ˮ��%ld",  tmp.modifyid);
        }

        FYD12864DispPrintf(2, 1, "��ֵ��%ld.%02d",  tmp.beforeparameter / 10000, tmp.beforeparameter % 10000);
        FYD12864DispPrintf(3, 1, "��ֵ��%ld.%02d",  tmp.afterparameter / 10000, tmp.afterparameter % 10000);
        FYD12864DispPrintf(4, 1, "%02x-%02x-%02x%02x:%02x:%02x", tmp.modifytime[1], tmp.modifytime[2], tmp.modifytime[3],
                           tmp.modifytime[4], tmp.modifytime[5], tmp.modifytime[6]);
    }
    else
    {
        if(tmp.dexnum == 2)
        {
            FYD12864DispPrintf(1, 1, "�޸���ˮ��%ld",  tmp.modifyid);		//tmp��һ��Reply5B�ṹ��
            FYD12864DispPrintf(2, 1, "����������%d",  tmp.paraindex);
            FYD12864DispPrintf(3, 1, "��ֵ��%ld.%02d",  tmp.beforeparameter / 100, tmp.beforeparameter % 100);
            FYD12864DispPrintf(4, 1, "��ֵ��%ld.%02d",  tmp.afterparameter / 100, tmp.afterparameter % 100);
        }
        else if(tmp.dexnum == 4)
        {
            FYD12864DispPrintf(1, 1, "�޸���ˮ��%ld",  tmp.modifyid);
            FYD12864DispPrintf(2, 1, "����������%d",  tmp.paraindex);
            FYD12864DispPrintf(3, 1, "��ֵ��%ld.%02d",  tmp.beforeparameter / 10000, tmp.beforeparameter % 10000);
            FYD12864DispPrintf(4, 1, "��ֵ��%ld.%02d",  tmp.afterparameter / 10000, tmp.afterparameter % 10000);
        }
        else if(tmp.dexnum == 7)
        {
            FYD12864DispPrintf(1, 1, "�޸���ˮ��%ld",  tmp.modifyid);
            FYD12864DispPrintf(2, 1, "����������%d",  tmp.paraindex);
            FYD12864DispPrintf(3, 1, "��ֵ��-%ld",  tmp.beforeparameter, tmp.beforeparameter);
            FYD12864DispPrintf(4, 1, "��ֵ��-%ld",  tmp.afterparameter, tmp.afterparameter);
        }
        else
        {
            FYD12864DispPrintf(1, 1, "�޸���ˮ��%ld",  tmp.modifyid);
            FYD12864DispPrintf(2, 1, "����������%d",  tmp.paraindex);
            FYD12864DispPrintf(3, 1, "��ֵ��%ld",  tmp.beforeparameter, tmp.beforeparameter);
            FYD12864DispPrintf(4, 1, "��ֵ��%ld",  tmp.afterparameter, tmp.afterparameter);
        }
    }

    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_UP, RecordXGUp, NULL);
    TaskAdd(KEY_DOWN, RecordXGDown, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
#endif
}

/************************************************************************
**	�������ƣ�RecordXGUp(void)
**	��������:	�޸���ˮ���¼���ѯ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void RecordXGUp(void)
{
    if(globalvar.xgtransnum > 1)
    {
        globalvar.xgtransnum--;
        globalvar.displayonce = 1;
    }

    DispUI = DispXGRecord;
}

/************************************************************************
**	�������ƣ�RecordXGDown(void)
**	��������:	�޸���ˮ���ϼ���ѯ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	
**	��������:	
*************************************************************************/
void RecordXGDown(void)
{
    if(globalvar.xgtransnum < sysparas.modnum)
    {
        globalvar.xgtransnum++;
        globalvar.displayonce = 1;
    }

    DispUI = DispXGRecord;
}

#if 0
void DebugHeiBai(uint32_t addr, uint32_t num, uint8_t *ptr)
{
    uint8_t pt[200], i;

//	uint8_t	blacktest[24]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
//	SaveFlash(0 , FlashBlkSADDR, blacktest, 24 );
//	SaveFlash(1 , FlashBlkSADDR, blacktest, 24 );


    if(globalvar.displayonce == 1)
    {
        //ReadFlash(1,addr, pt, num*8);//sysparas.blacknum *8);
        SELECT_PRINT();
        WHPrinterHead();
        WHPrintf("        %s\r\n", ptr);
        WHPrinterNormalCmd();

        for(i = 0; i < num; i++)   //sysparas.blacknum;i++)
        {
            WHPrintf("%d:%02x%02x%02x%02x%02x%02x%02x%02x\r\n", i, pt[i * 8], pt[i * 8 + 1], pt[i * 8 + 2], pt[i * 8 + 3], pt[i * 8 + 4], pt[i * 8 + 5], pt[i * 8 + 6], pt[i * 8 + 7]);
        }

        globalvar.displayonce = 0;
        SELECT_CARD();
    }

}
#endif
