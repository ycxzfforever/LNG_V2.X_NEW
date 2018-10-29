#include "kernel.h"

//初始化AD和阀门
void PgADCUinit(void)
{
	//将PK1设置为没有上拉的输入端口 ADD BY LY
    ADC_G_IN();
    ADC_G_L();
    //AD初始化
    ADCSRA = 0x00;    		//禁止使能
    ADMUX = (1 << REFS0); 	//通道选择--00000 - ADC0,单端输入
    
    //设置参考电压 01 --AVCC
    //转换结果右对齐 0
    ADCSRB = (1 << MUX5);
    ADMUX |= (1 << MUX0);   //ADMUX[4:0] = 00000 ,ADC输入为单通道第9通道,气相
    ACSR  = (1 << ACD);   	//禁用模拟比较器
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0);
}

/************************************************************************
**	函数名称:	PgADCGetValueOneTime(void)
**	函数功能:	气相，获取1次压力传感器的值
**	输入参数: 无
**	返回值		：1次压力转换的AD值

**	创 建 者:	
**	创建日期:	2018-09-10
*************************************************************************/
uint16_t PgADCGetValueOneTime(void)   //获取转换结果
{
    uint16_t rd1, rd2;

    ADCSRA |= (1 << ADSC) ;	   //启动转换

    while(ADCSRA & (1 << ADSC));     //等待转换结束

    rd1 = ADCL;
    rd2 = ADCH << 8;
    return rd1 + rd2;
}

/************************************************************************
**	函数名称:	PgADCGetValueOneTime(void)
**	函数功能:	气相，获取16次压力传感器的平均值
**	输入参数: 无
**	返回值		：16次压力转换的平均AD值

**	创 建 者:	
**	创建日期:	2018-09-10
*************************************************************************/
uint16_t PgADCGetValue(void)   //16次结果平均值
{
    uint8_t i;
    uint16_t  cal = 0;
	ADMUX |= (1<<MUX0);
	ADCSRA |= (1<<ADEN) ;//打开AD转换 ADD BY LY
    for(i = 0; i < 16; i++)
    {
        cal += PgADCGetValueOneTime();
    }
	ADCSRA &= ~(1<<ADEN) ;//关闭AD转换 ADD BY LY
    return cal / 16;
}

void PlADCUinit(void)
{
	//将PK0设置为没有上拉的输入端口 ADD BY LY
    ADC_L_IN();
    ADC_L_L();
    //AD初始化
    ADCSRA = 0x00;    	//禁止使能
    ADMUX = (1 << REFS0); 	//通道选择--00000 - ADC0,单端输入
    //REFS0 =1,REFS1=0 ,设置参考电压为 AVCC
    //转换结果右对齐 0
    ADCSRB = (1 << MUX5);   //ADCSRB->MUX5 =1
    ADMUX |= (1 << MUX0);   //ADMUX[4:0] = 00000 ,ADC输入为单通道第8通道，液相
    ACSR  = (1 << ACD);   //禁用模拟比较器
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0);       //使能ADC，预分频设置为 32
}

/************************************************************************
**	函数名称:	PlADCGetValueOneTime(void)
**	函数功能:	液相，获取1次压力传感器的值
**	输入参数: 无
**	返回值		：1次压力转换的AD值

**	创 建 者:	
**	创建日期:	2018-09-10
*************************************************************************/
uint16_t PlADCGetValueOneTime(void)   //获取转换结果
{
    uint16_t rd1, rd2;

    ADCSRA |= (1 << ADSC) ;	   //启动转换

    while(ADCSRA & (1 << ADSC));     //等待转换结束

    rd1 = ADCL;
    rd2 = ADCH << 8;
    return rd1 + rd2;
}

/************************************************************************
**	函数名称:	PgADCGetValueOneTime(void)
**	函数功能:	液相，获取16次压力传感器的平均值
**	输入参数: 无
**	返回值		：16次压力转换的平均AD值

**	创 建 者:	
**	创建日期:	2018-09-10
*************************************************************************/
uint16_t PlADCGetValue(void)   //16次结果平均值
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
**函数名称:PressureGetValue
**函数功能:获取压力值，并赋值给realtime.cur_pressure
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-12-12

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t PressureGetValue(void)
{

    uint32_t curAD = 0;  	//当前读出的AD值
    uint32_t deltaAD = 0;	//压力值矫正参数
    uint32_t curPressure;	//当前矫正过的AD值

    //其实气相和液相在读值时，都是读的相同的寄存器，只是输入的通道不同
    //PlADCGetValue() 与 PgADCGetValue() 两个函数其实是一样的 ADD BY LY

    curAD = PlADCGetValue(); //获取当前压力AD值

	// 80 只是认为给予的一个误差范围，此值大小根据自己AD零点波动范围而定，通常取几十到一边 ADD BY LY
	//通常，压力传感器零点的值，会大于等于sysparas.pzero（sysparas.pzero为压力传感器值为0时，AD采集到的真实值）
	if((curAD + 80) <= sysparas.pzero)     //压力零点
    {
    	//压力传感器异常
        return false;
    }
	
    else if(curAD <= sysparas.pzero)
    {
        deltaAD = 0;
    }
	//采集到的AD值大于0点的值，求偏移量deltaAD
    else
    {
        deltaAD = curAD - sysparas.pzero;
    }
	
	//计算压力值
    curPressure = (100 * deltaAD) / sysparas.pcoeff;   
    realtime.cur_pressure = curPressure;
    return true;
}
