#ifndef __MENU_H
#define __MENU_H

typedef void(*fun)(void);     //

typedef void(*fun1)(fun);

extern fun         DispUI;          //界面刷新处理
extern fun         DispNextUI;      //用于消息显示后的下一个界面

void InputInit(void);
void DispFirst(void);

void TaskAdd(uint32_t keyvalue, fun funname1, fun funname2);
void TaskMessage(uint32_t ms_100, fun funname);

void SetSuccess(void);   //设置成功显示
void SetFault(void);   //设置失败显示


void DispSet(uint8_t numpoint, uint32_t data, char __farflash*buf);   //设置参数时显示 numpoint:小数点后面位数,0,2,4有效，当大于4时为选择输入

void DispInputPwd(char __farflash* pwdbuf, fun funname);


void DispInputMenuPwd(void);   //输入菜单密码
void DispInputQcPwd(void);   //输入质检密码
void DispInputTestPwd(void);   //输入测试秒
void DispInputResetPwd(void);   //输入恢复出厂设置密码

void DispJudgeMenuPwd(void);   //菜单密码验证
void DispJudgeQcPwd(void);     //Qc密码验证
void DispJudgeTestPwd(void);   //硬件测试密码验证
void DispJudgeResetPwd(void);   //恢复出厂设置密码验证




void DispUnitQcReset(void);
void DispSetUnit(void);
void DispSetQCMode(void);
void DispEraseChip(void);
void DispFactoryReset(void);


void DispSetPrice(void);   //单价设置
void DispSetOthPrice1(void);   //优惠单价设置1
void DispSetOthPrice2(void);   //优惠单价设置2
void DispSetOthPrice3(void);   //优惠单价设置3


void DispMenu_1(void);   //菜单界面1显示



void DispGeneralInfo(void);   //通用信息设置
void DispCommPara(void);   //通讯参数设置
void DispCommonOpt(void);   //常用操作设置
void DispMenu_2(void);   //菜单界面2显示
void DispMenu_3(void);   //菜单界面3显示

void DispVerNum1(void);   //版本编号设置1
void DispVerNum2(void);   //版本编号设置2
void DispOthMsg1(void);   //其他信息设置1
void DispOthMsg2(void);   //其他信息设置2

void DispSetPwdBoard(void);
void SetPwdBoard(void);	

//void DispMenu_3(void);//菜单界面3显示
void DispSysSet1(void);   //系统参数设置1
void DispSysSet2(void);   //系统参数设置2
void DispSysSet3(void);   //系统参数设置3

void DispSetModbusAdr_G(void);
void DispSetModbusAdr_L(void);


void DispSetShiftInfo1(void);   //班组信息设置1
void DispSetShiftInfo2(void);   //班组信息设置2
void DispSetShiftInfo3(void);   //班组信息设置3
void DispSetPressure(void);   //压力设置

void DispSetICInfo1(void);   //IC卡信息设置1
void DispSetICInfo2(void);   //IC卡信息设置2
void DispSetICInfo3(void);   //IC卡信息设置3

void DispSetLossCard(void);

void DispInputOldPwd(void);   //输入旧密码
void DispOK(void);   //确认处理

void DispSetQcPwd(void);   //质检密码设置
void DispSetTestPwd(void);   //维护密码设置
void DispSetonoffworkpwd(void);   //上下班密码设置
void DispSetResetPwd(void);   //恢复出厂密码设置
void DispFactoryReset(void);   //恢复出厂设置
void DispQcPara(void);   //质检参数设置
void SimulatorSet(void);
void OnWork(void);   //上班。
void OffWork(void);   //下班。
void Diswork(void);
void WriteICcard(void);
uint16_t memcmpp(uint8_t *tr1, uint8_t *tr2);



void DispSetLockVer(void);   //密钥信息版本号设置

void DispCommonOpt1(void);   //常用操作设置1。


void DispSetPing(void);   //屏幕选择函数。

//void OnWorkWithPwd( void ); //密码上班
//void OffWorkWithPwd( void ); //密码下班
//void DispJudgeOnWorkPwd( void ); //上班密码验证
//void DispJudgeOffWorkPwd( void ); //下班密码验证

void DispSetOffline(void);   //脱机模式

void SystemReStart(void);   //软件重启

void DispEraseChip(void);   //Flash存储芯片擦除
void DispSetMenuPwd(void);
void CirculateFuelingSet(void);
void SystemReStartToBootLoader(void);
void SystemReStartToAPP(void);


#endif

