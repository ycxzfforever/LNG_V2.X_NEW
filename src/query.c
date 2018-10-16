
/***************************************************************************
**文本：query.c
**说明：用于查询显示，由于函数较多，且大同小异，
                        所以只对每个函数简单描述
**创建者 ：杨朝旭
**创建日期：2016-4-25 10:46:36

**修改者：
**修改日期：
**修改内容：
***************************************************************************/

#include "kernel.h"

//Reply5B  tmp;//用于修改记录查询变量使用。

/************************************************************************
**	函数名称：PrinterSum(void)
**	函数功能:	打印总累
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void PrinterSum(void)
{
    PrintSum();
    DispUI = DispSum;
}

/************************************************************************
**	函数名称：DispSum(void)
**	函数功能:	总累显示
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSum(void)
{
    FYD12864DispPrintfFlash(1, 1, "流水号%ld", sysparas.transnum);
    FYD12864DispPrintfFlash(2, 1, "已传流水号%ld", sysparas.uptransnum);
    FYD12864DispPrintfFlash(3, 1, "总气量%ld.%02d", sysparas.totvolume / 100, sysparas.totvolume % 100);
    FYD12864DispPrintfFlash(4, 1, "总金额%ld.%02d", sysparas.totmoney / 100, sysparas.totmoney % 100);

    TaskAdd(KEY_RET, DispQuery1, NULL);
    TaskAdd(KEY_OK, PrinterSum, NULL);
}

/************************************************************************
**	函数名称：DispShiftSum(void)
**	函数功能:	班累数据显示
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispShiftSum(void)
{
    FYD12864DispPrintfFlash(1, 1, "班号%d", sysparas.shiftnum);
    FYD12864DispPrintfFlash(2, 1, "班加气次数%ld", sysparas.shiftfueltimes);
    FYD12864DispPrintfFlash(3, 1, "班气量%ld.%02d", sysparas.shiftotvol / 100, sysparas.shiftotvol % 100);
    FYD12864DispPrintfFlash(4, 1, "班金额%ld.%02d", sysparas.shiftotmon / 100, sysparas.shiftotmon % 100);

    TaskAdd(KEY_RET, DispQuery1, NULL);
}

/************************************************************************
**	函数名称：DispInputTransNum(void)
**	函数功能:	输入流水号
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispInputTransNum(void)
{
    if(sysparas.transnum > 0)
    {

        FYD12864DispPrintfFlash(1, 1, "          %05ld", sysparas.transnum);
        FYD12864DispPrintfFlash(2, 1, "请输入流水号");
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
            FYD12864DispPrintfFlash(4, 1, "流水号超限");
        }
    }
    else
    {
        FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(2, 1, "  无流水记录");
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
        TaskMessage(20, DispQuery1);
    }
}

/************************************************************************
**	函数名称：DispInputShifTransNum(void)
**	函数功能:	输入班流水号
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispInputShifTransNum(void)
{
    //FYD12864ClearLine(1);

    FYD12864DispPrintfFlash(1, 1, "          %05ld", sysparas.shiftransnum);
    FYD12864DispPrintfFlash(2, 1, "请输入班流水号");
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
        FYD12864DispPrintfFlash(4, 1, "流水号超限");
    }
}

/************************************************************************
**	函数名称：DispshifRecord(void)
**	函数功能:	班流水记录显示
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispshifRecord(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        ReadFuelRecord(shiftrecordinfo.classid - 1, 1);   //此函数后续需要检查是否需要修改 ADD BY LY
    }

    FYD12864DispPrintfFlash(1, 1, "班流水号:%ld",  shiftrecordinfo.classid);
    FYD12864DispPrintfFlash(2, 1, "班加气次数:%ld",  shiftrecordinfo.classtimes);
    FYD12864DispPrintfFlash(3, 1, "班气量:%ld.%02d", shiftrecordinfo.classgassum / 100, shiftrecordinfo.classgassum % 100);
    FYD12864DispPrintfFlash(4, 1, "班金额:%ld.%02d", shiftrecordinfo.classgasmoney / 100, shiftrecordinfo.classgasmoney % 100);

    TaskAdd(KEY_UP, RecordShifUp, NULL);
    TaskAdd(KEY_DOWN, RecordShifDown, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, Printershifid, NULL);
}

/************************************************************************
**	函数名称：Printershifid(void)
**	函数功能:	打印班流水记录
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void Printershifid(void)
{
    PrintShifid();
    DispUI = DispshifRecord;
}

/************************************************************************
**	函数名称：RecordShifUp(void)
**	函数功能:	班流水号上键查询
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
**	函数名称：RecordShifDown(void)
**	函数功能:	班流水号下键查询
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
**	函数名称：RecordUp(void)
**	函数功能:	流水号上键查询
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
**	函数名称：RecordDown(void)
**	函数功能:	流水号下键查询
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
**	函数名称：DispRecord(void)
**	函数功能:	流水记录查询显示
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispRecord(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        ReadFuelRecord(fuelrecordinfo.transnum - 1, 0);   //此函数后续需要检查是否需要修改 ADD BY LY        
    }

	FYD12864DispPrintfFlash(1, 1, "流水号:%ld", fuelrecordinfo.transnum);
    FYD12864DispPrintfFlash(2, 1, "气量:%ld.%02d", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);
    FYD12864DispPrintfFlash(3, 1, "单价:%d.%02d", fuelrecordinfo.price / 100, fuelrecordinfo.price % 100);
    FYD12864DispPrintfFlash(4, 1, "金额:%ld.%02d", fuelrecordinfo.money / 100, fuelrecordinfo.money % 100);
    
    TaskAdd(KEY_UP, RecordUp, NULL);
    TaskAdd(KEY_DOWN, RecordDown, NULL);
    TaskAdd(KEY_RET, DispQuery1, NULL);
    TaskAdd(KEY_OK, Printerid, NULL);
}

/************************************************************************
**	函数名称：Printerid(void)
**	函数功能:	打印流水记录
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void Printerid(void)
{
    Printid();
    DispUI = DispRecord;
}

/************************************************************************
**	函数名称：PrinterSysInfo(void)
**	函数功能:	打印系统参数
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void PrinterSysInfo(void)
{
    PrintSysInfo();
    DispUI = DispSysParas1;
}

/************************************************************************
**	函数名称：DispSysParas1(void)
**	函数功能:	系统信息显示1(枪号、单价、修改流水号、已传修改流水号)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSysParas1(void)
{
    FYD12864DispPrintfFlash(1, 1, "枪号%02d", sysparas.gunnum);
    FYD12864DispPrintfFlash(2, 1, "单价%d.%02d", sysparas.price[0] / 100, sysparas.price[0] % 100);
    FYD12864DispPrintfFlash(3, 1, "修改流水号%ld", sysparas.modnum);
    FYD12864DispPrintfFlash(4, 1, "已传修改号%ld", sysparas.upmodnum);

//    TaskAdd( KEY_UP, DispSysParas8, NULL );
    TaskAdd(KEY_DOWN, DispSysParas2, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	函数名称：DispSysParas2(void)
**	函数功能:	系统信息显示2(总气量、总金额、班号、最大班号)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSysParas2(void)
{
    FYD12864DispPrintfFlash(1, 1, "总气量%ld.%02ld", sysparas.totvolume / 100, sysparas.totvolume % 100);
    FYD12864DispPrintfFlash(2, 1, "总金额%ld.%02ld", sysparas.totmoney / 100, sysparas.totmoney % 100);
    FYD12864DispPrintfFlash(3, 1, "班号%d", sysparas.shiftnum);
    FYD12864DispPrintfFlash(4, 1, "最大班号%d", sysparas.shiftmaxnum);

    TaskAdd(KEY_UP, DispSysParas1, NULL);
    TaskAdd(KEY_DOWN, DispSysParas3, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	函数名称：DispSysParas3(void)
**	函数功能:	系统信息显示3(流量计类型、计量单位、质检模式、EE写次数)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSysParas3(void)
{
    FYD12864DispPrintfFlash(1, 1, "流量计类型%d", sysparas.modbustype);
    FYD12864DispPrintfFlash(2, 1, "计量单位%d", sysparas.unit);
    FYD12864DispPrintfFlash(3, 1, "质检模式%d", sysparas.qcmode);
    FYD12864DispPrintfFlash(4, 1, "EE写次数%ld", (SYSPARA_ADDR / sizeof(Sysparas))*EEPROM_W_TIMES + sysparas.writeE2num);

    TaskAdd(KEY_UP, DispSysParas2, NULL);
    TaskAdd(KEY_DOWN, DispSysParas4, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	函数名称：DispSysParas4(void)
**	函数功能:	系统信息显示4密度、压力系数、压力零点、压力系数)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSysParas4(void)
{
    FYD12864DispPrintfFlash(1, 1, "密度%ld.%04ld", sysparas.densty / 10000, sysparas.densty % 10000);
    FYD12864DispPrintfFlash(2, 1, "修正系数%ld.%04ld", sysparas.coeff / 10000, sysparas.coeff % 10000);
    FYD12864DispPrintfFlash(3, 1, "压力零点%ld", sysparas.pzero);
    FYD12864DispPrintfFlash(4, 1, "压力系数%ld", sysparas.pcoeff);

    TaskAdd(KEY_UP, DispSysParas3, NULL);
    TaskAdd(KEY_DOWN, DispSysParas5, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	函数名称：DispSysParas5(void)
**	函数功能:	系统信息显示5(储蓄卡余额上限、储蓄卡余额下限、PASAM卡座选择、钢瓶检验间隔天数)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSysParas5(void)
{

    FYD12864DispPrintfFlash(1, 1, "卡上限%ld.%02ld", sysparas.cardmaxyue / 100, sysparas.cardmaxyue % 100);
    FYD12864DispPrintfFlash(2, 1, "卡下限%ld.%02ld", sysparas.cardminyue / 100, sysparas.cardminyue % 100);
    FYD12864DispPrintfFlash(3, 1, "PSAM卡座%d", sysparas.cardpsamnum);
    FYD12864DispPrintfFlash(4, 1, "间隔天数%d", sysparas.cardmaxday);

    TaskAdd(KEY_UP, DispSysParas4, NULL);
    TaskAdd(KEY_DOWN, DispSysParas6, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	函数名称：DispSysParas6(void)
**	函数功能:	系统信息显示6(是否检验钢瓶、流量计波特率、后台波特率、加气模式)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSysParas6(void)
{
    FYD12864DispPrintfFlash(1, 1, "是否检验钢瓶%d", sysparas.cardcheck);
    FYD12864DispPrintfFlash(2, 1, "流量计波特%ld", sysparas.modbusbaud);
    FYD12864DispPrintfFlash(3, 1, "后台波特率%ld", sysparas.pcbaud);
    FYD12864DispPrintfFlash(4, 1, "加气模式%d", sysparas.fuelmode);

    TaskAdd(KEY_UP, DispSysParas5, NULL);
    TaskAdd(KEY_DOWN, DispSysParas7, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	函数名称：DispSysParas7(void)
**	函数功能:	系统信息显示7(打印方式、单价版本号、通用信息版本号、密钥版本号)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSysParas7(void)
{
    FYD12864DispPrintfFlash(1, 1, "打印方式%d", sysparas.printmode);
    FYD12864DispPrintfFlash(2, 1, "单价版号%d", sysparas.pricever);
    FYD12864DispPrintfFlash(3, 1, "通用信息版号%d", sysparas.comminfover);
    FYD12864DispPrintfFlash(4, 1, "密钥版号%d", sysparas.lockver);

    TaskAdd(KEY_UP, DispSysParas6, NULL);
    TaskAdd(KEY_DOWN, DispSysParas8, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	函数名称：DispSysParas8(void)
**	函数功能:	系统信息显示8(是否使用加密板)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSysParas8(void)
{

    FYD12864DispPrintfFlash(1, 1, "是否使用加密板%d", sysparas.usepwdboard);
    FYD12864ClearLine(2);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);

    TaskAdd(KEY_UP, DispSysParas7, NULL);
//    TaskAdd( KEY_DOWN, DispSysParas1, NULL );
    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_OK, PrinterSysInfo, NULL);
}

/************************************************************************
**	函数名称：DispSystemVer(void)
**	函数功能:	查询系统软件版本号
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispSystemVer(void)
{
    FYD12864ClearLine(1);
    FYD12864DispPrintfFlash(2, 1, "系统版本号");
    FYD12864DispPrintfFlash(3, 1, VERSION);
    FYD12864ClearLine(4);

    TaskAdd(KEY_RET, DispQuery1, NULL);
}

/************************************************************************
**	函数名称：DispQuery1(void)
**	函数功能:	查询菜单显示1(总累数据、班累数据、流水记录、控制参数查询)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispQuery1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.总累数据查询");
    FYD12864DispPrintfFlash(2, 1, "2.班累数据查询");
    FYD12864DispPrintfFlash(3, 1, "3.流水记录查询");
    FYD12864DispPrintfFlash(4, 1, "4.控制参数查询");

    TaskAdd(KEY_1, DispSum, NULL);
    TaskAdd(KEY_2, DispShiftSum, NULL);
    TaskAdd(KEY_3, DispInputTransNum, NULL);
    TaskAdd(KEY_4, DispCtrlParas1, NULL);
    TaskAdd(KEY_5, DispSystemVer, NULL);
    TaskAdd(KEY_DOWN, DispQuery2, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/************************************************************************
**	函数名称：DispQuery2(void)
**	函数功能:	查询菜单显示2(系统信息、班流水号、修改流水号查询)
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispQuery2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.系统信息查询");
    FYD12864DispPrintfFlash(2, 1, "2.班流水号查询");
    FYD12864DispPrintfFlash(3, 1, "3.修改流水查询");
//    FYD12864DispPrintfFlash( 4, 1, "4.黑名单查询");
    FYD12864ClearLine(4);

    TaskAdd(KEY_1, DispSysParas1, NULL);
    TaskAdd(KEY_2, DispInputShifTransNum, NULL);
    TaskAdd(KEY_3, DispInputXGNum, NULL);
//    TaskAdd( KEY_4, DispHei, NULL );
    TaskAdd(KEY_UP, DispQuery1, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/************************************************************************
**	函数名称：DispInputXGNum(void)
**	函数功能:	输入修改流水号
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void DispInputXGNum(void)
{
    if(sysparas.modnum > 0)
    {
        //FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(1, 1, "          %05ld", sysparas.modnum);
        FYD12864DispPrintfFlash(2, 1, "请输入修改流水号", 1);
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
            FYD12864DispPrintfFlash(4, 1, "流水号超限");
        }
    }
    else
    {
        FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(2, 1, "  无修改记录");
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
        TaskMessage(20, DispQuery2);
    }
}
//修改流水记录查询显示
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
        FYD12864DispPrintf(1, 1, "修改流水号%ld",  tmp.modifyid);
        FYD12864DispPrintf(2, 1, "参数索引号%d",  tmp.paraindex);
        FYD12864DispPrintf(3, 1, "旧值：******", 1);
        FYD12864DispPrintf(4, 1, "新值：******", 1);
    }
    else if(tmp.paraindex == 30 || tmp.paraindex == 31)
    {
        if(tmp.paraindex == 30)
        {
            FYD12864DispPrintf(1, 1, "密度  流水号%ld",  tmp.modifyid);
        }
        else if(tmp.paraindex == 31)
        {
            FYD12864DispPrintf(1, 1, "比例  流水号%ld",  tmp.modifyid);
        }

        FYD12864DispPrintf(2, 1, "旧值：%ld.%02d",  tmp.beforeparameter / 10000, tmp.beforeparameter % 10000);
        FYD12864DispPrintf(3, 1, "新值：%ld.%02d",  tmp.afterparameter / 10000, tmp.afterparameter % 10000);
        FYD12864DispPrintf(4, 1, "%02x-%02x-%02x%02x:%02x:%02x", tmp.modifytime[1], tmp.modifytime[2], tmp.modifytime[3],
                           tmp.modifytime[4], tmp.modifytime[5], tmp.modifytime[6]);
    }
    else
    {
        if(tmp.dexnum == 2)
        {
            FYD12864DispPrintf(1, 1, "修改流水号%ld",  tmp.modifyid);		//tmp是一个Reply5B结构体
            FYD12864DispPrintf(2, 1, "参数索引号%d",  tmp.paraindex);
            FYD12864DispPrintf(3, 1, "旧值：%ld.%02d",  tmp.beforeparameter / 100, tmp.beforeparameter % 100);
            FYD12864DispPrintf(4, 1, "新值：%ld.%02d",  tmp.afterparameter / 100, tmp.afterparameter % 100);
        }
        else if(tmp.dexnum == 4)
        {
            FYD12864DispPrintf(1, 1, "修改流水号%ld",  tmp.modifyid);
            FYD12864DispPrintf(2, 1, "参数索引号%d",  tmp.paraindex);
            FYD12864DispPrintf(3, 1, "旧值：%ld.%02d",  tmp.beforeparameter / 10000, tmp.beforeparameter % 10000);
            FYD12864DispPrintf(4, 1, "新值：%ld.%02d",  tmp.afterparameter / 10000, tmp.afterparameter % 10000);
        }
        else if(tmp.dexnum == 7)
        {
            FYD12864DispPrintf(1, 1, "修改流水号%ld",  tmp.modifyid);
            FYD12864DispPrintf(2, 1, "参数索引号%d",  tmp.paraindex);
            FYD12864DispPrintf(3, 1, "旧值：-%ld",  tmp.beforeparameter, tmp.beforeparameter);
            FYD12864DispPrintf(4, 1, "新值：-%ld",  tmp.afterparameter, tmp.afterparameter);
        }
        else
        {
            FYD12864DispPrintf(1, 1, "修改流水号%ld",  tmp.modifyid);
            FYD12864DispPrintf(2, 1, "参数索引号%d",  tmp.paraindex);
            FYD12864DispPrintf(3, 1, "旧值：%ld",  tmp.beforeparameter, tmp.beforeparameter);
            FYD12864DispPrintf(4, 1, "新值：%ld",  tmp.afterparameter, tmp.afterparameter);
        }
    }

    TaskAdd(KEY_RET, DispQuery2, NULL);
    TaskAdd(KEY_UP, RecordXGUp, NULL);
    TaskAdd(KEY_DOWN, RecordXGDown, NULL);
    TaskAdd(KEY_RET, DispQuery2, NULL);
#endif
}

/************************************************************************
**	函数名称：RecordXGUp(void)
**	函数功能:	修改流水号下键查询
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
**	函数名称：RecordXGDown(void)
**	函数功能:	修改流水号上键查询
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
