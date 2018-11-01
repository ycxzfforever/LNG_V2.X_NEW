#include "kernel.h"

/************************************************************************
**	函数名称:	PumpSwitch(uint8_t state)
**	函数功能:	根据加气机的状态打开和关闭相应的阀门和PLC联动信号
**	输入参数: state:加气机的状态
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-4-1
*************************************************************************/
/*PCL1、PCL2两个继电器用于给PLC传输开关量，再通过PLC控制泵  BY  LY*/
/*还有三个继电器用于控制高、中、低三个电磁阀  BY  LY*/
void PumpSwitch(uint8_t state)
{
    switch(state)
    {
        case StaClose://停机
            PUMPCTRL_HIGH_CLOSE();  //泵高关
            PUMPCTRL_LOW_CLOSE();   //泵低关
            VCTRL_CLOSEKEY();       //主阀关
            VCTRL_MID_CLOSE();      //中关
            VCTRL_HIGH_CLOSE();     //高关
            break;

        case StaOtherFuel://对方加液
            PUMPCTRL_LOW_OPEN();    //泵低开
            PUMPCTRL_HIGH_CLOSE();  //泵高关
            VCTRL_OPENKEY();        //主阀开
            VCTRL_MID_CLOSE();      //中关
            VCTRL_HIGH_CLOSE();     //高关
            break;

        case StaSPrecool://小循环预冷
            AB_OUT_LOW();//一泵双机输出 ADD BY LY
            PUMPCTRL_HIGH_OPEN();
            PUMPCTRL_LOW_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();
            VCTRL_HIGH_CLOSE();
            break;

        case StaBPrecool://大循环预冷
            AB_OUT_LOW();//一泵双机输出 ADD BY LY
            PUMPCTRL_HIGH_OPEN();
            PUMPCTRL_LOW_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();
            VCTRL_HIGH_OPEN();
            break;

        case StaFuel://加液
            AB_OUT_LOW();//一泵双机输出 ADD BY LY
            PUMPCTRL_LOW_OPEN();    //PLC
            PUMPCTRL_HIGH_OPEN();   //PLC
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();
            VCTRL_HIGH_OPEN();
            break;

        case StaIdle://待机
        default:
            AB_OUT_HIGH();//一泵双机输出 ADD BY LY
            PUMPCTRL_LOW_OPEN();
            PUMPCTRL_HIGH_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();//待机时，打开中压阀，保持小循环状态，是为了防止憋压，因为管路中有LNG液，温度升高后，会气化，产生压力 ADD BY LY
            VCTRL_HIGH_CLOSE();
            break;
    }
}

/************************************************************************
**	函数名称:	PumpSwitch(uint8_t state)
**	函数功能:	加气过程中取整
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-12-2
*************************************************************************/
void GetIntNum(void)
{
    if(globalvar.KeyValue == KEY_PT)   //按小数点取整按定金额处理
    {
        globalvar.fix = 2;        //定金额。
        globalvar.fixmoney = ((fuelrecordinfo.money + 100) / 100) * 100;
    }
}

/************************************************************************
**	函数名称:	JudgeStopConditions(void)
**	函数功能:	停机条件判断
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-5-10
*************************************************************************/
void JudgeStopConditions(void)
{
    double money_tmp;
    double price_tmp;
    double volume_tmp;
	
    if(globalvar.timeout > (uint32_t)(sysparas.delaystoptime))       //停机延时过后才判断停机条件
    {
        if(realtime.cur_flowrate_l < (float)sysparas.lowratestop)     //低流速停机
        {
            globalvar.stoptimes++;
            stopreason = 8;
        }

        if(realtime.cur_flowrate_l > (float)sysparas.highratestop)     //高流速停机
        {
            globalvar.stoptimes++;
            stopreason = 9;
        }

        if(realtime.cur_pressure > ((float)sysparas.pressurestop))       //超压停机??
        {
            globalvar.stoptimes++;
            stopreason = 12;
        }

        if(realtime.cur_density_g > ((float)sysparas.backdenstystop / 10000.0))       //回气密度停机
        {
            globalvar.stoptimes++;
            stopreason = 10;
        }

        if(realtime.cur_gain_g > ((float)sysparas.backgainstop))       //回气增益停机
        {
            globalvar.stoptimes++;
            stopreason = 11;
        }
    }

    if(globalvar.stoptimes > 100)
    {
        globalvar.KeyValue = KEY_STOP;
    }
    else
    {
        stopreason = 0;
    }

    if(m1card.existflag == 1)
    {
        if(m1card.cardtype_another == 3)   //记账卡。
        {
            if((fuelrecordinfo.money + m1card.cardbalance_another) >= sysparas.accountcardmaxyue)     //余额上限。
            {
                money_tmp = (double)(fuelrecordinfo.money);
                price_tmp = (double)(realtime.price);
                fuelrecordinfo.volume = (uint32_t)(money_tmp / price_tmp * 100 + 0.5);
                stopreason = 14;
                globalvar.KeyValue = KEY_STOP;
            }
        }
        else if((m1card.cardtype_another != 3) && (m1card.cardtype_another != 7))       //非记账卡
        {
            if((fuelrecordinfo.money + sysparas.cardminyue) >= m1card.cardbalance_another)
            {
                money_tmp = (double)(fuelrecordinfo.money);
                price_tmp = (double)(realtime.price);
                fuelrecordinfo.volume = (uint32_t)(money_tmp / price_tmp * 100 + 0.5);
                stopreason = 15;
                globalvar.KeyValue = KEY_STOP;
            }
        }
    }

    if(sysparas.intnumflag == 1)   //判断是否需要取整
    {
        GetIntNum();
    }

    if(globalvar.fix == 1)   //定气量。
    {
        if(fuelrecordinfo.volume + 10 >= (globalvar.fixnum))     //提前0.1kg关阀
        {
            fuelrecordinfo.volume = globalvar.fixnum;
            volume_tmp = (double)fuelrecordinfo.volume / 100.0;
            fuelrecordinfo.money = (uint32_t)(volume_tmp * realtime.price + 0.5);
            stopreason = 6;
            globalvar.KeyValue = KEY_STOP;
        }
    }

    if(globalvar.fix == 2)   //定金额。
    {
        if(fuelrecordinfo.money + 5 >= (globalvar.fixmoney))     //提前0.5元关阀
        {
            fuelrecordinfo.money = (globalvar.fixmoney);
            money_tmp = (double)(globalvar.fixmoney);
            price_tmp = (double)(realtime.price);
            fuelrecordinfo.volume = (uint32_t)(money_tmp * 100 / price_tmp + 0.5);
            stopreason = 7;
            globalvar.KeyValue = KEY_STOP;
        }
    }

    if((m1card.existflag == 0) && (m1card.cardtype_another != 0))       //有卡加气时，中途拔卡
    {
        globalvar.KeyValue = KEY_STOP;
        stopreason = 13;
    }
}

/************************************************************************
**	函数名称:	StartPrecooling(void)
**	函数功能:	加气前的预冷
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-5-10
*************************************************************************/
void StartPrecooling(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
		//加气前清零实时数据一次ycx
        memset(&realtime.lcng, 0, sizeof(RealTimeData));     
    }

    ReadModbusAndPressure();//读取流量计和压力传感器的值 ADD BY LY

	//模拟加液数据处理 
    if(sysparas.simulation)
    {
        realtime.cur_flowrate_l += 10;    //流速
        realtime.cur_temperature_l -= 10; //预冷温度
        realtime.cur_gain_l += 1;        //预冷增益

        if(realtime.cur_gain_l >= sysparas.precoolgain)
        {
            realtime.cur_gain_l--;
        }

        realtime.cur_density_l += 1;     //预冷密度

    }
	
	//只预冷、且为大循环预冷
    if((DispNextUI == DispFirst) && (sysparas.precoolmode == 1))       
    {
    	//打开阀门，进入大循环
        PumpSwitch(StaBPrecool);
        globalvar.workstate = StaBPrecool;
    }
    else
    {
    	//打开阀门，进入小循环（加液前预冷，为小循环预冷）ADD BY LY
        PumpSwitch(StaSPrecool);
        globalvar.workstate = StaSPrecool;
    }

	//清当前液相总量
    if(realtime.cur_totalize_l > 20000)
    {
        ModbusClearTotal_L(_MODBUS_L_);
        memset(&realtime.lcng, 0, sizeof(RealTimeData));     //清总累后必须重新读取新数据
    }
	
	//清当前气相总量
    if(realtime.cur_totalize_g > 20000)
    {
        ModbusClearTotal_G(_MODBUS_G_);
        memset(&realtime.lcng, 0, sizeof(RealTimeData));     //清总累后必须重新读取新数据
    }

    FYD12864DispPrintfFlash(1, 1, "正在预冷%d", globalvar.timeout);
    FYD12864DispPrintfFlash(2, 1, "流速:%.02f", realtime.cur_flowrate_l);
    FYD12864DispPrintfFlash(3, 1, "温度:%.02f", realtime.cur_temperature_l);
    FYD12864DispPrintfFlash(4, 1, "增益:%.02f", realtime.cur_gain_l);

    OsplcShow(0, 0, 0, 0, 0);

    //预冷超时判断 每隔1s钟累加1次 ADD BY LY
    if(globalvar.timeout > sysparas.precooltimeout)   //预冷超时
    {
        if(globalvar.otherstate == 1)   //对方未加液
        {
            PumpSwitch(StaIdle);
        }
        else
        {
            PumpSwitch(StaOtherFuel);
        }

        DispUI = DispFirst;
    }
    else//达到预冷要求。
    {
        if(((realtime.cur_temperature_l < ((float)(0 - (int)sysparas.precooltemp)))             //预冷温度
                && (realtime.cur_density_l > ((float)(sysparas.precooldensty) / 10000.0))         //预冷密度
                && (realtime.cur_gain_l < ((float)(sysparas.precoolgain))))          //预冷增益
                || (globalvar.precoolfinished == 1))    //之前已经完成预冷，用于处理直接按加液键
        {
            globalvar.precoolfinished = 1; //预冷完成进来
			//有卡并且卡验证通过、加液
            if((m1card.existflag == 1) && (m1card.checkresult == 0) && (DispNextUI != DispFirst))
            {
            	//非VIP卡需要置灰
                if(m1card.cardtype_another != 7)   
                {
                	//卡状态设置为：插入（该标志暂时没用）
                    fuelrecordinfo.cardstate = 1;
					//置灰
                    DispUI = GreyLock;
                }
				//VIP 卡
                else if(m1card.cardtype_another == 7)
                {
                    fuelrecordinfo.cardstate = 1;
					//直接加液
                    DispUI = StartFueling;
                }
            }
			//无卡（预冷、加液）
            else if(DispNextUI != NULL)
            {
                DispUI = DispNextUI;
				//一泵双机互锁判断
                if(globalvar.otherstate == 1)   
                {
                	//对方待机，进入小循环
                    PumpSwitch(StaSPrecool);
                }
                else
                {
                	//对方加液，关闭阀门
                    PumpSwitch(StaOtherFuel);
                }
            }
            else
            {
                DispUI = DispFirst;
            }
        }
    }
	
	//预冷后也停泵延时。
    globalvar.stoppumptime = sysparas.stoppumptime + 2; 
    
	//流量计异常和压力传感器异常(globalvar.stoptimes = 1000)
    if(globalvar.stoptimes > 100)   
    {
        globalvar.KeyValue = KEY_STOP;
    }
	//按停机键，回到主界面
    TaskAdd(KEY_STOP, DispFirst, NULL);
}

/************************************************************************
**	函数名称:	StartFueling(void)
**	函数功能:	开始加气,预冷完成后开始加液前
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-5-10
*************************************************************************/
void StartFueling(void)
{
    uint32_t  availmoney;     //IC卡可用加气金额
    
	//清流水结构体
    memset(&fuelrecordinfo, 0, sizeof(FuelRecordInfo));   
	
	//开泵，进入加气状态
    PumpSwitch(StaFuel);	
	
	//加气步骤1
    globalvar.fuelstep = 1;         
	//加气状态
    globalvar.workstate = StaFuel;  
	
    globalvar.timeout = 0;
    globalvar.stoptimes = 0;
    stopreason = 0;
    globalvar.displayonce = 1;

    //加气开始时间
    memcpy(&fuelrecordinfo.starttime[0], &time.year, 6); 

    //有卡加气，加气流水中与卡相关的参数赋值 ADD BY LY
    if(m1card.existflag == 1)
    {
        //卡号
        fuelrecordinfo.usernum = m1card.cardnum_another;

        //卡类型
        fuelrecordinfo.cardtype = m1card.cardtype_another;

        //卡单价
        fuelrecordinfo.price = sysparas.price[m1card.pricelevel_another];//按照卡的类型，读出对应单价 ADD BY LY

		//车牌号
		fuelrecordinfo.carnum_another = m1card.carnum_another;
		
        //除VIP卡以外的所有卡都做卡定额处理 ADD BY LY
        if(m1card.cardtype_another != 7)
        {
            fuelrecordinfo.greyflag = 0x01;  //加气流水中的灰标记
            //卡加气前余额
            fuelrecordinfo.beforebalance = m1card.cardbalance_another;
            //卡加气后余额
            fuelrecordinfo.afterbalance = m1card.cardbalance_another;
            //卡期初余额
            fuelrecordinfo.InitialMoney = m1card.cardinitmoney;
            //卡计算余额
            fuelrecordinfo.CalMoney = m1card.cardcalmoney;
            //卡累计充值金额
            fuelrecordinfo.allrechargemoney = m1card.sumchargemoney;
            //卡累计充值次数
            fuelrecordinfo.allrechargetimes = m1card.sumchargetimes;
            //卡累积消费气量
            fuelrecordinfo.allconsumptionnum = m1card.sumvolme;
            //卡累积消费金额
            fuelrecordinfo.allconsumptionmoney = m1card.summoney;
            //卡累计消费次数
            fuelrecordinfo.allconsumptiontimes = m1card.sumtimes;


            //定额处理
            if(m1card.cardtype_another != 3)   //非记账卡 ADD BY LY
            {
                //程序中所有的钱，都是按分来存取计算的，只有在显示的时候，变成元来显示给客户看的 ADD BY LY
                availmoney = m1card.cardbalance_another - sysparas.cardminyue;//计算IC卡可用加气金额 ADD BY LY
            }
            else if(m1card.cardtype_another == 3)   //记账卡
            {
                availmoney = sysparas.accountcardmaxyue - m1card.cardbalance_another;
            }
			
			//不是定气量，也不是定金额，就是正常加气，直接将加气设置为定金额加气，金额为可用金额 ADD BY LY
            if(globalvar.fix == 0)            
            {
                //如果不是定量加气，则按定金额加气，如果没有到可用余额上限按下停止，则停止；如果达到可用余额上限，还未按停止键，则加气机达到余额上限自动停机 ADD BY LY
                globalvar.fix = 2;
                globalvar.fixmoney = availmoney;
            }		
        }
    }
	//无卡加气
    else
    {
        //默认交易价格
        fuelrecordinfo.price = sysparas.price[0];
    }

    //实时单价
    realtime.price = fuelrecordinfo.price;		//当前交易价格 ADD BY LY
    //班号
    fuelrecordinfo.classnum = sysparas.shiftnum;
    //班流水号
    fuelrecordinfo.classid = sysparas.shiftransnum;
    //站编号
    fuelrecordinfo.stationcode = sysparas.stationcode;
    //加气前压力
    fuelrecordinfo.beforepressure = realtime.cur_pressure;
    //液相流量计总累
    globalvar.starttotalizeL = realtime.cur_totalize_l;
    //加气前枪累
    fuelrecordinfo.beforegunsum = sysparas.totvolume;
    //加气后枪累
    fuelrecordinfo.aftergunsum = sysparas.totvolume;
    //液相流量计上一次总累
    globalvar.pretotalL = realtime.cur_totalize_l;

    //回气计量
    if(sysparas.backgascalc == 1)
    {
        globalvar.starttotalizeG = realtime.cur_totalize_g;
        globalvar.pretotalG = realtime.cur_totalize_g;
    }

    //设备类型
    fuelrecordinfo.devicetype = sysparas.devicetype;

    DispUI = DispStartFueling;	//开始加气 ADD BY LY
    realtime.powerdown = 0;     //未掉电标志                                            
}



//加液显示
double	 lngvolume = 0; //进液量
double   cngvolume = 0; //回气量
void DispStartFueling(void)
{
	//加气步骤2
    globalvar.fuelstep = 2; 
	//加气状态
    globalvar.workstate = StaFuel;    

	//模拟加气
    if(sysparas.simulation)
    {
        realtime.cur_totalize_l += 0.11;
        realtime.cur_flowrate_l++;
        realtime.cur_pressure += 1;

        if(realtime.cur_pressure > 150)
        {
            realtime.cur_pressure = 150;
        }

        if(realtime.cur_flowrate_l > 400)
        {
            realtime.cur_flowrate_l = 400;
        }
    }
	//正常加气
	//加气中，不断读取流量计与压力传感器的值 ADD BY LY
    ReadModbusAndPressure();
    
	//液相数据异常 fabs()求浮点数的绝对值 BY LY
    if(fabs(realtime.cur_totalize_l - globalvar.pretotalL) > 2)     
    {
        globalvar.KeyValue = KEY_STOP;
        stopreason = 3;
    }
	//液相数据正常
    else
    {
        //进液量计算
        lngvolume = realtime.cur_totalize_l - globalvar.starttotalizeL;
        globalvar.pretotalL = realtime.cur_totalize_l; //将此次实时总累赋值，用于下次判断瞬时数据
    }
	
	//回气计量
    if(sysparas.backgascalc == 1)   
    {
        if(globalvar.starttotalizeG < 0.1)   //防止第一次没有读到气象流量计的起始量
        {
            globalvar.starttotalizeG = realtime.cur_totalize_g;//回气量初值初始化 ADD BY LY
        }
        else
        {
        	//气相数据异常
            if(fabs(realtime.cur_totalize_g - globalvar.pretotalG) > 1)     
            {
                globalvar.KeyValue = KEY_STOP;
                stopreason = 3;
            }
			//气相数据正常
            else
            {
                //回气量计算
                cngvolume = realtime.cur_totalize_g - globalvar.starttotalizeG;
                globalvar.pretotalG = realtime.cur_totalize_g; //将此次实时总累赋值，用于下次判断瞬时数据

            }
        }
    }
	//回气不计量，直接设置为 0 ADD BY LY
    else
    {
        cngvolume = 0;
    }
	
	//按标方计量
    if(sysparas.unit == 2)   
    {
        lngvolume = lngvolume / ((double)sysparas.densty / 10000.0);
        cngvolume = cngvolume / ((double)sysparas.densty / 10000.0);
    }

    //当前交易气量计算
    lngvolume = (lngvolume - cngvolume) * ((double)sysparas.coeff / 10000.0);
    //当前回气量
    fuelrecordinfo.backgas = (uint32_t)(cngvolume * 100.0);
    //当前交易气量
    fuelrecordinfo.volume = (uint32_t)(lngvolume * 100.0);
    
    lngvolume = (double)fuelrecordinfo.volume / 100.0;
    //当前交易金额计算
    fuelrecordinfo.money = (uint32_t)(lngvolume * realtime.price + 0.5);
	
	//判断停机条件
    JudgeStopConditions();
	
    realtime.volume = fuelrecordinfo.volume;
    realtime.money = fuelrecordinfo.money;

    //每1s钟，向后台发送一次加气实时流水 ADD BY LY
    if(timer1.tick_100ms % 10 == 0)
    {
    	//加气机上传实时流水
        UpLoad_RunState(2);
    }

    FYD12864DispPrintfFlash(1, 1, "状态: 正在加液");
    FYD12864DispPrintfFlash(2, 1, "气量:%ld.%02d", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);
    FYD12864DispPrintfFlash(3, 1, "金额:%ld.%02d", fuelrecordinfo.money / 100, fuelrecordinfo.money % 100);
    FYD12864DispPrintfFlash(4, 1, "V:%.01f T:%.01f ", realtime.cur_flowrate_l, realtime.cur_temperature_l);

	//按停止键，停止加气
    TaskAdd(KEY_STOP, StopFueling, NULL);

	//未掉电标志
    realtime.powerdown = 0;    
	
    //动态显示瞬时加气数据
    OsplcShow(fuelrecordinfo.volume, fuelrecordinfo.money, realtime.price, 0, 0);

    //判断是否掉电
    if(PowerCheckIn == 1)
    {
    	//过20ms，再次检测是否已掉电
        _delay_ms(20);
        if(PowerCheckIn == 1)
        {
        	//已掉电（该标志暂时未使用）
            realtime.powerdown = 1;   
			
			//保存掉电实时数据realtime
            SaveRealtime();
			
			//关泵、阀，掉电提示
            PowerDown();
        }
    }
}

/************************************************************************
**	函数名称:	StartFueling(void)
**	函数功能:	停止加气
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-5-10
*************************************************************************/
void StopFueling(void)
{
	//加气步骤3
    globalvar.fuelstep = 3; 		
	//付款状态
    globalvar.workstate = StaPay;
	
    FYD12864DispPrintfFlash(1, 1, "状态：加液结束");

    if(globalvar.displayonce == 1)
    {
		//关阀前，互锁判断
		//对方待机
        if(globalvar.otherstate == 1)   
        {	
        	//进入小循环
            PumpSwitch(StaSPrecool);
        }
		//对方加液
        else
        {
        	//关闭中压、高压阀门
            PumpSwitch(StaOtherFuel);
        }

        globalvar.displayonce = 0;
		
		//加液结束后气量变量清零
        lngvolume = 0;                                  
		//加液后停泵延时
        globalvar.stoppumptime = sysparas.stoppumptime + 2; 
		
        //交易流水号
        sysparas.transnum++;
        fuelrecordinfo.transnum = sysparas.transnum;
        //枪总气量
        sysparas.totvolume += fuelrecordinfo.volume;
        //枪总金额
        sysparas.totmoney += fuelrecordinfo.money;
        //班累计气量(通过流水累加计算)
        sysparas.shiftotvol += fuelrecordinfo.volume;
        //班累计金额 (通过流水累加计算)
        sysparas.shiftotmon += fuelrecordinfo.money;
        //班加气次数
        sysparas.shiftfueltimes++;
		
        //保存系统参数
        StopFuelingSaveParas();

		//非VIP卡加气
        if((fuelrecordinfo.cardtype != 0) && (fuelrecordinfo.cardtype != 7))
        {
            //记账卡加气后余额	计算余额
            if(fuelrecordinfo.cardtype == 3)
            {
                fuelrecordinfo.afterbalance = fuelrecordinfo.afterbalance + fuelrecordinfo.money;
                fuelrecordinfo.CalMoney = fuelrecordinfo.CalMoney + fuelrecordinfo.money;
            }
			//除VIP卡、记账卡以外的卡 计算余额
            else
            {
                fuelrecordinfo.afterbalance = fuelrecordinfo.afterbalance - fuelrecordinfo.money;
                fuelrecordinfo.CalMoney = fuelrecordinfo.CalMoney - fuelrecordinfo.money;
            }

            //卡累计消费次数
            fuelrecordinfo.allconsumptiontimes = m1card.sumtimes + 1;
            //卡累计消费金额
            fuelrecordinfo.allconsumptionmoney += fuelrecordinfo.money;
            //卡累积消费气量
            fuelrecordinfo.allconsumptionnum += fuelrecordinfo.volume;
        }

        //加气结束时间
        memcpy(&fuelrecordinfo.endtime[0], &time.year, 6);
        //加气后枪累
        fuelrecordinfo.aftergunsum = fuelrecordinfo.beforegunsum + fuelrecordinfo.volume;
        //枪气量累计
        fuelrecordinfo.allgunnum = sysparas.totvolume;
        //枪金额累计
        fuelrecordinfo.allgunmoney = sysparas.totmoney;
        //加气后压力
        fuelrecordinfo.afterpressure = realtime.cur_pressure;
        //当量
        fuelrecordinfo.curnum = sysparas.coeff;
        //计量单位
        fuelrecordinfo.measuretype = sysparas.unit;//0-标方，1-Kg。
        //密度
        fuelrecordinfo.density = (uint32_t)realtime.cur_density_l;
        //班流水号
        fuelrecordinfo.classid = sysparas.shiftransnum;
        //停机原因
        realtime.stopreason = stopreason;
        fuelrecordinfo.stopreason = realtime.stopreason;
        //加气方式
        fuelrecordinfo.fuelmode = sysparas.fuelmode;
        //客显屏最后加气结果显示
        OsplcShow(fuelrecordinfo.volume, fuelrecordinfo.money, realtime.price, 0, 1);
        //最多申请10次扣款
        m1card.debittimes = 10;
    }

	 //用卡加气结束且未掉电
    if((m1card.existflag == 1) && (PowerCheckIn == 0))      
    {
    	//有非VIP卡且卡通过验证
        if((m1card.checkresult == 0x00) && (m1card.cardtype_another != 7))       
        {
            FYD12864DispPrintfFlash(1, 1, "状态：正在扣款..");
			//扣款
            if(IcCardPayMoney())
            {
                FYD12864DispPrintfFlash(1, 1, "状态：扣款成功");
				//解灰
                GreyUnLock();
            }
            else
            {
                FYD12864DispPrintfFlash(1, 1, "状态：扣款失败");
            }
        }

        globalvar.displayonce = 1;
        DispUI = FuelEndDisplay;
    }
	//无卡加气结束且未掉电
    else if(PowerCheckIn == 0)   
    {
        globalvar.displayonce = 1;
        DispUI = FuelEndDisplay;
    }

    realtime.powerdown = 0;        //未掉电标志。

    if(PowerCheckIn == 1)          //判断是否掉电。
    {
        _delay_ms(20);

        if(PowerCheckIn == 1)
        {
            realtime.powerdown = 1; //已掉电。
            SaveRealtime();         //保存掉电实时数据 realtime
            PowerDown();
        }

    }
}

/************************************************************************
**	函数名称:	FuelEndDisplay(void)
**	函数功能:	加液完毕后存储流水和打印
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-5-10
*************************************************************************/
void FuelEndDisplay(void)
{	
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;

        //crc计算 放到流水保存以前 以免数据再被修改 ADD BY LY
        fuelrecordinfo.crc = ModbusCrc16((uint8_t*)(&fuelrecordinfo.transnum), (sizeof(FuelRecordInfo) - 2));           //changed by LY
		//保存加气流水
        if(SaveFuelRecord(0))
        {
            FYD12864DispPrintfFlash(1, 1, "状态: 保存成功");
        }
        else
        {
            FYD12864DispPrintfFlash(1, 1, "状态: 保存失败");
        }

		//加气结束，加气机主动发送加气结束状态给后台 ADD BY LY
        UpLoad_RunState(0);   

		//判断是否自动打印一次
        if(sysparas.printmode == 1)  
        {
        	Printid();
        }

		//空闲状态
        globalvar.workstate = StaIdle;

		//防止多次调用函数TaskMessage而不起作用
        globalvar.MsgFlag = 0;
        globalvar.MsgTime = 0; 
    }

    FYD12864DispPrintfFlash(4, 1, "停机: %s", stopreasonbuf[stopreason]);

	
    TaskAdd(KEY_RET, DispFirst, NULL);//按返回键，回到主界面
    TaskAdd(KEY_OK, StopPrint, NULL); //按确认键，打印当前流水
    TaskMessage(50, DispFirst); 	  //没有任何操作，3s后返回主界面  
}

/************************************************************************
**	函数名称:	DispCardError(void)
**	函数功能:	IC卡A密或B密验证错误提示
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCX
**	创建日期:	2016-5-10
*************************************************************************/
void DispCardError(void)
{
    FYD12864DispPrintfFlash(2, 1, "读卡错误");

    if(mt318reader.recbuf[6] == 0x33)
    {
        if(mt318reader.recbuf[4] == KEYA)
            FYD12864DispPrintfFlash(3, 1, "A 密验证失败");
        else if(mt318reader.recbuf[4] == KEYB)
            FYD12864DispPrintfFlash(3, 1, "B 密验证失败");
    }

    if(m1card.existflag == 0)   //拔卡直接返回主界面
    {
        DispUI = DispFirst;
    }

    else
        TaskMessage(100, DispFirst);   // 5s后返回主界面
}

/************************************************************************
**	函数名称:	GetCardData(void)
**	函数功能:	比对IC卡密钥，读取卡内所有信息
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void GetCardData(void)
{
    uint8_t tmp = 0;                            //读块出错次数。
    uint8_t keyb[6];
    uint8_t ntry = 2;

    FYD12864ClearLine(1);
    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "    读卡中");
    FYD12864DispPrintfFlash(3, 1, "    请稍后...");

	//置卡存在标志
    m1card.existflag = 1;
	
	//初始化卡验证结果标志 0xFF表示等待验证
    m1card.checkresult = 0xFF;  
	
	//按任何按键，皆无效
    globalvar.KeyValue = 0;                      

    //从EEPROM中去读出卡B密，存入keyb[]中	ADD BY LY
    EepromInterReadBytes(LOCKVER_ADDR, keyb, 6);
    memcpy(&m1card.keyB[0], keyb, 6);  //将B密重新赋值

    /**** 将块 1、2、4、5、6、9、10、13、14、16、17、18的数据读出存入m1card结构体中 ADD BY LY *****/

    //块1经常读错，修改程序，在读错的情况下，再读一次，ADD BY LY
    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 1, &m1card.backUp0[0]))     //读块1。
        {
            tmp++;
        }
        else
        {
            tmp = 0;
            break;
        }
    }

	//tmp为非 0，表示读块失败 ADD BY LY
    if(tmp > 0)
    {
        if(m1card.checkresult == 0xFF)
            m1card.checkresult = 0x01;

        DispUI = CardDataError;
        return;
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, &m1card.card[0]))     //读块2。
        {
            tmp++;
            m1card.checkresult = 0x02;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 4, &m1card.balancecrc[0]))     //读块4。
        {
            tmp++;
            m1card.checkresult = 0x03;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 5, &m1card.lastBalanceCrc[0]))     //读块5。
        {
            tmp++;
            m1card.checkresult = 0x04;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 6, (uint8_t *)&m1card.payMoneyCrc[0]))       //读块6。
        {
            tmp++;
            m1card.checkresult = 0x05;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 9, (uint8_t *)&m1card.block9[0]))       //读块9。
        {
            tmp++;
            m1card.checkresult = 0x06;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 10, (uint8_t *)&m1card.block10[0]))       //读块10。
        {
            tmp++;
            m1card.checkresult = 0x07;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 13, &m1card.cylindersdate[0]))     //读块13。
        {
            tmp++;
            m1card.checkresult = 0x08;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 14, &m1card.backUp5[0]))     //读块14。
        {
            tmp++;
            m1card.checkresult = 0x09;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 16, &m1card.block16[0]))     //读块16。
        {
            tmp++;
            m1card.checkresult = 0x0A;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 17, &m1card.block17[0]))     //读块17。
        {
            tmp++;
            m1card.checkresult = 0x0B;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 18, &m1card.block18[0]))     //读块18。
        {
            tmp++;
            m1card.checkresult = 0x0C;
            DispUI = CardDataError;
            return;
        }
    }

    /**************************************************************************************************/
    if(tmp == 0)                                               //读卡成功。
    {
        DispUI = IcCardPasswd;
    }
}

/************************************************************************
**	函数名称:	IcCardPasswd(void)
**	函数功能:	读卡成功后，判断卡密码是否为0，不为0则需要输入密码
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void IcCardPasswd(void)
{
    uint8_t tmpbuffer[6], count = 0;
    uint32_t tmp1, tmp2;

    //车牌号，即M1卡密码：0000
    for(count = 0; count < 2; count++)
    {
    	//4个字节的ASCII 转换成 2个字节的BCD
        tmpbuffer[count] = AscToBcd(&m1card.carnum[count * 2]);   
    }
	//2个字节的BCD 转换成 一个32位的HEX
    m1card.carnum_another = BcdbufToHex(&tmpbuffer[0], 2);   

    m1card.key = m1card.carnum_another;//车牌号即卡密码
    
	//不需要检验钢瓶日期
    if(sysparas.cardcheck == 0)                  
    {
    	//卡密码为0，不输入密码
        if(m1card.key == 0)                     
        {
        	//卡信息验证
            DispUI = CardMessageCheck;
        }
		//卡密码不为0，则输入密码
        else                                     
        {
        	InputInit();
            DispUI = InputIccardPasswd;
        }
    }
	 //需要检验钢瓶日期
    else if(sysparas.cardcheck == 1)            
    {
    	//ASCII 转 十进制 ADD BY LY
        tmpbuffer[0] = (m1card.cylindersdate[0] - 0x30) * 10 + (m1card.cylindersdate[1] - 0x30);     
        tmpbuffer[1] = (m1card.cylindersdate[2] - 0x30) * 10 + (m1card.cylindersdate[3] - 0x30);
        tmpbuffer[2] = (m1card.cylindersdate[4] - 0x30) * 10 + (m1card.cylindersdate[5] - 0x30);

		//验证过期的方法：转换成天数比较
        tmp1 = tmpbuffer[0] * 365 + tmpbuffer[1] * 30 + tmpbuffer[2];
        tmp2 = time.year * 365 + time.month * 30 + time.day;

		//钢瓶未到期日期 ADD BY LY
        if(tmp1 > tmp2)		
        {
            if(m1card.key == 0)                 //卡密码为0，直接跳过。
            {
                DispUI = CardMessageCheck;
            }
            else                                 //卡密码不为0，则输入密码。
            {
                DispUI = InputIccardPasswd;
            }
        }
        else
        {
            DispUI = DispDateOver;//将卡置灰，提示钢瓶过期 ADD BY LY
        }
    }
}

/************************************************************************
**	函数名称:	DispDateOver(void)
**	函数功能:	钢瓶过期提示
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void DispDateOver(void)
{
    uint8_t tmpbuffer[3];

    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "钢瓶已经过期");

	//ASCII 转 十进制 ADD BY LY
    tmpbuffer[0] = (m1card.cylindersdate[0] - 0x30) * 10 + (m1card.cylindersdate[1] - 0x30);     
    tmpbuffer[1] = (m1card.cylindersdate[2] - 0x30) * 10 + (m1card.cylindersdate[3] - 0x30);
    tmpbuffer[2] = (m1card.cylindersdate[4] - 0x30) * 10 + (m1card.cylindersdate[5] - 0x30);

	//显示钢瓶到期日期 只取年月日，年取后两位数
    FYD12864DispPrintfFlash(3, 1, "%02d-%02d-%02d", tmpbuffer[0], tmpbuffer[1], tmpbuffer[2]);   

	//按返回键返回主界面
    TaskAdd(KEY_RET, DispFirst, NULL);

	//读卡序列号，实际是检测卡是否还存在 ADD BY LY
    if(Mt318GetCardSerialNum() != true)
    {
        DispUI = DispFirst;
    }
}

/************************************************************************
**	函数名称:	InputIccardPasswd(void)
**	函数功能:	输入IC卡密码页面提示
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCK
**	创建日期:	2016-7-25
*************************************************************************/
void InputIccardPasswd(void)
{
    DispInputPwd("请输入卡密码", ComparePasswd);

    if(m1card.existflag == 0)
    {
        DispUI = DispFirst;
    }
}

/************************************************************************
**	函数名称:	ComparePasswd(void)
**	函数功能:	IC卡密码判断
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	YCK
**	创建日期:	2016-7-25
*************************************************************************/
void ComparePasswd(void)
{
    if(globalvar.U32Temp != m1card.key)
    {
        FYD12864DispPrintfFlash(4, 1, "密码错误");
        TaskAdd(KEY_CLR, InputIccardPasswd, NULL);
        TaskAdd(KEY_RET, InputIccardPasswd, NULL);
    }
    else
    {
        DispUI = CardMessageCheck;
    }
}

/************************************************************************
**	函数名称:	DispCardInfo(void)
**	函数功能:	读卡成功后，显示卡信息，判断卡用途
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void DispCardInfo(void)
{
    globalvar.timeout = 0;
	//显示卡号
    FYD12864DispPrintfFlash(1, 1, "卡号: %ld", m1card.cardnum_another);

	//显示卡类型
    switch(m1card.cardtype_another)
    {
        case 0x01:
            FYD12864DispPrintfFlash(2, 1, "类型: 用户卡");
            break;

        case 0x02:
            FYD12864DispPrintfFlash(2, 1, "类型: 员工卡");
            break;

        case 0x03:
            FYD12864DispPrintfFlash(2, 1, "类型: 记账卡");
            break;

        case 0x04:
            FYD12864DispPrintfFlash(2, 1, "类型: 维护卡");
            break;

        case 0x05:
            FYD12864DispPrintfFlash(2, 1, "类型: 内部卡");
            break;

        case 0x06:
            FYD12864DispPrintfFlash(2, 1, "类型: 积分卡");
            break;

        case 0x07:
            FYD12864DispPrintfFlash(2, 1, "类型: VIP 卡");
            break;
    }
	//显示卡余额
    FYD12864DispPrintfFlash(3, 1, "余额: %ld.%02d", m1card.cardbalance_another / 100, m1card.cardbalance_another % 100);
	//显示卡对应单价
	FYD12864DispPrintfFlash(4, 1, "单价: %.02f", (double)m1card.currentprice / 100);

	//手动加气
    if(sysparas.fuelmode == 0)	    
    {
        globalvar.displayonce = 1;

		//此处调用TaskAdd，虽然NextUi = NULL，但是在StartPrecooling中，检测到有卡加气，就进入GreyLock
        TaskAdd(KEY_START, StartPrecooling, NULL);    //按加气键，开始预冷，然后开始加气
        TaskAdd(KEY_BACK, StartPrecooling, DispFirst);//按回显键，进入预冷，然后返回主界面
    }
	
    InputInit();
    TaskAdd(KEY_FIX, FixGas, NULL); 		 //按定量键，定量加气
    TaskAdd(KEY_QRY, DispQuery1, NULL);		 //按查询键，进入查询功能	
    TaskAdd(KEY_SET, DispInputMenuPwd, NULL);//按菜单键，进入设置功能

    if(m1card.existflag == 0)   //卡被拔出
    {
        DispUI = DispFirst;
    }
}

/************************************************************************
**	函数名称:	CardMessageCheck(void)
**	函数功能:	Check卡中信息
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void CardMessageCheck(void)
{
    uint8_t count;
    uint8_t tmpbuffer[6];
    uint32_t tmp = 0;
    UnionU32 trans;

    //卡号
    for(count = 0; count < 4; count++)
    {
    	// 4字节 ASCII to 2字节 BCD
        tmpbuffer[count] = AscToBcd(&m1card.cardnum[count * 2]);   
    }
	//2字节 BCD to 1个32位 HEX
    m1card.cardnum_another = BcdbufToHex(tmpbuffer, 4);   

    //交易流水
    for(count = 0; count < 4; count++)
    {
        //M1卡中的流水号，低地址存的是高位；而在CPU里，低地址是存在低位(小端)的，故赋值时，需要颠倒顺序 ADD BY LY
        trans.rdcv[count] = m1card.transnum[4 - count - 1]; //HEX
    }

    m1card.transnum_another = trans.data;//HEX
    //卡类型 块2中的卡类型
    tmp = AscToBcd(&m1card.cardtype[0]);   //2字节ASCII转换成1字节BCD
    m1card.cardtype_another = (uint8_t)BcdToHex(tmp);     //HEX

    //卡单价等级
    //tmp = AscToBcd(&m1card.pricelevel[0]);//BCD /* 后台未写单价等级 ycx.modify 2018-09-11 11:15:26 */
    m1card.pricelevel_another = m1card.cardtype_another;//HEX

    //卡余额
    for(count = 0; count < 4; count++)
    {
        tmpbuffer[count] = AscToBcd(&m1card.cardbalance[count * 2]);   //BCD
    }

    m1card.cardbalance_another = BcdbufToHex(tmpbuffer, 4);   //HEX

    //最后加气金额
    for(count = 0; count < 4; count++)
    {
        tmpbuffer[count] = AscToBcd(&m1card.payMoney[count * 2]);   //BCD
    }

    m1card.trademoney = BcdbufToHex(tmpbuffer, 4);   //HEX
    //累积消费次数
    m1card.sumtimes = BcdbufToHex(&m1card.block16[0], 4);
    //累计消费金额
    m1card.summoney = BcdbufToHex(&m1card.block16[4], 6);   //HEX
    //累积消费气量
    m1card.sumvolme = BcdbufToHex(&m1card.block16[12], 4);
    //卡初余额
    memset(&tmpbuffer[0], 0, sizeof(tmpbuffer));     //M1卡中卡期初余额为BCD码 ADD BY LY
    memcpy(&tmpbuffer[1], &m1card.block17[0], 5);   //BCD 因为卡期初余额只有5位，所以从tmpbuffer[1]开始复制 ADD BY LY
    m1card.cardinitmoney = BcdbufToHex(&tmpbuffer[0], 6);   //HEX
    //计算余额
    memset(&tmpbuffer[0], 0, sizeof(tmpbuffer));
    memcpy(&tmpbuffer[1], &m1card.block17[11], 5);   //BCD 因为计算余额只有5位，所以从tmpbuffer[1]开始复制 ADD BY LY
    m1card.cardcalmoney = BcdbufToHex(&tmpbuffer[0], 6);   //HEX
    //充值金额总累
    m1card.sumchargemoney = BcdbufToHex(&m1card.block18[0], 6);   //HEX M1卡中累积充值金额为BCD码 ADD BY LY
    //累积充值次数
    m1card.sumchargetimes = BcdbufToHex(&m1card.block18[6], 4);

    //校验1
    if(m1card.cardbalance_another != m1card.cardcalmoney)
    {
        if(m1card.greyFlag[1] != 0x32)   //如果是灰卡，继续读，预防扣款失败，无法继续扣款 ADD BY LY
        {
            m1card.checkresult = 0x0F;//卡余额错误
            DispUI = CardDataError;
            return;
        }
    }

    //校验2
    if(m1card.cardtype_another == 3)   //记账卡。
    {
        if(m1card.cardbalance_another >= sysparas.accountcardmaxyue)   //判断余额是否高于上限。
        {
            m1card.checkresult = 0x10;//余额超限
            DispUI = CardDataError;
            return;
        }
    }
    else if(m1card.cardtype_another != 7)   //储蓄卡。
    {
        if(m1card.cardbalance_another <= sysparas.cardminyue)   //判断余额是否低于余额下限
        {
            m1card.checkresult = 0x11;//余额不足
            DispUI = CardDataError;
            return;
        }
    }

    //效验3 计算余额校验
    if(m1card.cardtype_another == 3)   //记账卡 ADD BY LY
    {
        if(m1card.cardcalmoney != (m1card.cardinitmoney + m1card.sumchargemoney + m1card.summoney))     //记账卡 余额是加
        {
            if(m1card.greyFlag[1] != 0x32)   //如果是灰卡，继续读，预防扣款失败，无法继续扣款 ADD BY LY
            {
                m1card.checkresult = 0x12;//计算余额错误
                DispUI = CardDataError;
                return;
            }
        }
    }
    else if(m1card.cardtype_another != 7)   //不是VIP卡
    {
        if((m1card.cardcalmoney + m1card.summoney) != (m1card.sumchargemoney + m1card.cardinitmoney))
        {
            if(m1card.greyFlag[1] != 0x32)
            {
                m1card.checkresult = 0x13;//储蓄或其他卡计算余额错误
                DispUI = CardDataError;
                return;
            }
        }
    }

    //卡单价
    m1card.currentprice = sysparas.price[m1card.pricelevel_another];

    if(m1card.currentprice == 0)
    {
        m1card.checkresult = 0x14;
        DispUI = CardDataError;
        return;
    }

    //查询挂失卡、置IC卡挂失标志 ADD BY LY
    if(ReadLossCard(m1card.cardnum_another) == false)
    {
        globalvar.displayonce = 1;

        if(globalvar.displayonce == 1)
        {
            if(m1card.lossFlag[1] != 0x32)
            {
                if(!WriteLossCardFlag())	//置挂失卡标记 ADD BY LY
                {
                    carderror = 19;
                    DispUI = IcCardOperaError;
                    return;
                }
                else
                {
//					FYD12864DispPrintfFlash(1,1,"置挂失卡标志成功");
//					FYD12864DispPrintfFlash(2,1,"重新读卡中...");
                    DispUI = GetCardData;
                    return;
                }
            }

            globalvar.displayonce = 0;
        }
    }
	
	//0x32 为"2"的ASCII码 2表示灰卡 ADD BY LY
    if(m1card.greyFlag[1] == 0x32)   
    {
        m1card.checkresult = 0x0D;
        DispUI = CardDataError;
        return;
    }
	
	//0x32 为"2"的ASCII码 2表示挂失卡（只有在卡拿去后台写了以后，该标志才会改变）ADD BY LY
    else if(m1card.lossFlag[1] == 0x32)   
    {
        m1card.checkresult = 0x0E;
        DispUI = CardDataError;
        return;
    }
    else
    {
    	//该变量暂时未使用 ADD BY LY
        m1card.checkflag = 1;
		
        //M1卡验证通过
        m1card.checkresult = 0;
        globalvar.displayonce = 0;
    }

    if(m1card.existflag == 0)   //卡被拔出
    {
        DispUI = DispFirst;
    }
    else
    {
        DispUI = DispCardInfo;
    }
}

/************************************************************************
**	函数名称:	CardDataError(void)
**	函数功能:	IC卡验证失败，信息提示
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void CardDataError(void)
{
    if(globalvar.displayonce == 1)
    {
        FYD12864ClearLine(1);
        FYD12864ClearLine(4);
        globalvar.displayonce = 0;
    }

    switch(m1card.checkresult)
    {
        case 0xFF:
            FYD12864DispPrintfFlash(2, 1, "正在等待验证");
            FYD12864DispPrintfFlash(3, 1, "请稍后...");
            break;

        case 0x00:
            DispUI = DispCardInfo;
            globalvar.displayonce = 1;
            break;

        case 0x01:
            FYD12864DispPrintfFlash(2, 1, "读块01失败");
            FYD12864ClearLine(3);
            break;

        case 0x02:
            FYD12864DispPrintfFlash(2, 1, "读块02失败");
            FYD12864ClearLine(3);
            break;

        case 0x03:
            FYD12864DispPrintfFlash(2, 1, "读块04失败");
            FYD12864ClearLine(3);
            break;

        case 0x04:
            FYD12864DispPrintfFlash(2, 1, "读块05失败");
            FYD12864ClearLine(3);
            break;

        case 0x05:
            FYD12864DispPrintfFlash(2, 1, "读块06失败");
            FYD12864ClearLine(3);
            break;

        case 0x06:
            FYD12864DispPrintfFlash(2, 1, "读块09失败");
            FYD12864ClearLine(3);
            break;

        case 0x07:
            FYD12864DispPrintfFlash(2, 1, "读块10失败");
            FYD12864ClearLine(3);
            break;

        case 0x08:
            FYD12864DispPrintfFlash(2, 1, "读块13失败");
            FYD12864ClearLine(3);
            break;

        case 0x09:
            FYD12864DispPrintfFlash(2, 1, "读块14失败");
            FYD12864ClearLine(3);
            break;

        case 0x0A:
            FYD12864DispPrintfFlash(2, 1, "读块16失败");
            FYD12864ClearLine(3);
            break;

        case 0x0B:
            FYD12864DispPrintfFlash(2, 1, "读块17失败");
            FYD12864ClearLine(3);
            break;

        case 0x0C:
            FYD12864DispPrintfFlash(2, 1, "读块18失败");
            FYD12864ClearLine(3);
            break;

        case 0x0D:
            FYD12864DispPrintfFlash(2, 1, "灰卡");
            FYD12864DispPrintfFlash(3, 1, "解灰中...");
            DispUI = GreyDataDeal;
            break;

        case 0x0E:
            FYD12864DispPrintfFlash(2, 1, "挂失卡");
            FYD12864DispPrintfFlash(3, 1, "请退卡...");
            break;

        case 0x0F:
            FYD12864DispPrintfFlash(2, 1, "卡余额错误");
            FYD12864ClearLine(3);
            break;

        case 0x10:
            FYD12864DispPrintfFlash(2, 1, "余额: %ld.%02d", m1card.cardbalance_another / 100, m1card.cardbalance_another % 100);
            FYD12864DispPrintfFlash(3, 1, "余额超限，请付账");
            break;

        case 0x11:
            FYD12864DispPrintfFlash(2, 1, "余额: %ld.%02d", m1card.cardbalance_another / 100, m1card.cardbalance_another % 100);
            FYD12864DispPrintfFlash(3, 1, "余额不足，请充值");
            break;

        case 0x12:
            FYD12864DispPrintfFlash(2, 1, "记账计算余额错误");
            FYD12864ClearLine(3);
            break;

        case 0x13:
            FYD12864DispPrintfFlash(2, 1, "储蓄计算余额错误");
            FYD12864ClearLine(3);
            break;

        case 0x14:
            FYD12864DispPrintfFlash(2, 1, "单价为0");
            FYD12864ClearLine(3);
            break;

        case 0x16:
            FYD12864DispPrintfFlash(2, 1, "B 密错误");
            FYD12864ClearLine(3);
            break;

    }

    if(m1card.existflag == 0)
        DispUI = DispFirst; //卡被拔出
}

/************************************************************************
**	函数名称:	Card_WriteGunandTran(uint8_t gunnumber, uint32_t transum, uint8_t stationid)
**	函数功能:	更新IC卡中的枪号、流水号、站编号
**	输入参数: gunnumber: 待写入IC卡中的枪号
			  transum：	 待写入IC卡中的交易流水号
			  stationid：待写入IC卡中的站编码
**	返回值  	: flag：1：写入成功；0：写入失败

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
uint8_t Card_WriteGunandTran(uint8_t gunnumber, uint32_t transum, uint8_t stationid)
{
    uint8_t 	buf[16];//存读出的数据 ADD BY LY
    uint8_t   bufbak[16];//存写入后，再次读出的数据 ADD BY LY
    uint8_t 	ntry = 2; //控制写错时，再写的次数 ADD BY LY
    uint8_t	flag = false;
    //更新块14数据
    /*********************************************************/
    memcpy(buf, &m1card.backUp5[0], 16);
    //块14，前[0-5]字节=卡初始化信息;
    //[6]字节 =枪号；
    //[7-10]字节=最后一次流水号；ADD BY LY
    //[11]字节 = 最后加气加气站编码；
    //[12-15]=备用 ADD BY LY

    //枪号 ADD BY LY
    buf[6] = gunnumber;
    //buf[7-10] 最后一次交易流水号
    buf[7] = (uint8_t)(transum >> 24);
    buf[8] = (uint8_t)(transum >> 16);
    buf[9] = (uint8_t)(transum >> 8);
    buf[10] = (uint8_t)transum;
    //最后一次加气，加气站编码 LY
    buf[11] = stationid;

    //写块14卡 ADD BY LY
    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 14, buf))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //写块14失败
    if(flag == false)
    {
        carderror = 20;//ADD BY LAST
        DispUI = IcCardOperaError;
        return flag;
    }

    //读块14
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 14, bufbak))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //读块14失败
    if(flag == false)
    {
        carderror = 21;//ADD BY LAST
        DispUI = IcCardOperaError;
        return flag;
    }

    //校验写入的数据
    if(memcmp(buf, bufbak, 16) != 0)
    {
        //写枪号、流水号、加气站编号失败
        flag = false;
        carderror = 22;
        DispUI = IcCardOperaError;
        return flag;
    }

    //写枪号、流水号、加气站编号成功
    DispUI = StartFueling;
    return flag;
}

/************************************************************************
**	函数名称:	GreyLock(void)
**	函数功能:	IC卡置灰，同时设置对卡的最后操作为：加气
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void GreyLock(void)
{
    uint8_t buf[16];
    uint8_t bufbak[16];
    uint8_t step = 0;
    uint8_t ntry = 2;

    FYD12864ClearLine(1);
    FYD12864DispPrintfFlash(2, 1, "正在置灰...");
    FYD12864DispPrintfFlash(3, 1, "请稍后");
    FYD12864ClearLine(4);

    m1card.greyFlag[0] = 0x30;//灰卡标志
    m1card.greyFlag[1] = 0x32;

    //写入灰标记
    while((ntry--) && (step == 0))
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 2, &m1card.card[0]))     //置灰 ADD BY LY
        {
            continue;
        }
        else
        {
            step = 1;
            break;
        }
    }

    if(step != 1)
    {
        carderror = 23;//ADD BY LAST
        DispUI = IcCardOperaError;
        return;
    }

    //读出刚刚写入块2的数据
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, bufbak))
        {
            continue;
        }
        else
        {
            step = 2;
            break;
        }
    }

    if(step != 2)
    {
        carderror = 24;//ADD BY LAST
        DispUI = IcCardOperaError;
        return;
    }

    //检查数据是否写入正确
    memcpy(buf,  &m1card.card[0], 16);

    if(memcmp(buf, bufbak, 16) != 0)
    {
        //置灰失败
        carderror = 25;//ADD BY LAST
        DispUI = IcCardOperaError;
        return;
    }
    else
    {
        //置灰成功
        //写枪号、交易流水号、站编号
        if(Mt318ReadM1Card(KEYB, &m1card.keyB[0], 14, buf))     //置灰 ADD BY LY
        {
            m1card.gunnum = sysparas.gunnum;;	//	枪号
            m1card.transnum_another = sysparas.transnum + 1;//交易流水号
            m1card.laststationcode = (uint8_t)sysparas.stationcode;   //站编码

            if(Card_WriteGunandTran(m1card.gunnum, m1card.transnum_another, m1card.laststationcode))
            {
                //更新对卡的操作 ADD BY LY
                m1card.operateType[0] = 0x30;
                m1card.operateType[1] = 0x31;

                if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 6, &m1card.payMoneyCrc[0]))
                {
                    carderror = 26;//ADD BY LAST
                    DispUI = IcCardOperaError;
                    return;
                }

                DispUI = StartFueling;
                return;
            }
            else
            {
                carderror = 27;//ADD BY LAST
                DispUI = IcCardOperaError;
                return;
            }

        }
        else
        {
            carderror = 28;
            DispUI = IcCardOperaError;
            return;
        }

    }
}

/************************************************************************
**	函数名称:	GreyUnLock(void)
**	函数功能:	IC卡解灰
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void GreyUnLock(void)
{
    uint8_t step = 0;
    uint8_t buf[16];
    uint8_t bufbak[16];
    uint8_t ntry = 2;

    fuelrecordinfo.greyflag = 0x00; //加气流水中的灰标记清除
    
    //设置解灰标记
    m1card.greyFlag[0] = 0x30;
    m1card.greyFlag[1] = 0x31;

    //写解灰标记
    while((ntry--) && (step == 0))
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 2, &m1card.card[0]))     //写块2 ADD BY LY
        {
            continue;
        }
        else
        {
            step = 1;
            break;
        }
    }

    if(step != 1)
    {
        carderror = 29;
        DispUI = IcCardOperaError;
        return ;
    }

    //读出刚刚写入的数据 ADD BY LY
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, bufbak))     //读块2
        {
            continue;
        }
        else
        {
            step = 2;
            break;
        }
    }

    if(step != 2)
    {
        carderror = 30;
        DispUI = IcCardOperaError;
        return ;
    }

    //校验是否写入正确
    memcpy(buf,  &m1card.card[0], 16);

    if(memcmp(buf, bufbak, 16) != 0)
    {
        //解灰失败
        carderror = 31;
        DispUI = IcCardOperaError;
        return ;

    }
    else
    {
        //解灰成功
        DispUI = DispFirst;//调试完以后修改为DispFirst
    }

    return;
}

/************************************************************************
**	函数名称:	GreyDataDeal(void)
**	函数功能:	IC卡灰数据处理
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-5-10
*************************************************************************/
void GreyDataDeal(void)
{
    uint8_t 		buf[16];
    uint8_t 		bsecond = 0;
    uint8_t 		bthird = 0;
    uint8_t 		bforth = 0;
    uint8_t 		bfifth = 0;
    uint8_t 		bsixth = 0;
    uint8_t 		bfirst = false;

    //站编码比对 ADD BY LY
    if(m1card.laststationcode  == (uint8_t)(sysparas.stationcode))
    {
        //枪号比对 ADD BY LY
        if(m1card.gunnum == (uint8_t)sysparas.gunnum)
        {
            //枪号比对通过 ADD BY LY
            bfirst = true;
        }
        else
        {
            //枪号比对失败 ADD BY LY
            carderror = 32;
            DispUI = IcCardOperaError;
            return;
        }
    }
    else
    {
        //站编码比对失败 ADD BY LY
        carderror = 33;
        DispUI = IcCardOperaError;
        return;
    }

    //对比卡号 ADD BY LY
    if(bfirst)
    {
        //根据卡中流水号读取流水存入fuelrecordinfo中 ADD BY LY
        if(ReadFuelRecord(m1card.transnum_another - 1, 0))
        {
            //比对IC卡号 ADD BY LY
            if(fuelrecordinfo.usernum == m1card.cardnum_another)
            {
                bsecond = true;
            }
            else
            {
                carderror = 34;
                DispUI = IcCardOperaError;
                return;
            }
        }
        else
        {
            //流水读取失败 ADD BY LY
            carderror = 35;
            DispUI = IcCardOperaError;
            return;
        }
    }

    //更新块45
    if(bsecond)
    {
        //卡余额与加气前余额相等 表示未扣款，更新IC卡 ADD BY LY
        if(m1card.cardbalance_another == fuelrecordinfo.beforebalance)
        {
            if(Card_UpdateSec45())
            {
                bthird = true;
            }
            else
            {
                carderror = 36;
                DispUI = IcCardOperaError;
                return;
            }
        }		
        else if(m1card.cardbalance_another == fuelrecordinfo.afterbalance)
        {
            bthird = true;
        }
		else
		{	
            carderror = 41;
            DispUI = IcCardOperaError;
            return;		
		}		
    }

    //更新块6
    if(bthird)
    {
        if(Card_UpdateSec6(6))
        {
            bforth = true;
        }
        else
        {
            carderror = 37;
            DispUI = IcCardOperaError;
            return;
        }
    }

    //更新块16、17
    if(bforth)
    {
        //更新块16
        memset(buf, 0, sizeof(buf));
        HexToBcdbuf(fuelrecordinfo.allconsumptiontimes, &buf[0], 4);
        HexToBcdbuf(fuelrecordinfo.allconsumptionmoney, &buf[4], 6);
        HexToBcdbuf(fuelrecordinfo.allconsumptionnum, &buf[12], 4);

        if(Mt318WriteM1Card(KEYB, &m1card.keyB[0], 16, buf))
        {
            bfifth = true;
        }
        else
        {
            carderror = 38;
            DispUI = IcCardOperaError;
            return;
        }

        //更新块17
        memset(buf, 0, sizeof(buf));
        HexToBcdbuf(fuelrecordinfo.InitialMoney, &buf[0], 5);
        memcpy(&buf[5], &m1card.block17[5], 6);   //期初时间不变
        HexToBcdbuf(fuelrecordinfo.CalMoney, &buf[11], 5);

        if(Mt318WriteM1Card(KEYB, &m1card.keyB[0], 17, buf))
        {
            bsixth = true;
        }
        else
        {
            carderror = 39;
            DispUI = IcCardOperaError;
            return;
        }

        //IC 全部更新完成，解灰
        if((bfifth == true) && (bsixth == true))
        {
            GreyUnLock();
            DispUI = DispFirst;
        }
        else
        {
            carderror = 40;
            DispUI = IcCardOperaError;
            return;
        }
		
    }
	
	if(m1card.existflag == 0)   //卡被拔出
	{
		DispUI = DispFirst;
	}
}

/************************************************************************
**	函数名称:	IcCardOperaError(void)
**	函数功能:	显示与IC卡操作有关的错误
**	输入参数: 无
**	返回值  	: 无

**	创 建 者:	LY
**	创建日期:	2018-9-17
*************************************************************************/
void IcCardOperaError(void)
{
    switch(carderror)
    {
        case 1:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "写块5失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 2:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "读块5失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 3:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "更新上次余额、上");
            FYD12864DispPrintfFlash(3, 1, "次使用时间失败");
            FYD12864ClearLine(4);
            break;

        case 4:

            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "写块4失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 5:

            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "读块4失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 6:
	        FYD12864ClearLine(1);		
            FYD12864DispPrintfFlash(2, 1, "更新卡余额、最后");
            FYD12864DispPrintfFlash(3, 1, "使用时间失败");
  
            FYD12864ClearLine(4);
            break;

        case 7:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "写块6失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 8:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "读块6失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 9:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "更新最后加气或充");
            FYD12864DispPrintfFlash(3, 1, "值金额卡操作失败");
            FYD12864ClearLine(4);
            break;

        case 10:

            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "写块16失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 11:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "读块16失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 12:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "更新累积消费次数");
            FYD12864DispPrintfFlash(3, 1, "金额、气量失败");
            FYD12864ClearLine(4);
            break;

        case 13:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "写块17失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 14:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "读块17失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 15:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "更新计算余额失败");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);

        case 19:
            FYD12864DispPrintfFlash(1, 1, "挂失卡");
            FYD12864DispPrintfFlash(2, 1, "置挂失卡标志失败");
		    FYD12864ClearLine(3);
            FYD12864DispPrintfFlash(4, 1, "请退卡...");
            break;
			
        case 25:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "置灰卡失败");
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;
        case 31:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "解灰失败");
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;
        case 32:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "请到%02d号枪解灰", m1card.gunnum);
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;
        case 33:
            FYD12864DispPrintfFlash(1, 1, "请到站编码为:");
            FYD12864DispPrintfFlash(2, 1, "%ld", m1card.laststationcode);
            FYD12864DispPrintfFlash(3, 1, "的加气站解灰");
            FYD12864ClearLine(4);
            break;
		case 34:	
			//常发生于IC卡所加流水已保存，逃卡，但系统FLASH清除，再用非本张卡或无卡加气到该流水号，再插入改卡解灰 ADD BY LY
			//解决办法：后台解灰
			FYD12864ClearLine(1);
			FYD12864DispPrintfFlash(2, 1, "卡号不一致");		
			FYD12864ClearLine(3);
			FYD12864ClearLine(4);
			break;
		case 35:
			//常发生于IC卡置灰成功，已改写流水号，退卡，系统中找不到对应流水记录 ADD BY LY
			//解决办法：补卡
			FYD12864ClearLine(1);			
			FYD12864DispPrintfFlash(2, 1, "读流水错误");
			FYD12864ClearLine(3);
			FYD12864ClearLine(4);
			break;			
        default:
            FYD12864DispPrintfFlash(1, 1, "卡操作失败");
            FYD12864DispPrintfFlash(2, 1, "错误代码: %d", carderror);
            FYD12864ClearLine(3);
            FYD12864DispPrintfFlash(4, 1, "请退卡...");
            break;
    }

    //退卡，返回首界面 ADD BY LY
    if(m1card.existflag == 0)
    {
        DispUI = DispFirst;
    }

}

/***************************************************************************
**	函数名称：Card_UpdateSec45(void)
**	函数功能：更新IC卡块4、块5的数据
**	输入参数：无
**	返 回 值：true:	更新块4块5成功
			  false:更新块4块5失败

**	创 建 者：LY
**	创建日期：2018-9-17
***************************************************************************/
uint8_t Card_UpdateSec45(void)
{
    uint8_t 	rdbuf5[16];
    uint8_t 	rdbuf4[16];
    uint8_t 	rdbufbak[16];
    uint8_t 	n = 3;
    uint8_t 	tmp;
    uint8_t 	ntry = 2;
    uint8_t		flag = false; //true: 表示验证通过

    //更新块5数据
    /*****************************************************************/
    //将卡余额、最后使用时间赋值到rdbuf5后面14字节
    memcpy(&rdbuf5[2], &m1card.cardbalance[0], 14);

    //重新计算卡上次余额异或校验码的ASCII并赋值 ADD BY LY
    tmp = 0;

    for(n = 0; n < 4; n++)
    {
        tmp ^= AscToBcd(&rdbuf5[2 + n * 2]);
    }

    rdbuf5[0] = tmp / 0x10 + 0x30;
    rdbuf5[1] = tmp % 0x10 + 0x30;

    /******************************************************************/
    //写块5
    while(ntry--)
    {
    	//写失败，再写一次，最多写2次 ADD BY LY
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 5, rdbuf5))
        {
            continue;
        }
        else
        {
        	//写块5成功，退出循环 ADD BY LY
            flag = true;
            break;
        }
    }

    //写块5失败，显示错误代码
    if(flag == false)
    {
        carderror = 1;
        DispUI = IcCardOperaError;
        return flag;
    }

    //写块5成功，校验数据
    flag = false;
	//读块5
    while(ntry--)
    {
    	//读块5失败，再读一次，最多读2次
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 5, rdbufbak))     
        {
            continue;
        }
        else
        {
        	//读块5成功，退出循环 ADD BY LY
            flag = true;
            break;
        }
    }

    //读块5失败
    if(flag == false)
    {
        carderror = 2;
        DispUI = IcCardOperaError;
        return flag;
    }

    //读块5成功，比对写入与读出的数据
    if(memcmp(rdbuf5, rdbufbak, 16) != 0)   
    {
        //比对失败
        flag = false;
        carderror = 3;
        DispUI = IcCardOperaError;
        return flag;
    }

    /*************************************************************/
    //更新块4数据
    flag = false;//清标志
    
	//将块4的数据存入rdbuf4[]中
    memcpy(&rdbuf4[0], &m1card.balancecrc[0], 16);
	
    //将其后余额转换成ASCII 存于rdbuf[2]后的8个字节 ADD BY LY
    sprintf_P((char*)&rdbuf4[2], "%08ld", fuelrecordinfo.afterbalance);

    //卡最后使用时间更新
    if(Ds3232ReadTime())   //获取当前时间
    {
        //年
        rdbuf4[10] = time.year / 0x10 + 0x30; //Ds3232ReadTime()读出的十六进制，需转换成ASCII ADD BY LY
        rdbuf4[11] = time.year % 0x10 + 0x30;
        //月
        rdbuf4[12] = time.month / 0x10 + 0x30;
        rdbuf4[13] = time.month % 0x10 + 0x30;
        //日
        rdbuf4[14] = time.day / 0x10 + 0x30;
        rdbuf4[15] = time.day % 0x10 + 0x30;
    }

    //重新计算卡余额异或校验码的ASCII并赋值 ADD BY LY
    tmp = 0;

    for(n = 0; n < 4; n++)
    {
        tmp ^= AscToBcd(&rdbuf4[2 + n * 2]);
    }

    rdbuf4[0] = tmp / 0x10 + 0x30; //卡余额校验码 2字节 ADD BY LY
    rdbuf4[1] = tmp % 0x10 + 0x30;
    /*************************************************************/
	
    //最多写次数初始化
    ntry = 2;
	
	//写块4
    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 4, rdbuf4))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //写块4失败
    if(flag == false)
    {
        carderror = 4;
        DispUI = IcCardOperaError;
        return flag;
    }

    //读块4
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 4, rdbufbak))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //读块4失败
    if(flag == false)
    {
        carderror = 5;
        DispUI = IcCardOperaError;
        return flag;
    }

    //比对写入与读出的数据
    if(memcmp(rdbuf4, rdbufbak, 16) != 0)
    {
        //比对失败
        flag = false;
        carderror = 6;
        DispUI = IcCardOperaError;
        return flag;
    }

    return flag;
}

/***************************************************************************
**	函数名称：Card_UpdateSec6(void)
**	函数功能：更新IC卡块6的数据
**	输入参数：无
**	返 回 值：true:	更新块6成功
			  false:更新块6失败

**	创 建 者：LY
**	创建日期：2018-9-17
***************************************************************************/
uint8_t Card_UpdateSec6(uint8_t optype)
{
    uint8_t tmp, n;
    uint8_t flag = false;
    uint8_t rdbuf6[16];
    uint8_t rdbufbak[16];
    uint8_t ntry = 2;

    //更新块6数据
    /************************************************************************/
    //将块6中的数据赋值到rdbuf6中
    memcpy(rdbuf6, &m1card.payMoneyCrc[0], 16);
    //将最后加气金额或充值金额转换成ASCII ADD BY LY
    sprintf_P((char*)&rdbuf6[2], "%08ld", fuelrecordinfo.money);
    //设置操作类型
    rdbuf6[10] = '0'; //操作类型更新：01：加气

    if((optype > 0) && (optype < 9))
    {
        tmp = 0x30 + optype;
    }

    rdbuf6[11] = tmp;
    //重新计算最后加气金额及操作异或校验码的ASCII并赋值 ADD BY LY
    tmp = 0;

    for(n = 0; n < 4; n++)
    {
        tmp ^= AscToBcd(&rdbuf6[2 + n * 2]);   //最后加气或充值金额 从ASCII转换成BCD 再计算校验值存入tmp中 ADD BY LY
    }

    rdbuf6[0] = tmp / 0x10 + 0x30; //校验值赋值 ADD BY LY
    rdbuf6[1] = tmp % 0x10 + 0x30;
    /************************************************************************/
    //写块6
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 6, rdbuf6))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //写块6失败
    if(flag == false)
    {
        carderror = 7;
        DispUI = IcCardOperaError;
        return flag;
    }

    //读块6
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 6, rdbufbak))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //读块6失败
    if(flag == false)
    {
        carderror = 8;
        DispUI = IcCardOperaError;
        return flag;
    }

    //写入数据校验
    if(memcmp(rdbuf6, rdbufbak, 16) != 0)     //比对
    {
        carderror = 9;
        DispUI = IcCardOperaError;
        return flag;
    }

    return flag;
}

/***************************************************************************
**	函数名称：Card_UpdateSec1_67(void)
**	函数功能：更新IC卡块16、17的数据
**	输入参数：无
**	返 回 值：true:	更新块6成功
			  false:更新块6失败

**	创 建 者：LY
**	创建日期：2018-9-17
***************************************************************************/
uint8_t Card_UpdateSec1_67(void)
{
    uint8_t 	rdbuf16[16];
    uint8_t 	rdbuf17[16];
    uint8_t 	tmpbuf[6];
    uint8_t 	rdbufbak[16];
    uint8_t 	ntry = 2;
    uint8_t  	flag = false;
	
	//将块16的数据赋值到rdbuf16[]中
    memcpy(rdbuf16, &m1card.block16, 16);
	
    //卡累计消费次数 更新
    HexToBcdbuf(fuelrecordinfo.allconsumptiontimes, &rdbuf16[0], 4);
    //卡累积消费金额 更新
    HexToBcdbuf(fuelrecordinfo.allconsumptionmoney / 10000, &rdbuf16[4], 4);
    HexToBcdbuf(fuelrecordinfo.allconsumptionmoney % 10000, &rdbuf16[8], 2);
    //卡消费气量总量 更新
    HexToBcdbuf(fuelrecordinfo.allconsumptionnum, &rdbuf16[12], 4);

    //更新卡累积消费次数、累积消费金额、累积消费气量 ADD BY LY
    flag = 0;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 16, rdbuf16))     //写块16
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //写块16失败
    if(flag == false)
    {
        carderror = 10;
        DispUI = IcCardOperaError;
        return flag;
    }

    //读块16
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 16, rdbufbak))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //读块16失败
    if(flag == false)
    {
        carderror = 11;
        DispUI = IcCardOperaError;
        return flag;
    }

    //写入数据校验
    if(memcmp(rdbuf16, rdbufbak, 16) != 0)     //比对
    {
        flag = false;
        carderror = 12;
        DispUI = IcCardOperaError;
        return flag;
    }

    //更新块17
    memcpy(rdbuf17, &m1card.block17, 16);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    HexToBcdbuf(fuelrecordinfo.CalMoney / 10000, &tmpbuf[0], 4);
    HexToBcdbuf(fuelrecordinfo.CalMoney % 10000, &tmpbuf[4], 2);
    memcpy(&rdbuf17[11], &tmpbuf[1], 5);

    //写块17
    flag = false;
    ntry = 3;

    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 17, rdbuf17))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //写块17失败
    if(flag == false)
    {
        carderror = 13;
        DispUI = IcCardOperaError;
        return flag;
    }

    //读块17
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        //读块18 取出累积充值金额、累积充值次数 ADD BY LY
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 17, rdbufbak))
        {
            continue;
        }
        else
        {
            flag =  true;
            break;
        }
    }

    //读块17失败
    if(flag == false)
    {
        carderror = 14;
        DispUI = IcCardOperaError;
        return flag;
    }

    //写入数据校验
    if(memcmp(rdbuf17, rdbufbak, 16) != 0)     //比对
    {
        flag = false;
        carderror = 15;
        DispUI = IcCardOperaError;
        return flag;
    }

    return flag;
}

/***************************************************************************
**	函数名称：IcCardPayMoney(void)
**	函数功能：用卡加气，加气结束，扣款
**	输入参数：无
**	返 回 值：true:	扣款成功
			  false:扣款失败

**	创 建 者：LY
**	创建日期：2018-9-17
***************************************************************************/
uint8_t IcCardPayMoney(void)
{
    uint8_t ret = false;

    //	卡余额与加气前余额相等，说明未扣款 ADD BY LY
    if(m1card.cardbalance_another == fuelrecordinfo.beforebalance)
    {
    	//更新块4、块5
        ret = Card_UpdateSec45();
    }
	// 卡余额与加气后余额相等，说明已扣款
    else if(m1card.cardbalance_another == fuelrecordinfo.afterbalance)
    {
        ret = true;
    }

    //更新块45失败
    if(ret == false)
    {
        carderror = 16;
        DispUI = IcCardOperaError;
        return ret;
    }

    //更新块45成功，再更新块6
    ret = false;

	//更新块6
    if(Card_UpdateSec6(3))
    {
        ret = true;
    }

    //更新块6失败
    if(ret == false)
    {
        carderror = 17;
        DispUI = IcCardOperaError;
        return ret;
    }

    //更新块6成功，再更新块16、17
    ret = false;
	
	//更新块16、17
    if(Card_UpdateSec1_67())
    {
        ret = true;
    }
    else
    {
        carderror = 18;
        DispUI = IcCardOperaError;
        return ret;
    }

    return ret;
}

/***************************************************************************
**	函数名称：PowerDown(void)
**	函数功能：掉电显示函数
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCK
**	创建日期：2016-7-25
***************************************************************************/
void PowerDown(void)
{
	//停机
    PumpSwitch(StaClose);     
	
    FYD12864DispPrintfFlash(1, 1, "系统已掉电...");
    FYD12864DispPrintfFlash(4, 1, "数据已保存");
    DispUI = PowerDown;

	//按返回键，重启系统
    TaskAdd(KEY_RET, SoftReset, NULL);
}

/***************************************************************************
**	函数名称：SaveRealtime(void)
**	函数功能：保存掉电实时数据
**	输入参数：无
**	返 回 值：无

**	创 建 者：LY
**	创建日期：2018-9-17
***************************************************************************/
void SaveRealtime(void)
{
    //设备类型
    realtime.lcng = sysparas.devicetype;
    //当前交易流水号 放在最后赋值 ADD BY LY
    //加气机工作状态
    realtime.workstatus = globalvar.workstate;
    //单价
    realtime.price = fuelrecordinfo.price;
    //回气量
    realtime.backgas = fuelrecordinfo.backgas;
    //当前加气量
    realtime.volume = fuelrecordinfo.volume;
    //当前加气金额
    realtime.money = fuelrecordinfo.money;
    //当前班号
    realtime.shiftnum = sysparas.shiftnum;
    //IC卡号 在有卡里面赋值 ADD BY LY
    //当前车牌号
    realtime.carnum = m1card.carnum_another;
    //卡类型 在有卡里面赋值 ADD BY LY
    //加气开始时间
    memcpy(&realtime.starttime[0], &fuelrecordinfo.starttime[0], 6);
    //加气结束时间
    realtime.endtime[0] = 0x20;
    memcpy(&realtime.endtime[0], &time.year, 6);

    /********************加入卡判断里面**********************************/
    //加气前余额
    //加气后余额
    //累计充值次数
    //累计充值金额
    //累积消费次数
    //累积消费气量
    //累积消费金额
    //计算余额
    /********************************************************************/
    //加气前压力
    realtime.beforepressure = fuelrecordinfo.beforepressure;
    //加气后压力
    realtime.afterpressure = realtime.cur_pressure;
    //当量
    realtime.curnum = sysparas.coeff;
    //计量单位
    realtime.measuretype = sysparas.unit;
    //停机原因
    realtime.stopreason = 5;
    //解灰相关流水号
    realtime.jhid = 0;
    //班流水号
    realtime.classid = sysparas.shiftransnum;
    //加气模式
    realtime.fuelmode = sysparas.fuelmode;

    if(globalvar.fuelstep != 3)
    {
        //在StopFueling() 以前掉电 需先计算，再保存 ADD BY LY
        globalvar.stoppumptime = sysparas.stoppumptime + 2; //加液后停泵延时
        //交易流水号
        sysparas.transnum++;
        fuelrecordinfo.transnum = sysparas.transnum;
        //枪总气量
        sysparas.totvolume += fuelrecordinfo.volume;
        //枪总金额
        sysparas.totmoney += fuelrecordinfo.money;
        //班累计气量(通过流水累加计算)
        sysparas.shiftotvol += fuelrecordinfo.volume;
        //班累计金额 (通过流水累加计算)
        sysparas.shiftotmon += fuelrecordinfo.money;
        //班加气次数
        sysparas.shiftfueltimes++;
		
        //参数保存
        StopFuelingSaveParas();

        //与卡有关的计算 ADD BY LY
        if((fuelrecordinfo.cardtype != 0) && (fuelrecordinfo.cardtype != 7))
        {
            //卡加气后余额	计算余额
            if(fuelrecordinfo.cardtype == 3)
            {
                fuelrecordinfo.afterbalance = fuelrecordinfo.afterbalance + fuelrecordinfo.money;
                fuelrecordinfo.CalMoney = fuelrecordinfo.CalMoney + fuelrecordinfo.money;
            }
            else
            {
                fuelrecordinfo.afterbalance = fuelrecordinfo.afterbalance - fuelrecordinfo.money;
                fuelrecordinfo.CalMoney = fuelrecordinfo.CalMoney - fuelrecordinfo.money;
            }

            //卡累计消费次数
            fuelrecordinfo.allconsumptiontimes = m1card.sumtimes + 1;
            //卡累计消费金额
            fuelrecordinfo.allconsumptionmoney += fuelrecordinfo.money;
            //卡累积消费气量
            fuelrecordinfo.allconsumptionnum += fuelrecordinfo.volume;
        }
    }

    //与卡有关实时数据赋值
    if(fuelrecordinfo.cardtype != 0)
    {
        //当前用户卡号
        realtime.cardnum = fuelrecordinfo.usernum;
        //卡类型
        realtime.cardtype = m1card.cardtype_another;
        //灰卡标记
        realtime.greyflag = fuelrecordinfo.greyflag;
        //加气后余额
        realtime.afterbalance = fuelrecordinfo.afterbalance;
        //计算余额
        realtime.calmoney = fuelrecordinfo.CalMoney;
        //加气前余额
        realtime.beforebalance = fuelrecordinfo.beforebalance;
        //累积充值次数
        realtime.allrechargetimes = fuelrecordinfo.allrechargetimes;
        //累计充值金额
        realtime.allrechargemoney = fuelrecordinfo.allrechargemoney;
        //实时累计消费次数赋值
        realtime.allconsumptiontimes = fuelrecordinfo.allconsumptiontimes;
        //实时累计消费气量赋值
        realtime.allconsumptionnum = fuelrecordinfo.allconsumptionnum;
        //实时累计消费金额赋值
        realtime.allconsumptionmoney = fuelrecordinfo.allconsumptionmoney;
    }

    //交易流水号
    realtime.transnum = sysparas.transnum;
    //保存实时流水
    EepromInterWriteBytes(REALTIMEDATA_ADDR, (uint8_t *)(&realtime.lcng), sizeof(RealTimeData));         //存储掉电实时流水到EEPROM 中
}

/***************************************************************************
**	函数名称：PowerDownRealDate(void)
**	函数功能：掉电数据恢复
**	输入参数：无
**	返 回 值：无

**	创 建 者：LY
**	创建日期：2018-9-17
***************************************************************************/
void PowerDownRealDate(void)
{
    uint8_t *tmp;

    fuelrecordinfo.beforegunsum = sysparas.totvolume - realtime.volume;	//加气前枪累
    fuelrecordinfo.aftergunsum = sysparas.totvolume;					//加气后枪累

    fuelrecordinfo.transnum = realtime.transnum;                        //交易流水号
    fuelrecordinfo.classnum = sysparas.shiftnum;                        //班号
	fuelrecordinfo.carnum_another = realtime.carnum;					//车牌号
    fuelrecordinfo.usernum = realtime.cardnum;							//用户卡号
    fuelrecordinfo.cardtype = realtime.cardtype;                        //卡类型
    memcpy(&fuelrecordinfo.starttime[0], &realtime.starttime[0], 6);    //加气开始时间
    memcpy(&fuelrecordinfo.endtime[0], &realtime.endtime[0], 6);        //加气结束时间

    fuelrecordinfo.price = realtime.price;                              //单价
    fuelrecordinfo.volume = realtime.volume;                            //气量
    fuelrecordinfo.money = realtime.money;                              //金额
    fuelrecordinfo.backgas = realtime.backgas;                          //回气量

    fuelrecordinfo.allrechargetimes = realtime.allrechargetimes;        //累计充值次数
    fuelrecordinfo.allrechargemoney = realtime.allrechargemoney;        //累计充值金额

    fuelrecordinfo.beforebalance = realtime.beforebalance;              //加气前余额
    fuelrecordinfo.afterbalance = realtime.afterbalance;                //加气后余额

    fuelrecordinfo.allconsumptiontimes = realtime.allconsumptiontimes;  //累计消费次数
    fuelrecordinfo.allconsumptionnum = realtime.allconsumptionnum;      //累计消费气量
    fuelrecordinfo.allconsumptionmoney = realtime.allconsumptionmoney;  //累计消费金额
    fuelrecordinfo.CalMoney = realtime.calmoney;						//计算余额

    fuelrecordinfo.allgunnum = sysparas.totvolume;                      //枪累气量
    fuelrecordinfo.allgunmoney = sysparas.totmoney;                     //枪累金额
    fuelrecordinfo.beforepressure = realtime.beforepressure;            //加气前压力
    fuelrecordinfo.afterpressure = realtime.afterpressure;              //加气后压力
    fuelrecordinfo.density = (uint32_t)(realtime.cur_density_g * 10000 + 0.5);     //密度
    fuelrecordinfo.curnum = realtime.curnum;                            //当量
    fuelrecordinfo.devicetype = realtime.lcng;                          //气品代码
    fuelrecordinfo.measuretype = realtime.measuretype;                  //计量单位
    fuelrecordinfo.stopreason = realtime.stopreason;                    //停机原因
    //fuelrecordinfo.jhid = realtime.jhid;                              //解灰相关流水号
    fuelrecordinfo.classid = realtime.classid;                          //班流水号
    fuelrecordinfo.fuelmode = realtime.fuelmode;						//加气模式
    fuelrecordinfo.greyflag = realtime.greyflag;						//灰标记
    tmp = (uint8_t *)(&fuelrecordinfo.transnum);
    fuelrecordinfo.crc = ModbusCrc16(tmp, sizeof(FuelRecordInfo) - 2);		//changed by LY

	//保存掉电后，恢复的流水
    if(SaveFuelRecord(0))
    {
        FYD12864DispPrintfFlash(4, 1, "数据已恢复");
		//打印流水
        Printid();
    }
    else
    {
        FYD12864DispPrintfFlash(4, 1, "数据恢复失败");
    }
}

/***************************************************************************
**	函数名称：FixGas(void)
**	函数功能：定量加气，选择页面
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCK
**	创建日期：2016-7-25
***************************************************************************/
void FixGas(void)
{
    FYD12864ClearLine(1);
    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "  1.定气量");
    FYD12864DispPrintfFlash(3, 1, "  2.定金额");

    TaskAdd(KEY_RET, DispFirst, NULL);	//按返回键，回主界面
    TaskAdd(KEY_1, FixNum, NULL);		//按【1】，进入定气量加气
    TaskAdd(KEY_2, FixMoney, NULL);		//按【2】，进入定金额加气
	
	//卡被拔出
    if((m1card.cardtype_another != 0) && (m1card.existflag == 0))       
    {
        DispUI = DispFirst;
    }

    globalvar.fixnum = 0;
    globalvar.fixmoney = 0;
    globalvar.displayonce = 1;
	
    InputInit();//输入结构体初始化
}

/***************************************************************************
**	函数名称：FixNum(void)
**	函数功能：定气量加气
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCK
**	创建日期：2016-7-25
***************************************************************************/
void FixNum(void)
{
    char buf[16] = {0};
    uint32_t tmp_fixmoney = 0;
    uint32_t  availmoney;     //IC卡可用加气金额

    ScreenInput.inputmaxlen = 5;//输入上限99999

    if(globalvar.KeyValue == KEY_CLR)
    {
    	//清除定气量报警
        FYD12864ClearLine(4);
    }

    NumKeyHandler();
    globalvar.fix = 1;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    globalvar.fixnum = atol(buf);
    FYD12864ClearLine(3);
    FYD12864DispPrintfFlash(2, 1, "定气量:%ld.%02d", globalvar.fixnum / 100, globalvar.fixnum % 100);

	TaskAdd(KEY_RET, DispFirst, NULL);//按【返回】，回主界面

	//根据所定气量，计算定的金额
    tmp_fixmoney = (globalvar.fixnum * fuelrecordinfo.price) / 100 ;
	
	//定气量，必须大于1L或1kg
	if((globalvar.fixnum < 100) && (globalvar.KeyValue == KEY_OK))
    {
        FYD12864DispPrintfFlash(4, 1, "所定气量过小");
    }
	//非VIP 卡加气，做定金额处理
    else if((m1card.existflag == 1) && (m1card.checkresult == 0)&&(m1card.cardtype_another != 7))
    {
    	//非记账卡，定金额 ADD BY LY
        if(m1card.cardtype_another != 3)   
        {
            //程序中所有的钱，都是按分来存取，计算的，只有在显示的时候，变成元来显示给客户看的 ADD BY LY
            availmoney = m1card.cardbalance_another - sysparas.cardminyue;//计算IC卡可用加气金额 ADD BY LY
        }
		//记账卡，定金额
        else if(m1card.cardtype_another == 3)   
        {
            availmoney = sysparas.accountcardmaxyue - m1card.cardbalance_another;
        }
		
		//所定气量超出卡余额上限报警
        if(tmp_fixmoney > availmoney)
        {
            FYD12864DispPrintfFlash(4, 1, "所定气量超出限额");
        }
		//有卡手动定量加气，按确认开始越冷加气  ADD BY LY
        else if((sysparas.fuelmode == 0)&& (globalvar.KeyValue == KEY_OK))   
        {
            FYD12864ClearLine(4);
            TaskAdd(KEY_OK, StartPrecooling, NULL);
        }
    }
	//无卡手动定量加气
    else
    {
        if((sysparas.fuelmode == 0) && (sysparas.usecard != 1))       
        {
        	//输入金额后，按【确认】，开始预冷、加气
            TaskAdd(KEY_OK, StartPrecooling, StartFueling);
        }
    }

}

/***************************************************************************
**	函数名称：FixMoney(void)
**	函数功能：定金额加气
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCK
**	创建日期：2016-7-25
***************************************************************************/
void FixMoney(void)
{
    char buf[16] = {0};
    uint32_t  availmoney;     //IC卡可用加气金额

    ScreenInput.inputmaxlen = 5;//输入上限99999

    //当所定金额过小时，删除原有输入，重新输入，同时清掉错误提示，因NumKeyHandler()对删除键键值做了处理
    //故要在NumKeyHandler()处理以前，进行错误提示的清除动作 ADD BY LY
    if(globalvar.KeyValue == KEY_CLR)
    {
    	//清除定金额报警提示
        FYD12864ClearLine(4);
    }

    NumKeyHandler();
    globalvar.fix = 2;
	//输入的是元，但是使用的是分，故需要乘以100 ADD BY LY
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);      
    globalvar.fixmoney = atol(buf);
    FYD12864ClearLine(3);
    FYD12864DispPrintfFlash(2, 1, "定金额:%ld.%02d", globalvar.fixmoney / 100, globalvar.fixmoney % 100);

	//按【返回】，回到主界面
	TaskAdd(KEY_RET, DispFirst, NULL);

	//定金额大小不能小于默认单价
	if((globalvar.fixmoney < sysparas.price[0]) && (globalvar.KeyValue == KEY_OK))
    {
        FYD12864DispPrintfFlash(4, 1, "所定金额过小");
    }
    else if((m1card.existflag == 1) && (m1card.checkresult == 0) && (m1card.cardtype_another != 7))
    {	
    	//非记账卡 ADD BY LY
        if(m1card.cardtype_another != 3)   
        {
            //程序中所有的钱，都是按分来存取，计算的，只有在显示的时候，变成元来显示给客户看的 ADD BY LY
            availmoney = m1card.cardbalance_another - sysparas.cardminyue;//计算IC卡可用加气金额 ADD BY LY
        }
		//记账卡
        else if(m1card.cardtype_another == 3)   
        {
            availmoney = sysparas.accountcardmaxyue - m1card.cardbalance_another;
        }
		
		//所定金额超出余额上限报警
        if(globalvar.fixmoney > availmoney)
        {
            FYD12864DispPrintfFlash(4, 1, "所定金额超出余额");
        }
		//所订金额未超限，手动加气模式，输入金额后，按确定，开始预冷加气
        else if((sysparas.fuelmode == 0)&& (globalvar.KeyValue == KEY_OK)) 
        {
            FYD12864ClearLine(4);
            TaskAdd(KEY_OK, StartPrecooling, NULL);
        }
    }
	//无卡手动加气
    else
    {
        if((sysparas.fuelmode == 0) && (sysparas.usecard != 1))       
        {
        	//输入金额后，按【确认】开始预冷、加气
            TaskAdd(KEY_OK, StartPrecooling, StartFueling);
        }
    }
}

