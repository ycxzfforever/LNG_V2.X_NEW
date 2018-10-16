#ifndef __KEYBOARD_H
#define __KEYBOARD_H


enum  EVENT_SYS_ID
{
    SYS_NOEVENT = 0,
    KEY_0_EV, //��ֵ0---1
    KEY_1_EV,
    KEY_2_EV,
    KEY_3_EV,
    KEY_4_EV,
    KEY_5_EV,
    KEY_6_EV,
    KEY_7_EV,
    KEY_8_EV,
    KEY_9_EV,//10
    KEY_START_EV,//����---11
    KEY_STOP_EV,//ֹͣ---12
    KEY_PT_EV,//�ٺ�----13
    KEY_OK_EV,//ȷ��----14
    KEY_BACK_EV,//����--15
    KEY_RET_EV,//����---16
    KEY_CLR_EV,//���---17
    KEY_FIX_EV,//����---18
    KEY_SET_EV,//�˵�---19
    KEY_QRY_EV,//��ѯ--20
    KEY_COMB_1_EV, /*STOP+QRY��ѯ--21*/
    KEY_COMB_2_EV, /*STOP+SET�˵�--22*/
    KEY_COMB_3_EV, /*STOP+FIX����*/
    KEY_COMB_4_EV, /*STOP+BACK����--23*/
    KEY_COMB_5_EV, /*STOP+BACK���--24*/
    KEY_COMB_6_EV, /*STOP+RET����---25*/
    KEY_END,
    EV_CURSOR,
    EV_DISPTIME,
    EV_LOGOUT,    //ϵͳ�ǳ�
    MAX_EV_ID
};
/*
���ܣ��漰���̵�һЩ����
*/
#define KEY_0    0x00001000
#define KEY_1    0x00000001
#define KEY_2    0x00000002
#define KEY_3    0x00000004
#define KEY_4    0x00000010
#define KEY_5    0x00000020
#define KEY_6    0x00000040
#define KEY_7    0x00010000
#define KEY_8    0x00020000
#define KEY_9    0x00040000

#define KEY_UP      KEY_6
#define KEY_DOWN    KEY_9

#define KEY_QRY     0x00000008    //��ѯ
#define KEY_SET     0x00000080    //����
#define KEY_FIX     0x00080000    //����
#define KEY_BACK    0x00004000    //���ԣ�CNG��Ԥ�䣨LNG����
#define KEY_CLR     0x00008000    //���
#define KEY_START   0x00000100    //����
#define KEY_STOP    0x00000200    //ֹͣ
#define KEY_OK      0x00000400    //ȷ��
#define KEY_RET     0x00000800    //����
#define KEY_PT      0x00002000    //С���� 

#define KEY_COMB_SQRY 	0x00000208 /*STOP+QRY��ѯ*/
#define KEY_COMB_SSET 	0x00000280 /*STOP+SET����*/
#define KEY_COMB_SFIX 	0x00080200 /*STOP+FIX����*/
#define KEY_COMB_SBACK 	0x00004200 /*STOP+BACK����*/
#define KEY_COMB_SCLR 	0x00008200 /*STOP+BACK���*/
#define KEY_COMB_SRET 	0x00000A00 /*STOP+RET����*/

#define KEY_COMB_SRETCLR 	0x00008A00	/*STOP+����+���*/ 		 //����������Ӧ�ó���
#define KEY_COMB_SRETPOINT 	0x00002A00	/*STOP+����+С����*/ //����������BootLoader

#define KEY_MODULE_EV       KEY_COMB_1_EV
#define KEY_FACSET_EV       KEY_COMB_2_EV
//#define KEY_SYSQRY_EV       KEY_COMB_3_EV
#define KEY_EXSHIFTID_EV    KEY_COMB_4_EV
#define KEY_CARDFUNC_EV     KEY_COMB_5_EV


typedef struct def_Screeninput//��������
{
    uint8_t  inputlen;       //ʵʱ����仯���ȣ����ȴ�0��ʼ����
    uint8_t  inputmaxlen;    //������볤�ȣ����ȴ�0��ʼ����
    uint8_t  pointnum;       //С�������
    uint8_t  array[16];      //�洢������������
} Screeninput;

extern  Screeninput ScreenInput;

void KeyBoardInit(void);
uint32_t KeyBoardReadKey(void);
void KeyBoardReadKeyValue(void);
void NumKeyHandler(void);


#endif
