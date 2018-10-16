#ifndef __ADC_H
#define __ADC_H

/*******************  气相压力传感器 **************************/
void PgADCUinit(void);   //气相压力传感器初始化
uint16_t PgADCGetValueOneTime(void);   //获取转换结果
uint16_t PgADCGetValue(void);   //16次结果平均值

/*******************  液相压力传感器 **************************/
void PlADCUinit(void);
uint16_t PlADCGetValueOneTime(void);   //获取转换结果
uint16_t PlADCGetValue(void);   //16次结果平均值

/*********************** 气相、液相共用************************/
uint8_t PressureGetValue(void);
#endif