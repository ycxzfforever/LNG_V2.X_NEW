#include "kernel.h"

uint8_t SendBack485Buf[BUFSIZE];
uint8_t	SendBack485Count = 0;

uint8_t RecBack485Buf[BUFSIZE];

const uint8_t  CRC8_TAB[] =	 //����ʽX^8 + X^5 + X^4 + 1
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
**	��������:	BackCommunicationHead(uint8_t cmd)
**	��������:	�������̨ͨ�ŵ�֡ͷ
**	���������cmdID������ID
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void BackCommunicationHead(uint8_t cmdID)
{
    SendBack485Count = 0;
    SendBack485Buf[SendBack485Count++] = 0xBB;    			//��ʼ��1, BBH
    SendBack485Buf[SendBack485Count++] = 0xEE;    			//��ʼ��2, EEH
    SendBack485Buf[SendBack485Count++] = 0x00;    			//Ŀ���豸��
    SendBack485Buf[SendBack485Count++] = cmdID;              //����ID
    SendBack485Buf[SendBack485Count++] = sysparas.gunnum;    //ǹ��
}
/************************************************************************
**	��������:	void BackCommunicationEnd(uint8_t len)
**	��������:	�������̨ͨ�ŵ�֡β
**	���������len�������ܳ��ȣ�len = ����ͷ���֣�6�ֽڣ� + ��Ϣ�峤��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void BackCommunicationEnd(uint8_t len)
{
    uint8_t CRC;
    uint8_t crclen = 0;

    SendBack485Buf[5] = len - 6; //len=����ͷ����+��Ϣ�峤�� (����ͷ���ֳ���Ϊ6�����Ժ󣬾�����Ϣ��ĳ���)
    crclen = len - 2;         //��ȥ֡ͷ��2���ֽ�

    // ��ΪCRC���㷶Χ ������֡ͷ��֡β��CRC���� ���� crclen = len-2;

    CRC = int_crc8((uint8_t*)&SendBack485Buf[2], crclen);     //������CRCֵ
    SendBack485Buf[SendBack485Count++] = CRC;    // CRC ��ֵ
    SendBack485Buf[SendBack485Count++] = 0xDD;   //��ֹ��1, DDH
    SendBack485Buf[SendBack485Count++] = 0xEE; 	 //��ֹ��2, EEH
}

/************************************************************************
**	��������:	SendBackCommunicationData(uint8_t cmd,uint16_t len)
**	��������:	���̨����һ������
**	�������:	cmd:����ID
**		   	  len:len = ����ͷ���֣�6�ֽڣ� + ��Ϣ�峤��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void SendBackCommunicationData(uint8_t cmd)
{
    BackCommunicationHead(cmd);
	//��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//�ȸ�ֵΪ0�������޸�
    BackCommunicationEnd(6);
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	��������:	ClearBuf(volatile uint8_t *buf,uint8_t num,uint16_t len)
**	��������:	�崮�ڽ������顣
**	�������:	buf:���ջ�����
**		   	  num:�����ջ�����д�������
			  len�����ջ���������
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
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
**	��������:	RecBlckDataCheck(void)
**	��������:	ͨ�����ݳ��ȡ�֡ͷ��֡β���жϴӺ�̨�������ݵ���Ч��
			  �����ݴ�����ս�������buffer,����uart2�����йصĽṹ�����
			  ������CRCУ�飬����Ϊ��̨��������ݲ���CRC�����Ա���CRCУ�鹦��
**	�������:	��
**	����ֵ		��true  : ���յ�������Ч
			  false : ���յ�������Ч

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void RecBlckDataCheck(void)
{
    uint8_t	flag = true;

	//֡ͷ�ж�
	if((uart2.recbuf[0] != 0xAA)||(uart2.recbuf[1] != 0xFF))
    {
    	flag = false;
    }
	
	//���ݳ����ж�
	if(uart2.reclen >= (uart2.recbuf[5]+9))
	{
		CloseUart2RcvIRQ();
		//ǹ���ж�
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
		OpenUart2RcvIRQ();    //�������ж�
		uart2.reclen = 0;
		uart2.flag = 0;
		memset(uart2.recbuf, 0, uart2.reclen);   //�����������㡣
	}

}
/************************************************************************
**	��������:	  RecBlckDataRplyCheck(void)
**	��������:	  ������Ч����֤ͨ�������ݽ��յ�����ID �ظ���Ӧ������
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void RecBlckDataRplyCheck(void)
{
	//������Ч���ж�
    RecBlckDataCheck();

	//������Ч
    if(globalvar.comreceiveflag == 1)
    {
        globalvar.comreceiveflag = 0;
		//�ظ���̨����
        BGCommunication();
    }
}

void UpLoad_Fuel(void)
{
	//�����һ����ˮ�ݴ���fuelrecordinfo_tmp��
	fuelrecordinfo_tmp = fuelrecordinfo;

	//��������Ѵ���ˮ�����ϴ��Ѵ���ˮ����ȣ���ʾ����ͬһ����ˮ ADD BY LY
    if(globalvar.uptransnum == sysparas.uptransnum)
    {
    	//�ظ�����ˮ�������ۼ�
		globalvar.UploadIdCount++;
		
		//�ظ��ϴ�10�Σ������ñ���ˮ
		//�ϴ�10�β��ɹ��������˱���ˮ��������һ����ˮ
		if(globalvar.UploadIdCount > 10)   
		{
			globalvar.UploadIdCount = 0;
			sysparas.uptransnum++;
			SaveSignalSysparas(4,1);//�����Ѵ���ˮ��
			return;
		}
    }

	//���globalvar.uptransnum
	globalvar.uptransnum = sysparas.uptransnum;			

	//�ж��Ƿ��ϴ���ˮ
	if(sysparas.uptransnum < sysparas.transnum)
	{
		//�Ӽ�������ȡҪ�ϴ��������ˮ
		ReadFuelRecord(sysparas.uptransnum, 0);
		//�ϴ���ˮ
		UpLoad_11H();
	}	
	
	//�ָ�ԭ������ˮ���� ADD BY LY
	fuelrecordinfo = fuelrecordinfo_tmp;

}
/************************************************************************
**	��������:	  RecBlckDataRplyCheck(void)
**	��������:	  ���������ڿ���״̬�����յ�51Ѳ�������Ժ󣬻ظ���̨����
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void ReplyCmd_Poll(void)
{
	//���������ڿ���״̬
    if(globalvar.workstate == StaIdle)
    {
    	//��δ�ϴ���ˮ�������ϴ�
        if(sysparas.uptransnum < sysparas.transnum)
        {
        	UpLoad_Fuel();
        }
		//��δ�ϴ�����ˮ�������ϴ�
        else if(sysparas.shiftuptransnum < sysparas.shiftransnum)
        {
        	//�Ӽ�������ȡҪ�ϴ��İ���ˮ
            ReadFuelRecord(sysparas.shiftuptransnum, 1);
			//�ϴ�����ˮ
            UpLoad_Shift();
        }
		//��δ�ϴ�������ˮ�������ϴ�
        else if(sysparas.uperr_id < sysparas.err_id)
        {
        	//�ù�����ʱδʹ��
            UpLoad_Error();
        }
		//û����ˮ���ϴ����ظ�51
        else
        {
        	Reply51();
        }
    }
	//���������ڼ����Ϳ���״̬�����״̬ʱ���ظ�51
    else
    {
        Reply51();
    }
}

/************************************************************************
**	��������:	  ReplyCmd_ParaSet(void)
**	��������:	  �������յ�52H����޸ļ������������Ժ󣬻ظ���̨
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void ReplyCmd_ParaSet(void)
{
    uint32_t index; 	//��������ֵ
    uint8_t revbuf[9];
    uint8_t keyb[6];	//BCD
    uint8_t keyb_bak[6];//BCD
    uint8_t i;

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
	
	//ȡ��������
    index = BcdToUint(&revbuf[1]);   
	//3002 B��
    if(index == 3002)
    {
        for(i = 0; i < 6; i++)
        {
            keyb[i] = revbuf[i + 3]; //ȡ���·�����Կ

        }
		
		//����̨�·�����Կ�浽�ڲ�eeprom�� ADD BY LY
        EepromInterWriteBytes(LOCKVER_ADDR, keyb, 6);   
    }
	//�޸��������������ʱ��д
    else
    {

    }
	
	//���ڴ��ж���B��//ADD BY LY
    EepromInterReadBytes(LOCKVER_ADDR, keyb_bak, 6);   

    //֡ͷ
    BackCommunicationHead(COMM_PARASET);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//�ȸ�ֵΪ0�������޸�
    //��������
    SendBack485Buf[SendBack485Count++] = 0x30;//BCD
    SendBack485Buf[SendBack485Count++] = 0x02;

    //����ֵ
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = keyb_bak[i];
    }

    //֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	��������:	  ReplyCmd_ParaSet(void)
**	��������:	  �������յ�53H�����ȡ�������������Ժ󣬻ظ���̨
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void ReplyCmd_ParaRead(void)
{
    uint32_t index; 	//��������ֵ
    uint8_t revbuf[9];
    uint8_t i;
    uint8_t keyb[9];	//BCD

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
	
	//ȡ��������
    index = BcdToUint(&revbuf[1]);	
	
	//3002 B��
    if(index == 3002)
    {
    	//���ڴ��ж���B��//ADD BY LY
        EepromInterReadBytes(LOCKVER_ADDR, keyb, 6);   
    }

    //֡ͷ
    BackCommunicationHead(COMM_RDPARA);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//�ȸ�ֵΪ0�������޸�
    //��������
    SendBack485Buf[SendBack485Count++] = 0x30;//BCD
    SendBack485Buf[SendBack485Count++] = 0x02;

    //����ֵ
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = keyb[i];
    }

    //֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	��������:	  ReplyCmd_UidRead(void)
**	��������:	  �������յ�67H�����ȡָ��UID��ˮ��¼���Ժ󣬻ظ���̨
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void ReplyCmd_UidRead(void)
{
    uint8_t  revbuf[9];
    uint32_t id;
    uint32_t voluem;
    uint32_t money;

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
	
	//Ҫȡ����ˮ�ţ�������ˮ������ˮ��������ˮ��֮һ��
    id = BcdbufToHex(&revbuf[3], 6);   
	
	//��������ˮ
    if(revbuf[0] == 0x11)   
    {
        voluem = fuelrecordinfo.volume;
        money = fuelrecordinfo.money;
		//�Ӽ���������ˮ
        ReadFuelRecord(id - 1, 0);
		//�ϴ���ȡ����ˮ
        UpLoad_11H();
        fuelrecordinfo.volume = voluem;
        fuelrecordinfo.money = money;
    }
	//������ˮ
    else if(revbuf[0] == 0x12)   
    {
    	//�Ӽ�����������ˮ
        ReadFuelRecord(id - 1, 1);
		//�ϴ���ȡ�İ���ˮ
        UpLoad_Shift();
    }
	//��������ˮ
    else if(revbuf[0] == 0x13)   
    {
        //������ˮ������ʱδд
    }
}


/************************************************************************
**	��������:	  ReplyCmd_ResponseOk(void)
**	��������:	  �������յ�69H�����̨�ɹ�������ˮ��¼�󷵻صĽ��ճɹ�����Ժ󣬻ظ���̨
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void ReplyCmd_ResponseOk(void)
{
    uint8_t revbuf[9];
    uint32_t id;

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
	
	//���յ�����ˮ�ţ�������ˮ������ˮ��������ˮ֮һ�� ADD BY LY
    id = BcdbufToHex(&revbuf[3], 6);   

	//������ˮ���ճɹ�
    if(revbuf[0] == 0x11)   
    {
        fuelrecordinfo.upflag = 0x01;
		
        if(sysparas.uptransnum == id - 1)
        {
        	//�����Ѵ���ˮ��
            sysparas.uptransnum = id;
			
			//�����Ѵ���ˮ�� ADD BY LY
            SaveSignalSysparas(4, 1);
        }
    }
	 //����ˮ���ճɹ�
    else if(revbuf[0] == 0x12)  
    {
        shiftrecordinfo.upflag = 0x01;

        if(sysparas.shiftuptransnum == id - 1)
        {	
        	//�����Ѵ�����ˮ��
            sysparas.shiftuptransnum = id;
			//�����Ѵ�����ˮ��
            SaveSignalSysparas(39, 1);
        }

    }
	//������ˮ���ճɹ�
    else if(revbuf[0] == 0x13)   
    {
        errorRecordinfo.upflag = 0x01;

        if(sysparas.uperr_id == id - 1)
        {	
        	//�����Ѵ�������ˮ��
            sysparas.uperr_id = id;
			//�����Ѵ�������ˮ��
            SaveSignalSysparas(84, 1);
        }
    }

    //֡ͷ ֡β ����
    SendBackCommunicationData(COMM_RESPONSEOK);
}

/************************************************************************
**	��������:	ReplyCmd_Losscard(void)
**	��������:	  �������յ�54H�����ʧ�����Ժ󣬻ظ���̨
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void ReplyCmd_Losscard(void)
{
    uint8_t revbuf[9];
    uint32_t cid;

    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));

    cid = BcdbufToHex(&revbuf[3], 6);

	//��ʧ
    if(RecBack485Buf[6] == 0x01)
    {
        SetLossCard(cid, 1);   //��ʧ
    }
    else
    {
        SetLossCard(cid, 0);	//��� ����Ŀǰ��̨����û�н�ҹ���
    }

    //֡ͷ
    BackCommunicationHead(COMM_CARDINFO);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//�ȸ�ֵΪ0�������޸�
    //��ʧ/ȡ��
    SendBack485Buf[SendBack485Count++] = RecBack485Buf[6];
    //����
    HexToBcdbuf(cid, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;

    //֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}
/************************************************************************
**	��������:	ReplyCmd_LossCardsRead(void)
**	��������:	  �������յ�59H�����ȫ����ʧ���б��Ժ󣬻ظ���̨
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-08-10
*************************************************************************/
void ReplyCmd_LossCardsRead(void)
{
    uint8_t revbuf[6];
    uint32_t cid;

    memcpy(revbuf, &RecBack485Buf[10], sizeof(revbuf));
    cid = BcdbufToHex(revbuf, 6);

    //֡ͷ
    BackCommunicationHead(CMD_UPLDRPT);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//�ȸ�ֵΪ0�������޸�

    //��ʧ/ȡ��
    if(ReadLossCard(cid))
    {
        SendBack485Buf[SendBack485Count++] = 0x01;//��ʧ��
    }
    else
    {
        SendBack485Buf[SendBack485Count++] = 0x00;//�ǹ�ʧ��
    }

    //����
    HexToBcdbuf(cid, &SendBack485Buf[SendBack485Count], 6);
    //֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	��������:	Reply_ReadVersion(void)
**	��������:	  �������յ�A0H���Զ������ʱ�����ڶ�ȡ�汾�ţ��Ժ󣬻ظ���̨
**	�������:	��
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2018-09-10
*************************************************************************/
void Reply_ReadVersion(void)
{
    uint8_t i, len = 0;
	
    //֡ͷ
    BackCommunicationHead(COMM_READVERSION);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//�ȸ�ֵΪ0�������޸�
    //�汾��
    len = sizeof(VERSION);

    for(i = 0; i < len; i++)
    {
        SendBack485Buf[SendBack485Count++] = VERSION[i];
    }

    //֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}
/************************************************************************
**	��������:	Reply_ReadVersion(void)
**	��������:	  �������յ�55H�����̨�·���������Ժ󣬻ظ���̨
**	�������:	��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void Reply_ExShiftId(void)
{
    uint8_t revbuf[15];
    uint8_t tmpbuf[6];
    uint8_t newshiftid;
	
	//�����ˮ�ṹ�� ADD BY LY
    memset(&shiftrecordinfo.classid, 0, sizeof(shiftrecordinfo));     
    memcpy(revbuf, &RecBack485Buf[6], sizeof(revbuf));
    //�°��
    newshiftid = (uint8_t)BcdToDec(revbuf[0]);
	
	//��ǰ�����Ҫ���İ�Ų���� ADD BY LY
    if(sysparas.shiftnum != newshiftid)   
    {
        //������һ����Ļ���ʱ��
        tmpbuf[0] = revbuf[6];//(uint8_t)BcdToDec(revbuf[6]); //yy
        tmpbuf[1] = revbuf[7];//(uint8_t)BcdToDec(revbuf[7]); //mm
        tmpbuf[2] = revbuf[8];//(uint8_t)BcdToDec(revbuf[8]); //dd
        tmpbuf[3] = revbuf[12];//(uint8_t)BcdToDec(revbuf[12]);
        tmpbuf[4] = revbuf[13];//(uint8_t)BcdToDec(revbuf[13]);
        tmpbuf[5] = revbuf[14];//(uint8_t)BcdToDec(revbuf[14]);
        
		//�ж��·��������Ƿ�Ϸ� ADD BY LY
        if(!CheckDate(&tmpbuf[0]))     
            return;
		
		//�ж��·���ʱ���Ƿ�Ϸ� ADD BY LY
        if(!CheckTime(&tmpbuf[3]))     
            return;
		
		//�·�������ʱ����Ϸ�����������ʱ�� ADD BY LY
        Ds3232SetTime(tmpbuf); 	   
		
        //��һ��Ŀ�ʼʱ��
        memcpy(&shiftrecordinfo.classstarttime[0], &sysparas.shiftstarttime[0], 6);
		
        //��һ�����ʱ�䣨���ǵ�ǰʱ�䣩
        Ds3232ReadTime();
        memcpy(&sysparas.shiftendtime[0], &time, 6);
        memcpy(&shiftrecordinfo.classendtime[0], &sysparas.shiftendtime, 6);
		
        //��һ�࿪ʼʱ��
        memcpy(&sysparas.shiftstarttime[0], &sysparas.shiftendtime[0], 6);
		
		//�·��İ���Ƿ�Ϸ������ڣ�1 - sysparas.shiftmaxnum��֮�� ADD BY LY
        if(newshiftid > sysparas.shiftmaxnum  || newshiftid < 1)   
        {
            return;
        }

		globalvar.U8Temp = sysparas.shiftnum;
		
		//����
        Exshiftid(newshiftid);   

    }

    //�������ȴ�
    if(sysparas.shiftuptransnum < sysparas.shiftransnum)   
    {
    	//���ϴ����ۼ�¼����
        UpLoad_Shift();
        //�յ���̨ȷ�������Ժ���ȷ����������д�Ѵ�����ˮ�� ADD BY LY
    }
}
/************************************************************************
**	��������:	Reply_ReadVersion(void)
**	��������:	����
**	�������: id���µİ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void Exshiftid(uint8_t id)
{
    uint8_t *tmp;

    //�°���ۼ�¼
    //����ˮ��
    sysparas.shiftransnum++;//���°���ˮ�� ADD BY LY
    shiftrecordinfo.classid = sysparas.shiftransnum;
    //���
    shiftrecordinfo.classnum  = globalvar.U8Temp;  	//����ֶ�����ʱ����Ų���  
    //����ʼǹ��
    shiftrecordinfo.startgunvolume = sysparas.shiftstartvol;
    //���ۺϼƣ�����-��ʼ��
    sysparas.shiftvolsum = sysparas.shiftendvol - sysparas.shiftstartvol;
    shiftrecordinfo.classcalgasnum = sysparas.shiftvolsum;
    //���ۻ��ϼƣ���ˮ�ۼӣ�
    shiftrecordinfo.classgassum = sysparas.shiftotvol;
    //�����ǹ��
    sysparas.shiftendvol = sysparas.shiftstartvol + sysparas.shiftotvol;
    shiftrecordinfo.endallgunvolume = sysparas.shiftendvol;
    //���������ۻ�
    shiftrecordinfo.endagunmoney = sysparas.shiftstartmon + sysparas.shiftotmon;
    //���
    shiftrecordinfo.classgasmoney = sysparas.shiftotmon;
    //�ֽ�
    shiftrecordinfo.classrealmoney = sysparas.shiftrealmoney;
    //����
    shiftrecordinfo.classtimes = sysparas.shiftfueltimes;
    //�ϴ��ɹ���־
    shiftrecordinfo.upflag = 0;
    //��ȡ������λ
    shiftrecordinfo.measuretype = sysparas.unit;
    //����վ���
    shiftrecordinfo.stationcode = sysparas.stationcode;
    //�����ۼ�¼�洢 crc����
    tmp = (uint8_t *)(&shiftrecordinfo.classid);
    //crc
    shiftrecordinfo.crc  = ModbusCrc16(tmp, sizeof(shiftrecordinfo) - 2);

	//�������ˮ
    if(SaveFuelRecord(1))
    {
    	//�°��
    	sysparas.shiftnum = id;
        //�°࿪ʼǹ��
        sysparas.shiftstartvol  = sysparas.totvolume;
        //�°࿪ʼ���
        sysparas.shiftstartmon = sysparas.totmoney;
        //�°��ۻ�������
        sysparas.shiftotvol = 0;
        //�°��ۻ��ӽ��
        sysparas.shiftotmon = 0;
        //�°��ۻ�ʵ���ֽ�
        sysparas.shiftrealmoney = 0;
        //�°��ۻ���������
        sysparas.shiftfueltimes = 0;
        //��ӡ����ˮ
        
		//��ϵͳ�����л�������ı仯���� ADD BY LY
        if(ExshiftidSaveParas())   
        {
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "����ɹ���");
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            PrintShifid();
        }
        else
        {
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "����ʧ�ܣ�");
            FYD12864ClearLine(3);
            FYD12864DispPrintfFlash(4, 1, "�洢����ʧ�ܣ�");
        }
    }
    else
    {
        FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(2, 1, "����ʧ�ܣ�");
        FYD12864ClearLine(3);
        FYD12864DispPrintfFlash(4, 1, "�洢����ˮʧ�ܣ�");
    }
}

/************************************************************************
**	��������:	  Reply_ReceiveFail(void)
**	��������:	��������ʧ��ʱ���ظ���̨
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void Reply_ReceiveFail(void)
{
    //֡ͷ ֡β ����
    SendBackCommunicationData(CMD_RECVERR);
}

/************************************************************************
**	��������:	Reply51(void)
**	��������:	�������ڷǿ���״̬�£��յ�51HѲ������ظ���̨
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void Reply51(void)
{
    //֡ͷ ֡β ����
    SendBackCommunicationData(COMM_POLL);
}

/************************************************************************
**	��������:	RS485_Test(void)
**	��������:	���������̨ͨѶ����
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void RS485_Test(void)
{
    uint8_t  crclen;
    uint8_t  CRC;

    memset(SendBack485Buf, 0, sizeof(SendBack485Buf));     //�����������㡣
    SendBack485Count = 0;
    SendBack485Buf[SendBack485Count++] = 0xBB;				//��ʼ��1, BBH
    SendBack485Buf[SendBack485Count++] = 0xEE;				//��ʼ��2, EEH
    SendBack485Buf[SendBack485Count++] = 0x00;				//Ŀ���豸��
    SendBack485Buf[SendBack485Count++] = 0X61; 			    //����ID
    SendBack485Buf[SendBack485Count++] = 0X01;				//ǹ��
    SendBack485Buf[SendBack485Count++] = 5;					//��Ϣ�峤��

    SendBack485Buf[SendBack485Count++] = 0x01;
    SendBack485Buf[SendBack485Count++] = 0x02;
    SendBack485Buf[SendBack485Count++] = 0x03;
    SendBack485Buf[SendBack485Count++] = 0x04;
    SendBack485Buf[SendBack485Count++] = 0x05;



    SendBack485Buf[5] = 5; //len=����ͷ����+��Ϣ�峤�� (����ͷ���ֳ���Ϊ6�����Ժ󣬾�����Ϣ��ĳ���)
    crclen = 5 + 4;        //crc���㳤�� ����֡ͷ��֡β��CRC����

    // ��ΪCRC���㷶Χ ������֡ͷ��֡β��CRC���� ���� crclen = len-2;

    CRC = int_crc8((uint8_t*)&SendBack485Buf[2], crclen);     //������CRCֵ
    SendBack485Buf[SendBack485Count++] = CRC;    // CRC ��ֵ
    SendBack485Buf[SendBack485Count++] = 0xDD;   //��ֹ��1, DDH
    SendBack485Buf[SendBack485Count++] = 0xEE; 	 //��ֹ��2, EEH

    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	��������:	BGCommunication(void)
**	��������:	�жϺ�̨�������ͣ�������ִ�в�ͬ�������
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void BGCommunication(void)
{
    switch(RecBack485Buf[3])   //��������ID�жϻظ�
    {	
    	//Ѳ������
        case COMM_POLL:     
			//���������ڼ���״̬
            if(globalvar.workstate == StaFuel)
            {
            	//��̨��׼���ý���ʵʱ����
                if(RecBack485Buf[6] == 1)
                {
                	//�������ظ�����״̬�������У����ϴ�ʵʱ
                    UpLoad_RunState(2);
                }
				//��ͨ51Ѳ������
                else
                {
                	//�����������̨ȷ�ϣ��Ƿ�׼���ý���ʵʱ
                    UpLoad_RunState(1);
                }
            }
            else
            {
            	//���������ڷǼ���״̬
                ReplyCmd_Poll();
            }

            break;
		//��̨���ò���
        case COMM_PARASET:  
            ReplyCmd_ParaSet();
            break;
		//��̨��ȡ��������
        case COMM_RDPARA:
            ReplyCmd_ParaRead();
            break;
		//�ϴ�ָ����ˮ����̨�Ӽ�����ȡ��ˮ��
        case COMM_RDUIDREC: 
            ReplyCmd_UidRead();
            break;
		//��̨�յ������Ժ󣬻ظ�������OK
        case COMM_RESPONSEOK:
            ReplyCmd_ResponseOk();
            break;
		//�¹�ʧ��
        case COMM_CARDINFO: 
            ReplyCmd_Losscard();
            break;
		//��ȫ����ʧ���б�
        case COMM_RDALLCARD:
            ReplyCmd_LossCardsRead();
            break;
		//��̨�·���������
        case COMM_EXWORK:   	
            Reply_ExShiftId();
            break;
		//Զ������ʱ�����ڶ�ȡ�汾��
        case COMM_READVERSION:   
            Reply_ReadVersion();
            break;
		//���������,�ͱ�ʾ���յ����Ǵ�������� �ش�50H
        default:
            Reply_ReceiveFail();
            break;
    }

	//����ʹ����ϣ���ս���buf��Ϊ��һ�ν���������׼��
    uart2.reclen = 0; //�����峤�����㡣
    memset((uint8_t *)&uart2.recbuf[0], 0, sizeof(RecBack485Buf));       //����ջ�����
    OpenUart2RcvIRQ();    //�������ж�
}

/************************************************************************
**	��������:	UpLoad_11H(void)
**	��������:	װ�ز����ͼ�����ˮ(11H)
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void UpLoad_11H(void)
{
    uint8_t i;

    //֡ͷ
    BackCommunicationHead(CMD_UPLDREC);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//11H����� �ȸ�ֵΪ0�������޸�
    //������ˮ��
    HexToBcdbuf(fuelrecordinfo.transnum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //���
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.classnum);       //HEXתBCD;
    //����ˮ��
    HexToBcdbuf(fuelrecordinfo.classid, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;

    //������ʼʱ��
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = fuelrecordinfo.starttime[i];
    }

    //��������ʱ��
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = fuelrecordinfo.endtime[i];
    }

    //IC������
    HexToBcdbuf(fuelrecordinfo.usernum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //IC������
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.cardtype);;
    //����ǰ���
    HexToBcdbuf(fuelrecordinfo.beforebalance, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //���μ�������
    HexToBcdbuf(fuelrecordinfo.volume, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //���μ�������
    HexToBcdbuf(fuelrecordinfo.price, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //���μ������
    HexToBcdbuf(fuelrecordinfo.money, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //���������
    HexToBcdbuf(fuelrecordinfo.afterbalance, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //�ۻ���ֵ����
    HexToBcdbuf(fuelrecordinfo.allrechargetimes, &SendBack485Buf[SendBack485Count], 3);
    SendBack485Count = SendBack485Count + 3;
    //�ۻ���ֵ���
    HexToBcdbuf(fuelrecordinfo.allrechargemoney, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //�ۻ����Ѵ���
    HexToBcdbuf(fuelrecordinfo.allconsumptiontimes, &SendBack485Buf[SendBack485Count], 3);
    SendBack485Count = SendBack485Count + 3;
    //�ۻ���������
    HexToBcdbuf(fuelrecordinfo.allconsumptionnum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //�ۻ����ѽ��
    HexToBcdbuf(fuelrecordinfo.allconsumptionmoney, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //�������
    HexToBcdbuf(fuelrecordinfo.InitialMoney, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //�������
    HexToBcdbuf(fuelrecordinfo.CalMoney, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //���ź���λ
    HexToBcdbuf(fuelrecordinfo.carnum_another, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //����ǰѹ��
    HexToBcdbuf(fuelrecordinfo.beforepressure, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //������ѹ��
    HexToBcdbuf(fuelrecordinfo.afterpressure, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //����ǰǹ��
    HexToBcdbuf(fuelrecordinfo.beforegunsum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //������ǹ��
    HexToBcdbuf(fuelrecordinfo.aftergunsum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //�ҿ���־
    SendBack485Buf[SendBack485Count++] = fuelrecordinfo.greyflag;//(uint8_t)HexToBcd((uint32_t)fuelrecordinfo.greyflag);
    //��ӡ��־
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.printflag);
    //�ϴ���־
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.upflag);;
    //������λ
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.measuretype);
    //����������
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.devicetype);
    //����վ���
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.stationcode);

    //֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	��������:	UpLoad_Error(void)
**	��������:	װ�ز����ͱ�����ˮ (13H)
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void UpLoad_Error(void)
{
    uint8_t temp;
    uint8_t i;

    //֡ͷ
    BackCommunicationHead(CMD_UPLDERR);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//�ȸ�ֵΪ0�������޸�
    //������ˮ��
    HexToBcdbuf(errorRecordinfo.errtransactionid, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;

    //����ʱ��
    for(i = 0; i < 6; i++)
    {
        temp = (uint8_t)HexToBcd((uint32_t)errorRecordinfo.errtime[i]);
        SendBack485Buf[SendBack485Count++] =  temp;
    }

    //���
    temp = HexToBcd(errorRecordinfo.classnum);
    SendBack485Buf[SendBack485Count++] = temp;
    //�������
    temp = HexToBcd(errorRecordinfo.errcode);
    SendBack485Buf[SendBack485Count++] = temp;
    //������Ŀ���
    HexToBcdbuf(errorRecordinfo.errusernum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //�ϴ��ɹ����
    temp = HexToBcd(errorRecordinfo.upflag);
    SendBack485Buf[SendBack485Count++] = temp;
    //����2�ֽ�
    HexToBcdbuf(errorRecordinfo.data, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	��������:	UpLoad_Shift(void)
**	��������:	װ�ز����Ͱ���ˮ (12H)
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void UpLoad_Shift(void)
{
    uint8_t temp;
    uint8_t i;

    //֡ͷ
    BackCommunicationHead(CMD_UPLDRPT);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0;//�ȸ�ֵΪ0�������޸�
    //������ˮ��
    HexToBcdbuf(shiftrecordinfo.classid, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //���
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.classnum);       //HEXתBCD
    SendBack485Buf[SendBack485Count++] = temp;

    //������ʼʱ��
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = shiftrecordinfo.classstarttime[i];
    }

    //��������ʱ��
    for(i = 0; i < 6; i++)
    {
        SendBack485Buf[SendBack485Count++] = shiftrecordinfo.classendtime[i];
    }

    //�࿪ʼǹ��
    HexToBcdbuf(shiftrecordinfo.startgunvolume, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //�����ǹ��
    HexToBcdbuf(shiftrecordinfo.endallgunvolume, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;
    //��ϼ������������ǹ��-�࿪ʼǹ�ۣ�
    HexToBcdbuf(shiftrecordinfo.classcalgasnum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //��ϼ�����������ˮ��¼�ϼƼ��㣩
    HexToBcdbuf(shiftrecordinfo.classgassum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //��ϼƽ�����ˮ��¼�ϼƼ��㣩
    HexToBcdbuf(shiftrecordinfo.classgasmoney, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //��ʵ���ֽ��ֽ�+��ֵ��
    HexToBcdbuf(shiftrecordinfo.classrealmoney, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //���ۻ���������
    HexToBcdbuf(shiftrecordinfo.classtimes, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //�ϴ���־
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.upflag);       //HEXתBCD
    SendBack485Buf[SendBack485Count++] = temp;
    //������λ
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.measuretype);       //HEXתBCD
    SendBack485Buf[SendBack485Count++] = temp;
    //����������
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.devicetype);       //HEXתBCD
    SendBack485Buf[SendBack485Count++] = temp;
    //����վ���
    temp = (uint8_t)HexToBcd((uint32_t)shiftrecordinfo.stationcode);       //HEXתBCD
    SendBack485Buf[SendBack485Count++] = temp;
    //֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}

/************************************************************************
**	��������:	UpLoad_Print(void)
**	��������:	װ�ز����ʹ�ӡ��ˮ (15H)
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void UpLoad_Print(void)
{

}
/************************************************************************
**	��������:	UpLoad_RunState(uint8_t state)
**	��������:	װ�ز����ͼ���������״̬ (19H)
**	�������: state������������״̬ 
			  state��0������������1�������������̨ȷ���Ƿ�׼���ý���ʵʱ���ݣ�2�������У�����������ʵʱ
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-09-10
*************************************************************************/
void UpLoad_RunState(uint8_t state)
{
    //֡ͷ
    BackCommunicationHead(CMD_RUNSTAT);
    //��Ϣ�峤��
    SendBack485Buf[SendBack485Count++] = 0x00;
    //IC������
    HexToBcdbuf(fuelrecordinfo.usernum, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //IC������
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.cardtype);
    //����ǰ���
    HexToBcdbuf(fuelrecordinfo.beforebalance, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //���ź���λ
    HexToBcdbuf(fuelrecordinfo.carnum_another, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //����ǰѹ��
    HexToBcdbuf(fuelrecordinfo.beforepressure, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //����ǰ����
    SendBack485Buf[SendBack485Count++] = 0;
    SendBack485Buf[SendBack485Count++] = 0;
    //����ǰǹ��
    HexToBcdbuf(fuelrecordinfo.aftergunsum, &SendBack485Buf[SendBack485Count], 6);
    SendBack485Count = SendBack485Count + 6;

    //��״̬
    if(m1card.existflag == 1)
        SendBack485Buf[SendBack485Count++] = 1;
    else
        SendBack485Buf[SendBack485Count++] = 0;

    //��ѹ������״̬ LNGû�з���״̬
    SendBack485Buf[SendBack485Count++] = 0;
    //��ѹ������״̬
    SendBack485Buf[SendBack485Count++] = 0;
    //��ѹ������״̬
    SendBack485Buf[SendBack485Count++] = 0;
    //����״̬
    SendBack485Buf[SendBack485Count++] = state;
    //���μ�������
    HexToBcdbuf(fuelrecordinfo.price, &SendBack485Buf[SendBack485Count], 2);
    SendBack485Count = SendBack485Count + 2;
    //���μ�������
    HexToBcdbuf(fuelrecordinfo.volume, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //���μ������
    HexToBcdbuf(fuelrecordinfo.money, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //�����
    HexToBcdbuf(fuelrecordinfo.afterbalance, &SendBack485Buf[SendBack485Count], 4);
    SendBack485Count = SendBack485Count + 4;
    //������λ
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.measuretype);
    //����������
    SendBack485Buf[SendBack485Count++] = (uint8_t)HexToBcd((uint32_t)fuelrecordinfo.devicetype);

    //֡ͷ ֡β
    BackCommunicationEnd(SendBack485Count);
    //����
    Uart2SendStr(SendBack485Buf, SendBack485Count);
}
