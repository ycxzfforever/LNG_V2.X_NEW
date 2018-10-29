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

/***************************************************************************
**	函数名称：DispErrorInfo()
**	函数功能：显示系统错误
**	输入参数：无
**	返 回 值：无

**	创 建 者：ycx
**	创建日期：2018-9-17
***************************************************************************/
void DispErrorInfo()
{
    uint8_t row = 0, num = 0, i = 4;
    uint32_t tmpsyserror = syserr;
	
	//加气机故障
    globalvar.workstate = StaFault;

	//一次最多显示4个错误 ADD BY LY
    for(row = 0; row < 4;)
    {
    	//返回为1的位的位置 ADD BY LY
        num = GetOnePosition(tmpsyserror);  
		
		//Sys_Err 的8、9、10 三个元素对应的就是气相流量计、液相流量计、压力传感器错误标志
		//但由于Sys_Err最低位是从0开始的，而GetOnePosition()返回值最低位是从1开始的 ADD BY LY
        if((num == 9) || (num == 10) || (num == 11))  
       	{
        	//读流量计和压力传感器 ADD BY LY
            ReadModbusAndPressure();
        }

		//有系统错误存在
        if(num > 0)
        {
        	//找到1的位置后将其清除 ADD BY LY
        	//此处ClrBit()用(num-1)也是因为将num的最低位编号为0，而GetOnePosition()返回的最低位是1导致 ADD BY LY
			ClrBit(tmpsyserror, num - 1);   
            row++;
			//显示对应错误 ADD BY LY
            FYD12864DispString(row, 1, ErrorInfo[num - 1], 1);   
        }
        else
            break;//没有系统错误
    }

    for(i = 4; i > row; i--)
    {
    	//剩余没有错误的行，清空
        FYD12864ClearLine(i);
    }
	
	//主要用于大屏显示报警
    if(sysparas.lcdtype == 1)   
    {
        OsplcShowN(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price);
    }
}

/***************************************************************************
**	函数名称：ClearSyserror()
**	函数功能：从系统错误提示界面回到主界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：ycx
**	创建日期：2018-9-17
***************************************************************************/
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

	//屏幕输入结构体初始化
    InputInit();
	//卡数据结构体清零
    memset(&m1card.serialnumber[0], 0, sizeof(M1carddata));  
	//一泵双机，互锁输出High
    AB_OUT_HIGH();

    if(syserr)
    {
    	//系统错误提示
        DispErrorInfo();
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
	            DispDateTime();//time结构体每500ms更新一次 ADD BY LY
	        }
        }
        //按回显键，进入预冷，然后回到主界面 ADD BY LY
        TaskAdd(KEY_BACK, StartPrecooling, DispFirst);			  

		//手动、不是必须用卡加气
        if((sysparas.fuelmode == 0) && (sysparas.usecard != 1))	 
        {
        	//按加液键，开始预冷，越冷结束，开始加液（只使用手动模式）
            TaskAdd(KEY_START, StartPrecooling, StartFueling);    
        }

		//无卡、可不用卡加液（手动、后台、电子标签三种加气模式都适应）
        if(sysparas.usecard != 1)
        {
        	//按定量键，进入定量加液
            TaskAdd(KEY_FIX, FixGas, NULL);	//定量加气。
        }
		//只能用卡加液
        else
        {
        	//按加液键和定量键，都提示插卡
            TaskAdd(KEY_START, DispInserCard, NULL);
            TaskAdd(KEY_FIX, DispInserCard, NULL);
        }
    }

	//模拟加液模式
    if(sysparas.simulation == 1)
    {
    	//在错误提示界面，按返回，可回到主界面（正常模式不可以）
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
    globalvar.fix = 1;  	 //定气量

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

	//蜂鸣器控制
    if((globalvar.BeepCount--) == 0)
    {
        BEEP_OFF();
    }

    if(globalvar.MsgTime > 0)
        globalvar.MsgTime--;

	//每100ms回复一次后台命令
    RecBlckDataRplyCheck();

	//每100ms刷新一下界面
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
	
	//读取系统时间
    Ds3232ReadTime();

	//运行指示灯控制
    if(LED1_RUN == 0)
        LED1_ON();
    else
        LED1_OFF();

	//一泵双机，互锁判断
    if(BA == 1)   
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

	//加气机处于非电磁阀测试、空闲、故障三个状态，根据对方加液机状态，控制阀门
    if((globalvar.devicetest == 0) && ((globalvar.workstate == StaIdle) || (globalvar.workstate == StaFault)))           //下班，空闲，故障下实时判断，电磁阀测试时，不判断
    {
    	//对方加液，关掉阀门
        if(globalvar.otherstate == 0)  		   
        {
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();      //中关
            VCTRL_HIGH_CLOSE();     //高关
        }
		//对方待机，打开中压阀
        else if(globalvar.otherstate == 1)   //对方待机
        {
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();      //中开
            VCTRL_HIGH_CLOSE();    //高关
        }
    }

    if(globalvar.stoppumptime > 0)
    {
        globalvar.stoppumptime--;//延时停泵倒计时
    }
	
	//加液后延时停泵 （一泵双机，任何涉及阀门的动作，都要判断对方的状态 ADD BY LY）
    if((globalvar.devicetest == 0) && (globalvar.stoppumptime == 0) && (globalvar.workstate != StaFuel))       
    {
    	//对方加液
        if(globalvar.otherstate == 0) 		   
        {
        	//关闭中、高压阀
            PumpSwitch(StaOtherFuel);
        }
		//对方待机
        else if(globalvar.otherstate == 1)   
        {
        	//阀门进入小循环
            PumpSwitch(StaIdle);
        }
    }

	//无卡、加气机处于空闲、不是无卡加气状态
    if((m1card.existflag == 0) && (globalvar.workstate == StaIdle) && (sysparas.usecard != 2))         
    {
    	//获取卡序列号
        if(Mt318GetCardSerialNum())                          
        {
        	//获取卡序列号成功后，读卡
            DispUI = GetCardData;                            
        }
    }

	 //若有卡，实时检测卡状态 ycx20161125
    if(m1card.existflag == 1)                               
    {
    	//卡状态为：无卡
        if(Mt318GetCardStatus() == false)
        {
        	//再读一次卡状态，仍然是：无卡
            if(Mt318GetCardStatus() == false)                //两次判断都没有卡，证明卡被拔出
            {
                m1card.existflag = 0;
                m1card.checkresult = 0xFF;
            }
        }
    }

	//循环加气，加气机处于空闲状态、不是无卡加气、卡存在且通过验证（有卡加气）或无卡加气，当前界面在主界面或提示插卡界面
	//当加气机处于空闲状态，程序停留在主界面（可不用卡、无卡）或插卡界面（必须用卡），如果检测到加气模式为循环加气，2s后进入循环加气
	if((globalvar.circulatefue == 1) &&
            (globalvar.workstate == StaIdle) &&
            (globalvar.displayonce == 1) &&
            (((sysparas.usecard != 2) && (m1card.existflag == 1) && (m1card.checkresult == 0)) ||          //有卡加气
             ((sysparas.usecard != 1) && (m1card.existflag == 0))) &&        //无卡加气
            ((DispUI == DispFirst) || (DispUI == DispCardInfo)) 
      ) 
    {
        globalvar.displayonce = 0;
        TaskMessage(20, CirculateFueling);//2s后，进入循环加气状态
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
    BA_IN();					//互锁输入
    BA_OUT_HIGH();				//配置BA为上拉输入 ADD BY LY

    AB_OUT() ;					//互锁输出
    AB_OUT_HIGH();

    LED1_OUT();					//运行指示灯

    LedInit();
    FYD12864Init();				//主显初始化
    Uart0Init();				//uart0 初始化 用于打印机与读卡器
    KeyBoardInit();				//键盘初始化

    JS28F128FlashInit();		//flash初始化
    I2cInit();					//I2C初始化 用于DS3232 和 外部 EEPROM

    PgADCUinit();				//ADC初始化 气相压力传感器初始化
    PlADCUinit();				//ADC初始化 液相压力传感器初始化
    ValveInit();				//电磁阀初始化

    SELECT_PRINT();				//uart0功能选择 选择为打印机
    WHPrinterInit();			//打印机初始化后
    WHPrinterNormalCmd();		//打印机常用模式初始化
    SELECT_CARD();				//uart0功能选择 选择为读卡器

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
	
	//电磁阀及泵控制信号初始化
    PumpSwitch(StaIdle);
	
    //读取系统参数
    ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(sysparas));
			
    //判断EEPROM读写是否超出限制
    if(sysparas.writeE2num >= EEPROM_W_TIMES)
    {	
    	//系统参数存储块1 读写次数超限，继续读块2
        SYSPARA_ADDR = SYSPARA_ADDR2;
        ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //保证读的最新那一块的系统参数

		
        if(sysparas.writeE2num >= EEPROM_W_TIMES)
        {
        	//系统参数存储块2 读写次数超限，继续读块3
            SYSPARA_ADDR = SYSPARA_ADDR3;
            ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //保证读的最新那一块的系统参数

            if(sysparas.writeE2num >= EEPROM_W_TIMES)
            {
            	//系统参数存储块3 读写次数超限，继续读块4
                SYSPARA_ADDR = SYSPARA_ADDR4;
                ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //保证读的最新那一块的系统参数

				//4个存储系统参数的区域读写都超限，提示更换主板
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
    Uart1Init(sysparas.modbusbaud);   //液相流量计端口初始化 常用19200
    Uart3Init(sysparas.modbusbaud);   //气相流量计端口初始化后 常用19200
    Uart2Init(sysparas.pcbaud);   	  //后台通讯 485 端口初始化 常用38400

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
	
	//显示上次加气流水
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
		//读取键值
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


