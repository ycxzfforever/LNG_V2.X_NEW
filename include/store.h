#ifndef __STORE_H
#define __STORE_H

/*****************************************************************************
** 说明：EEPROM 内存划分
**       1、内部EEPROM存储容量：4K Bytes    (Atmega1280自带) MAXADDR 0x1000
**       2、外部EEPROM存储容量：16K Bytes   (AT24C128) 		     MAXADDR 0x4000
**       3、存储内容：a、系统参数；掉电流水
**	     4、内部、外部EEPROM存储地址分配相同
******************************************************************************/
#define EEPROM_W_TIMES        		90000    //内部EEPROM最大写次数 

#define LOCKVER_ADDR            	0x0010   //B密钥存储地址

#define SYSPARA_ADDR1          		0x0040   //系统参数在内部EEPROM中的起始存储地址 从第2页开始
/*#define SYSPARA_ADDR1_STR    		0x0040   //系统参数在内部EEPROM中的起始存储地址 从第2页开始
**#define SYSPARA_ADDR1_END	   		0x017F   //第2页 到 第6页       系统参数存放空间1 共320字节空间*/

#define SYSPARA_ADDR2	       		0x0180   //第7页 到 第11页 系统参数存放空间2 共320字节空间
/*#define SYSPARA_ADDR2_STR	   		0x0180   //第7页 到 第11页 系统参数存放空间2 共320字节空间
**#define SYSPARA_ADDR2_END	   		0X02BF 	*/

#define SYSPARA_ADDR3	       		0x02C0   //第12页 到 第16页 系统参数存放空间3 共320字节空间
/*#define SYSPARA_ADDR3_STR	   		0x02C0   //第12页 到 第16页 系统参数存放空间3 共320字节空间
**#define SYSPARA_ADDR3_END	   		0x03FF   // */

#define SYSPARA_ADDR4	   	   		0x0400   //第17页 到 第21页 系统参数存放空间4 共320字节空间
/*#define SYSPARA_ADDR4_STR	   		0x0400   //第17页 到 第21页 系统参数存放空间4 共320字节空间
**#define SYSPARA_ADDR4_END	   		0x053F   //除去前面存储4个字节的空间，到0x07FF */

#define REALTIMEDATA_ADDR 	   		0x0540	//保存最后一笔掉电流水 剩余704 字节全部用于保存最后一笔实时流水
/*#define REALTIMEDATA_ADDR_STR 	0x0540	//保存最后一笔掉电流水 剩余704 字节全部用于保存最后一笔实时流水
**#define REALTIMEDATA_ADDR_END     0x07FF    */

#define SYSPARA_MAX_ADDR       	    0x07FF   //内部EEPROM 0-2K 范围用于存储系统参数(系统参数276字节) 剩余2K预留


#define EEPROM_FUELRECORDINFO_STR       0x0800  //外部EEPROM 从3K范围开始，存储40条最近的加气流水 (个人使用 ADD BY LY)
#define EEPROM_TESTADDR					0x3000  //外部EEPROM 测试地址

/***************************************************************************************************
** 说明：挂失卡存储地址 外部EERPOM 从第4K 空间开始存储
**       1、CARD_XX_LEN：表示CARD所占空间的长度，卡的张数 = CARD_XX_LEN * 8
**       2、该分段只使用与外部EEPROM为16的情况
**		 3、设计思想：将EEPROM的存储空间分为几块，每一块存相同类型的CARD，
**		 EEPOM一个字节中的一bit，代表一张卡的挂失情况，bit=0,非挂失卡；bit=1 挂失卡
**		 相同类型的卡的卡号最高字节相同 ADD BY LY
****************************************************************************************************/
#define    CARD_USRER_LEN   1024*5 /*用户卡  类型1*/
#define    CARD_WORKER_LEN  512    /*员工卡  类型2*/
#define    CARD_MEM_LEN     512    /*记账卡  类型3*/
#define    CARD_SERV_LEN    16     /*维修卡  类型4*/
#define    CARD_OWN_LEN     64     /*内部卡  类型5*/
#define    CARD_CAL_LEN     2*1024 /*积分卡  类型6*/
#define    CARD_VIP_LEN     100    /*新增VIP卡类型7 -201610*/

//共占用大约9K的存储空间
enum _CARD_PARAS_ADDR
{
    CARD_USRER_ADDR   = 0x1000,
    CARD_WORKER_ADDR  = CARD_USRER_ADDR + CARD_USRER_LEN,    	//  0x2400
    CARD_MEM_ADDR     = CARD_WORKER_ADDR + CARD_WORKER_LEN,    	//	0x2600
    CARD_SERV_ADDR    = CARD_MEM_ADDR + CARD_MEM_LEN,           //	0x2800
    CARD_OWN_ADDR     = CARD_SERV_ADDR + CARD_SERV_LEN,        	//	0x2810
    CARD_CAL_ADDR     = CARD_OWN_ADDR + CARD_OWN_LEN, 			//	0x2850
    CARD_VIP_ADDR     = CARD_CAL_ADDR + CARD_CAL_LEN,  			//	0x3010增加VIP卡201610
};

#define    LOCK_ADDR        0x3500								//加密板数据存储地址 ADD BY LY
/***********************************************************************************************/


/**********************************************************************************************************************
** 说明：外部FLASH E28F128FLASH 内存分配
**       1、存储容量：16M Bytes(128K/block 共128 blocks)
**       2、存储内容：a、挂失卡卡号；b、流水(前面40 block 用于存流水);c、班累记录
**
** 备注：1、LNG 只有黑名单，没有新增/删 黑白名单
**********************************************************************************************************************/
/*****************流水记录 20480条 (0-39 block)  ********************/
#define  FlashRecSADDR        0x000000
#define  FlashRecEADDR        0x4FFFFF
#define  FlashRecLen          256
#define  FlashRecMaxID        20000 //128K/256=512(每个block存512条流水),512*40(block)=20480 需要40个blocks 可存20480条，实存20000条

/*****************  E28F128FLASH 相关参数  ************************************************/
#define   MAX_ADDR            0x0FFFFFF   // 芯片内部最大地址        
#define   MAX_BLOCK_NUM       127         // 最大块号                                     

/*****************参数修改记录  8192条  (40 - 47 block)    LNG2.7没有参数记录********************/
#define  FlashModSADDR  	  0x500000
#define  FlashModEAddr  	  0x5FFFFF
#define  FlashModLen    	  128
#define  FlashModMaxID		  8000//128K/128 =1024，1024*8=8192 需要8个blocks 可存8192条，实存8000条


/*****************上下班记录  3072条（48 - 50 block) ********************/
#define  FlashShiftSADDR       0x600000
#define  FlashShiftEADDR       0x66FFFF
#define  FlashShiftLen         128
#define  FlashShiftMaxID       3000//128K/128=1024, 1024*3=3072 需要3个blocks，可存3072条，实存3000条


/**************黑名单 32768 （block 51-52  block) *****************/
#define  FlashBlkSADDR       0x670000
#define  FlashBlkEADDR       0x6AFFFF
#define  FlashBlkLen         8  		//存储的是挂失卡卡号 存储时需要考虑如何组织存储、查找等问题
#define  FlashBlkMaxID       32768 		//128K/8=16384 16384*2=32768 需要2个 blocks
/**********************************************************************/



/***************************以上为2.7原来的内存分配**********************************/
//班记录信息 54字节
typedef struct ShiftRecordInfo
{
    uint32_t  classid;                //上下班记录流水号 班流水
    uint8_t   classnum;               //班号，1字节。
    uint8_t   classstarttime[6];      //班开始时间，6字节，BCD码。
    uint8_t   classendtime[6];        //班结束时间，6字节，BCD码。
    uint32_t  startgunvolume;		  //班开始气量枪累
    uint32_t  endallgunvolume;		  //班结束气量枪累
    uint32_t  startgunmoney;		  //班开始气量枪累
    uint32_t  endagunmoney;		  	  //班结束气量枪累
    uint32_t  classcalgasnum;         //班合计气量(班结束气量-班开始气量)
    uint32_t  classgassum;		  	  //合计气量 根据流水记录计算
    uint32_t  classgasmoney;          //班加气金额 根据流水记录计算
    uint32_t  classrealmoney;		  //班实收现金(现金卡+充值)
    uint32_t  classtimes;             //班加气次数，4字节。
    uint8_t   upflag;             	  //上传成功标志
    uint8_t   measuretype;            //计量单位，1字节，1-标方，0-Kg。
    uint8_t   devicetype;             //设备类型0： LNG 加气机1： CNG 加气机2： 加气柱3： 卸气柱
    uint32_t  stationcode;            //加气站编号
    uint16_t  crc;
} ShiftRecordInfo;

//系统错误信息 19字节
typedef struct ErrorRecordInfo
{
    uint32_t  errtransactionid;     //6 BCD 错误流水号
    uint8_t   errtime[6];              //6 BCD 报错时间
    uint8_t   classnum     ;           //1 BCD 班号 (取值1,2,3,4)
    uint8_t   errcode;                 //1 BCD 出错代码
    uint32_t  errusernum;              //4 BCD 相关联的卡号
    uint8_t   upflag;             //1 BCD 上传标志 (0:没有上传;1:已经上传)
    uint16_t  data;                    //2 BCD 预留
} ErrorRecordInfo;


//加气流水记录信息 137字节
typedef struct FuelRecordInfo
{
    uint32_t  transnum;               //交易流水号，4字节。
    uint8_t   classnum;               //班号，1字节。
    uint32_t  classid;                //班流水号。
    uint8_t   starttime[6];           //加气开始时间，6字节，BCD码。
    uint8_t   endtime[6];             //加气结束时间，6字节，BCD码。
    uint32_t  usernum;			  	  //用户卡号
    uint8_t   cardtype;               //卡类型 ， 1字节
    uint32_t  beforebalance;          //卡加气前余额，4字节。
    uint32_t  volume;                 //当前加气气量，4字节。
    uint16_t  price;                  //本次加气单价,2字节。
    uint32_t  money;                  //当前加气金额，4字节。
    uint32_t  afterbalance;           //卡加气后余额，4字节。
    uint32_t  allrechargetimes;       //卡累计充值次数，4字节。
    uint32_t  allrechargemoney;       //卡累计充值金额，4字节。
    uint32_t  allconsumptiontimes;    //卡累计消费次数，4字节。
    uint32_t  allconsumptionnum;      //卡累计消费气量，4字节。
    uint32_t  allconsumptionmoney;    //卡累计消费金额，4字节。
    uint32_t  InitialMoney;			  //卡期初余额
    uint32_t  CalMoney;				  //卡计算余额
    uint32_t  carnum_another;		  //车牌号后四位 HEX
    uint16_t  beforepressure;         //加气前压力，2字节。
    uint16_t  afterpressure;          //加气后压力，2字节。
    uint32_t  beforegunsum;		  	  //加气前气量枪累
    uint32_t  aftergunsum;		  	  //加气后气量枪累
    uint8_t   greyflag;               //是否灰卡 0：否；1：是
    uint8_t	  printflag;			  //是否打印     0：否；1：是
    uint8_t   upflag;				  //上传成功标志 0：否；1：是
    uint8_t   measuretype;            //计量单位，1字节，1-标方，0-Kg。
    uint8_t   devicetype;             //设备类型0： LNG 加气机1： CNG 加气机2： 加气柱3： 卸气柱
    uint8_t   stationcode;			  //加气中编号
    uint32_t  allgunnum;              //枪气量累计，4字节。
    uint32_t  allgunmoney;            //枪金额累计,4字节。
    uint8_t   stopreason;             //停机原因
    uint32_t  backgas;                //回气量
    uint32_t  density;                //密度，4字节。
    uint32_t  curnum;                 //当量，4字节。修正系数。
    uint8_t   cardstate;			  //卡状态 1：插入；0：拔出
    uint8_t   fuelmode;               //加气方式 手动

    uint16_t  crc;
} FuelRecordInfo;

//加气机参数 267字节 + 33字节备份,共300字节
typedef struct def_SysParas
{
    uint8_t     head;       		//0 头
    uint32_t    writeE2num;		 	//1写eeprom次数
    uint8_t     gunnum;     		//2枪号
    uint32_t    transnum;   		//3交易流水号(枪流水号)
    uint32_t    uptransnum; 		//4已传交易流水号
    uint32_t    totvolume;  		//5枪总气量    2位小数
    uint32_t    totmoney;   		//6枪总金额    2位小数
    uint16_t    price[20];  		//7-26 20个单价，单价0为默认单价，根据单价等级选择相应单价
    uint8_t     modbustype; 		//27流量计类型，1-罗斯蒙特，2-E+H
    uint8_t     unit;       		//28计量单位 1-kg,2-标方
    uint8_t     qcmode;     		//29质检模式设置
    uint32_t    densty;             //30密度              4位小数//以下4个参数这里有16位修改为32位查找表需要改变
    uint32_t    coeff;              //31修正系数  4位小数
    uint32_t    pzero;              //32压力零点
    uint32_t    pcoeff;             //33压力系数  2位小数
    uint32_t    modnum;     		//34修改记录流水号
    uint32_t    upmodnum;   		//35已传修改记录流水号

    //班组信息
    uint8_t     shiftmaxnum;        //36最大班号
    uint8_t     shiftnum;           //37班号
    uint32_t    shiftransnum;       //38班流水号
    uint32_t    shiftuptransnum;    //39已传班流水号
    uint8_t     shiftstarttime[6];  //40班开始时间   BCD
    uint8_t     shiftendtime[6];    //41班结束时间    BCD
    uint32_t    shiftstartvol;      //42班开始枪累    2位小数
    uint32_t    shiftendvol;        //43班结束枪累    2位小数
    uint32_t	shiftstartmon;		//44班开始金额
    uint32_t	shiftvolsum;		//45班合计加气量 = 枪累结束 - 枪累开始
    uint32_t    shiftotvol;         //46班合计气量    2位小数（根据流水计算出来）
    uint32_t    shiftotmon;         //47班合计金额    2位小数（根据流水计算出来）
    uint32_t	shiftrealmoney;		//48实收现金
    uint32_t    shiftfueltimes;     //49班加气次数

    //通用信息
    uint8_t     shengcode[2];       //50省编码
    uint8_t     stationcodearray[4];//51站编码
    uint32_t    stationcode;        //52站编码
    uint8_t     citycode[2];        //53地市代码
    uint8_t     parentcode[4];      //54上级单位代码

    //卡信息
    uint32_t    cardmaxyue;         //55储蓄卡余额上限    2位小数
    uint32_t    cardminyue;         //56储蓄卡卡余额下限    2位小数
    uint32_t	accountcardmaxyue;	//57记账卡余额上限    2位小数
    uint8_t     cardpsamnum;        //58psam卡卡座选择
    uint16_t    cardmaxday;         //59卡未使用最大间隔天数
    uint8_t     cardcheck;          //60是否检验钢瓶 0：不检验钢瓶；1：检验钢瓶
    uint8_t     usecard;            //61是否用卡    	0-可不使用卡，1-用卡，2-无卡

    //波特率设置
    uint32_t    modbusbaud;         //62流量计波特率
    uint32_t    pcbaud;             //63后台波特率
    uint8_t     fuelmode;           //64加气模式 		 0：手动；1：后台；2：电子标签
    uint8_t     printmode;          //65小票打印方式 0：不自动打印；1：自动打印

    //版本编号设置
    uint8_t     pricever;           //66单价版本号
    //uint8_t     blackver;         //67黑名单版本号

    /* LNG2.7 只有黑名单 其他的都没有 ADD BY LY*/
    uint8_t     comminfover;        //68通用信息版本号
    uint8_t     lockver;            //69密钥版本号。

    //密码设置
    uint32_t    menupwd;            //70菜单进入密码
    uint32_t    qcpwd;              //71质检密码
    uint32_t    testpwd;            //72测试密码，如按键、电磁阀等测试
    uint32_t    resetpwd;           //73恢复出厂密码
    uint8_t     pwdboard[6];        //74加密板数据
    uint8_t     usepwdboard;        //75是否使用加密板 0：不使用加密板，1：使用加密板
    uint8_t		offlinemode;		//76脱机模式 1：脱机 0：在线
    uint8_t     lcdtype;            //77屏幕选择，0蓝屏，1大屏
    uint8_t     simulation;         //78加气模式    1:模拟加气；0：正常加气。
    uint8_t     intnumflag;         //79是否取整    0:加气时不能取整 1:可以取整
    uint32_t    maxvolumeonce;      //80单次最大加气量
    uint32_t    maxmoneyonce;       //81单次最大加气金额
    uint8_t     devicetype;         //82设备类型0： LNG 加气机1： CNG 加气机2： 加气柱3： 卸气柱
    uint32_t	err_id;				//83异常记录流水号
    uint32_t 	uperr_id;			//84未上传异常记录流水号
    uint8_t		IsPrintCarnum;		//85是否打印车牌号 1：打印 0：不打印
	uint8_t		backup1[20];		//备用，防止增加参数后EEPROM错误
	
    //LNG控制参数
    uint16_t    backgascalc;        //201是否回气计量，0-不计量，1-计量
    uint16_t    precoolmode;        //202预冷方式，0-小循环，1-大循环
    uint16_t    precooltemp;        //203预冷临界温度 ，负数
    uint16_t    precoolgain;        //204预冷临界增益
    uint16_t    precooldensty;      //205预冷临界密度，4位小数，0-1.0000
    uint16_t    precooltimeout;     //206预冷超时时间
    uint16_t    delaystoptime;      //207刚开始加液时,不判断停机条件的延时时间
    uint16_t    lowratestop;        //208低流速停机
    uint16_t    highratestop;       //209高流速停机
    uint16_t    backdenstystop;     //210回气密度停机，4位小数，0-1.0000
    uint16_t    backgainstop;       //211回气增益停机
    uint16_t    pressurestop;       //212停机压力
    uint16_t    stoppumptime;       //213加液后延时停泵
    uint16_t    precoolfinishedtime;//214重新预冷延时
    uint8_t		modbusaddr_g;		//215气相流量计地址
    uint8_t		modbusaddr_l;		//216液相流量计地址
    uint8_t		modbusRS485;		//217 1、气液相RS485线合并为1,2、气液相RS485分
    uint8_t		printtimes;			//218 加气结束 小票打印次数设置 ADD BY LY
	uint8_t		backup2[12];		//备用，防止增加参数后EEPROM错误（没增加N个参数，backup2就要减少N个备份，避免出现EEPROM错误）
	
    uint16_t     crc;               //16位数据校验

} Sysparas;

//全局变量 82字节
typedef struct GlobalVar
{
    uint8_t         U8Temp;         //临时存储u8整型值
    uint8_t         U16Temp;         //临时存储u16整型值
    uint32_t        U32Temp;        //临时存储u32整型值

    uint32_t        KeyValue;       //按键值,全局变量
    uint32_t        KeyPreValue;    //先前按键值

    uint8_t         MsgTime;        //用于显示某些简短的消息
    uint8_t         MsgFlag;        //等于1时，msg未处理
    uint8_t         displayonce;    //等于1时，界面只显示一次或只执行一次

    uint8_t         circulatefue;   //循环加气设置。1-循环，0-停止。
    uint8_t         fuelstep;       //加气步骤。
    uint32_t        fixnum;         //定气量。
    uint32_t        fixmoney;       //定金额。
    uint8_t         fix;            //定量标志，1-定气量，2-定金额。

    uint8_t         workstate;      //加气机工作状态
    uint8_t         BeepCount;      //蜂鸣器响的时间 100ms为单位
    uint8_t         eepromflag;     //0-不需要更换，1-需要更换

    uint16_t        paraindex;      //参数索引号。
    uint32_t        xgtransnum;     //修改流水号变量，用于查询。
    uint32_t        uptransnum;     //上传流水时的已传流水号
    uint8_t         UploadIdCount;  //上传流水失败次数。
    uint8_t         comreceiveflag; //后台来命令标志。
    uint8_t         comfailcount;   //后台与主板通信是否正常标志，如果主板10s未收到后台信息，则表示已掉线。

    uint8_t         modbus_g_error; //加液时，气相流量计出错次数
    uint8_t         modbus_l_error; //加液时，液相流量计出错次数
    uint8_t         pressure_error; //压力传感器错误次数

    uint32_t        beforedex;      //参数修改前的值。
    uint32_t        afterdex;       //参数修改后的值。
    uint8_t         pointnum;       //参数小数点个数。

    uint32_t        timeout;        //超时时间
    uint16_t        stoptimes;      //停机条件满足次数
    uint16_t        stoppumptime;   //加液后停泵延时
    uint8_t         otherstate;     //一泵双机时，对方加液机状态0-对方加液，1-对方待机
    uint8_t         devicetest;     //等于1时在进行电磁阀测试
    uint8_t         precoolfinished;//预冷已经完成，加液时直接加液
    uint8_t         finishedtimeout;//预冷完成后超时时间

    float           starttotalizeG; //气象流量计开始总累
    float           starttotalizeL; //液象流量计开始总累
    float           pretotalG;      //气象流量计上一次总累，用于判断瞬时数据
    float           pretotalL;      //液象流量计上一次总累，用于判断瞬时数据
} GlobalVar;


// M1 卡结构  172字节
typedef	struct	M1carddata
{
    uint8_t		serialnumber[4];        //厂商序列号
    uint32_t    key;                    //用户设的卡密码
    uint8_t     keyB[6];
    uint8_t     existflag;              //是否有卡存在 1：卡插入；0：卡拔出
    uint8_t     checkflag;              //验证卡标志。 1：验证通过；0：验证失败；
    uint8_t		checkresult;            //卡验证结果；0xFF:等待验证；0：验证通过；其他：错误代码 ADD BY LY
    uint16_t    debittimes;             //申请扣款次数。

    //块1.
    uint8_t		 backUp0[2];			//1
    uint8_t		 cardnum[8];			//卡号
    uint8_t		 projectNo[2];			//项目公司标号
    uint8_t		 backUp1[4];
    //块2.
    uint8_t	 	 card[2];				//01金额卡、02气量卡
    uint8_t	 	 cardtype[2];			//卡类型(0-9)
    uint8_t 	 pricelevel[2];			//单价类型(0-9)
    uint8_t 	 greyFlag[2];			//灰卡标志(1:正常卡；2：灰卡)
    uint8_t 	 lossFlag[2];			//挂失卡标志（0：正常卡；1：挂失）
    uint8_t 	 backUp2[6];			//备用
    //块4.
    uint8_t 	 balancecrc[2];			//卡余额效验
    uint8_t 	 cardbalance[8];		//卡余额
    uint8_t		 lasttradetime[6];		//最后使用时间
    //块5.
    uint8_t		 lastBalanceCrc[2];		//上次卡余额效验
    uint8_t		 lastBalance[8];		//上次卡余额
    uint8_t		 lastUseDate[6];		//上次使用时间
    //块6.
    uint8_t		 payMoneyCrc[2];		//加气金额校验
    uint8_t		 payMoney[8];			//最后加气或充值金额
    uint8_t		 operateType[2];		//操作模式 0：没有操作；1：加气；2：充值；3：扣款；4：转存；5：兑换；6：解灰
    uint8_t		 backUp3[4];
    //块9.
    uint8_t		 block9[16];			//备份
    //块10.
    uint8_t		 block10[16];			//备份

    //块13.
    uint8_t		 cylindersdate[6];		//钢瓶到检日期
    uint8_t		 carnum[4];				//车牌号后四位或密码
    uint8_t		 backUp4[6];			//预留
    //块14.
    uint8_t		 backUp5[6];			//14
    uint8_t		 gunnum;				//最后一次加气枪号 HEX
    uint8_t		 transnum[4];			//最后一次加气流水号 HEX
    uint8_t		 laststationcode;		//最后加气加气站编号
    uint8_t		 backUp6[4];
    //块16.
    uint8_t block16[16];			//16
    //块17.
    uint8_t block17[16];			//17
    //块18.
    uint8_t block18[16];//18

    /******* 以上各块定义是依照卡协议编写，最好不再做出修改 ADD BY LY ********/

    uint8_t		cardtype_another;		   //卡类型
    uint32_t 	carnum_another;			   //车牌号
    uint32_t	cardnum_another;		   //卡号(区别于卡序列号，卡序列号是卡出厂时厂家设定的)ADD BY LY
    uint16_t 	pricelevel_another;		   //单价登记
    uint32_t	currentprice;			   //此卡当前单价
    uint8_t 	createcardtime[7];		   //发卡时间YYYYMMDDHHMMSS
    uint8_t 	lastoptmode;			   //最后一次操作方式
    uint8_t	    stationcode_another;	   //最后一次加气站编码
    uint32_t	transnum_another;		   //交易流水号

    uint32_t	trademoney; 			   //最后加气金额 2位小数
    uint32_t	cardbalance_another;	   //卡余额  2位小数
    uint32_t	sumtimes;				   //累计消费次数
    uint32_t	summoney;				   //累计消费金额  2位小数
    uint32_t	sumvolme;				   //累计消费气量  2位小数
    uint32_t	sumchargetimes; 		  //累计充值次数
    uint32_t	sumchargemoney; 		  //累计充值金额
    uint32_t	cardinitmoney;			  //卡期初金额
    uint32_t	cardcalmoney;			  //卡计算余额

    uint8_t 	newkeyb[6];				  //设置B密
} M1carddata;

extern M1carddata       m1card;				//卡结构数据。
extern uint32_t         syserr;				//系统异常
extern uint32_t         stopreason;			//停机原因
extern uint32_t		 	carderror;			//与IC卡操作有关的错误
extern GlobalVar        globalvar;			//全局变量
extern ShiftRecordInfo  shiftrecordinfo;	//上下班记录
extern FuelRecordInfo   fuelrecordinfo;    	//流水记录信息
extern FuelRecordInfo   fuelrecordinfo_tmp;    	//流水记录信息
extern ErrorRecordInfo  errorRecordinfo;	//错误流水
extern Sysparas         sysparas;			//系统参数
extern Sysparas         tmp_sysparas;		//用户修改系统参数时，临时存储需要修改的参数

#endif

bool 	SetLossCard(uint32_t cid, bool bset);
uint8_t ReadLossCard(uint32_t cid);
void 	ClearLossCard(void);
uint8_t WriteLossCardFlag(void);

