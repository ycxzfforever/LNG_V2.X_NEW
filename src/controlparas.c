#include "kernel.h"
/************************************************************************
**	函数名称:	DispSetPrecoolingTimeout(void)
**	函数功能:	预冷超时时间设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetPrecoolingTimeout(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 4;
    tmp_sysparas.precooltimeout = atol((char *)ScreenInput.array);
    globalvar.paraindex = 206;
    DispSet(0, sysparas.precooltimeout, "预冷超时时间");
    TaskAdd(KEY_RET, DispSetControl1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl1);
}

/************************************************************************
**	函数名称:	DispSetPrecoolingTemp(void)
**	函数功能:	预冷临界温度设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetPrecoolingTemp(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.precooltemp = atol((char *)ScreenInput.array);
    globalvar.paraindex = 203;
    DispSet(7, sysparas.precooltemp, "预冷临界温度设置");
    TaskAdd(KEY_RET, DispSetControl1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl1);
}

/************************************************************************
**	函数名称:	DispSetPrecoolingGain(void)
**	函数功能:	预冷临界增益设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetPrecoolingGain(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.precoolgain = atol((char *)ScreenInput.array);
    globalvar.paraindex = 204;
    DispSet(0, sysparas.precoolgain, "预冷临界增益设置");
    TaskAdd(KEY_RET, DispSetControl1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl1);
}

/************************************************************************
**	函数名称:	DispSetPrecoolingDensty(void)
**	函数功能:	预冷临界密度设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetPrecoolingDensty(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 10000);
    tmp_sysparas.precooldensty = atol(buf);
    globalvar.paraindex = 205;
    DispSet(4, sysparas.precooldensty, "预冷临界密度设置");
    TaskAdd(KEY_RET, DispSetControl1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl1);
}

/************************************************************************
**	函数名称:	DispSetControl1(void)
**	函数功能:	LNG设置控制参数1 菜单显示
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetControl1(void)
{
    InputInit();

    FYD12864DispPrintfFlash(1, 1, "1.预冷超时时间");
    FYD12864DispPrintfFlash(2, 1, "2.预冷临界温度");
    FYD12864DispPrintfFlash(3, 1, "3.预冷临界增益");
    FYD12864DispPrintfFlash(4, 1, "4.预冷临界密度");
    TaskAdd(KEY_1, DispSetPrecoolingTimeout, NULL);
    TaskAdd(KEY_2, DispSetPrecoolingTemp, NULL);
    TaskAdd(KEY_3, DispSetPrecoolingGain, NULL);
    TaskAdd(KEY_4, DispSetPrecoolingDensty, NULL);
    TaskAdd(KEY_DOWN, DispSetControl2, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/************************************************************************
**	函数名称:	DispSetDelayStopTime(void)
**	函数功能:	停机延时时间设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetDelayStopTime(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.delaystoptime = atol((char *)ScreenInput.array);
    globalvar.paraindex = 207;
    DispSet(0, sysparas.delaystoptime, "停机延时时间");
    TaskAdd(KEY_RET, DispSetControl2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl2);
}

/************************************************************************
**	函数名称:	DispSetPressureStop(void)
**	函数功能:	超压停机设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetPressureStop(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 4;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.pressurestop = atol(buf);
    globalvar.paraindex = 212;
    DispSet(2, sysparas.pressurestop, "超压停机设置");
    TaskAdd(KEY_RET, DispSetControl2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl2);
}

/************************************************************************
**	函数名称:	DispSetLowRateStop(void)
**	函数功能:	低停机流速设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetLowRateStop(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.lowratestop = atol((char *)ScreenInput.array);
    globalvar.paraindex = 208;
    DispSet(0, sysparas.lowratestop, "低停机流速设置");
    TaskAdd(KEY_RET, DispSetControl2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl2);
}

/************************************************************************
**	函数名称:	DispSetHighRateStop(void)
**	函数功能:	高停机流速设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetHighRateStop(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 4;
    tmp_sysparas.highratestop = atol((char *)ScreenInput.array);
    globalvar.paraindex = 209;
    DispSet(0, sysparas.highratestop, "高停机流速设置");

    TaskAdd(KEY_RET, DispSetControl2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl2);
}

/************************************************************************
**	函数名称:	DispSetControl2(void)
**	函数功能:	LNG设置控制参数2 菜单显示
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetControl2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.停机延时");
    FYD12864DispPrintfFlash(2, 1, "2.超压停机");
    FYD12864DispPrintfFlash(3, 1, "3.低流速停机");
    FYD12864DispPrintfFlash(4, 1, "4.高流速停机");
    TaskAdd(KEY_1, DispSetDelayStopTime, NULL);
    TaskAdd(KEY_2, DispSetPressureStop, NULL);
    TaskAdd(KEY_3, DispSetLowRateStop, NULL);
    TaskAdd(KEY_4, DispSetHighRateStop, NULL);
    TaskAdd(KEY_UP, DispSetControl1, NULL);
    TaskAdd(KEY_DOWN, DispSetControl3, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/************************************************************************
**	函数名称:	DispSetBackDenstyStop(void)
**	函数功能:	回气密度停机设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetBackDenstyStop(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 10000);
    tmp_sysparas.backdenstystop = atol(buf);
    globalvar.paraindex = 210;
    DispSet(4, sysparas.backdenstystop, "回气密度停机");
    TaskAdd(KEY_RET, DispSetControl3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl3);
}

/************************************************************************
**	函数名称:	DispSetBackGainStop(void)
**	函数功能:	回气增益停机设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetBackGainStop(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.backgainstop = atol((char *)ScreenInput.array);
    globalvar.paraindex = 211;
    DispSet(0, sysparas.backgainstop, "回气增益停机");
    TaskAdd(KEY_RET, DispSetControl3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl3);
}

/************************************************************************
**	函数名称:	DispSetBackGasCalc(void)
**	函数功能:	是否回气计量设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetBackGasCalc(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1))
    {
        tmp_sysparas.backgascalc = tmp;
        globalvar.paraindex = 201;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.backgascalc, "是否回气计量");
    FYD12864DispPrintfFlash(2, 1, "0.不计量");
    FYD12864DispPrintfFlash(3, 1, "1.计量");
    TaskAdd(KEY_RET, DispSetControl3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl3);
}

/************************************************************************
**	函数名称:	DispSetPrecoolMode(void)
**	函数功能:	预冷方式选择
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetPrecoolMode(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1))
    {
        tmp_sysparas.precoolmode = tmp;
        globalvar.paraindex = 202;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.precoolmode, "是否回气计量");
    FYD12864DispPrintfFlash(2, 1, "0.小循环预冷");
    FYD12864DispPrintfFlash(3, 1, "1.大循环预冷");
    TaskAdd(KEY_RET, DispSetControl3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl3);
}
/************************************************************************
**	函数名称:	DispSetControl3(void)
**	函数功能:	LNG设置控制参数3 菜单显示
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetControl3(void)
{
    InputInit();

    FYD12864DispPrintfFlash(1, 1, "1.回气密度停机");
    FYD12864DispPrintfFlash(2, 1, "2.回气增益停机");
    FYD12864DispPrintfFlash(3, 1, "3.是否回气计量");
    FYD12864DispPrintfFlash(4, 1, "4.预冷方式选择");
    TaskAdd(KEY_1, DispSetBackDenstyStop, NULL);
    TaskAdd(KEY_2, DispSetBackGainStop, NULL);
    TaskAdd(KEY_3, DispSetBackGasCalc, NULL);
    TaskAdd(KEY_4, DispSetPrecoolMode, NULL);
    TaskAdd(KEY_UP, DispSetControl2, NULL);
    TaskAdd(KEY_DOWN, DispSetControl4, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/************************************************************************
**	函数名称:	DispSetStopPumpTime(void)
**	函数功能:	加液后停泵延时时间设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetStopPumpTime(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.stoppumptime = atol((char *)ScreenInput.array);
    globalvar.paraindex = 213;
    DispSet(0, sysparas.stoppumptime, "加液后停泵延时");
    TaskAdd(KEY_RET, DispSetControl4, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl4);
}

/************************************************************************
**	函数名称:	DispSetPreStartPumpTime(void)
**	函数功能:	预冷完成后起泵超时时间设置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetPreStartPumpTime(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.precoolfinishedtime = atol((char *)ScreenInput.array);
    globalvar.paraindex = 214;
    DispSet(0, sysparas.precoolfinishedtime, "重新预冷延时");
    TaskAdd(KEY_RET, DispSetControl4, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetControl4);
}

/************************************************************************
**	函数名称:	DispSetControl4(void)
**	函数功能:	LNG设置控制参数4 菜单显示
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispSetControl4(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.停泵延时");
    FYD12864DispPrintfFlash(2, 1, "2.重新预冷延时");
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetStopPumpTime, NULL);
    TaskAdd(KEY_2, DispSetPreStartPumpTime, NULL);
    TaskAdd(KEY_UP, DispSetControl3, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/************************************************************************
**	函数名称:	DispCtrlParas1(void)
**	函数功能:	控制参数1查询
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispCtrlParas1(void)
{
    FYD12864DispPrintfFlash(1, 1, "预冷超时时间%d", sysparas.precooltimeout);
    FYD12864DispPrintfFlash(2, 1, "预冷临界温度-%d", sysparas.precooltemp);
    FYD12864DispPrintfFlash(3, 1, "预冷临界增益%d", sysparas.precoolgain);
    FYD12864DispPrintfFlash(4, 1, "预冷临界密度%d.%04d", sysparas.precooldensty / 10000, sysparas.precooldensty % 10000);

    TaskAdd(KEY_DOWN, DispCtrlParas2, NULL);
    TaskAdd(KEY_RET, DispQuery1, NULL);
    TaskAdd(KEY_OK, QueryPrintContInfo, NULL);
}
/************************************************************************
**	函数名称:	DispCtrlParas2(void)
**	函数功能:	控制参数2查询
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispCtrlParas2(void)
{
    FYD12864DispPrintfFlash(1, 1, "停机延时%d", sysparas.delaystoptime);
    FYD12864DispPrintfFlash(2, 1, "超压停机%d.%02d", sysparas.pressurestop / 100, sysparas.pressurestop % 100);
    FYD12864DispPrintfFlash(3, 1, "低流速停机%d", sysparas.lowratestop);
    FYD12864DispPrintfFlash(4, 1, "高流速停机%d", sysparas.highratestop);

    TaskAdd(KEY_UP, DispCtrlParas1, NULL);
    TaskAdd(KEY_DOWN, DispCtrlParas3, NULL);
    TaskAdd(KEY_OK, QueryPrintContInfo, NULL);
    TaskAdd(KEY_RET, DispQuery1, NULL);
}

/************************************************************************
**	函数名称:	DispCtrlParas3(void)
**	函数功能:	控制参数3查询
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispCtrlParas3(void)
{
    FYD12864DispPrintfFlash(1, 1, "回气密度停机%d.%04d", sysparas.backdenstystop / 10000, sysparas.backdenstystop % 10000);
    FYD12864DispPrintfFlash(2, 1, "回气增益停机%d", sysparas.backgainstop);
    FYD12864DispPrintfFlash(3, 1, "是否回气计量%d", sysparas.backgascalc);
    FYD12864DispPrintfFlash(4, 1, "预冷方式选择%d", sysparas.precoolmode);

    TaskAdd(KEY_UP, DispCtrlParas2, NULL);
    TaskAdd(KEY_DOWN, DispCtrlParas4, NULL);
    TaskAdd(KEY_OK, QueryPrintContInfo, NULL);
    TaskAdd(KEY_RET, DispQuery1, NULL);
}

/************************************************************************
**	函数名称:	DispCtrlParas4(void)
**	函数功能:	控制参数4查询
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void DispCtrlParas4(void)
{
    FYD12864DispPrintfFlash(1, 1, "停泵延时%d", sysparas.stoppumptime);
    FYD12864DispPrintfFlash(2, 1, "重新预冷延时%d", sysparas.precoolfinishedtime);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_UP, DispCtrlParas3, NULL);
    TaskAdd(KEY_OK, QueryPrintContInfo, NULL);
    TaskAdd(KEY_RET, DispQuery1, NULL);
}


/************************************************************************
**	函数名称:	FactoryReset(void)
**	函数功能:	恢复出厂设置时，各个参数重置
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void FactoryReset(void)
{
    uint8_t i;

    tmp_sysparas.head = 0xAA;          //头
    tmp_sysparas.writeE2num = 0;       //写eeprom次数

//	tmp_sysparas.offonwork = 1;			//恢复出厂设置后处于上班状态

    tmp_sysparas.gunnum = 1;           //枪号
    tmp_sysparas.transnum = 0;         //交易流水号
    tmp_sysparas.uptransnum = 0;       //已传交易流水号

    tmp_sysparas.totvolume = 0;        //枪总气量    2位小数
    tmp_sysparas.totmoney = 0;         //枪总金额    2位小数

    for(i = 0; i < 20; i++)
        tmp_sysparas.price[i] = 450;    //20个单价，单价0为默认单价，根据单价等级选择相应单价

    //班组信息
    tmp_sysparas.shiftnum = 1;         //班号
    tmp_sysparas.shiftmaxnum = 3;      //最大班号
    tmp_sysparas.shiftransnum = 0;     //班流水号

    tmp_sysparas.shiftuptransnum = 0;  //已传班流水号

    tmp_sysparas.shiftfueltimes = 0;   //班加气次数
    tmp_sysparas.shiftstartvol = 0;    //班开始枪累    2位小数
    //tmp_sysparas.shiftstartmon = 0;    //班开始金额    2位小数
    tmp_sysparas.shiftotvol = 0;       //班累计气量    2位小数
    tmp_sysparas.shiftotmon = 0;       //班累计金额    2位小数

    tmp_sysparas.shiftstarttime[0] = time.year;  //班开始时间    BCD
    tmp_sysparas.shiftstarttime[1] = time.month;
    tmp_sysparas.shiftstarttime[2] = time.day;
    tmp_sysparas.shiftstarttime[3] = time.hour;
    tmp_sysparas.shiftstarttime[4] = time.minute;
    tmp_sysparas.shiftstarttime[5] = time.second;

    for(i = 0; i < 6; i++)
        tmp_sysparas.shiftendtime[i] = 0;  //班结束时间    BCD

    //通用信息
    tmp_sysparas.stationcode = 0;//站编码 ADD BY LY

    for(i = 0; i < 2; i++) tmp_sysparas.shengcode[i] = 0;   //省编码

    for(i = 0; i < 4; i++) tmp_sysparas.stationcodearray[i] = 0;   //站编码

    for(i = 0; i < 2; i++) tmp_sysparas.citycode[i] = 0;   //地市代码

    for(i = 0; i < 4; i++) tmp_sysparas.parentcode[i] = 0;   //上级单位代码

    //卡信息
    tmp_sysparas.cardmaxyue = 100000000; //储蓄卡卡余额上限    2位小数 默认100万
    tmp_sysparas.accountcardmaxyue = 100000000; //记账卡卡余额上限    2位小数 默认100万
    tmp_sysparas.cardminyue = 100;       //卡余额下限    2位小数
    tmp_sysparas.cardpsamnum = 1;        //psam卡卡座选择
    tmp_sysparas.cardmaxday = 90;        //卡未使用最大间隔天数
    tmp_sysparas.cardcheck = 0;          //是否检验钢瓶
    tmp_sysparas.usecard = 0;            //默认不用卡

    //波特率设置
    tmp_sysparas.modbusbaud = 19200;     //流量计波特率
    tmp_sysparas.pcbaud = 38400;         //后台波特率

    tmp_sysparas.fuelmode = 0;           //加气模式
    tmp_sysparas.printmode = 0;          //小票打印方式

    //版本编号设置
    tmp_sysparas.pricever = 0;           //单价版本号
//    tmp_sysparas.blackver = 0;           //黑名单版本号
//    tmp_sysparas.whitever=0;             //白名单版本号
//    tmp_sysparas.addbkver=0;             //新增黑名单版本号
//    tmp_sysparas.delbkver=0;             //新删黑名单版本号
//    tmp_sysparas.comminfover=0;          //通用信息版本号

    //密码设置
    tmp_sysparas.menupwd = 0;            //菜单进入密码
    tmp_sysparas.qcpwd = 0;              //质检密码
    tmp_sysparas.testpwd = 0;            //测试密码，如按键、电磁阀等测试
    tmp_sysparas.resetpwd = 0;           //恢复出厂密码

    for(i = 0; i < 6; i++)
        tmp_sysparas.pwdboard[i] = 0;      //加密板数据

    tmp_sysparas.usepwdboard = 0;        //0-不使用加密板，1-使用加密板


    tmp_sysparas.modbustype = 1;         //流量计类型
    tmp_sysparas.unit = 1;               //计量单位
    tmp_sysparas.qcmode = 0;             //质检模式设置

    tmp_sysparas.backgascalc = 0;        //0-回气不计量，1-回气计量
    tmp_sysparas.precoolmode = 0;        //预冷方式，0-小循环，1-大循环
    tmp_sysparas.precooltemp = 120;      //预冷临界温度
    tmp_sysparas.precoolgain = 100;      //预冷临界增益
    tmp_sysparas.precooldensty = 3000;   //预冷临界密度，4位小数，0-1.0000
    tmp_sysparas.precooltimeout = 900;   //预冷超时时间
    tmp_sysparas.delaystoptime = 30;     //刚开始加液时,不判断停机条件的延时时间
    tmp_sysparas.lowratestop = 300;      //低流速停机
    tmp_sysparas.highratestop = 6000;    //高流速停机
    tmp_sysparas.backdenstystop = 4000;  //回气密度停机，4位小数，0-1.0000
    tmp_sysparas.backgainstop = 30;      //回气增益停机
    tmp_sysparas.pressurestop = 180;     //停机压力,超压停机，两位小数
    tmp_sysparas.stoppumptime = 10;      //加液后延时停泵

    tmp_sysparas.densty = 6804;          //密度              4位小数
    tmp_sysparas.coeff = 10000;          //修正系数  4位小数
    tmp_sysparas.pzero = 145;            //压力零点
    tmp_sysparas.pcoeff = 227;           //压力系数  2位小数

    tmp_sysparas.modnum = 0;             //修改记录流水号
    tmp_sysparas.upmodnum = 0;           //已传修改记录流水号
    tmp_sysparas.simulation = 0;         //默认正常加气。
    tmp_sysparas.intnumflag = 0;         //默认不取整
    tmp_sysparas.devicetype = 1;         //设备类型 默认LNG
    tmp_sysparas.lcdtype = 0;            //LNG默认蓝屏
    tmp_sysparas.offlinemode = 0;		 //默认在线
    tmp_sysparas.modbusaddr_g = 1;		 //气相流量计地址默认设置为1
    tmp_sysparas.modbusaddr_l = 1;		 //液相流量计地址默认设置为1
    tmp_sysparas.modbusRS485 = 2;		 //气液相地址默认分开
    tmp_sysparas.precoolfinishedtime = 30;//默认预冷完成后30秒内有效

}


