#ifndef __ADC_H
#define __ADC_H

/*******************  ����ѹ�������� **************************/
void PgADCUinit(void);   //����ѹ����������ʼ��
uint16_t PgADCGetValueOneTime(void);   //��ȡת�����
uint16_t PgADCGetValue(void);   //16�ν��ƽ��ֵ

/*******************  Һ��ѹ�������� **************************/
void PlADCUinit(void);
uint16_t PlADCGetValueOneTime(void);   //��ȡת�����
uint16_t PlADCGetValue(void);   //16�ν��ƽ��ֵ

/*********************** ���ࡢҺ�๲��************************/
uint8_t PressureGetValue(void);
#endif