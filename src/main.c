#include "kernel.h"

GlobalVar        globalvar;			//全局变量
ShiftRecordInfo  shiftrecordinfo;	//上下班记录信息
FuelRecordInfo   fuelrecordinfo;	//加气流水记录信息
FuelRecordInfo   fuelrecordinfo_tmp;//加气流水记录信息
ErrorRecordInfo	 errorRecordinfo;	//故障流水信息
Sysparas         sysparas;			//系统参数
Sysparas         tmp_sysparas;		//用户修改系统参数时，临时存储需要修改的参数
M1carddata       m1card;    		//卡结构数据。

uint32_t         syserr;       		//系统异常
uint32_t         stopreason;   		//停机原因
uint32_t		 carderror;	   		//与IC卡操作有关的错误

uint32_t    	 SYSPARA_ADDR = SYSPARA_ADDR1;
uint8_t			 _MODBUS_G_ = 1;
uint8_t			 _MODBUS_L_ = 1;

//syserr 32位，代表32个异常，某位为1表示1种异常
__farflash char ErrorInfo[][16] =
{
    "FLASH 检测失败",//0
    "FLASH 写失败",
    "FLASH 读失败",
    "写内部EEPROM错误",
    "读内部EEPROM错误",
    "写外部EEPROM错误",//5
    "读外部EEPROM错误",
    "内外EE数据不一致",
    "气相流量计通讯错",
    "液相流量计通讯错",// 9
    "压力传感器异常",
};


//必须用卡加气时，未插卡而扫描了电子标签
void DispInserCard(void)
{
    FYD12864ClearLine(1);

    if(sysparas.offlinemode == 1)
    {
        FYD12864DispPrintfFlash(2, 1, "脱机模式不可用卡");
        FYD12864ClearLine(3);
    }
    else
    {
        FYD12864DispPrintfFlash(2, 1, "请插入IC卡后，再");
        FYD12864DispPrintfFlash(3, 1, "启动加气机");
    }

    FYD12864ClearLine(4);
    TaskAdd(KEY_RET, DispFirst, NULL);
}


void DispErrorInfo()
{
    uint8_t row = 0, num = 0, i = 4;
    uint32_t tmpsyserror = syserr;

    globalvar.workstate = StaFault; //加气机故障。

    for(row = 0; row < 4;)
    {
        num = GetOnePosition(tmpsyserror);   //返回为1的位的位置 ADD BY LY

        if((num == 9) || (num == 10) || (num == 11))        //如果9、10、11中的任意位为1，表示流量计和压力传感器错误 ADD BY LY
            ReadModbusAndPressure();//读流量计和压力传感器 ADD BY LY

        if(num > 0)
        {
            ClrBit(tmpsyserror, num - 1);   //找到1的位置后将其清除 ADD BY LY
            row++;
            FYD12864DispString(row, 1, ErrorInfo[num - 1], 1);   //显示对应错误 ADD BY LY
        }
        else
            break;
    }

    for(i = 4; i > row; i--)
    {
        FYD12864ClearLine(i);
    }

    if(sysparas.lcdtype == 1)   //主要用于大屏显示报警
    {
        OsplcShowN(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price);
    }
}

void ClearSyserror(void)
{
    syserr = 0;
    DispUI = DispFirst;
}


void DispFirst(void)
{
    globalvar.timeout = 0;           				//超时时间清零。
    globalvar.modbus_g_error = 0;    				//读气相流量计错次数清零。
    globalvar.modbus_l_error = 0;    				//读液相流量计错次数清零。
    globalvar.fuelstep = 0;         				//加气步骤。
    globalvar.fix = 0;               				//定量标志。
    globalvar.fixnum = 0;
    globalvar.fixmoney = 0;
    globalvar.U8Temp = 1;
    globalvar.devicetest = 0;
    globalvar.stoptimes = 0;
    globalvar.workstate = StaIdle;
	
    InputInit();
    memset(&m1card.serialnumber[0], 0, sizeof(M1carddata));   	//卡数据结构体清零。
    AB_OUT_HIGH();

    if(syserr)
    {
        DispErrorInfo();//虽然协议里面有报警流水，实际没有使用 故暂时不写上传错误报警的部分 ADD BY LY
    }
    else
    {
        globalvar.workstate = StaIdle;			
		if(globalvar.eepromflag == 1)
        {
        	FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "EEPROM次数超限");
            FYD12864DispPrintfFlash(3, 1, "请及时更换主板");
            FYD12864ClearLine(4);
        }
        else
        {
        	FYD12864DispPrintfFlash(1, 1, "枪号:%02d 班号:%02d", sysparas.gunnum, sysparas.shiftnum);
            FYD12864DispPrintfFlash(2, 1, "气量:%ld.%02d", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);
            FYD12864DispPrintfFlash(3, 1, "金额:%ld.%02d", fuelrecordinfo.money / 100, fuelrecordinfo.money % 100);
			if((globalvar.comfailcount > 10) && (sysparas.offlinemode == 0))       //加气机处于在线模式，但连接超时
	        {
	            FYD12864DispPrintfFlash(4, 1, "加气机已掉线");
	        }
	        else
	        {
	            DispDateTime();
	        }
        }
        
        TaskAdd(KEY_BACK, StartPrecooling, DispFirst);			  //只是预冷
        if((sysparas.fuelmode == 0) && (sysparas.usecard != 1))	 //手动加气。
        {
            TaskAdd(KEY_START, StartPrecooling, StartFueling);    //预冷后加液。
        }

        if(sysparas.usecard != 1)
        {
            TaskAdd(KEY_FIX, FixGas, NULL);	//定量加气。
        }
        else
        {
            TaskAdd(KEY_START, DispInserCard, NULL);
            TaskAdd(KEY_FIX, DispInserCard, NULL);
        }
    }

    if(sysparas.simulation == 1)
    {
        TaskAdd(KEY_RET, ClearSyserror, NULL);
    }

    TaskAdd(KEY_QRY, DispQuery1, NULL);								//查询。
    TaskAdd(KEY_SET, DispInputMenuPwd, NULL);						//设置。
    TaskAdd(KEY_COMB_SBACK, DispInputQcPwd, NULL);					//停止+回显 修改质检参数	(还需根据质检模式修改)
    TaskAdd(KEY_COMB_SQRY, DispInputTestPwd, NULL);					//停止+查询 硬件测试。
    TaskAdd(KEY_COMB_SSET, DispInputResetPwd, NULL); 				//停止+菜单 计量单位，质检模式，恢复出厂设置。
    TaskAdd(KEY_COMB_SRETPOINT, SystemReStartToBootLoader, NULL);   //重启到BootLoader
    TaskAdd(KEY_COMB_SRETCLR, SystemReStartToAPP, NULL);   			//重启到APP
}

//循环加气测试时，按定量处理
void CirculateFueling(void)
{
    globalvar.fixnum = 100 + time.day + time.hour + time.minute + time.second;
    globalvar.fix = 1;  	 //定量

    if((m1card.existflag == 0) && (sysparas.usecard != 1)) 		//无卡加气
    {
        DispUI = StartPrecooling;
        DispNextUI = StartFueling;
    }
    else if((m1card.existflag == 1) && (sysparas.usecard != 2))       //有卡加气
    {
        DispUI = StartPrecooling;
        DispNextUI = GreyLock;
    }
    else
    {
        DispUI = DispFirst;
    }

    globalvar.displayonce = 1;
}

/*************************************************************************
** 函数名称：Task100msProcess
** 函数功能：100ms任务处理
** 入口参数：无
** 返 回 值：无
** 创 建 者：杨朝旭
** 创建时间：2016-12-7 13:21:02

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void Task100msProcess(void)
{
    timer1.flag_100ms = 0;

    if((globalvar.BeepCount--) == 0)
    {
        BEEP_OFF();
    }

    if(globalvar.MsgTime > 0)
        globalvar.MsgTime--;

    RecBlckDataRplyCheck();//后台接收数据有效性判断

    DispUI();// DispUI()在不同时间，执行的不同函数，在当前UI显示完以后，对DispUI进行赋值，初次显示DispFirst() BY  LY
    globalvar.KeyValue = 0; //界面刷新后，需将键值清零

}

/*************************************************************************
** 函数名称：Task500msProcess
** 函数功能：500ms任务处理
** 入口参数：无
** 返 回 值：无
** 创 建 者：杨朝旭
** 创建时间：2016-12-7 13:21:02

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void Task500msProcess(void)
{
    timer1.flag_500ms = 0;
    Ds3232ReadTime();

    if(LED1_RUN == 0)
        LED1_ON();
    else
        LED1_OFF();

    if(BA == 1)   //一泵双机加液时，判断状态
    {
        globalvar.otherstate = 1; //对方待机
    }
    else
    {
        globalvar.otherstate = 0;//对方加液
    }
}

/*************************************************************************
** 函数名称：Task1sProcess
** 函数功能：1s任务处理
** 入口参数：无
** 返 回 值：无
** 创 建 者：杨朝旭
** 创建时间：2016-12-7 13:21:02

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void Task1sProcess(void)
{
    timer1.flag_1s = 0;

    if((globalvar.devicetest == 0) && ((globalvar.workstate == StaIdle) || (globalvar.workstate == StaFault)))           //下班，空闲，故障下实时判断，电磁阀测试时，不判断
    {
        if(globalvar.otherstate == 0)  		   //对方加液
        {
            //PumpSwitch(StaOtherFuel);
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();      //中关
            VCTRL_HIGH_CLOSE();     //高关
        }
        else if(globalvar.otherstate == 1)   //对方待机
        {
            //PumpSwitch(StaIdle);
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();      //中开
            VCTRL_HIGH_CLOSE();    //高关
        }
    }

    if(globalvar.stoppumptime > 0)
    {
        globalvar.stoppumptime--;//延时停泵倒计时
    }

    if((globalvar.devicetest == 0) && (globalvar.stoppumptime == 0) && (globalvar.workstate != StaFuel))       //加液后延时停泵
    {
        if(globalvar.otherstate == 0) 		   //对方加液
        {
            PumpSwitch(StaOtherFuel);
        }
        else if(globalvar.otherstate == 1)   //对方待机
        {
            PumpSwitch(StaIdle);
        }
    }

    if((m1card.existflag == 0) && (globalvar.workstate == StaIdle) && (sysparas.usecard != 2))         //无卡状态且在空闲下班时寻卡。
    {
        if(Mt318GetCardSerialNum())                          //寻卡。
        {
            DispUI = GetCardData;                            //读卡。
        }
    }

    if(m1card.existflag == 1)                                //ycx20161125若有卡，实时检测卡状态
    {
        if(Mt318GetCardStatus() == false)
        {
            if(Mt318GetCardStatus() == false)                //两次判断都没有卡，证明卡被拔出
            {
                m1card.existflag = 0;
                m1card.checkresult = 0xFF;
            }
        }
    }

    if((globalvar.circulatefue == 1) &&
            (globalvar.workstate == StaIdle) &&
            (globalvar.displayonce == 1) &&
            (((sysparas.usecard != 2) && (m1card.existflag == 1) && (m1card.checkresult == 0)) ||          //有卡加气
             ((sysparas.usecard != 1) && (m1card.existflag == 0))) &&        //无卡加气
            ((DispUI == DispFirst) || (DispUI == DispCardInfo))       //只在首界面和卡界面进行判断是否进入自动加气
      ) //是否循环加气。
    {
        globalvar.displayonce = 0;
        TaskMessage(20, CirculateFueling);
    }

}

/*************************************************************************
** 函数名称：HW_Initial
** 函数功能：各硬件初始化
** 入口参数：无
** 返 回 值：无
*************************************************************************/
void HW_Initial(void)
{
    _delay_ms(3000);
    OS_ENTER_CRITICAL();								//关全局中断

    memset(&globalvar.U8Temp, 0, sizeof(GlobalVar));    //全局变量初始化后  此处也可以写成 memset(&globalvaribles.U8Temp,0,sizeof(struct GlobalVar)); ADD BY LY 结构体的两种定义
    memset(&m1card, 0, sizeof(M1carddata));     		//M1卡结构体初始化 ADD BY LY

    PowerCheck();				//掉电检测
    SI_INDATE_IN();				//加密板
    SI_INDATE_H();				//设置加密板引脚为上拉
    BA_IN();					//互锁
    BA_OUT_HIGH();				//配置BA为上拉输入 ADD BY LY

    AB_OUT() ;					//互锁
    AB_OUT_HIGH();

    LED1_OUT();					//运行指示灯

    LedInit();
    FYD12864Init();				//初始化主显
    Uart0Init();				//uart0 初始化 用于打印机与读卡器
    KeyBoardInit();				//键盘初始化

    JS28F128FlashInit();
    I2cInit();					//I2C初始化 用于DS3232 和 外部 EEPROM

    PgADCUinit();				//ADC初始化 气相压力传感器初始化
    PlADCUinit();				//ADC初始化 液相压力传感器初始化
    ValveInit();				//电磁阀初始化

    SELECT_PRINT();				//uart0 选择为打印机
    WHPrinterInit();			//打印机初始化后
    WHPrinterNormalCmd();
    SELECT_CARD();				//uart0 选择为读卡器
    JS28F128FlashInit();		//外部flash IO 初始化

    Timer1Init();				//Timer1 初始化
    //Timer3Init();				//Timer3 初始化
    OS_EXIT_CRITICAL();			//开全局中断
}



/*************************************************************************
** 函数名称：HW_Test
** 函数功能：各硬件功能测试
** 入口参数：无
** 返 回 值：无
*************************************************************************/
void HW_Test(void)
{
    if(!EepromDeviceTest())   ////EEPROM测试
    {
        FYD12864ClrScreen();
        FYD12864DispPrintfFlash(2, 1, "外EERPOM Fail");
        SetBit(syserr, SYSERR_W_IN_EEPROM);
        TaskMessage(50, DispFirst);
    }
    else
    {
        FYD12864ClrScreen();
        FYD12864DispPrintfFlash(2, 1, "外EERPOM Pass");
        ClrBit(syserr, SYSERR_W_IN_EEPROM);

    }

    if(!JS28F128FlashCheckId())   //NORflash测试
    {
        FYD12864ClrScreen();
        FYD12864DispPrintfFlash(2, 1, "FLASH 检测失败");
        SetBit(syserr, SYSERR_FLASH_CHK);
        TaskMessage(50, DispFirst);
    }
    else
    {
        FYD12864ClrScreen();
        FYD12864DispPrintfFlash(2, 1, "FLASH 检测成功");
        ClrBit(syserr, SYSERR_FLASH_CHK);
    }

    Mt318GetCardSerialNum();
}


void main(void)
{
    HW_Initial();
    Timer1On();
    //判断是否掉电,防止掉电后电容电压在临界点时使cpu复位，造成数据错乱
    if(PowerCheckIn == 1)
    {
        _delay_ms(20);
        if(PowerCheckIn == 1)
        {
 			return;
        }
    }
    //读取时钟
    Ds3232ReadTime();
    //主界面初始化
    DispUI = DispFirst;
    globalvar.comreceiveflag = 0;
    PumpSwitch(StaIdle);
    //读系统参数
    ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(sysparas));
			
    //判断EEPROM读写是否超出限制
    if(sysparas.writeE2num >= EEPROM_W_TIMES)
    {
        SYSPARA_ADDR = SYSPARA_ADDR2;
        ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //保证读的最新那一块的系统参数

        if(sysparas.writeE2num >= EEPROM_W_TIMES)
        {
            SYSPARA_ADDR = SYSPARA_ADDR3;
            ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //保证读的最新那一块的系统参数

            if(sysparas.writeE2num >= EEPROM_W_TIMES)
            {
                SYSPARA_ADDR = SYSPARA_ADDR4;
                ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //保证读的最新那一块的系统参数

                if(sysparas.writeE2num >= EEPROM_W_TIMES)
                {
                    globalvar.eepromflag = 1; //提示更换eeprom
                }
                else
                    globalvar.eepromflag = 0;
            }
        }
    }
    
	//客显初始化
    OsplcInit();	

    //先读取系统参数，再根据系统参数初始化各串口
    Uart1Init(sysparas.modbusbaud);   //液相流量计端口初始化 19200
    Uart3Init(sysparas.modbusbaud);   //气相流量计端口初始化后
    Uart2Init(sysparas.pcbaud);   //后台通讯 485 端口初始化

    //读最后一笔实时流水
    EepromInterReadBytes(REALTIMEDATA_ADDR, (uint8_t*)&realtime, sizeof(realtime));      //读最后一笔实时流水

    //判断加气机最后一次的工作状态
    if((realtime.workstatus == StaFuel) || (realtime.workstatus == StaPay))       //如果上次加气机正在加气或者扣费过程中掉电。
    {
        //恢复掉电数据 ADD BY LY
        PowerDownRealDate();
        memset(&realtime.lcng, 0, sizeof(RealTimeData));     //ycx加气后清零实时数据。
        EepromInterWriteBytes(REALTIMEDATA_ADDR, (uint8_t*)&realtime, sizeof(realtime));      //加气结束擦掉实时流水数据，为下一次加气做准备
    }
    else
    {
        //不是首次加气，读取上次流水记录
        if(sysparas.transnum > 0)
            ReadFuelRecord(sysparas.transnum - 1, 0);   //如果加气流水号大于0，则读取上一笔流水BY  LY
    }    
	OsplcShow(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price, 0, 1);

    //正常加气，读取流量计的值
    if(sysparas.simulation == 0)
    {
        _MODBUS_G_ = sysparas.modbusaddr_g;
        _MODBUS_L_ = sysparas.modbusaddr_l;
        ModbusInitReg_L();

        if(sysparas.backgascalc == 1)   //回气计量时才初始化气象流量计
        {
            ModbusInitReg_G();
        }
    }

    while(1)
    {
        realtime.workstatus = globalvar.workstate;
        KeyBoardReadKeyValue();
#if 0

        if(modbus.flag == 1)   //采用查询标志的方式执行相关的程序
        {
            modbus.flag = 0;
            ModbusDataDisposs_G();
            ModbusDataDisposs_L();
        }

#endif

        if(timer1.flag_100ms == 1)
        {
            Task100msProcess();
        }

        if(timer1.flag_500ms == 1)
        {
            Task500msProcess();
        }

        if(timer1.flag_1s == 1)
        {
            Task1sProcess();
        }
    }
}


