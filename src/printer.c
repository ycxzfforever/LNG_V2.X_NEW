/************针对北京炜煌打印机驱动*********/
#include "kernel.h"

uint8_t stopreasonbuf[][16] =
{
    "手动停机",         // 0
    "气相流量计异常",   // 1
    "液相流量计异常",   // 2
    "瞬时数据错误",     // 3
    "压力传感器异常",   // 4
    "掉电或复位停机",   // 5
    "定气量停机",       // 6
    "定金额停机",       // 7
    "低流速停机",       // 8
    "高流速停机",       // 9
    "回气密度停机",     // 10
    "回气增益停机",     // 11
    "超压停机",         // 12
    "逃卡停机",         // 13
    "记账超额",         // 14
    "储蓄卡余额不足",//15
    "单次超额或超量",   // 16
};
	
/************************************************************************
**	函数名称:	WHPrinterInit(void)
**	函数功能:	打印机初始化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
**	函数名称:	WHPrinterNormalCmd(void)
**	函数功能:	常用打印模式
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void WHPrinterNormalCmd(void)
{
    uint8_t cmd[14] = {0x1b, 0x40, 0x1c, 0x26, 0x1b, 0x31, 0x08,
                       0x1b, 0x70, 0x02, 0x1b, 0x63, 0x00, 0x0D};
    //0x1c 26:进入汉字打印方式
    //0x1b 56 02:正常高度两倍打印
    //0x1b 63 00:字符从右向左的反向打印
    Uart0SendStr(cmd, 14);
}

/************************************************************************
**	函数名称:	WHPrinterFreeRow(uint8_t rows)
**	函数功能:	打印空行
**	输入参数: rows：行号
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void WHPrinterFreeRow(uint8_t rows)
{
    while(rows--)
    {
        Uart0SendByte(0x0A);   //换行
        Uart0SendByte(0x0D);   //回车，确认打印换行(及之前的)命令
    }
}

/************************************************************************
**	函数名称:	WHPrinterHead(void)
**	函数功能:	常用打印头
**	输入参数: rows：行号
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void WHPrinterHead(void)
{
    uint8_t buff[8] = {0x1c, 0x26, 0x1b, 0x56, 0x02, 0x1b, 0x63, 0x00};
    //0x1c 26:进入汉字打印方式
    //0x1b 56 02:正常高度两倍打印
    //0x1b 63 00:字符从右向左的反向打印
    Uart0SendStr(buff, 8);
}

/************************************************************************
**	函数名称:	WHPrinterCheck(void)
**	函数功能:	打印机返回确认信号
**	输入参数: 无
**	返回值  	: true:打印机确认成功；非true：打印机确认失败

**	创 建 者:	
**	创建日期:	
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
**	函数名称:	WHPrintReady(void)
**	函数功能:	打印机准备好
**	输入参数: 无
**	返回值  	: true:打印已准备好；false：打印机未准备好

**	创 建 者:	
**	创建日期:	
*************************************************************************/
uint8_t WHPrintReady(void)
{
    if(WHPrinterCheck() != true)    
		return false;

    return true;
}

/************************************************************************
**	函数名称:	WHPrintReady(void)
**	函数功能:	打印机按指定格式打印字符串
**	输入参数: fmt:字符指针，执行指定格式的字符串
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
**	函数名称:	WHPrintfFlash(char __farflash*fmt, ...)
**	函数功能:	打印机按指定格式打印字符串，不同的是，字符串存储于flash中
**	输入参数: fmt:字符指针，执行指定格式的字符串
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
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
**	函数名称:	WHPrintfString(char __farflash* str)
**	函数功能:	打印机打印一串存储于flash中的字符串
**	输入参数: str:字符指针，指向flash中的字符串
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void WHPrintfString(char __farflash* str)
{
    Uart0PutStringflash(str);
}

/************************************************************************
**	函数名称:	WHFuelRecord(void)
**	函数功能:	交易流水打印格式化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void WHFuelRecord(void)

{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("          LCNG加气凭条\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintfFlash("枪号:%02d #\r\n", sysparas.gunnum);
    WHPrintfFlash("班号:%02d #\r\n", fuelrecordinfo.classnum);
    WHPrintfFlash("流水号:%ld\r\n", fuelrecordinfo.transnum);
    if(sysparas.unit==2)
    {
	    WHPrintfFlash("单价:%d.%02d 元/方\r\n", fuelrecordinfo.price / 100, fuelrecordinfo.price % 100);
	    WHPrintfFlash("气量:%ld.%02ld 方\r\n", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);
	}
	else
	{
		WHPrintfFlash("单价:%d.%02d 元/kg\r\n", fuelrecordinfo.price / 100, fuelrecordinfo.price % 100);
	    WHPrintfFlash("气量:%ld.%02ld kg\r\n", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);	
	}
    WHPrintfFlash("金额:%ld.%02ld 元\r\n", fuelrecordinfo.money / 100, fuelrecordinfo.money % 100);

    if(fuelrecordinfo.usernum != 0)
    {
        WHPrintfFlash("卡号:%08ld\r\n", fuelrecordinfo.usernum);
        WHPrintfFlash("余额:%ld.%02ld 元\r\n", fuelrecordinfo.afterbalance / 100, fuelrecordinfo.afterbalance % 100);

    }

    if((sysparas.backgascalc == 1) && (fuelrecordinfo.stopreason != 0xFF))       //解灰数据不打印进液量和回气量
    {
        WHPrintfFlash("进液量:%ld.%02d kg\r\n", (fuelrecordinfo.volume + fuelrecordinfo.backgas) / 100, (fuelrecordinfo.volume + fuelrecordinfo.backgas) % 100);
        WHPrintfFlash("回气量:%ld.%02d kg\r\n", fuelrecordinfo.backgas / 100, fuelrecordinfo.backgas % 100);
    }
    if(sysparas.IsPrintCarnum == 1)
		WHPrintfFlash("车牌号:%04ld\r\n", fuelrecordinfo.carnum_another);
	WHPrintfFlash("站编码:%ld\r\n", sysparas.stationcode);
    WHPrintfFlash("停机原因:%s\r\n", stopreasonbuf[fuelrecordinfo.stopreason]);
    WHPrintfFlash("开始时间:20%02X-%02X-%02X %02X:%02X:%02X\r\n", fuelrecordinfo.starttime[0], fuelrecordinfo.starttime[1], fuelrecordinfo.starttime[2],
                  fuelrecordinfo.starttime[3], fuelrecordinfo.starttime[4], fuelrecordinfo.starttime[5]);
    WHPrintfFlash("结束时间:20%02X-%02X-%02X %02X:%02X:%02X\r\n", fuelrecordinfo.endtime[0], fuelrecordinfo.endtime[1], fuelrecordinfo.endtime[2],
                  fuelrecordinfo.endtime[3], fuelrecordinfo.endtime[4], fuelrecordinfo.endtime[5]);
    WHPrinterFreeRow(1);
    WHPrintfString("     欢迎光临  出行平安\r\n");   //下分割线
    WHPrintfString("     <请妥善保存此凭条>\r\n");   //脚注
    WHPrinterFreeRow(6);
    SELECT_CARD();
}

/************************************************************************
**	函数名称:	 Printid(void)
**	函数功能:	打印流水记录
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void Printid(void)
{
    WHPrinterInit();	//初始化。

    if(WHPrintReady())	//打印机已准备好
    {
        WHFuelRecord();	//打印流水
    }
}

/************************************************************************
**	函数名称：StopPrint(void)
**	函数功能:	停止打印
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void StopPrint(void)
{
    Printid();
    DispUI = DispFirst;
}

/************************************************************************
**	函数名称：WHFuelShifRecord(void)
**	函数功能:	班流水打印格式化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void WHFuelShifRecord(void)
//打印班流水记录
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("         LCNG班累记录\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintfFlash("枪号:%d\r\n", sysparas.gunnum);
    WHPrintfFlash("班号:%d\r\n", shiftrecordinfo.classnum);
    WHPrintfFlash("班流水号:%ld\r\n", shiftrecordinfo.classid);
    WHPrintfFlash("班加气次数:%ld\r\n", shiftrecordinfo.classtimes);
    WHPrintfFlash("班气量累积:%ld.%02d\r\n", shiftrecordinfo.classgassum / 100, shiftrecordinfo.classgassum % 100);
    WHPrintfFlash("班金额累计:%ld.%02d\r\n", shiftrecordinfo.classgasmoney / 100, shiftrecordinfo.classgasmoney % 100);
    WHPrintfFlash("枪气量累计:%ld.%02d\r\n", shiftrecordinfo.endallgunvolume / 100, shiftrecordinfo.endallgunvolume % 100);
    WHPrintfFlash("枪金额累计:%ld.%02d\r\n", shiftrecordinfo.endagunmoney / 100, shiftrecordinfo.endagunmoney % 100);

    if(shiftrecordinfo.devicetype == 0)
    {
        WHPrintfString("气品代码:LNG\r\n");
    }
    else if(shiftrecordinfo.devicetype == 1)
    {
        WHPrintfString("气品代码:CNG\r\n");
    }

    if(shiftrecordinfo.measuretype == 1)
    {
        WHPrintfString("计量单位:公斤\r\n");
    }

    else if(shiftrecordinfo.measuretype == 2)
    {
        WHPrintfString("计量单位:标方\r\n");
    }

    WHPrintfFlash("开始时间:%02X-%02X-%02X %02X:%02X:%02X\r\n", shiftrecordinfo.classstarttime[0], shiftrecordinfo.classstarttime[1], shiftrecordinfo.classstarttime[2],
                  shiftrecordinfo.classstarttime[3], shiftrecordinfo.classstarttime[4], shiftrecordinfo.classstarttime[5]);
    WHPrintfFlash("结束时间:%02X-%02X-%02X %02X:%02X:%02X\r\n", shiftrecordinfo.classendtime[0], shiftrecordinfo.classendtime[1], shiftrecordinfo.classendtime[2],
                  shiftrecordinfo.classendtime[3], shiftrecordinfo.classendtime[4], shiftrecordinfo.classendtime[5]);
    WHPrinterFreeRow(1);
    WHPrintfString("     欢迎光临  出行平安\r\n");   //下分割线
    WHPrintfString("     <请妥善保存此凭条>\r\n");   //脚注
    WHPrinterFreeRow(6);
    SELECT_CARD();
}

/************************************************************************
**	函数名称：PrintSum(void)
**	函数功能:	打印总累
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void PrintSum(void)
{
    WHPrinterInit();//初始化。

    if(WHPrintReady())
    {
        WHFuelSumRecord();
    }
}

/************************************************************************
**	函数名称：WHFuelSumRecord(void)
**	函数功能:	总累打印格式化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void WHFuelSumRecord(void)
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("         LCNG总累记录\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintfFlash("枪号:%d\r\n", sysparas.gunnum);
    WHPrintfFlash("总累流水号:%ld\r\n", sysparas.transnum);
    WHPrintfFlash("总气量累计:%ld.%02d\r\n", sysparas.totvolume / 100, sysparas.totvolume % 100);
    WHPrintfFlash("总金额累计:%ld.%02d\r\n", sysparas.totmoney / 100, sysparas.totmoney % 100);

    if(fuelrecordinfo.devicetype == 1)
    {
        WHPrintfString("气品代码:LNG\r\n");
    }
    else if(fuelrecordinfo.devicetype == 2)
    {
        WHPrintfString("气品代码:CNG\r\n");
    }

    if(fuelrecordinfo.measuretype == 1)
    {
        WHPrintfString("计量单位:公斤\r\n");
    }
    else if(fuelrecordinfo.measuretype == 2)
    {
        WHPrintfString("计量单位:标方\r\n");
    }

    WHPrinterFreeRow(1);
    WHPrintfString("     <请妥善保存此凭条>\r\n");   //脚注
    WHPrinterFreeRow(6);
    SELECT_CARD();
}

/************************************************************************
**	函数名称：PrintSysInfo(void)
**	函数功能:	打印系统参数
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void PrintSysInfo(void)
{
    WHPrinterInit();//初始化。

    if(WHPrintReady())
    {
        WHFuelSysInfo();
    }
}

/************************************************************************
**	函数名称：WHFuelSysInfo(void)
**	函数功能:	系统参数打印格式化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void WHFuelSysInfo(void)
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("         LCNG系统参数\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();

    WHPrintfFlash("枪号:%d\r\n", sysparas.gunnum);
    WHPrintfFlash("单价:%d.%02d\r\n", sysparas.price[0] / 100, sysparas.price[0] % 100);
    WHPrintfFlash("修改流水号:%ld\r\n", sysparas.modnum);
    WHPrintfFlash("已传修改号:%ld\r\n", sysparas.upmodnum);
    WHPrintfFlash("总气量:%ld.%02d\r\n", sysparas.totvolume / 100, sysparas.totvolume % 100);
    WHPrintfFlash("总金额:%ld.%02d\r\n", sysparas.totmoney / 100, sysparas.totmoney % 100);
    WHPrintfFlash("班号:%d\r\n", sysparas.shiftnum);
    WHPrintfFlash("最大班号:%d\r\n", sysparas.shiftmaxnum);
    WHPrintfFlash("流量计类型:%d\r\n", sysparas.modbustype);
    WHPrintfFlash("计量单位:%d\r\n", sysparas.unit);
    WHPrintfFlash("质检模式:%d\r\n", sysparas.qcmode);
    WHPrintfFlash("EE写次数:%ld\r\n", (SYSPARA_ADDR / sizeof(Sysparas))*EEPROM_W_TIMES + sysparas.writeE2num);
    WHPrintfFlash("密度:%d.%04d\r\n", sysparas.densty / 10000, sysparas.densty % 10000);
    WHPrintfFlash("修正系数:%d.%04d\r\n", sysparas.coeff / 10000, sysparas.coeff % 10000);
    WHPrintfFlash("压力零点:%d\r\n", sysparas.pzero);
    WHPrintfFlash("压力系数:%d\r\n", sysparas.pcoeff);
    WHPrintfFlash("储蓄卡上限:%ld.%02d\r\n", sysparas.cardmaxyue / 100, sysparas.cardmaxyue % 100);
    WHPrintfFlash("储蓄卡下限:%ld.%02d\r\n", sysparas.cardminyue / 100, sysparas.cardminyue % 100);
    WHPrintfFlash("记账卡上限:%ld.%02d\r\n", sysparas.accountcardmaxyue / 100, sysparas.accountcardmaxyue % 100);
    WHPrintfFlash("PSAM卡座:%d\r\n", sysparas.cardpsamnum);
    WHPrintfFlash("钢瓶间隔天数:%d\r\n", sysparas.cardmaxday);
    WHPrintfFlash("是否检验钢瓶:%d\r\n", sysparas.cardcheck);
    WHPrintfFlash("流量计波特:%ld\r\n", sysparas.modbusbaud);
    WHPrintfFlash("后台波特率:%ld\r\n", sysparas.pcbaud);
    WHPrintfFlash("加气模式:%d\r\n", sysparas.fuelmode);
    WHPrintfFlash("打印方式:%d\r\n", sysparas.printmode);
    WHPrintfFlash("单价版本号:%d\r\n", sysparas.pricever);
    WHPrintfFlash("通用信息版本号:%d\r\n", sysparas.comminfover);
    WHPrintfFlash("密钥版本号:%d\r\n", sysparas.lockver);
    WHPrintfFlash("是否使用加密板:%d\r\n", sysparas.usepwdboard);
    WHPrintfFlash("班开始气量:%ld.%02d\r\n", sysparas.shiftstartvol / 100, sysparas.shiftstartvol % 100);
    WHPrintfFlash("班开始金额:%ld.%02d\r\n", sysparas.shiftstartmon / 100, sysparas.shiftstartmon % 100);
    WHPrintfFlash("班加气次数:%ld\r\n", sysparas.shiftfueltimes);
    WHPrintfFlash("班总气量:%ld.%02d\r\n", sysparas.shiftotvol / 100, sysparas.shiftotvol % 100);
    WHPrintfFlash("班总金额:%ld.%02d\r\n", sysparas.shiftotmon / 100, sysparas.shiftotmon % 100);
    WHPrintfFlash("流水号:%ld\r\n", sysparas.transnum);
    WHPrintfFlash("已传流水号:%ld\r\n", sysparas.uptransnum);
    WHPrintfFlash("班流水号:%ld\r\n", sysparas.shiftransnum);
    WHPrintfFlash("已传班流水号:%ld\r\n", sysparas.shiftuptransnum);
    WHPrintfString("系统版本号:");
    WHPrintfString(VERSION);
    WHPrintfString("\r\n");
    WHPrinterFreeRow(1);
    WHPrintfString("     <请妥善保存此凭条>\r\n");   //脚注
    WHPrinterFreeRow(6);
    SELECT_CARD();
}

/************************************************************************
**	函数名称：PrintShifid(void)
**	函数功能:	打印班流水
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void PrintShifid(void)
{
    WHPrinterInit();//初始化。

    if(WHPrintReady())
    {
        WHFuelShifRecord();
    }
}

/************************************************************************
**	函数名称：QueryPrintContInfo(void)
**	函数功能:	打印控制参数
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void QueryPrintContInfo(void)
{
    PrintContInfo();
    DispUI = DispCtrlParas1;
}

/************************************************************************
**	函数名称：PrintContInfo(void)
**	函数功能:	控制参数打印格式化
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	
**	创建日期:	
*************************************************************************/
void PrintContInfo(void)
{
    SELECT_PRINT();

    WHPrinterHead();
    WHPrintfString("         LNG控制参数\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintfFlash("预冷超时时间:%d\r\n", sysparas.precooltimeout);
    WHPrintfFlash("预冷临界温度:-%d\r\n", sysparas.precooltemp);
    WHPrintfFlash("预冷临界增益:%d\r\n", sysparas.precoolgain);
    WHPrintfFlash("预冷临界密度:%d.%04d\r\n", sysparas.precooldensty / 10000, sysparas.precooldensty % 10000);
    WHPrintfFlash("停机延时:%d\r\n", sysparas.delaystoptime);
    WHPrintfFlash("超压停机:%d.%02d\r\n", sysparas.pressurestop / 100, sysparas.pressurestop % 100);
    WHPrintfFlash("低流速停机:%d\r\n", sysparas.lowratestop);
    WHPrintfFlash("高流速停机:%d\r\n", sysparas.highratestop);
    WHPrintfFlash("回气密度停机:%d.%04d\r\n", sysparas.backdenstystop / 10000, sysparas.backdenstystop % 10000);
    WHPrintfFlash("回气增益停机:%d\r\n", sysparas.backgainstop);
    WHPrintfFlash("是否回气计量:%d\r\n", sysparas.backgascalc);
    WHPrintfFlash("预冷方式选择:%d\r\n", sysparas.precoolmode);
    WHPrintfFlash("停泵延时:%d\r\n", sysparas.stoppumptime);
    WHPrintfFlash("重新预冷延时:%d\r\n", sysparas.precoolfinishedtime);

    WHPrinterFreeRow(1);
    WHPrintfString("     <请妥善保存此凭条>\r\n");   //脚注
    WHPrinterFreeRow(6);
    SELECT_CARD();
}


