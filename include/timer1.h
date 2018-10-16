#ifndef __TIMER1_H
#define __TIMER1_H

void Timer1On(void);
void Timer1Off(void);
void Timer1Init(void);
uint32_t Timer1GetTick(void);
void Timer1ClcTick(uint32_t tick);

//extern volatile uint32_t ticks;//定时器2用

typedef struct TIMER1   // 4.0 LNG的Timer结构体
{
    uint32_t tick_100ms;
    uint32_t tick_start;
    uint32_t tick_current;
    uint8_t  flag_100ms;
    uint8_t  flag_500ms;
    uint8_t  flag_1s;
} TIMER1;

#if 0
struct TIMER1     //  2.7 LNG的Timer结构体
{
    uint32_t ticks;//定时器从开始到现在的值
    uint32_t starttime;//计时起始时间
    uint32_t currenttime;//当前计时值
    bool readCardFlag;//读卡标志
    bool readDs3232Flag;//读3232标志
    bool compareTimeFlag;//比对时间标志
    uint8_t flag;
    bool fixtime;
};
#endif
extern struct TIMER1 timer1;

#endif
