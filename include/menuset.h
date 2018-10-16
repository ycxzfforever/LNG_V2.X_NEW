#ifndef __MENU_H
#define __MENU_H

typedef void(*fun)(void);     //

typedef void(*fun1)(fun);

extern fun         DispUI;          //����ˢ�´���
extern fun         DispNextUI;      //������Ϣ��ʾ�����һ������

void InputInit(void);
void DispFirst(void);

void TaskAdd(uint32_t keyvalue, fun funname1, fun funname2);
void TaskMessage(uint32_t ms_100, fun funname);

void SetSuccess(void);   //���óɹ���ʾ
void SetFault(void);   //����ʧ����ʾ


void DispSet(uint8_t numpoint, uint32_t data, char __farflash*buf);   //���ò���ʱ��ʾ numpoint:С�������λ��,0,2,4��Ч��������4ʱΪѡ������

void DispInputPwd(char __farflash* pwdbuf, fun funname);


void DispInputMenuPwd(void);   //����˵�����
void DispInputQcPwd(void);   //�����ʼ�����
void DispInputTestPwd(void);   //���������
void DispInputResetPwd(void);   //����ָ�������������

void DispJudgeMenuPwd(void);   //�˵�������֤
void DispJudgeQcPwd(void);     //Qc������֤
void DispJudgeTestPwd(void);   //Ӳ������������֤
void DispJudgeResetPwd(void);   //�ָ���������������֤




void DispUnitQcReset(void);
void DispSetUnit(void);
void DispSetQCMode(void);
void DispEraseChip(void);
void DispFactoryReset(void);


void DispSetPrice(void);   //��������
void DispSetOthPrice1(void);   //�Żݵ�������1
void DispSetOthPrice2(void);   //�Żݵ�������2
void DispSetOthPrice3(void);   //�Żݵ�������3


void DispMenu_1(void);   //�˵�����1��ʾ



void DispGeneralInfo(void);   //ͨ����Ϣ����
void DispCommPara(void);   //ͨѶ��������
void DispCommonOpt(void);   //���ò�������
void DispMenu_2(void);   //�˵�����2��ʾ
void DispMenu_3(void);   //�˵�����3��ʾ

void DispVerNum1(void);   //�汾�������1
void DispVerNum2(void);   //�汾�������2
void DispOthMsg1(void);   //������Ϣ����1
void DispOthMsg2(void);   //������Ϣ����2

void DispSetPwdBoard(void);
void SetPwdBoard(void);	

//void DispMenu_3(void);//�˵�����3��ʾ
void DispSysSet1(void);   //ϵͳ��������1
void DispSysSet2(void);   //ϵͳ��������2
void DispSysSet3(void);   //ϵͳ��������3

void DispSetModbusAdr_G(void);
void DispSetModbusAdr_L(void);


void DispSetShiftInfo1(void);   //������Ϣ����1
void DispSetShiftInfo2(void);   //������Ϣ����2
void DispSetShiftInfo3(void);   //������Ϣ����3
void DispSetPressure(void);   //ѹ������

void DispSetICInfo1(void);   //IC����Ϣ����1
void DispSetICInfo2(void);   //IC����Ϣ����2
void DispSetICInfo3(void);   //IC����Ϣ����3

void DispSetLossCard(void);

void DispInputOldPwd(void);   //���������
void DispOK(void);   //ȷ�ϴ���

void DispSetQcPwd(void);   //�ʼ���������
void DispSetTestPwd(void);   //ά����������
void DispSetonoffworkpwd(void);   //���°���������
void DispSetResetPwd(void);   //�ָ�������������
void DispFactoryReset(void);   //�ָ���������
void DispQcPara(void);   //�ʼ��������
void SimulatorSet(void);
void OnWork(void);   //�ϰࡣ
void OffWork(void);   //�°ࡣ
void Diswork(void);
void WriteICcard(void);
uint16_t memcmpp(uint8_t *tr1, uint8_t *tr2);



void DispSetLockVer(void);   //��Կ��Ϣ�汾������

void DispCommonOpt1(void);   //���ò�������1��


void DispSetPing(void);   //��Ļѡ������

//void OnWorkWithPwd( void ); //�����ϰ�
//void OffWorkWithPwd( void ); //�����°�
//void DispJudgeOnWorkPwd( void ); //�ϰ�������֤
//void DispJudgeOffWorkPwd( void ); //�°�������֤

void DispSetOffline(void);   //�ѻ�ģʽ

void SystemReStart(void);   //�������

void DispEraseChip(void);   //Flash�洢оƬ����
void DispSetMenuPwd(void);
void CirculateFuelingSet(void);
void SystemReStartToBootLoader(void);
void SystemReStartToAPP(void);


#endif

