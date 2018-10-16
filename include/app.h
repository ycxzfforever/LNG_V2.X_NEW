#ifndef __APP_H
#define __APP_H
/*******************  ��̨���͸������� ******************/
#define COMM_POLL		    0x51  //ʵʱ����Ѳ��
#define COMM_PARASET		0x52  //���ò�������
#define COMM_RDPARA		    0x53  //��ȡ��������
#define COMM_CARDINFO		0x54  //���͹�ʧ����Ϣ
#define COMM_EXWORK		    0x55  //���ͻ�����Ϣ
#define COMM_RDALLCARD		0x59  //��ȫ����ʧ���б�
#define COMM_RDUIDREC		0x67  //��ȡָ��UID������¼
#define COMM_RESPONSEOK		0x69  //��̨ PC �ɹ�������ˮ��¼���������ؽ��ճɹ�ȷ������  ����Ϣ��
#define COMM_READVERSION	0xA0  //Զ������ʱ�����ڶ�ȡ�汾��

/*******************  ���������͸���̨ ******************/
#define CMD_UPLDREC		0x11//������ ���ͼ�������ˮ��¼ ��̨�յ����69H
#define CMD_UPLDRPT		0x12//������ ���Ͱ�ͳ�Ƽ�¼ ��̨�յ����69H
#define CMD_UPLDERR     0x13//������ ���͹��ϼ�������¼ ��̨�յ����69H
#define CMD_UPPRINT		0x15//��ˮ��ӡ
#define CMD_RUNSTAT		0x19//������ ���͹���״̬ ��̨�յ����51H ����������ؼ���������״̬(��Ϣ��Ϊ�գ�����IDΪ51H)
#define CMD_RECVERR		0x50//���������պ�̨����ʧ��(CRCУ��������������50H��û����Ϣ��)
#define CMD_SETPARA 	0x52//����ʱ�ӡ�ǹ����Ϣ

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
