#include "kernel.h"

//��ʼ��AD�ͷ���
void PgADCUinit(void)
{
	//��PK1����Ϊû������������˿� ADD BY LY
    ADC_G_IN();
    ADC_G_L();
    //AD��ʼ��
    ADCSRA = 0x00;    		//��ֹʹ��
    ADMUX = (1 << REFS0); 	//ͨ��ѡ��--00000 - ADC0,��������
    
    //���òο���ѹ 01 --AVCC
    //ת������Ҷ��� 0
    ADCSRB = (1 << MUX5);
    ADMUX |= (1 << MUX0);   //ADMUX[4:0] = 00000 ,ADC����Ϊ��ͨ����9ͨ��,����
    ACSR  = (1 << ACD);   	//����ģ��Ƚ���
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0);
}

/************************************************************************
**	��������:	PgADCGetValueOneTime(void)
**	��������:	���࣬��ȡ1��ѹ����������ֵ
**	�������: ��
**	����ֵ		��1��ѹ��ת����ADֵ

**	�� �� ��:	
**	��������:	2018-09-10
*************************************************************************/
uint16_t PgADCGetValueOneTime(void)   //��ȡת�����
{
    uint16_t rd1, rd2;

    ADCSRA |= (1 << ADSC) ;	   //����ת��

    while(ADCSRA & (1 << ADSC));     //�ȴ�ת������

    rd1 = ADCL;
    rd2 = ADCH << 8;
    return rd1 + rd2;
}

/************************************************************************
**	��������:	PgADCGetValueOneTime(void)
**	��������:	���࣬��ȡ16��ѹ����������ƽ��ֵ
**	�������: ��
**	����ֵ		��16��ѹ��ת����ƽ��ADֵ

**	�� �� ��:	
**	��������:	2018-09-10
*************************************************************************/
uint16_t PgADCGetValue(void)   //16�ν��ƽ��ֵ
{
    uint8_t i;
    uint16_t  cal = 0;
	ADMUX |= (1<<MUX0);
	ADCSRA |= (1<<ADEN) ;//��ADת�� ADD BY LY
    for(i = 0; i < 16; i++)
    {
        cal += PgADCGetValueOneTime();
    }
	ADCSRA &= ~(1<<ADEN) ;//�ر�ADת�� ADD BY LY
    return cal / 16;
}

void PlADCUinit(void)
{
	//��PK0����Ϊû������������˿� ADD BY LY
    ADC_L_IN();
    ADC_L_L();
    //AD��ʼ��
    ADCSRA = 0x00;    	//��ֹʹ��
    ADMUX = (1 << REFS0); 	//ͨ��ѡ��--00000 - ADC0,��������
    //REFS0 =1,REFS1=0 ,���òο���ѹΪ AVCC
    //ת������Ҷ��� 0
    ADCSRB = (1 << MUX5);   //ADCSRB->MUX5 =1
    ADMUX |= (1 << MUX0);   //ADMUX[4:0] = 00000 ,ADC����Ϊ��ͨ����8ͨ����Һ��
    ACSR  = (1 << ACD);   //����ģ��Ƚ���
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0);       //ʹ��ADC��Ԥ��Ƶ����Ϊ 32
}

/************************************************************************
**	��������:	PlADCGetValueOneTime(void)
**	��������:	Һ�࣬��ȡ1��ѹ����������ֵ
**	�������: ��
**	����ֵ		��1��ѹ��ת����ADֵ

**	�� �� ��:	
**	��������:	2018-09-10
*************************************************************************/
uint16_t PlADCGetValueOneTime(void)   //��ȡת�����
{
    uint16_t rd1, rd2;

    ADCSRA |= (1 << ADSC) ;	   //����ת��

    while(ADCSRA & (1 << ADSC));     //�ȴ�ת������

    rd1 = ADCL;
    rd2 = ADCH << 8;
    return rd1 + rd2;
}

/************************************************************************
**	��������:	PgADCGetValueOneTime(void)
**	��������:	Һ�࣬��ȡ16��ѹ����������ƽ��ֵ
**	�������: ��
**	����ֵ		��16��ѹ��ת����ƽ��ADֵ

**	�� �� ��:	
**	��������:	2018-09-10
*************************************************************************/
uint16_t PlADCGetValue(void)   //16�ν��ƽ��ֵ
{
    uint8_t i;
    uint16_t  cal = 0;
    ADMUX &= ~(1<<MUX0);
	ADCSRA |= (1<<ADEN) ;
    for(i = 0; i < 16; i++)
    {
        cal += PlADCGetValueOneTime();
    }
	ADCSRA &= ~(1<<ADEN) ;
    return cal / 16;
}

/********************************************************
**��������:PressureGetValue
**��������:��ȡѹ��ֵ������ֵ��realtime.cur_pressure
**�������:��
**���ֵ  :��

**������  :�ƺ���
**��������:2016-12-12

**�޸���  :
**�޸�����:
**�޸�����:
********************************************************/
uint8_t PressureGetValue(void)
{

    uint32_t curAD = 0;  	//��ǰ������ADֵ
    uint32_t deltaAD = 0;	//ѹ��ֵ��������
    uint32_t curPressure;	//��ǰ��������ADֵ

    //��ʵ�����Һ���ڶ�ֵʱ�����Ƕ�����ͬ�ļĴ�����ֻ�������ͨ����ͬ
    //PlADCGetValue() �� PgADCGetValue() ����������ʵ��һ���� ADD BY LY

    curAD = PlADCGetValue(); //��ȡ��ǰѹ��ADֵ

	// 80 ֻ����Ϊ�����һ����Χ����ֵ��С�����Լ�AD��㲨����Χ������ͨ��ȡ��ʮ��һ�� ADD BY LY
	//ͨ����ѹ������������ֵ������ڵ���sysparas.pzero��sysparas.pzeroΪѹ��������ֵΪ0ʱ��AD�ɼ�������ʵֵ��
	if((curAD + 80) <= sysparas.pzero)     //ѹ�����
    {
    	//ѹ���������쳣
        return false;
    }
	
    else if(curAD <= sysparas.pzero)
    {
        deltaAD = 0;
    }
	//�ɼ�����ADֵ����0���ֵ����ƫ����deltaAD
    else
    {
        deltaAD = curAD - sysparas.pzero;
    }
	
	//����ѹ��ֵ
    curPressure = (100 * deltaAD) / sysparas.pcoeff;   
    realtime.cur_pressure = curPressure;
    return true;
}
