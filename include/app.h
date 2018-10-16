#ifndef __APP_H
#define __APP_H
/*******************  后台发送给加气机 ******************/
#define COMM_POLL		    0x51  //实时周期巡检
#define COMM_PARASET		0x52  //设置参数命令
#define COMM_RDPARA		    0x53  //读取参数命令
#define COMM_CARDINFO		0x54  //发送挂失卡信息
#define COMM_EXWORK		    0x55  //发送换班信息
#define COMM_RDALLCARD		0x59  //读全部挂失卡列表
#define COMM_RDUIDREC		0x67  //读取指定UID加气记录
#define COMM_RESPONSEOK		0x69  //后台 PC 成功接收流水记录后立即返回接收成功确认命令  无消息体
#define COMM_READVERSION	0xA0  //远程升级时，用于读取版本号

/*******************  加气机发送给后台 ******************/
#define CMD_UPLDREC		0x11//加气机 发送加气机流水记录 后台收到后回69H
#define CMD_UPLDRPT		0x12//加气机 发送班统计记录 后台收到后回69H
#define CMD_UPLDERR     0x13//加气机 发送故障及报警记录 后台收到后回69H
#define CMD_UPPRINT		0x15//流水打印
#define CMD_RUNSTAT		0x19//加气机 发送工作状态 后台收到后回51H 后加气机返回加气机工作状态(消息体为空，命令ID为51H)
#define CMD_RECVERR		0x50//加气机接收后台命令失败(CRC校验错或其他错，返回50H，没有消息体)
#define CMD_SETPARA 	0x52//设置时钟、枪累信息

uint8_t int_crc8(uint8_t *p, uint16_t m);

void 	BackCommunicationHead(uint8_t cmdID);
void 	BackCommunicationEnd(uint8_t len);
void 	SendBackCommunicationData(uint8_t cmd);

void 	RecBlckDataCheck(void);
void 	RecBlckDataRplyCheck(void) ;

void 	BGCommunication(void);

void 	ReplyCmd_Poll(void);

void 	ReplyCmd_ParaSet(void);
void 	ReplyCmd_ParaRead(void);
void 	ReplyCmd_UidRead(void);
void 	ReplyCmd_ResponseOk(void);
void 	ReplyCmd_Losscard(void);
void 	ReplyCmd_LossCardsRead(void);
void 	Reply_ExShiftId(void);
void 	Exshiftid(uint8_t id);

void 	Reply_ReceiveFail(void);
void 	Reply51(void);

void 	RS485_Test(void);

void 	UpLoad_RealFuel(void);
void 	UpLoad_11H(void);
void 	UpLoad_Shift(void);
void 	UpLoad_Error(void);
void 	UpLoad_Print(void);
void 	UpLoad_RunState(uint8_t state);
void 	UpLoad_RecvErr(void);
void 	UpLoad_SetPara(void);
#endif
