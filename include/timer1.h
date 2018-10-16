#ifndef __TIMER1_H
#define __TIMER1_H

void Timer1On(void);
void Timer1Off(void);
void Timer1Init(void);
uint32_t Timer1GetTick(void);
void Timer1ClcTick(uint32_t tick);

//extern volatile uint32_t ticks;//��ʱ��2��

typedef struct TIMER1   // 4.0 LNG��Timer�ṹ��
{
    uint32_t tick_100ms;
    uint32_t tick_start;
    uint32_t tick_current;
    uint8_t  flag_100ms;
    uint8_t  flag_500ms;
    uint8_t  flag_1s;
} TIMER1;

#if 0
struct TIMER1     //  2.7 LNG��Timer�ṹ��
{
    uint32_t ticks;//��ʱ���ӿ�ʼ�����ڵ�ֵ
    uint32_t starttime;//��ʱ��ʼʱ��
    uint32_t currenttime;//��ǰ��ʱֵ
    bool readCardFlag;//������־
    bool readDs3232Flag;//��3232��־
    bool compareTimeFlag;//�ȶ�ʱ���־
    uint8_t flag;
    bool fixtime;
};
#endif
extern struct TIMER1 timer1;

#endif
