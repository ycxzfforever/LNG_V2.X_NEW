#include "kernel.h"

uint8_t SendBack485Buf[BUFSIZE];
uint8_t	SendBack485Count = 0;

uint8_t RecBack485Buf[BUFSIZE];

const uint8_t  CRC8_TAB[] =	 //多项式X^8 + X^5 + X^4 + 1
{
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, //0~7
    0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41, //8~15
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, //16_23
    0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, //24~31
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, //32~39
    0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, //40~47
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, //48~55
    0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff, //56~63
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, //64~71
    0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, //72~79
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, //80~87
    0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a, //88~95
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, //96~103
    0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, //104~111
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, //112~119
    0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9, //120~127
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, //128~135
    0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd, //136~143
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, //144~151
    0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, //152~159
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, //160~167
    0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, //168~175
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, //176~183
    0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, //184~191
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, //192~199
    0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b, //200~207
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, //208~215
    0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, //216~223
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, //224~231
    0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8, //232~239
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, //240~247
    0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35, //248~255
};

uint8_t int_crc8(uint8_t *p, uint16_t m)   /////////////////////CRC8
{

    uint16_t  i;
    uint8_t CRC8 = 0;

    for(i = 0; i < m; i++)
        CRC8 = CRC8_TAB[p[i] ^ CRC8];

    return(CRC8);
}

/************************************************************************
**	函数名称:	BackCommunicationHead(uint8_t cmd)
**	函数功能:	主板与后台通信的帧头
**	输入参数：cmdID：命令ID
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void BackCommunicationHead(uint8_t cmdID)
{
    SendBack485Count = 0;
    SendBack485Buf[SendBack485Count++] = 0xBB;    			//起始符1, BBH
    SendBack485Buf[SendBack485Count++] = 0xEE;    			//起始符2, EEH
    SendBack485Buf[SendBack485Count++] = 0x00;    			//目标设备，
    SendBack485Buf[SendBack485Count++] = cmdID;              //命令ID
    SendBack485Buf[SendBack485Count++] = sysparas.gunnum;    //枪号
}
/************************************************************************
**	函数名称:	void BackCommunicationEnd(uint8_t len)
**	函数功能:	主板与后台通信的帧尾
**	输入参数：len：命令总长度；len = 公共头部分（6字节） + 消息体长度
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void BackCommunicationEnd(uint8_t len)
{
    uint8_t CRC;
    uint8_t crclen = 0;

    SendBack485Buf[5] = len - 6; //len=公共头部分+消息体长度 (公共头部分长度为6减掉以后，就是消息体的长度)
    crclen = len - 2;         //减去帧头的2个字节

    // 因为CRC计算范围 不包括帧头、帧尾和CRC本身 所以 crclen = len-2;

    CRC = int_crc8((uint8_t*)&SendBack485Buf[2], crclen);     //查表计算CRC值
    SendBack485Buf[SendBack485Count++] = CRC;    // CRC 赋值
    SendBack485Buf[SendBack485Count++] = 0xDD;   //截止符1, DDH
    SendBack485Buf[SendBack485Count++] = 0xEE; 	 //截止符2, EEH
}

/************************************************************************
**	函数名称:	SendBackCommunicationData(uint8_t cmd,uint16_t len)
**	函数功能:	向后台发送一条命令
**	输入参数:	cmd:命令ID
**		   	  len:len = 公共头部分（6字节） + 消息体长度
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void SendBackCommunicationData(uint8_t cmd)
{
    BackCommunicationHead(cmd);
	//消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//先赋值为0，后再修改
    BackCommunicationEnd(6);
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	函数名称:	ClearBuf(volatile uint8_t *buf,uint8_t num,uint16_t len)
**	函数功能:	清串口接收数组。
**	输入参数:	buf:接收缓冲区
**		   	  num:往接收缓冲区写入的数据
			  len：接收缓冲区长度
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void ClearBuf(volatile uint8_t *buf, uint8_t num, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        buf[i] = num;
    }
}
/************************************************************************
**	函数名称:	RecBlckDataCheck(void)
**	函数功能:	通过数据长度、帧头、帧尾简单判断从后台接收数据的有效性
			  若数据错误，清空接收数据buffer,即与uart2接收有关的结构体变量
			  本来有CRC校验，但因为后台下午的数据并无CRC，所以保存CRC校验功能
**	输入参数:	无
**	返回值		：true  : 接收的数据有效
			  false : 接收的数据无效

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void RecBlckDataCheck(void)
{
    uint8_t	flag = true;

	//帧头判断
	if((uart2.recbuf[0] != 0xAA)||(uart2.recbuf[1] != 0xFF))
    {
    	flag = false;
    }
	
	//数据长度判断
	if(uart2.reclen >= (uart2.recbuf[5]+9))
	{
		CloseUart2RcvIRQ();
		//枪号判断
		if(uart2.recbuf[4] != sysparas.gunnum)	 
		{
			flag = false;
		}

		else
		{
			globalvar.comfailcount = 0;
	        globalvar.comreceiveflag = 1;
	        memcpy(&RecBack485Buf, &uart2.recbuf[0], uart2.reclen);
		}
	}
    
	if(flag == false)
	{
		OpenUart2RcvIRQ();    //开接收中断
		uart2.reclen = 0;
		uart2.flag = 0;
		memset(uart2.recbuf, 0, uart2.reclen);   //发送数组清零。
	}

}
/************************************************************************
**	函数名称:	  RecBlckDataRplyCheck(void)
**	函数功能:	  数据有效性验证通过，根据接收的命令ID 回复对应的数据
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void RecBlckDataRplyCheck(void)
{
	//数据有效性判断
    RecBlckDataCheck();

	//数据有效
    if(globalvar.comreceiveflag == 1)
    {
        globalvar.comreceiveflag = 0;
		//回复后台命令
        BGCommunication();
    }
}

void UpLoad_Fuel(void)
{
	//将最后一笔流水暂存于fuelrecordinfo_tmp中
	fuelrecordinfo_tmp = fuelrecordinfo;

	//如果本次已传流水号与上次已传流水号相等，表示传的同一条流水 ADD BY LY
    if(globalvar.uptransnum == sysparas.uptransnum)
    {
    	//重复传流水计数器累加
		globalvar.UploadIdCount++;
		
		//重复上传10次，跳过该笔流水
		//上传10次不成功，跳过此笔流水，传输下一笔流水
		if(globalvar.UploadIdCount > 10)   
		{
			globalvar.UploadIdCount = 0;
			sysparas.uptransnum++;
			SaveSignalSysparas(4,1);//保存已传流水号
			return;
		}
    }

	//变更globalvar.uptransnum
	globalvar.uptransnum = sysparas.uptransnum;			

	//判断是否上传流水
	if(sysparas.uptransnum < sysparas.transnum)
	{
		//从加气机读取要上传的这笔流水
		ReadFuelRecord(sysparas.uptransnum, 0);
		//上传流水
		UpLoad_11H();
	}	
	
	//恢复原来的流水数据 ADD BY LY
	fuelrecordinfo = fuelrecordinfo_tmp;

}
/************************************************************************
**	函数名称:	  RecBlckDataRplyCheck(void)
**	函数功能:	  加气机处于空闲状态，接收到51巡检命令以后，回复后台数据
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void ReplyCmd_Poll(void)
{
	//加气机处于空闲状态
    if(globalvar.workstate == StaIdle)
    {
    	//有未上传流水，立即上传
        if(sysparas.uptransnum < sysparas.transnum)
        {
        	UpLoad_Fuel();
        }
		//有未上传班流水，立即上传
        else if(sysparas.shiftuptransnum < sysparas.shiftransnum)
        {
        	//从加气机读取要上传的班流水
            ReadFuelRecord(sysparas.shiftuptransnum, 1);
			//上传班流水
            UpLoad_Shift();
        }
		//有未上传报警流水，立即上传
        else if(sysparas.uperr_id < sysparas.err_id)
        {
        	//该功能暂时未使用
            UpLoad_Error();
        }
		//没有流水可上传，回复51
        else
        {
        	Reply51();
        }
    }
	//加气机处于加气和空闲状态以外的状态时，回复51
    else
    {
        Reply51();
    }
}

/************************************************************************
**	函数名称:	  ReplyCmd_ParaSet(void)
**	函数功能:	  加气机收到52H命令（修改加气机参数）以后，回复后台
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void ReplyCmd_ParaSet(void)
{
    uint32_t index; 	//参数索引值
    uint8_t revbuf[9];
    uint8_t keyb[6];	//BCD
    uint8_t keyb_bak[6];//BCD
    uint8_t i;

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
	
	//取参数索引
    index = BcdToUint(&revbuf[1]);   
	//3002 B密
    if(index == 3002)
    {
        for(i = 0; i < 6; i++)
        {
            keyb[i] = revbuf[i + 3]; //取出下发的密钥

        }
		
		//将后台下发的密钥存到内部eeprom中 ADD BY LY
        EepromInterWriteBytes(LOCKVER_ADDR, keyb, 6);   
    }
	//修改其余参数功能暂时不写
    else
    {

    }
	
	//从内存中读出B密//ADD BY LY
    EepromInterReadBytes(LOCKVER_ADDR, keyb_bak, 6);   

    //帧头
    BackCommunicationHead(COMM_PARASET);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//先赋值为0，后再修改
    //参数索引
    SendBack485Buf[SendBack485Count++] = 0x30;//BCD
    SendBack485Buf[SendBack485Count++] = 0x02;

    //参数值
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = keyb_bak[i];
    }

    //帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	函数名称:	  ReplyCmd_ParaSet(void)
**	函数功能:	  加气机收到53H命令（读取加气机参数）以后，回复后台
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void ReplyCmd_ParaRead(void)
{
    uint32_t index; 	//参数索引值
    uint8_t revbuf[9];
    uint8_t i;
    uint8_t keyb[9];	//BCD

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
	
	//取参数索引
    index = BcdToUint(&revbuf[1]);	
	
	//3002 B密
    if(index == 3002)
    {
    	//从内存中读出B密//ADD BY LY
        EepromInterReadBytes(LOCKVER_ADDR, keyb, 6);   
    }

    //帧头
    BackCommunicationHead(COMM_RDPARA);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//先赋值为0，后再修改
    //参数索引
    SendBack485Buf[SendBack485Count++] = 0x30;//BCD
    SendBack485Buf[SendBack485Count++] = 0x02;

    //参数值
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = keyb[i];
    }

    //帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	函数名称:	  ReplyCmd_UidRead(void)
**	函数功能:	  加气机收到67H命令（读取指定UID流水记录）以后，回复后台
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void ReplyCmd_UidRead(void)
{
    uint8_t  revbuf[9];
    uint32_t id;
    uint32_t voluem;
    uint32_t money;

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
	
	//要取的流水号（交易流水、班流水、报警流水号之一）
    id = BcdbufToHex(&revbuf[3], 6);   
	
	//读加气流水
    if(revbuf[0] == 0x11)   
    {
        voluem = fuelrecordinfo.volume;
        money = fuelrecordinfo.money;
		//从加气机读流水
        ReadFuelRecord(id - 1, 0);
		//上传读取的流水
        UpLoad_11H();
        fuelrecordinfo.volume = voluem;
        fuelrecordinfo.money = money;
    }
	//读班流水
    else if(revbuf[0] == 0x12)   
    {
    	//从加气机读班流水
        ReadFuelRecord(id - 1, 1);
		//上传读取的班流水
        UpLoad_Shift();
    }
	//读报警流水
    else if(revbuf[0] == 0x13)   
    {
        //报警流水功能暂时未写
    }
}


/************************************************************************
**	函数名称:	  ReplyCmd_ResponseOk(void)
**	函数功能:	  加气机收到69H命令（后台成功接收流水记录后返回的接收成功命令）以后，回复后台
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void ReplyCmd_ResponseOk(void)
{
    uint8_t revbuf[9];
    uint32_t id;

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
	
	//接收到的流水号（交易流水、班流水、报警流水之一） ADD BY LY
    id = BcdbufToHex(&revbuf[3], 6);   

	//加气流水接收成功
    if(revbuf[0] == 0x11)   
    {
        fuelrecordinfo.upflag = 0x01;
		
        if(sysparas.uptransnum == id - 1)
        {
        	//更新已传流水号
            sysparas.uptransnum = id;
			
			//保存已传流水号 ADD BY LY
            SaveSignalSysparas(4, 1);
        }
    }
	 //班流水接收成功
    else if(revbuf[0] == 0x12)  
    {
        shiftrecordinfo.upflag = 0x01;

        if(sysparas.shiftuptransnum == id - 1)
        {	
        	//更新已传班流水号
            sysparas.shiftuptransnum = id;
			//保存已传班流水号
            SaveSignalSysparas(39, 1);
        }

    }
	//报警流水接收成功
    else if(revbuf[0] == 0x13)   
    {
        errorRecordinfo.upflag = 0x01;

        if(sysparas.uperr_id == id - 1)
        {	
        	//更新已传报警流水号
            sysparas.uperr_id = id;
			//保存已传报警流水号
            SaveSignalSysparas(84, 1);
        }
    }

    //帧头 帧尾 发送
    SendBackCommunicationData(COMM_RESPONSEOK);
}

/************************************************************************
**	函数名称:	ReplyCmd_Losscard(void)
**	函数功能:	  加气机收到54H命令（挂失卡）以后，回复后台
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void ReplyCmd_Losscard(void)
{
    uint8_t revbuf[9];
    uint32_t cid;

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));

    cid = BcdbufToHex(&revbuf[3], 6);

	//挂失
    if(RecBack485Buf[6] == 0x01)
    {
        SetLossCard(cid, 1);   //挂失
    }
    else
    {
        SetLossCard(cid, 0);	//解挂 但是目前后台好像没有解挂功能
    }

    //帧头
    BackCommunicationHead(COMM_CARDINFO);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//先赋值为0，后再修改
    //挂失/取消
    SendBack485Buf[SendBack485Count++] = RecBack485Buf[6];
    //卡号
    HexToBcdbuf(cid, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;

    //帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}
/************************************************************************
**	函数名称:	ReplyCmd_LossCardsRead(void)
**	函数功能:	  加气机收到59H命令（读全部挂失卡列表）以后，回复后台
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-08-10
*************************************************************************/
void ReplyCmd_LossCardsRead(void)
{
    uint8_t revbuf[6];
    uint32_t cid;

    memcpy(revbuf, &RecBack485Buf[10], sizeof(revbuf));
    cid = BcdbufToHex(revbuf, 6);

    //帧头
    BackCommunicationHead(CMD_UPLDRPT);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//先赋值为0，后再修改

    //挂失/取消
    if(ReadLossCard(cid))
    {
        SendBack485Buf[SendBack485Count++] = 0x01;//挂失卡
    }
    else
    {
        SendBack485Buf[SendBack485Count++] = 0x00;//非挂失卡
    }

    //卡号
    HexToBcdbuf(cid, &SendBack485Buf[SendBack485Count], 6);
    //帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	函数名称:	Reply_ReadVersion(void)
**	函数功能:	  加气机收到A0H命令（远程升级时，用于读取版本号）以后，回复后台
**	输入参数:	无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-10
*************************************************************************/
void Reply_ReadVersion(void)
{
    uint8_t i, len = 0;
	
    //帧头
    BackCommunicationHead(COMM_READVERSION);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//先赋值为0，后再修改
    //版本号
    len = sizeof(VERSION);

    for(i = 0; i < len; i++)
    {
        SendBack485Buf[SendBack485Count++] = VERSION[i];
    }

    //帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}
/************************************************************************
**	函数名称:	Reply_ReadVersion(void)
**	函数功能:	  加气机收到55H命令（后台下发换班命令）以后，回复后台
**	输入参数:	无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void Reply_ExShiftId(void)
{
    uint8_t revbuf[15];
    uint8_t tmpbuf[6];
    uint8_t newshiftid;
	
	//清班流水结构体 ADD BY LY
    memset(&shiftrecordinfo.classid, 0, sizeof(shiftrecordinfo));     
    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
    //新班号
    newshiftid = (uint8_t)BcdToDec(revbuf[0]);
	
	//当前班号与要换的班号不相等 ADD BY LY
    if(sysparas.shiftnum != newshiftid)   
    {
        //设置下一个班的换班时间
        tmpbuf[0] = revbuf[6];//(uint8_t)BcdToDec(revbuf[6]); //yy
        tmpbuf[1] = revbuf[7];//(uint8_t)BcdToDec(revbuf[7]); //mm
        tmpbuf[2] = revbuf[8];//(uint8_t)BcdToDec(revbuf[8]); //dd
        tmpbuf[3] = revbuf[12];//(uint8_t)BcdToDec(revbuf[12]);
        tmpbuf[4] = revbuf[13];//(uint8_t)BcdToDec(revbuf[13]);
        tmpbuf[5] = revbuf[14];//(uint8_t)BcdToDec(revbuf[14]);
        
		//判断下发的日期是否合法 ADD BY LY
        if(!CheckDate(&tmpbuf[0]))     
            return;
		
		//判断下发的时间是否合法 ADD BY LY
        if(!CheckTime(&tmpbuf[3]))     
            return;
		
		//下发的日期时间均合法，设置日历时钟 ADD BY LY
        Ds3232SetTime(tmpbuf); 	   
		
        //上一班的开始时间
        memcpy(&shiftrecordinfo.classstarttime[0], &sysparas.shiftstarttime[0], 6);
		
        //上一班结束时间（就是当前时间）
        Ds3232ReadTime();
        memcpy(&sysparas.shiftendtime[0], &time, 6);
        memcpy(&shiftrecordinfo.classendtime[0], &sysparas.shiftendtime, 6);
		
        //下一班开始时间
        memcpy(&sysparas.shiftstarttime[0], &sysparas.shiftendtime[0], 6);
		
		//下发的班号是否合法，需在（1 - sysparas.shiftmaxnum）之间 ADD BY LY
        if(newshiftid > sysparas.shiftmaxnum  || newshiftid < 1)   
        {
            return;
        }

		globalvar.U8Temp = sysparas.shiftnum;
		
		//换班
        Exshiftid(newshiftid);   

    }

    //班累优先传
    if(sysparas.shiftuptransnum < sysparas.shiftransnum)   
    {
    	//待上传班累记录数据
        UpLoad_Shift();
        //收到后台确认命令以后，在确认命令里面写已传班流水号 ADD BY LY
    }
}
/************************************************************************
**	函数名称:	Reply_ReadVersion(void)
**	函数功能:	换班
**	输入参数: id：新的班号
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void Exshiftid(uint8_t id)
{
    uint8_t *tmp;

    //下班班累记录
    //班流水号
    sysparas.shiftransnum++;//更新班流水号 ADD BY LY
    shiftrecordinfo.classid = sysparas.shiftransnum;
    //班号
    shiftrecordinfo.classnum  = globalvar.U8Temp;  	//解决手动换班时，班号不对  
    //班起始枪累
    shiftrecordinfo.startgunvolume = sysparas.shiftstartvol;
    //班累合计（结束-开始）
    sysparas.shiftvolsum = sysparas.shiftendvol - sysparas.shiftstartvol;
    shiftrecordinfo.classcalgasnum = sysparas.shiftvolsum;
    //班累积合计（流水累加）
    shiftrecordinfo.classgassum = sysparas.shiftotvol;
    //班结束枪累
    sysparas.shiftendvol = sysparas.shiftstartvol + sysparas.shiftotvol;
    shiftrecordinfo.endallgunvolume = sysparas.shiftendvol;
    //班结束金额累积
    shiftrecordinfo.endagunmoney = sysparas.shiftstartmon + sysparas.shiftotmon;
    //金额
    shiftrecordinfo.classgasmoney = sysparas.shiftotmon;
    //现金
    shiftrecordinfo.classrealmoney = sysparas.shiftrealmoney;
    //次数
    shiftrecordinfo.classtimes = sysparas.shiftfueltimes;
    //上传成功标志
    shiftrecordinfo.upflag = 0;
    //读取计量单位
    shiftrecordinfo.measuretype = sysparas.unit;
    //加气站编号
    shiftrecordinfo.stationcode = sysparas.stationcode;
    //换班累记录存储 crc计算
    tmp = (uint8_t *)(&shiftrecordinfo.classid);
    //crc
    shiftrecordinfo.crc  = ModbusCrc16(tmp, sizeof(shiftrecordinfo) - 2);

	//保存班流水
    if(SaveFuelRecord(1))
    {
    	//新班号
    	sysparas.shiftnum = id;
        //新班开始枪累
        sysparas.shiftstartvol  = sysparas.totvolume;
        //新班开始金额
        sysparas.shiftstartmon = sysparas.totmoney;
        //新班累积加气量
        sysparas.shiftotvol = 0;
        //新班累积加金额
        sysparas.shiftotmon = 0;
        //新班累积实收现金
        sysparas.shiftrealmoney = 0;
        //新班累积加气次数
        sysparas.shiftfueltimes = 0;
        //打印班流水
        
		//存系统参数中换班产生的变化参数 ADD BY LY
        if(ExshiftidSaveParas())   
        {
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "换班成功！");
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            PrintShifid();
        }
        else
        {
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "换班失败！");
            FYD12864ClearLine(3);
            FYD12864DispPrintfFlash(4, 1, "存储参数失败！");
        }
    }
    else
    {
        FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(2, 1, "换班失败！");
        FYD12864ClearLine(3);
        FYD12864DispPrintfFlash(4, 1, "存储班流水失败！");
    }
}

/************************************************************************
**	函数名称:	  Reply_ReceiveFail(void)
**	函数功能:	接收命令失败时，回复后台
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void Reply_ReceiveFail(void)
{
    //帧头 帧尾 发送
    SendBackCommunicationData(CMD_RECVERR);
}

/************************************************************************
**	函数名称:	Reply51(void)
**	函数功能:	加气机在非空闲状态下，收到51H巡检命令，回复后台
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void Reply51(void)
{
    //帧头 帧尾 发送
    SendBackCommunicationData(COMM_POLL);
}

/************************************************************************
**	函数名称:	RS485_Test(void)
**	函数功能:	加气机与后台通讯测试
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void RS485_Test(void)
{
    uint8_t  crclen;
    uint8_t  CRC;

    memset(SendBack485Buf, 0, sizeof(SendBack485Buf));     //发送数组清零。
    SendBack485Count = 0;
    SendBack485Buf[SendBack485Count++] = 0xBB;				//起始符1, BBH
    SendBack485Buf[SendBack485Count++] = 0xEE;				//起始符2, EEH
    SendBack485Buf[SendBack485Count++] = 0x00;				//目标设备，
    SendBack485Buf[SendBack485Count++] = 0X61; 			    //命令ID
    SendBack485Buf[SendBack485Count++] = 0X01;				//枪号
    SendBack485Buf[SendBack485Count++] = 5;					//消息体长度

    SendBack485Buf[SendBack485Count++] = 0x01;
    SendBack485Buf[SendBack485Count++] = 0x02;
    SendBack485Buf[SendBack485Count++] = 0x03;
    SendBack485Buf[SendBack485Count++] = 0x04;
    SendBack485Buf[SendBack485Count++] = 0x05;



    SendBack485Buf[5] = 5; //len=公共头部分+消息体长度 (公共头部分长度为6减掉以后，就是消息体的长度)
    crclen = 5 + 4;        //crc计算长度 不含帧头、帧尾、CRC本身

    // 因为CRC计算范围 不包括帧头、帧尾和CRC本身 所以 crclen = len-2;

    CRC = int_crc8((uint8_t*)&SendBack485Buf[2], crclen);     //查表计算CRC值
    SendBack485Buf[SendBack485Count++] = CRC;    // CRC 赋值
    SendBack485Buf[SendBack485Count++] = 0xDD;   //截止符1, DDH
    SendBack485Buf[SendBack485Count++] = 0xEE; 	 //截止符2, EEH

    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	函数名称:	BGCommunication(void)
**	函数功能:	判断后台命令类型，加气机执行不同的命令函数
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void BGCommunication(void)
{
    switch(RecBack485Buf[3])   //根据命令ID判断回复
    {	
    	//巡检命令
        case COMM_POLL:     
			//加气机处于加气状态
            if(globalvar.workstate == StaFuel)
            {
            	//后台已准备好接收实时数据
                if(RecBack485Buf[6] == 1)
                {
                	//加气机回复加气状态：加气中，并上传实时
                    UpLoad_RunState(2);
                }
				//普通51巡检命令
                else
                {
                	//加气机请求后台确认，是否准备好接收实时
                    UpLoad_RunState(1);
                }
            }
            else
            {
            	//加气机处于非加气状态
                ReplyCmd_Poll();
            }

            break;
		//后台设置参数
        case COMM_PARASET:  
            ReplyCmd_ParaSet();
            break;
		//后台读取加气参数
        case COMM_RDPARA:
            ReplyCmd_ParaRead();
            break;
		//上传指定流水（后台从加气机取流水）
        case COMM_RDUIDREC: 
            ReplyCmd_UidRead();
            break;
		//后台收到命令以后，回复加气机OK
        case COMM_RESPONSEOK:
            ReplyCmd_ResponseOk();
            break;
		//新挂失卡
        case COMM_CARDINFO: 
            ReplyCmd_Losscard();
            break;
		//读全部挂失卡列表
        case COMM_RDALLCARD:
            ReplyCmd_LossCardsRead();
            break;
		//后台下发换班命令
        case COMM_EXWORK:   	
            Reply_ExShiftId();
            break;
		//远程升级时，用于读取版本号
        case COMM_READVERSION:   
            Reply_ReadVersion();
            break;
		//如果都不是,就表示接收到的是错误的命令 回传50H
        default:
            Reply_ReceiveFail();
            break;
    }

	//数据使用完毕，清空接收buf，为下一次接收数据做准备
    uart2.reclen = 0; //将缓冲长度清零。
    memset((uint8_t *)&uart2.recbuf[0], 0, sizeof(RecBack485Buf));       //清接收缓冲区
    OpenUart2RcvIRQ();    //开接收中断
}

/************************************************************************
**	函数名称:	UpLoad_11H(void)
**	函数功能:	装载并发送加气流水(11H)
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void UpLoad_11H(void)
{
    uint8_t i;

    //帧头
    BackCommunicationHead(CMD_UPLDREC);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//11H命令长度 先赋值为0，后再修改
    //交易流水号
    HexToBcdbuf(fuelrecordinfo.transnum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //班号
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.classnum);       //HEX转BCD;
    //班流水号
    HexToBcdbuf(fuelrecordinfo.classid, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;

    //加气开始时间
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = fuelrecordinfo.starttime[i];
    }

    //加气结束时间
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = fuelrecordinfo.endtime[i];
    }

    //IC卡卡号
    HexToBcdbuf(fuelrecordinfo.usernum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //IC卡类型
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.cardtype);;
    //加气前余额
    HexToBcdbuf(fuelrecordinfo.beforebalance, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //本次加气气量
    HexToBcdbuf(fuelrecordinfo.volume, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //本次加气单价
    HexToBcdbuf(fuelrecordinfo.price, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //本次加气金额
    HexToBcdbuf(fuelrecordinfo.money, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //加气后余额
    HexToBcdbuf(fuelrecordinfo.afterbalance, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //累积充值次数
    HexToBcdbuf(fuelrecordinfo.allrechargetimes, &SendBack485Buf[SendBack485Count], 3);
    SendBack485Count = SendBack485Count + 3;
    //累积充值金额
    HexToBcdbuf(fuelrecordinfo.allrechargemoney, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //累积消费次数
    HexToBcdbuf(fuelrecordinfo.allconsumptiontimes, &SendBack485Buf[SendBack485Count], 3);
    SendBack485Count = SendBack485Count + 3;
    //累积消费气量
    HexToBcdbuf(fuelrecordinfo.allconsumptionnum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //累积消费金额
    HexToBcdbuf(fuelrecordinfo.allconsumptionmoney, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //初期余额
    HexToBcdbuf(fuelrecordinfo.InitialMoney, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //计算余额
    HexToBcdbuf(fuelrecordinfo.CalMoney, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //车号后三位
    HexToBcdbuf(fuelrecordinfo.carnum_another, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //加气前压力
    HexToBcdbuf(fuelrecordinfo.beforepressure, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //加气后压力
    HexToBcdbuf(fuelrecordinfo.afterpressure, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //加气前枪累
    HexToBcdbuf(fuelrecordinfo.beforegunsum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //加气后枪累
    HexToBcdbuf(fuelrecordinfo.aftergunsum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //灰卡标志
    SendBack485Buf[SendBack485Count++] = fuelrecordinfo.greyflag;//(uint8_t)HexToBcd((uint32_t)fuelrecordinfo.greyflag);
    //打印标志
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.printflag);
    //上传标志
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.upflag);;
    //计量单位
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.measuretype);
    //加气机类型
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.devicetype);
    //加气站编号
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.stationcode);

    //帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	函数名称:	UpLoad_Error(void)
**	函数功能:	装载并发送报警流水 (13H)
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void UpLoad_Error(void)
{
    uint8_t temp;
    uint8_t i;

    //帧头
    BackCommunicationHead(CMD_UPLDERR);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//先赋值为0，后再修改
    //错误流水号
    HexToBcdbuf(errorRecordinfo.errtransactionid, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;

    //报错时间
    for(i = 0; i < 6; i++)
    {
        temp = (uint8_t)HexToBcd((uint32_t)errorRecordinfo.errtime[i]);
        SendBack485Buf[SendBack485Count++] =  temp;
    }

    //班号
    temp = HexToBcd(errorRecordinfo.classnum);
    SendBack485Buf[SendBack485Count++] = temp;
    //出错代码
    temp = HexToBcd(errorRecordinfo.errcode);
    SendBack485Buf[SendBack485Count++] = temp;
    //相关联的卡号
    HexToBcdbuf(errorRecordinfo.errusernum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //上传成功标记
    temp = HexToBcd(errorRecordinfo.upflag);
    SendBack485Buf[SendBack485Count++] = temp;
    //备用2字节
    HexToBcdbuf(errorRecordinfo.data, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	函数名称:	UpLoad_Shift(void)
**	函数功能:	装载并发送班流水 (12H)
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void UpLoad_Shift(void)
{
    uint8_t temp;
    uint8_t i;

    //帧头
    BackCommunicationHead(CMD_UPLDRPT);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0;//先赋值为0，后再修改
    //交易流水号
    HexToBcdbuf(shiftrecordinfo.classid, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //班号
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.classnum);       //HEX转BCD
    SendBack485Buf[SendBack485Count++] = temp;

    //加气开始时间
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = shiftrecordinfo.classstarttime[i];
    }

    //加气结束时间
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = shiftrecordinfo.classendtime[i];
    }

    //班开始枪累
    HexToBcdbuf(shiftrecordinfo.startgunvolume, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //班结束枪累
    HexToBcdbuf(shiftrecordinfo.endallgunvolume, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //班合计气量（班结束枪累-班开始枪累）
    HexToBcdbuf(shiftrecordinfo.classcalgasnum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //班合计气量（按流水记录合计计算）
    HexToBcdbuf(shiftrecordinfo.classgassum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //班合计金额（按流水记录合计计算）
    HexToBcdbuf(shiftrecordinfo.classgasmoney, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //班实收现金（现金+充值）
    HexToBcdbuf(shiftrecordinfo.classrealmoney, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //班累积加气次数
    HexToBcdbuf(shiftrecordinfo.classtimes, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //上传标志
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.upflag);       //HEX转BCD
    SendBack485Buf[SendBack485Count++] = temp;
    //计量单位
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.measuretype);       //HEX转BCD
    SendBack485Buf[SendBack485Count++] = temp;
    //加气机类型
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.devicetype);       //HEX转BCD
    SendBack485Buf[SendBack485Count++] = temp;
    //加气站编号
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.stationcode);       //HEX转BCD
    SendBack485Buf[SendBack485Count++] = temp;
    //帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	函数名称:	UpLoad_Print(void)
**	函数功能:	装载并发送打印流水 (15H)
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void UpLoad_Print(void)
{

}
/************************************************************************
**	函数名称:	UpLoad_RunState(uint8_t state)
**	函数功能:	装载并发送加气机工作状态 (19H)
**	输入参数: state：加气机工作状态 
			  state：0，加气结束；1，加气机请求后台确认是否准备好接收实时数据；2，加气中，加气机发送实时
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-09-10
*************************************************************************/
void UpLoad_RunState(uint8_t state)
{
    //帧头
    BackCommunicationHead(CMD_RUNSTAT);
    //消息体长度
    SendBack485Buf[SendBack485Count++] = 0x00;
    //IC卡卡号
    HexToBcdbuf(fuelrecordinfo.usernum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //IC卡类型
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.cardtype);
    //加气前余额
    HexToBcdbuf(fuelrecordinfo.beforebalance, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //车号后三位
    HexToBcdbuf(fuelrecordinfo.carnum_another, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //加气前压力
    HexToBcdbuf(fuelrecordinfo.beforepressure, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //加气前流速
    SendBack485Buf[SendBack485Count++] = 0;
    SendBack485Buf[SendBack485Count++] = 0;
    //加气前枪累
    HexToBcdbuf(fuelrecordinfo.aftergunsum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;

    //卡状态
    if(m1card.existflag == 1)
        SendBack485Buf[SendBack485Count++] = 1;
    else
        SendBack485Buf[SendBack485Count++] = 0;

    //低压阀开启状态 LNG没有阀的状态
    SendBack485Buf[SendBack485Count++] = 0;
    //中压阀开启状态
    SendBack485Buf[SendBack485Count++] = 0;
    //高压阀开启状态
    SendBack485Buf[SendBack485Count++] = 0;
    //加气状态
    SendBack485Buf[SendBack485Count++] = state;
    //本次加气单价
    HexToBcdbuf(fuelrecordinfo.price, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //本次加气气量
    HexToBcdbuf(fuelrecordinfo.volume, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //本次加气金额
    HexToBcdbuf(fuelrecordinfo.money, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //卡余额
    HexToBcdbuf(fuelrecordinfo.afterbalance, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //计量单位
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.measuretype);
    //加气机类型
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.devicetype);

    //帧头 帧尾
    BackCommunicationEnd(SendBack485Count);
    //发送
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}
