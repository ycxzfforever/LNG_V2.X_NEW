#ifndef __KEYBOARD_H
#define __KEYBOARD_H


enum  EVENT_SYS_ID
{
    SYS_NOEVENT = 0,
    KEY_0_EV, //键值0---1
    KEY_1_EV,
    KEY_2_EV,
    KEY_3_EV,
    KEY_4_EV,
    KEY_5_EV,
    KEY_6_EV,
    KEY_7_EV,
    KEY_8_EV,
    KEY_9_EV,//10
    KEY_START_EV,//加气---11
    KEY_STOP_EV,//停止---12
    KEY_PT_EV,//顿号----13
    KEY_OK_EV,//确认----14
    KEY_BACK_EV,//回显--15
    KEY_RET_EV,//返回---16
    KEY_CLR_EV,//清除---17
    KEY_FIX_EV,//定量---18
    KEY_SET_EV,//菜单---19
    KEY_QRY_EV,//查询--20
    KEY_COMB_1_EV, /*STOP+QRY查询--21*/
    KEY_COMB_2_EV, /*STOP+SET菜单--22*/
    KEY_COMB_3_EV, /*STOP+FIX定量*/
    KEY_COMB_4_EV, /*STOP+BACK回显--23*/
    KEY_COMB_5_EV, /*STOP+BACK清除--24*/
    KEY_COMB_6_EV, /*STOP+RET返回---25*/
    KEY_END,
    EV_CURSOR,
    EV_DISPTIME,
    EV_LOGOUT,    //系统登出
    MAX_EV_ID
};
/*
功能：涉及键盘的一些函数
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

#define KEY_QRY     0x00000008    //查询
#define KEY_SET     0x00000080    //设置
#define KEY_FIX     0x00080000    //定量
#define KEY_BACK    0x00004000    //回显（CNG、预冷（LNG））
#define KEY_CLR     0x00008000    //清除
#define KEY_START   0x00000100    //加气
#define KEY_STOP    0x00000200    //停止
#define KEY_OK      0x00000400    //确认
#define KEY_RET     0x00000800    //返回
#define KEY_PT      0x00002000    //小数点 

#define KEY_COMB_SQRY 	0x00000208 /*STOP+QRY查询*/
#define KEY_COMB_SSET 	0x00000280 /*STOP+SET设置*/
#define KEY_COMB_SFIX 	0x00080200 /*STOP+FIX定量*/
#define KEY_COMB_SBACK 	0x00004200 /*STOP+BACK回显*/
#define KEY_COMB_SCLR 	0x00008200 /*STOP+BACK清除*/
#define KEY_COMB_SRET 	0x00000A00 /*STOP+RET返回*/

#define KEY_COMB_SRETCLR 	0x00008A00	/*STOP+返回+清除*/ 		 //用于重启到应用程序
#define KEY_COMB_SRETPOINT 	0x00002A00	/*STOP+返回+小数点*/ //用于重启到BootLoader

#define KEY_MODULE_EV       KEY_COMB_1_EV
#define KEY_FACSET_EV       KEY_COMB_2_EV
//#define KEY_SYSQRY_EV       KEY_COMB_3_EV
#define KEY_EXSHIFTID_EV    KEY_COMB_4_EV
#define KEY_CARDFUNC_EV     KEY_COMB_5_EV


typedef struct def_Screeninput//主显输入
{
    uint8_t  inputlen;       //实时输入变化长度，长度从0开始计算
    uint8_t  inputmaxlen;    //最大输入长度，长度从0开始计算
    uint8_t  pointnum;       //小数点个数
    uint8_t  array[16];      //存储数据内容数组
} Screeninput;

extern  Screeninput ScreenInput;

void KeyBoardInit(void);
uint32_t KeyBoardReadKey(void);
void KeyBoardReadKeyValue(void);
void NumKeyHandler(void);


#endif
