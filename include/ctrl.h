#ifndef _CTRL_H
#define _CTRL_H

//������״̬����
//#define StaOffDuty      0xA0    // �°�	LNG2.7 ���°�״̬ ADD BY LY
#define StaIdle         0xA1    // ����
#define StaSPrecool     0xA2    // Сѭ��
#define StaBPrecool     0xA3    // ��ѭ��
#define StaFuel         0xA4    // ������
#define StaPay          0xA5    // ���۷�
#define StaClose        0xA6    //  �ر�
#define StaOtherFuel    0xA7    //�Է���Һ
#define StaFault        0xF1    // �豸����

enum    Sys_Err                //ϵͳ�쳣ԭ��
{

    SYSERR_FLASH_CHK,			//FLASH���ʧ�� 0
    SYSERR_W_FLASH,             //FLASH дʧ�� 1
    SYSERR_R_FLASH,             //FLASH ��ʧ�� 2

    SYSERR_W_IN_EEPROM,         //д�ڲ�eeprom���� 3
    SYSERR_R_IN_EEPROM,         //���ڲ�eeprom���� 4
    SYSERR_W_EX_EEPROM,         //д�ⲿeeprom���� 5
    SYSERR_R_EX_EEPROM,         //���ⲿeeprom���� 6
    SYSERR_INANDEX_EEPROM,      //�ڲ�eeprom���ⲿeeprom�������ݲ�һ�� 7

    SYSERR_MODBUS_G_AUTO_ERR,   //����������ͨѶ���� 8
    SYSERR_MODBUS_L_AUTO_ERR,   //Һ��������ͨѶ���� 9
    SYSERR_PRESSURE,            //ѹ���������쳣 10
};



#endif