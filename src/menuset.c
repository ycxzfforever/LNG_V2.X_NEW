#include "kernel.h"

fun         DispUI;         //界面刷新处理函数
fun         DispNextUI;     //用于消息显示后的下一个界面

/***************************************************************************
**	函数名称：TaskAdd(uint32_t keyvalue, fun funname1, fun funname2)
**	函数功能：事件处理
**	输入参数：keyvalue ：按键值
         	  funname1 ：对应按键需要执行的任务
         	  funname2 ：按键事情响应之后，紧接着需要执行的事情，可以为 NULL
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2016-7-25
***************************************************************************/
void TaskAdd(uint32_t keyvalue, fun funname1, fun funname2)
{
	//按键值等于设定值，执行funname1，如果funname2不为NULL，接着执行funname2
    if(globalvar.KeyValue == keyvalue)
    {
        globalvar.KeyValue = 0;
        DispUI = funname1; //界面刷新赋值 BY LY

        if(funname2 != NULL)
            DispNextUI = funname2; //界面刷新赋值 BY LY
        else
            DispNextUI = NULL;
    }
}

/***************************************************************************
**	函数名称：TaskMessage(uint32_t ms_100, fun funname)
**	函数功能：等待一定时间 ms_100，执行某个事件 funname
**	输入参数：ms_100 ：需要等待的时间，计数单位是 100ms
         	  funname： 等待这么长时间以后，需要执行的函数
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2016-7-25
***************************************************************************/
void TaskMessage(uint32_t ms_100, fun funname)
{
    if((globalvar.MsgTime == 0) && (globalvar.MsgFlag == 0))       //第一次添加msg任务
    {
        globalvar.MsgFlag = 1;
        globalvar.MsgTime = ms_100;
    }

    if((globalvar.MsgTime == 0) && (globalvar.MsgFlag == 1))       //时间到了过后处理msg任务
    {
        globalvar.MsgFlag = 0;
        DispUI = funname;
    }
}

/***************************************************************************
**	函数名称：SetSuccess(void)
**	函数功能：设置成功提示
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2016-7-25
***************************************************************************/
void SetSuccess(void)
{
    FYD12864DispPrintfFlash(4, 1, "设置成功");
}

/***************************************************************************
**	函数名称：SetFault(void)
**	函数功能：设置失败提示
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2016-7-25
***************************************************************************/
void SetFault(void)
{
    FYD12864DispPrintfFlash(4, 1, "设置失败");
}

/***************************************************************************
**	函数名称：DispSet(uint8_t numpoint, uint32_t data, char __farflash * buf)
**	函数功能：设置参数
**	输入参数：numpoint：小数点数；<5,表示输入带小数的数；5表示输入的非小数；6:密码；7：负数
		   	  data：	 输入参数值
		      buf：	 "需要设置的内容"，存于flash中
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSet(uint8_t numpoint, uint32_t data, char __farflash * buf)
{
    char buff[16];
    char tmpbuf[7] = {0};//6个放密码的"*",一个放字符串结束符
    char i;

    NumKeyHandler();
    globalvar.pointnum = 0;
	
	//保存设置参数原来的值
    globalvar.beforedex = data;

    if(numpoint < 5)   //有小数点输入(小数点后面的位数为0,2,4)
    {
		//提示设置内容
		FYD12864DispPrintfFlash(1, 1, buf);

		//输入的数据没有小数，若输入了小数，进行空字符处理 ADD BY LY
        if(numpoint == 0)  
        {
            if(ScreenInput.array[ScreenInput.inputlen - 1] == 0x2E)
            {
                ScreenInput.array[ScreenInput.inputlen - 1] = 0x20;
                ScreenInput.inputlen--;
            }

            globalvar.pointnum = numpoint; //主要用于主显输入的时候，小数点是否显示的判断 BY LY
            //保存设置参数现在的值
            globalvar.afterdex = atol((char *)ScreenInput.array);
			//没有小数部分，直接显示原来的值
            FYD12864DispPrintfFlash(2, 1, "旧值:%ld", data);
        }
        else if(numpoint == 2)
        {
            globalvar.pointnum = numpoint;
            sprintf_P(buff, "%f", atof((char *)ScreenInput.array) * 100);
            globalvar.afterdex = atol(buff);
			//显示原来的值：2位小数， 先将小数点显示出来，再分别显示小数点前后小数点后的置 BY LY
            FYD12864DispPrintfFlash(2, 1, "旧值:%ld.%02ld", data / 100, data % 100);   
        }
        else if(numpoint == 4)
        {
            globalvar.pointnum = numpoint;
			
            sprintf_P(buff, "%f", atof((char *)ScreenInput.array) * 10000);
            globalvar.afterdex = atol(buff);
			//显示原来的值：4位小数， 先将小数点显示出来，再分别显示小数点前后小数点后的置 BY LY
            FYD12864DispPrintfFlash(2, 1, "旧值:%ld.%04ld", data / 10000, data % 10000);
        }
		//显示新值
        FYD12864DispPrintfFlash(3, 1, "新值:%s", ScreenInput.array);
        FYD12864ClearLine(4);
    }
	
	//选择输入 只输入选项，没有小数点，故对小数点做无效处理 BY LY
    else  if(numpoint == 5)   
    {
        if(ScreenInput.array[ScreenInput.inputlen - 1] == 0x2E)
        {
            ScreenInput.array[ScreenInput.inputlen - 1] = 0x20;
            ScreenInput.inputlen--;
        }
		//保存设置参数现在的值
        globalvar.afterdex = atol((char *)ScreenInput.array);

		//显示提示信息
        strcpy_P(buff, buf);   //将far flash char*转换成char* ADD BY LY
        FYD12864DispPrintfFlash(1, 1, "%s %ld", buff, data);
		//显示现在的值
        FYD12864DispPrintfFlash(4, 1, "新值:%s", ScreenInput.array);
    }
	//密码输入 如果密码有小数点，则通过屏幕输入数组来显示 BY LY
    else  if(numpoint == 6)   
    {
    	//显示提示信息
        FYD12864DispPrintfFlash(1, 1, buf);   
    	//保存设置参数现在的值
        globalvar.afterdex = atol((char *)ScreenInput.array);

		//密码不直接显示，用"*"代替显示
        for(i = 0; i < ScreenInput.inputlen; i++)
        {
            tmpbuf[i] = '*';

        }
		//显示密码"*"
        FYD12864DispPrintfFlash(2, 1, "新密码:%s", tmpbuf);
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
    }
	//负数显示 如果有小数点输入，则通过屏幕输入数组来显示 ADD BY LY
    else  if(numpoint == 7)   
    {
    	//显示提示信息
        FYD12864DispString(1, 1, buf, 1);    
		//保存设置参数的新值
        globalvar.afterdex = atol((char *)ScreenInput.array);
		//显示设置参数原来的值
        FYD12864DispPrintfFlash(2, 1, "旧值:-%ld", data);
		//显示设置参数新的值
        FYD12864DispPrintfFlash(3, 1, "新值:%s", ScreenInput.array);
        FYD12864ClearLine(4);
    }
}

/***************************************************************************
**	函数名称：InputInit(void)
**	函数功能：初始化输入数据BUF
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void InputInit(void)
{
    memset(&ScreenInput, 0, sizeof(Screeninput));     //初始化ScreenInput 结构体为 0
    globalvar.displayonce = 1;
}

//确认处理
void DispOK(void)
{
    if(ScreenInput.inputlen > 0)
    {
        if(globalvar.displayonce == 1)
        {
            sysparas = tmp_sysparas;

            if(globalvar.paraindex == 0)   //只针对恢复出厂设置功能 ADD BY LY
            {
                if(SaveSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas)))
                {
                    FYD12864DispPrintfFlash(4, 1, "恢复出厂设置成功");
                }
                else
                {
                    FYD12864DispPrintfFlash(4, 1, "恢复出厂设置失败");
                }
            }
            else
            {
                if(SaveSignalSysparas(globalvar.paraindex, 1))
                {
                    SetSuccess();
                }
                else
                {
                    SetFault();
                }
            }
        }

        globalvar.displayonce = 0;

        if(DispNextUI != NULL)
        {
            TaskMessage(10, DispNextUI);
        }
    }
    else
    {
        SetFault();
        TaskMessage(10, DispNextUI);
    }
}

/***************************************************************************
**	函数名称：DispInputPwd(char __farflash* pwdbuf, fun funname)
**	函数功能：显示密码输入界面
**	输入参数：pwbuf : 待显示的与密码有关的提示内容如"菜单密码、设置密码、恢复出厂密码"等，在函数被调用时赋值
	          funname :密码输入以后，按确定键，需要执行的函数
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispInputPwd(char __farflash* pwdbuf, fun funname)
{
    char  buf[7] = {0};//buf用于存放输入的密码 BY LY
    uint8_t i;

    ScreenInput.inputmaxlen = 6; //输入长度为 6

    NumKeyHandler(); //数据处理

    FYD12864ClearLine(1);          //清第一行。
    FYD12864DispPrintfFlash(2, 1, pwdbuf);   //显示 pwdbuf所指向字符数组的内容
    FYD12864ClearLine(4);          //清第四行。

    for(i = 0; i < ScreenInput.inputlen; i++)
    {
        buf[i] = '*'; //根据inputlen中输入数据的个数，显示'*'的个数
    }

    FYD12864DispPrintf(3, 1, buf);
    //FYD12864DispPrintfFlash( 3, 1,buf); //显示 buf中的内容

    globalvar.U32Temp = atol((char *)ScreenInput.array);     //将ScreenInput.array中的字符串转换成长整型数据，存放于globalvar.U32Temp 中 为后续密码验证做准备 ADD BY LY

    TaskAdd(KEY_RET, DispFirst, NULL);   //按返回键，回到DsipFirst界面
    TaskAdd(KEY_OK, funname, NULL);   //按确认件执行funname所指向函数
}

/***************************************************************************
**	函数名称：DispInputMenuPwd(void)
**	函数功能：提示输入菜单密码
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispInputMenuPwd(void)
{
    DispInputPwd("请输入菜单密码", DispJudgeMenuPwd);
}

/***************************************************************************
**	函数名称：DispInputQcPwd(void)
**	函数功能：提示输入质检密码
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispInputQcPwd(void)
{
    DispInputPwd("请输入质检密码", DispJudgeQcPwd);
}

/***************************************************************************
**	函数名称：DispInputTestPwd(void)
**	函数功能：提示输入维护密码
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispInputTestPwd(void)
{
    DispInputPwd("请输入维护密码", DispJudgeTestPwd);
}

/***************************************************************************
**	函数名称：DispInputResetPwd(void)
**	函数功能：提示输入恢复出厂密码
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispInputResetPwd(void)
{
    DispInputPwd("请输入恢复密码", DispJudgeResetPwd);
}

/***************************************************************************
**	函数名称：DispJudgeMenuPwd(void)
**	函数功能：菜单密码验证
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispJudgeMenuPwd(void)
{
    if(globalvar.U32Temp != sysparas.menupwd)   //如果输入的密码与系统参数中的密码不一致，则提示密码错误  ADD BY LY
    {
        FYD12864DispPrintfFlash(4, 1, "密码错误");
        TaskAdd(KEY_CLR, DispInputMenuPwd, NULL);
        TaskAdd(KEY_RET, DispInputMenuPwd, NULL);
    }
    else
    {
        DispUI = DispMenu_1; //密码输入正确，进入下一个菜单
    }
}

/***************************************************************************
**	函数名称：DispJudgeQcPwd(void)
**	函数功能：质检密码验证
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispJudgeQcPwd(void)
{
    if(globalvar.U32Temp != sysparas.qcpwd)
    {
        FYD12864DispPrintfFlash(4, 1, "密码错误");
        TaskAdd(KEY_CLR, DispInputQcPwd, NULL);
        TaskAdd(KEY_RET, DispInputQcPwd, NULL);
    }
    else
    {
        DispUI = DispQcPara;
    }
}

/***************************************************************************
**	函数名称：DispJudgeTestPwd(void)
**	函数功能：维护密码验证
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispJudgeTestPwd(void)
{
    if(globalvar.U32Temp != sysparas.testpwd)
    {
        FYD12864DispPrintfFlash(4, 1, "密码错误");
        TaskAdd(KEY_CLR, DispInputTestPwd, NULL);
        TaskAdd(KEY_RET, DispInputTestPwd, NULL);
    }
    else
    {
        DispUI = DeviceTestMenu;
    }
}

/***************************************************************************
**	函数名称：DispJudgeResetPwd(void)
**	函数功能：恢复出厂设置密码验证
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispJudgeResetPwd(void)
{
    if((globalvar.U32Temp != sysparas.resetpwd) && (globalvar.U32Temp != 760103))       //此处，若输入密码与系统中的恢复出厂密码相同或输入密码为760103，都可恢复出厂设置
    {
        FYD12864DispPrintfFlash(4, 1, "密码错误");
        TaskAdd(KEY_CLR, DispInputResetPwd, NULL);
        TaskAdd(KEY_RET, DispInputResetPwd, NULL);
    }
    else
    {
        DispUI = DispUnitQcReset;
    }
}

/***************************************************************************
**	函数名称：DispMenu_1(void)
**	函数功能：菜单显示界面1
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispMenu_1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.单价设置");
    FYD12864DispPrintfFlash(2, 1, "2.系统参数设置");
    FYD12864DispPrintfFlash(3, 1, "3.班组信息设置");
    FYD12864DispPrintfFlash(4, 1, "4.IC卡参数设置");

    TaskAdd(KEY_1, DispSetPrice, NULL);
    TaskAdd(KEY_2, DispSysSet1, NULL);
    TaskAdd(KEY_3, DispSetShiftInfo1, NULL);
    TaskAdd(KEY_4, DispSetICInfo1, NULL);
    TaskAdd(KEY_DOWN, DispMenu_2, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**	函数名称：DispSetDefPrice(void)
**	函数功能：默认单价设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetDefPrice(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);       //将atof()中的数据按照规定的格式打印到buf数组中 BY LY
    tmp_sysparas.price[0] = atol(buf);   //atol()将字符串转换成长整型数 BY LY
    globalvar.paraindex = 7;

    if(tmp_sysparas.price[0] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[0], "默认单价设置");
    TaskAdd(KEY_RET, DispSetPrice, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetPrice);
}

//单价设置
/***************************************************************************
**	函数名称：DispSetPrice(void)
**	函数功能：单价设置选择界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.默认单价设置");
    FYD12864DispPrintfFlash(2, 1, "2.优惠单价设置");
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetDefPrice, NULL);
    TaskAdd(KEY_2, DispSetOthPrice1, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetPrice1(void)
**	函数功能：优惠单价1设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice1(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[1] = atol(buf);
    globalvar.paraindex = 8;

    if(tmp_sysparas.price[1] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[1], "用户卡单价");
    TaskAdd(KEY_RET, DispSetOthPrice1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice1);
}

/***************************************************************************
**	函数名称：DispSetPrice2(void)
**	函数功能：优惠单价2设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice2(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[2] = atol(buf);
    globalvar.paraindex = 9;

    if(tmp_sysparas.price[2] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[2], "员工卡单价");
    TaskAdd(KEY_RET, DispSetOthPrice1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice1);
}

/***************************************************************************
**	函数名称：DispSetPrice3(void)
**	函数功能：优惠单价3设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice3(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[3] = atol(buf);
    globalvar.paraindex = 10;

    if(tmp_sysparas.price[3] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[3], "记账卡单价");
    TaskAdd(KEY_RET, DispSetOthPrice1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice1);
}

/***************************************************************************
**	函数名称：DispSetPrice4(void)
**	函数功能：优惠单价4设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice4(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[4] = atol(buf);

    if(tmp_sysparas.price[4] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[4], "维护卡单价");
    globalvar.paraindex = 11;
    TaskAdd(KEY_RET, DispSetOthPrice1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice1);
}

/***************************************************************************
**	函数名称：DispSetPrice5(void)
**	函数功能：优惠单价5设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice5(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[5] = atol(buf);

    if(tmp_sysparas.price[5] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[5], "内部卡单价");
    globalvar.paraindex = 12;
    TaskAdd(KEY_RET, DispSetOthPrice2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice2);
}

/***************************************************************************
**	函数名称：DispSetPrice6(void)
**	函数功能：优惠单价6设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice6(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[6] = atol(buf);

    if(tmp_sysparas.price[6] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[6], "积分卡单价");
    globalvar.paraindex = 13;
    TaskAdd(KEY_RET, DispSetOthPrice2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice2);
}

/***************************************************************************
**	函数名称：DispSetPrice7(void)
**	函数功能：优惠单价7设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice7(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[7] = atol(buf);

    if(tmp_sysparas.price[7] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[7], "VIP 卡单价");
    globalvar.paraindex = 14;
    TaskAdd(KEY_RET, DispSetOthPrice2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice2);
}

/***************************************************************************
**	函数名称：DispSetPrice8(void)
**	函数功能：优惠单价8设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice8(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[8] = atol(buf);

    if(tmp_sysparas.price[8] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[8], "备用卡2 单价");
    globalvar.paraindex = 15;
    TaskAdd(KEY_RET, DispSetOthPrice2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice2);
}

/***************************************************************************
**	函数名称：DispSetPrice9(void)
**	函数功能：优惠单价9设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrice9(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.price[9] = atol(buf);

    if(tmp_sysparas.price[9] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "单价超限");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[9], "备用卡3 单价");
    globalvar.paraindex = 16;
    TaskAdd(KEY_RET, DispSetOthPrice3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice3);
}


/***************************************************************************
**	函数名称：DispSetOthPrice1(void)
**	函数功能：优惠单价设置选择界面1
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetOthPrice1(void)
{
    InputInit();

    FYD12864DispPrintfFlash(1, 1, "1.用户卡 %d.%02d", sysparas.price[1] / 100, sysparas.price[1] % 100);
    FYD12864DispPrintfFlash(2, 1, "2.员工卡 %d.%02d", sysparas.price[2] / 100, sysparas.price[2] % 100);
    FYD12864DispPrintfFlash(3, 1, "3.记账卡 %d.%02d", sysparas.price[3] / 100, sysparas.price[3] % 100);
    FYD12864DispPrintfFlash(4, 1, "4.维护卡 %d.%02d", sysparas.price[4] / 100, sysparas.price[4] % 100);

    TaskAdd(KEY_1, DispSetPrice1, NULL);
    TaskAdd(KEY_2, DispSetPrice2, NULL);
    TaskAdd(KEY_3, DispSetPrice3, NULL);
    TaskAdd(KEY_4, DispSetPrice4, NULL);
    TaskAdd(KEY_DOWN, DispSetOthPrice2, NULL);
    TaskAdd(KEY_RET, DispSetPrice, NULL);
}

/***************************************************************************
**	函数名称：DispSetOthPrice2(void)
**	函数功能：优惠单价设置选择界面2
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetOthPrice2(void)
{
    InputInit();

    FYD12864DispPrintfFlash(1, 1, "1.内部卡  %d.%02d", sysparas.price[5] / 100, sysparas.price[5] % 100);
    FYD12864DispPrintfFlash(2, 1, "2.积分卡  %d.%02d", sysparas.price[6] / 100, sysparas.price[6] % 100);
    FYD12864DispPrintfFlash(3, 1, "3.VIP 卡  %d.%02d", sysparas.price[7] / 100, sysparas.price[7] % 100);
    FYD12864DispPrintfFlash(4, 1, "4.备用卡2 %d.%02d", sysparas.price[8] / 100, sysparas.price[8] % 100);

    TaskAdd(KEY_1, DispSetPrice5, NULL);
    TaskAdd(KEY_2, DispSetPrice6, NULL);
    TaskAdd(KEY_3, DispSetPrice7, NULL);
    TaskAdd(KEY_4, DispSetPrice8, NULL);
    TaskAdd(KEY_UP, DispSetOthPrice1, NULL);
    TaskAdd(KEY_DOWN, DispSetOthPrice3, NULL);
    TaskAdd(KEY_RET, DispSetPrice, NULL);
}

/***************************************************************************
**	函数名称：DispSetOthPrice3(void)
**	函数功能：优惠单价设置选择界面3
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetOthPrice3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.备用卡3 %d.%02d", sysparas.price[9] / 100, sysparas.price[9] % 100);
    FYD12864ClearLine(2);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetPrice9, NULL);
    TaskAdd(KEY_UP, DispSetOthPrice2, NULL);
    TaskAdd(KEY_RET, DispSetPrice, NULL);
}

/***************************************************************************
**	函数名称：DispSetUnit(void)
**	函数功能：计量单位设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetUnit(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 1) || (tmp == 2))
    {
        tmp_sysparas.unit = tmp;
        globalvar.paraindex = 28;
        TaskAdd(KEY_OK, DispOK, DispUnitQcReset);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.unit, "计量单位设置");
    FYD12864DispPrintfFlash(2, 1, "1.公斤");
    FYD12864DispPrintfFlash(3, 1, "2.标方");
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);

}

/***************************************************************************
**	函数名称：DispSetQCMode(void)
**	函数功能：质检模式设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetQCMode(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1) || (tmp == 2) || (tmp == 3))
    {
        tmp_sysparas.qcmode = tmp;
        globalvar.paraindex = 29;
        TaskAdd(KEY_OK, DispOK, DispUnitQcReset);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.qcmode, "质检模式设置");

    FYD12864DispPrintfFlash(2, 1, "0.密码  1.手持机");
    FYD12864DispPrintfFlash(3, 1, "2.IC卡  3.加密板");
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);

}

/***************************************************************************
**	函数名称：DispEraseChip(void)
**	函数功能：Flash存储芯片擦除选择界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispEraseChip(void)
{
    globalvar.displayonce = 1;

    FYD12864DispPrintfFlash(1, 1, "擦除整个FLASH");
    FYD12864DispPrintfFlash(2, 1, "【确认】  继续");
    FYD12864DispPrintfFlash(3, 1, "【返回】  取消");
    FYD12864ClearLine(4);

    TaskAdd(KEY_OK, JS28F128FlashEraseAll, NULL);   //需要添加擦除flash的代码 ADD BY LY
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);
}

/***************************************************************************
**	函数名称：DispFactoryReset(void)
**	函数功能：恢复出厂设置选择界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispFactoryReset(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "  恢复出厂设置");
    FYD12864DispPrintfFlash(2, 1, "【确认】  继续");
    FYD12864DispPrintfFlash(3, 1, "【返回】  取消");
    FYD12864ClearLine(4);
    FactoryReset();
    globalvar.paraindex = 0;

    if(globalvar.KeyValue == KEY_OK)
        ScreenInput.inputlen = 1;

    TaskAdd(KEY_OK, DispOK, SystemReStart);
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);
}

/***************************************************************************
**	函数名称：DispUnitQcReset(void)
**	函数功能：计量单位、质检模式、恢复出厂设置选择界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispUnitQcReset(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.计量单位设置");
    FYD12864DispPrintfFlash(2, 1, "2.质检模式设置");
    FYD12864DispPrintfFlash(3, 1, "3.存储芯片擦除");
    FYD12864DispPrintfFlash(4, 1, "0.恢复出厂设置");
    TaskAdd(KEY_1, DispSetUnit, NULL);
    TaskAdd(KEY_2, DispSetQCMode, NULL);
    TaskAdd(KEY_3, DispEraseChip, NULL);
    TaskAdd(KEY_0, DispFactoryReset, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**	函数名称：DispSetGunNum(void)
**	函数功能：枪号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetGunNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 2;
    tmp_sysparas.gunnum = atol((char *)ScreenInput.array);

    if((tmp_sysparas.gunnum > 0) && (tmp_sysparas.gunnum < 100))       //枪号只能取1-99 ADD BY LY
    {
        TaskAdd(KEY_OK, DispOK, DispSysSet1);
    }

    DispSet(0, sysparas.gunnum, "枪号设置");
    globalvar.paraindex = 2;
    TaskAdd(KEY_RET, DispSysSet1, NULL);
    //TaskAdd( KEY_OK, DispOK, DispSysSet1 );
}

/***************************************************************************
**	函数名称：DispSetTotvolume(void)
**	函数功能：总气量设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetTotvolume(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.totvolume = atol(buf);
    globalvar.paraindex = 5;
    DispSet(2, sysparas.totvolume, "总气量设置");
    TaskAdd(KEY_RET, DispSysSet1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet1);
}

/***************************************************************************
**	函数名称：DispSetTotmoney(void)
**	函数功能：总金额设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetTotmoney(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.totmoney = atol(buf);
    globalvar.paraindex = 6;
    DispSet(2, sysparas.totmoney, "总金额设置");
    TaskAdd(KEY_RET, DispSysSet1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet1);
}

/***************************************************************************
**	函数名称：DispSetModbusType(void)
**	函数功能：流量计类型设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetModbusType(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 1) || (tmp == 2))
    {
        tmp_sysparas.modbustype = tmp;
        globalvar.paraindex = 27;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.modbustype, "流量计类型设置");
    FYD12864DispPrintfFlash(2, 1, "1.罗斯蒙特");
    FYD12864DispPrintfFlash(3, 1, "2.E+H");
    TaskAdd(KEY_RET, DispSysSet1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet1);
}

/***************************************************************************
**	函数名称：DispSysSet1(void)
**	函数功能：系统参数设置界面1
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSysSet1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.枪号");
    FYD12864DispPrintfFlash(2, 1, "2.总气量");
    FYD12864DispPrintfFlash(3, 1, "3.总金额");
    FYD12864DispPrintfFlash(4, 1, "4.流量计类型");

    TaskAdd(KEY_1, DispSetGunNum, NULL);
    TaskAdd(KEY_2, DispSetTotvolume, NULL);
    TaskAdd(KEY_3, DispSetTotmoney, NULL);
    TaskAdd(KEY_4, DispSetModbusType, NULL);
    TaskAdd(KEY_DOWN, DispSysSet2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetTransnum(void)
**	函数功能：流水号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetTransnum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    tmp_sysparas.transnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 3;
    DispSet(0, sysparas.transnum, "流水号设置");
    TaskAdd(KEY_RET, DispSysSet2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet2);
}

/***************************************************************************
**	函数名称：DispSetUpTransnum(void)
**	函数功能：已传流水号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetUpTransnum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    tmp_sysparas.uptransnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 4;
    DispSet(0, sysparas.uptransnum, "已传流水号设置");
    TaskAdd(KEY_RET, DispSysSet2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet2);
}

/***************************************************************************
**	函数名称：DispSetModnum(void)
**	函数功能：修改记录流水号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetModnum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    tmp_sysparas.modnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 34;
    DispSet(0, sysparas.modnum, "修改流水号设置");
    TaskAdd(KEY_RET, DispSysSet2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet2);
}

/***************************************************************************
**	函数名称：DispSetUpModnum(void)
**	函数功能：已传参数修改流水号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSetUpModnum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    tmp_sysparas.upmodnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 35;
    DispSet(0, sysparas.upmodnum, "已传修改号设置");
    TaskAdd(KEY_RET, DispSysSet2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet2);
}

/***************************************************************************
**	函数名称：DispSysSet2(void)
**	函数功能：系统参数设置界面2
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSysSet2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.流水号");
    FYD12864DispPrintfFlash(2, 1, "2.已传流水号");
    FYD12864DispPrintfFlash(3, 1, "3.修改记录流水号");
    FYD12864DispPrintfFlash(4, 1, "4.已传修改流水号");

    TaskAdd(KEY_1, DispSetTransnum, NULL);
    TaskAdd(KEY_2, DispSetUpTransnum, NULL);
    TaskAdd(KEY_3, DispSetModnum, NULL);
    TaskAdd(KEY_4, DispSetUpModnum, NULL);
    TaskAdd(KEY_UP, DispSysSet1, NULL);
    TaskAdd(KEY_DOWN, DispSysSet3, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetModbusAdr_G(void)
**	函数功能：气相流量计地址设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetModbusAdr_G(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    ScreenInput.pointnum = 0;
    tmp_sysparas.modbusaddr_g = atol((char *)ScreenInput.array);
    globalvar.paraindex = 215;

    if((tmp_sysparas.modbusaddr_g > 0) && (tmp_sysparas.modbusaddr_g < 255))
    {
        TaskAdd(KEY_OK, DispOK, DispSysSet3);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(0, sysparas.modbusaddr_g, "气相流量计地址");

    TaskAdd(KEY_RET, DispSysSet3, NULL);
}

/***************************************************************************
**	函数名称：DispSetModbusAdr_L(void)
**	函数功能：液相流量计地址设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetModbusAdr_L(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    ScreenInput.pointnum = 0;
    tmp_sysparas.modbusaddr_l = atol((char *)ScreenInput.array);
    globalvar.paraindex = 216;

    if((tmp_sysparas.modbusaddr_l > 0) && (tmp_sysparas.modbusaddr_l < 255))
    {
        TaskAdd(KEY_OK, DispOK, DispSysSet3);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(0, sysparas.modbusaddr_l, "液相流量计地址");

    TaskAdd(KEY_RET, DispSysSet3, NULL);

}

/***************************************************************************
**	函数名称：DispSetModbusRS485(void)
**	函数功能：流量计地址总线设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetModbusRS485(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 1) || (tmp == 2))
    {
        tmp_sysparas.modbusRS485 = tmp;
        globalvar.paraindex = 217;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.modbusRS485, "流量计总线设置");
    FYD12864DispPrintfFlash(2, 1, "1.气液相合并");
    FYD12864DispPrintfFlash(3, 1, "2.气液相分开");
    TaskAdd(KEY_RET, DispSysSet3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet3);
}

/***************************************************************************
**	函数名称：DispSysSet3(void)
**	函数功能：系统参数设置界面4
**	输入参数：无
**	返 回 值：无

**	创 建 者：
**	创建日期：2018-9-17
***************************************************************************/
void DispSysSet3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.气相流量计地址");
    FYD12864DispPrintfFlash(2, 1, "2.液相流量计地址");
    FYD12864DispPrintfFlash(3, 1, "3.流量计地址总线");
    FYD12864ClearLine(4);

    TaskAdd(KEY_1, DispSetModbusAdr_G, NULL);
    TaskAdd(KEY_2, DispSetModbusAdr_L, NULL);
    TaskAdd(KEY_3, DispSetModbusRS485, NULL);
    TaskAdd(KEY_UP, DispSysSet2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetShiftNum(void)
**	函数功能：班号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.shiftnum = atol((char *)ScreenInput.array);

    //输入的班号需在0到shiftmaxnum之间 ADD BY LY
    if((tmp_sysparas.shiftnum > 0) && (tmp_sysparas.shiftnum <= tmp_sysparas.shiftmaxnum))
    {
    	globalvar.U8Temp = sysparas.shiftnum;
        TaskAdd(KEY_OK, DispOK, DispSetShiftInfo1);
    }
    else if(tmp_sysparas.shiftnum > tmp_sysparas.shiftmaxnum)
    {
        FYD12864DispPrintfFlash(4, 1, "班号超限");
        NumKeyHandler();
        return;
    }

    globalvar.paraindex = 37;
    DispSet(0, sysparas.shiftnum, "班号设置");
    TaskAdd(KEY_RET, DispSetShiftInfo1, NULL);
}

/***************************************************************************
**	函数名称：DispSetShiftMaxNum(void)
**	函数功能：最大班号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftMaxNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 2;
    tmp_sysparas.shiftmaxnum = atol((char *)ScreenInput.array);

    if(tmp_sysparas.shiftmaxnum > 0)
    {
        TaskAdd(KEY_OK, DispOK, DispSetShiftInfo1);
    }

    globalvar.paraindex = 36;
    DispSet(0, sysparas.shiftmaxnum, "最大班号设置");
    TaskAdd(KEY_RET, DispSetShiftInfo1, NULL);
}


/***************************************************************************
**	函数名称：DispSetShiftTransNum(void)
**	函数功能：班流水号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftTransNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    tmp_sysparas.shiftransnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 38;
    DispSet(0, sysparas.shiftransnum, "班流水号设置");
    TaskAdd(KEY_RET, DispSetShiftInfo1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo1);
}

/***************************************************************************
**	函数名称：DispSetShiftUpTransNum(void)
**	函数功能：已传班流水号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftUpTransNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    tmp_sysparas.shiftuptransnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 39;
    DispSet(0, sysparas.shiftuptransnum, "已传班流水号设置");
    TaskAdd(KEY_RET, DispSetShiftInfo1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo1);
}

/***************************************************************************
**	函数名称：DispSetShiftInfo1(void)
**	函数功能：班组信息设置界面1
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftInfo1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.班号");
    FYD12864DispPrintfFlash(2, 1, "2.最大班号");
    FYD12864DispPrintfFlash(3, 1, "3.班流水号");
    FYD12864DispPrintfFlash(4, 1, "4.已传班流水号");
    TaskAdd(KEY_1, DispSetShiftNum, NULL);
    TaskAdd(KEY_2, DispSetShiftMaxNum, NULL);
    TaskAdd(KEY_3, DispSetShiftTransNum, NULL);
    TaskAdd(KEY_4, DispSetShiftUpTransNum, NULL);
    //TaskAdd(KEY_UP, DispSetShiftInfo3,NULL);
    TaskAdd(KEY_DOWN, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetShiftStartvol(void)
**	函数功能：班开始枪累设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftStartvol(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.shiftstartvol = atol(buf);
    globalvar.paraindex = 42;
    DispSet(2, sysparas.shiftstartvol, "班开始枪累设置");
    TaskAdd(KEY_RET, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo2);
}

/***************************************************************************
**	函数名称：DispSetShiftStartmon(void)
**	函数功能：班开始金额设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftStartmon(void)
{
    char buf[16] = {0};

    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.shiftstartmon = atol(buf);
    globalvar.paraindex = 44;
    DispSet(2, sysparas.shiftstartmon, "班开始金额设置");

    TaskAdd(KEY_RET, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo2);
}

/***************************************************************************
**	函数名称：DispSetShiftTotvol(void)
**	函数功能：班累计气量设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftTotvol(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.shiftotvol = atol(buf);
    globalvar.paraindex = 46;
    DispSet(2, sysparas.shiftotvol, "班累计气量设置");
    TaskAdd(KEY_RET, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo2);
}

/***************************************************************************
**	函数名称：DispSetShiftTotmon(void)
**	函数功能：班累计金额设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftTotmon(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.shiftotmon = atol(buf);
    globalvar.paraindex = 47;
    DispSet(2, sysparas.shiftotmon, "班累计金额设置");
    TaskAdd(KEY_RET, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo2);
}

/***************************************************************************
**	函数名称：DispSetShiftInfo2(void)
**	函数功能：班组信息设置界面2
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftInfo2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.班开始枪累");
    FYD12864DispPrintfFlash(2, 1, "2.班开始金额");
    FYD12864DispPrintfFlash(3, 1, "3.班累计气量");
    FYD12864DispPrintfFlash(4, 1, "4.班累计金额");

    TaskAdd(KEY_1, DispSetShiftStartvol, NULL);
    TaskAdd(KEY_2, DispSetShiftStartmon, NULL);
    TaskAdd(KEY_3, DispSetShiftTotvol, NULL);
    TaskAdd(KEY_4, DispSetShiftTotmon, NULL);
    TaskAdd(KEY_UP, DispSetShiftInfo1, NULL);
    TaskAdd(KEY_DOWN, DispSetShiftInfo3, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetShiftFuelTimes(void)
**	函数功能：班加气次数设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftFuelTimes(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    tmp_sysparas.shiftfueltimes = atol((char *)ScreenInput.array);
    globalvar.paraindex = 49;
    DispSet(0, sysparas.shiftfueltimes, "班加气次数设置");
    TaskAdd(KEY_RET, DispSetShiftInfo3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo3);
}

/***************************************************************************
**	函数名称：DispSetShiftStartDate(void)
**	函数功能：班开始日期设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftStartDate(void)
{
    uint8_t *tmp, i, ret = true;

    if(globalvar.KeyValue != KEY_PT)
    {
        tmp_sysparas = sysparas;
        NumKeyHandler();
        ScreenInput.inputmaxlen = 6;
        tmp = (uint8_t*)&tmp_sysparas.shiftstarttime;

        for(i = 0; i < (ScreenInput.inputlen / 2 + 1) && i < 3; i++)
        {
            *(tmp + i) = AscToBcd(&ScreenInput.array[2 * i]);
        }

        FYD12864DispPrintfFlash(1, 1, "班开始日期设置");
        FYD12864DispPrintfFlash(2, 1, "旧值: %02X年%02X月%02X日",
                                sysparas.shiftstarttime[0],
                                sysparas.shiftstarttime[1],
                                sysparas.shiftstarttime[2]);


        FYD12864DispPrintfFlash(3, 1, "新值: %02X年%02X月%02X日",
                                tmp_sysparas.shiftstarttime[0],
                                tmp_sysparas.shiftstarttime[1],
                                tmp_sysparas.shiftstarttime[2]);

        if(ScreenInput.inputlen > 5) ret = CheckDate(tmp);

        if(ret == false)
        {
            FYD12864DispPrintfFlash(4, 1, "输入格式错误");
            return;
        }
        else
        {
            FYD12864ClearLine(4);
        }

        TaskAdd(KEY_RET, DispSetShiftInfo3, NULL);

        if(ScreenInput.inputlen == 6)   //输入完毕后才能按确认键
        {
            globalvar.paraindex = 40;
            TaskAdd(KEY_OK, DispOK, DispSetShiftInfo3);
        }
    }
}

/***************************************************************************
**	函数名称：DispSetShiftStartTime(void)
**	函数功能：班开始时间设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftStartTime(void)
{
    uint8_t *tmp, i, ret = true;

    if(globalvar.KeyValue != KEY_PT)
    {
        tmp_sysparas = sysparas;
        NumKeyHandler();
        ScreenInput.inputmaxlen = 6;
        tmp = (uint8_t*)&tmp_sysparas.shiftstarttime[3];

        for(i = 0; i < (ScreenInput.inputlen / 2 + 1) && i < 3; i++)
        {
            *(tmp + i) = AscToBcd(&ScreenInput.array[2 * i]);
        }

        FYD12864DispPrintfFlash(1, 1, "班开始时间设置");
        FYD12864DispPrintfFlash(2, 1, "旧值: %02X:%02X:%02X",
                                sysparas.shiftstarttime[3],
                                sysparas.shiftstarttime[4],
                                sysparas.shiftstarttime[5]);
        FYD12864DispPrintfFlash(3, 1, "新值: %02X:%02X:%02X",
                                tmp_sysparas.shiftstarttime[3],
                                tmp_sysparas.shiftstarttime[4],
                                tmp_sysparas.shiftstarttime[5]);



        if(ScreenInput.inputlen > 5) ret = CheckTime(tmp);

        if(ret == false)
        {
            FYD12864DispPrintfFlash(4, 1, "输入格式错误");
            return;
        }
        else
        {
            FYD12864ClearLine(4);
        }

        TaskAdd(KEY_RET, DispSetShiftInfo3, NULL);

        if(ScreenInput.inputlen == 6)   //输入完毕后才能按确认键
        {
            globalvar.paraindex = 40;
            TaskAdd(KEY_OK, DispOK, DispSetShiftInfo3);
        }
    }
}

/***************************************************************************
**	函数名称：DispSetShiftInfo3(void)
**	函数功能：班组信息设置界面3
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShiftInfo3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.班加气次数");
    FYD12864DispPrintfFlash(2, 1, "2.班开始日期");
    FYD12864DispPrintfFlash(3, 1, "3.班开始时间");
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetShiftFuelTimes, NULL);
    TaskAdd(KEY_2, DispSetShiftStartDate, NULL);
    TaskAdd(KEY_3, DispSetShiftStartTime, NULL);
    TaskAdd(KEY_UP, DispSetShiftInfo2, NULL);
    //TaskAdd(KEY_DOWN, DispSetShiftInfo1,NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetCardMaxYuE(void)
**	函数功能：储蓄卡余额上限设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetCardMaxYuE(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.cardmaxyue = atol(buf);

    if(tmp_sysparas.cardmaxyue <= 100000000)
    {
        TaskAdd(KEY_OK, DispOK, DispSetICInfo1);
    }
    else
    {
        FYD12864DispPrintfFlash(4, 1, "储蓄卡余额超限");
        NumKeyHandler();
        return;
    }

    globalvar.paraindex = 55;
    DispSet(2, sysparas.cardmaxyue, "储蓄卡额上限设置");
    TaskAdd(KEY_RET, DispSetICInfo1, NULL);

}

/***************************************************************************
**	函数名称：DispSetCardMinYuE(void)
**	函数功能：储蓄卡余额下限设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetCardMinYuE(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.cardminyue = atol(buf);
    globalvar.paraindex = 56;
    DispSet(2, sysparas.cardminyue, "储蓄卡额下限设置");
    TaskAdd(KEY_RET, DispSetICInfo1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo1);
}

/***************************************************************************
**	函数名称：DispSetAccountCardMaxYuE(void)
**	函数功能：记账卡余额上限设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetAccountCardMaxYuE(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.accountcardmaxyue = atol(buf);

    if(tmp_sysparas.accountcardmaxyue <= 100000000)
    {
        TaskAdd(KEY_OK, DispOK, DispSetICInfo1);
    }
    else
    {
        FYD12864DispPrintfFlash(4, 1, "记账卡余额超限");
        NumKeyHandler();
        return;
    }

    globalvar.paraindex = 57;
    DispSet(2, sysparas.accountcardmaxyue, "记账卡额上限设置");
    TaskAdd(KEY_RET, DispSetICInfo1, NULL);

}

/***************************************************************************
**	函数名称：DispSetAccountCardMaxYuE(void)
**	函数功能：PSAM卡座选择
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPsamNum(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 1) || (tmp == 2) || (tmp == 3) || (tmp == 4))
    {
        tmp_sysparas.cardpsamnum = tmp;
        globalvar.paraindex = 58;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(0, sysparas.cardpsamnum, "PSAM卡座选择 1-4");
    TaskAdd(KEY_RET, DispSetICInfo3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo3);
}

/***************************************************************************
**	函数名称：DispSetCardMaxDay(void)
**	函数功能：IC卡未使用最大间隔时间设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetCardMaxDay(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.cardmaxday = atol((char *)ScreenInput.array);
    globalvar.paraindex = 59;
    DispSet(0, sysparas.cardmaxday, "最大间隔天数");
    TaskAdd(KEY_RET, DispSetICInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo2);
}

/***************************************************************************
**	函数名称：DispSetUseCard(void)
**	函数功能：是否用卡设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetUseCard(void)
{
    uint8_t tmp;

    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if(tmp < 3)
    {
        tmp_sysparas.usecard = tmp;
        globalvar.paraindex = 61;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.usecard, "是否用卡");
    FYD12864DispPrintfFlash(2, 1, "0.可不使用卡");
    FYD12864DispPrintfFlash(3, 1, "1.用卡  2.无卡");

    TaskAdd(KEY_RET, DispSetICInfo1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo1);
}

/***************************************************************************
**	函数名称：DispSetICInfo1(void)
**	函数功能：IC卡信息设置界面1
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetICInfo1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.储蓄卡余额上限");
    FYD12864DispPrintfFlash(2, 1, "2.储蓄卡余额下限");
    FYD12864DispPrintfFlash(3, 1, "3.记账卡余额上限");
    FYD12864DispPrintfFlash(4, 1, "4.是否用卡");

    TaskAdd(KEY_1, DispSetCardMaxYuE, NULL);
    TaskAdd(KEY_2, DispSetCardMinYuE, NULL);
    TaskAdd(KEY_3, DispSetAccountCardMaxYuE, NULL);   //ycx20161202
    TaskAdd(KEY_4, DispSetUseCard, NULL);
    TaskAdd(KEY_DOWN, DispSetICInfo2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetCardCheck(void)
**	函数功能：是否检验钢瓶选择
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetCardCheck(void)
{
    uint8_t tmp;

    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;

    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1))
    {
        tmp_sysparas.cardcheck = tmp;
        globalvar.paraindex = 60;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.cardcheck, "是否检验钢瓶");
    FYD12864DispPrintfFlash(2, 1, "0.不检验");
    FYD12864DispPrintfFlash(3, 1, "1.检验");
    TaskAdd(KEY_RET, DispSetICInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo2);
}

/***************************************************************************
**	函数名称：DispSetKeybOK(void)
**	函数功能：设置参数完成，设置结果提示
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetKeybOK(void)
{
    if(EepromInterWriteBytes(LOCKVER_ADDR, &m1card.newkeyb[0], 6))     //在LOCKVER_ADDR位置保存新的B密 ADD BY LY
    {
        FYD12864DispPrintfFlash(4, 1, "B 密设置成功");
    }
    else
    {
        FYD12864DispPrintfFlash(4, 1, "B 密设置失败");
    }

    if(DispNextUI != NULL)
    {
        TaskMessage(10, DispNextUI);
    }
}

/***************************************************************************
**	函数名称：DispSetKeyB(void)
**	函数功能：IC卡B密设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：LY
**	创建日期：2018-9-17
***************************************************************************/
void DispSetKeyB(void)
{
	
    ScreenInput.inputmaxlen = 6;
    FYD12864DispPrintfFlash(1, 1, "请输入卡B 密:");

    NumKeyHandler();

    if(ScreenInput.array[ScreenInput.inputlen - 1] == 0x2E)
    {
        ScreenInput.array[ScreenInput.inputlen - 1] = 0x20;
        ScreenInput.inputlen--;
    }

    if(ScreenInput.inputmaxlen > 5)
    {
        memcpy(&m1card.newkeyb[0], &ScreenInput.array[0], 6);
        TaskAdd(KEY_OK, DispSetKeybOK, DispSetICInfo2);
    }
    else
    {
        FYD12864DispPrintfFlash(4, 1, "密码输入错误");
        NumKeyHandler();
        return;
    }

    FYD12864DispPrintfFlash(2, 1, "密码:%s", m1card.newkeyb);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_RET, DispSetICInfo2, NULL);
}

/***************************************************************************
**	函数名称：DispSetLossCard(void)
**	函数功能：清除所有挂失卡标记选择界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：LY
**	创建日期：2018-9-17
***************************************************************************/
void DispSetLossCard(void)
{

    FYD12864DispPrintfFlash(1, 1, "按【确认】清除所");
    FYD12864DispPrintfFlash(2, 1, "有挂失卡记录");
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);


    TaskAdd(KEY_OK, ClearLossCard, NULL);
    TaskAdd(KEY_RET, DispSetICInfo2, NULL);
}

/***************************************************************************
**	函数名称：DispSetICInfo2(void)
**	函数功能：IC卡信息设置界面2
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetICInfo2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.是否检验钢瓶");
    FYD12864DispPrintfFlash(2, 1, "2.最大间隔天数");
    FYD12864DispPrintfFlash(3, 1, "3.清除所有挂失卡");
    FYD12864DispPrintfFlash(4, 1, "4.B 密设置");

    TaskAdd(KEY_1, DispSetCardCheck, NULL);
    TaskAdd(KEY_2, DispSetCardMaxDay, NULL);
    TaskAdd(KEY_3, DispSetLossCard, NULL);
    TaskAdd(KEY_4, DispSetKeyB, NULL);
    TaskAdd(KEY_UP, DispSetICInfo1, NULL);
    TaskAdd(KEY_DOWN, DispSetICInfo3, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetICInfo3(void)
**	函数功能：IC卡信息设置界面4
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetICInfo3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.PSAM卡座选择");
    FYD12864ClearLine(2);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);

    TaskAdd(KEY_1, DispSetPsamNum, NULL);
    TaskAdd(KEY_UP, DispSetICInfo2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	函数名称：DispSetShengCode(void)
**	函数功能：省编码设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetShengCode(void)
{
    uint32_t    tmp;

    if(globalvar.KeyValue != KEY_PT)
    {
        tmp_sysparas = sysparas;
        NumKeyHandler();
        ScreenInput.inputmaxlen = 4;
        tmp = atol((char *)ScreenInput.array);
        HexToBcdbuf(tmp, tmp_sysparas.shengcode, 2);
        globalvar.paraindex = 50;
        FYD12864DispPrintfFlash(1, 1, "省编码设置");
        FYD12864DispPrintfFlash(2, 1, "旧值: %02X%02X", sysparas.shengcode[0], sysparas.shengcode[1]);
        globalvar.beforedex = BcdbufToHex(sysparas.shengcode, 2);
        FYD12864DispPrintfFlash(3, 1, "新值: %02X%02X", tmp_sysparas.shengcode[0], tmp_sysparas.shengcode[1]);
        globalvar.afterdex = BcdbufToHex(tmp_sysparas.shengcode, 2);
        globalvar.pointnum = 0;
        FYD12864ClearLine(4);
        TaskAdd(KEY_RET, DispGeneralInfo, NULL);
        TaskAdd(KEY_OK, DispOK, DispGeneralInfo);
    }
}

/***************************************************************************
**	函数名称：DispSetStationCode(void)
**	函数功能：站编码设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetStationCode(void)
{
    if(globalvar.KeyValue != KEY_PT)
    {
        tmp_sysparas = sysparas;
        NumKeyHandler();
        ScreenInput.inputmaxlen = 8;
        globalvar.beforedex = sysparas.stationcode;
        tmp_sysparas.stationcode = atol((char *)ScreenInput.array);
        globalvar.afterdex = tmp_sysparas.stationcode;
        globalvar.paraindex = 52;
		globalvar.pointnum = 0;

        FYD12864DispPrintfFlash(1, 1, "站编码设置");
        FYD12864DispPrintfFlash(2, 1, "旧值: %ld",sysparas.stationcode);
        FYD12864DispPrintfFlash(3, 1, "新值: %ld",tmp_sysparas.stationcode);
        FYD12864ClearLine(4);
        
        TaskAdd(KEY_RET, DispGeneralInfo, NULL);
        TaskAdd(KEY_OK, DispOK, DispGeneralInfo);
    }

}

/***************************************************************************
**	函数名称：DispSetCityCode(void)
**	函数功能：地市编码设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetCityCode(void)
{
    uint32_t    tmp;

    if(globalvar.KeyValue != KEY_PT)
    {
        tmp_sysparas = sysparas;
        NumKeyHandler();
        ScreenInput.inputmaxlen = 4;
        tmp = atol((char *)ScreenInput.array);
        HexToBcdbuf(tmp, tmp_sysparas.citycode, 2);
        globalvar.paraindex = 53;
        FYD12864DispPrintfFlash(1, 1, "地市编码设置");

        FYD12864DispPrintfFlash(2, 1, "旧值: %02X%02X",
                                sysparas.citycode[0],
                                sysparas.citycode[1]);
        globalvar.beforedex = BcdbufToHex(sysparas.citycode, 2);

        FYD12864DispPrintfFlash(3, 1, "新值: %02X%02X",
                                tmp_sysparas.citycode[0],
                                tmp_sysparas.citycode[1]);
        globalvar.afterdex = BcdbufToHex(tmp_sysparas.citycode, 2);
        globalvar.pointnum = 0;
        FYD12864ClearLine(4);
        TaskAdd(KEY_RET, DispGeneralInfo, NULL);
        TaskAdd(KEY_OK, DispOK, DispGeneralInfo);
    }
}

/***************************************************************************
**	函数名称：DispSetParentCode(void)
**	函数功能：上级单位编码设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetParentCode(void)
{
    uint32_t    tmp;

    if(globalvar.KeyValue != KEY_PT)
    {
        tmp_sysparas = sysparas;
        NumKeyHandler();
        ScreenInput.inputmaxlen = 8;
        tmp = atol((char *)ScreenInput.array);
        HexToBcdbuf(tmp, tmp_sysparas.parentcode, 4);
        globalvar.paraindex = 54;
        FYD12864DispPrintfFlash(1, 1, "上级单位编码设置", 1);

        FYD12864DispPrintfFlash(2, 1, "旧值: %02X%02X%02X%02X",
                                sysparas.parentcode[0],
                                sysparas.parentcode[1],
                                sysparas.parentcode[2],
                                sysparas.parentcode[3]);

        globalvar.beforedex = BcdbufToHex(sysparas.parentcode, 4);

        FYD12864DispPrintfFlash(3, 1, "新值: %02X%02X%02X%02X",
                                tmp_sysparas.parentcode[0],
                                tmp_sysparas.parentcode[1],
                                tmp_sysparas.parentcode[2],
                                tmp_sysparas.parentcode[3]);

        globalvar.afterdex = BcdbufToHex(tmp_sysparas.parentcode, 4);
        globalvar.pointnum = 0;
        FYD12864ClearLine(4);
        TaskAdd(KEY_RET, DispGeneralInfo, NULL);
        TaskAdd(KEY_OK, DispOK, DispGeneralInfo);
    }
}

/***************************************************************************
**	函数名称：DispGeneralInfo(void)
**	函数功能：通用信息设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispGeneralInfo(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.省编码");
    FYD12864DispPrintfFlash(2, 1, "2.站编码");
    FYD12864DispPrintfFlash(3, 1, "3.地市代码");
    FYD12864DispPrintfFlash(4, 1, "4.上级单位代码");

    TaskAdd(KEY_1, DispSetShengCode, NULL);
    TaskAdd(KEY_2, DispSetStationCode, NULL);
    TaskAdd(KEY_3, DispSetCityCode, NULL);
    TaskAdd(KEY_4, DispSetParentCode, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/***************************************************************************
**	函数名称：SystemReStart(void)
**	函数功能：系统从0地址重启
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SystemReStart(void)
{
    FYD12864ClearLine(1);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);

    FYD12864DispPrintfFlash(2, 1, "系统重启中");
    _delay_ms(500);
    //DispUI = NULL; 				//重启到应用程序
    asm("jmp 0");					//重启到应用程序
    //asm("jmp 0x1e000");
    //__indirect_jump_to(0x1e000);	//重启到BootLoader程序
    //(*(void(*)())0x1E000)();		//重启到BootLoader程序
    //((void(*)(void))0X1E000)();	//重启到BootLoader程序
}

/***************************************************************************
**	函数名称：SystemReStartToBootLoader(void)
**	函数功能：系统从BootLoader重启
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SystemReStartToBootLoader(void)
{
    FYD12864ClearLine(1);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "系统重启中");
    _delay_ms(500);
    asm("jmp 0x1e000");			//重启到BootLoader程序
}

/***************************************************************************
**	函数名称：SystemReStartToAPP(void)
**	函数功能：系统从指定应用地址重启（0地址）
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SystemReStartToAPP(void)
{
    FYD12864ClearLine(1);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "系统重启中");
    _delay_ms(500);
    asm("jmp 0");				//重启到应用程序
}

/***************************************************************************
**	函数名称：DispSetModbusBaud(void)
**	函数功能：流量计波特率设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetModbusBaud(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 1) || (tmp == 2) || (tmp == 3) || (tmp == 4))
    {
        switch(tmp)
        {
            case 1:
                tmp_sysparas.modbusbaud = 9600;
                break;

            case 2:
                tmp_sysparas.modbusbaud = 19200;
                break;

            case 3:
                tmp_sysparas.modbusbaud = 38400;
                break;

            case 4:
                tmp_sysparas.modbusbaud = 115200;
                break;
        }

        globalvar.paraindex = 62;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误", 1);
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.modbusbaud, "流量计波特率");
    FYD12864DispPrintfFlash(2, 1, "1.9600  2.19200", 1);
    FYD12864DispPrintfFlash(3, 1, "3.38400 4.115200", 1);
    TaskAdd(KEY_RET, DispCommPara, NULL);

    if(tmp != 0)
    {
        TaskAdd(KEY_OK, DispOK, SystemReStart);
    }
}

/***************************************************************************
**	函数名称：DispSetPCBaud(void)
**	函数功能：后台波特率设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPCBaud(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);    //输入的值

    if((tmp == 1) || (tmp == 2) || (tmp == 3) || (tmp == 4))
    {
        switch(tmp)
        {
            case 1:
                tmp_sysparas.pcbaud = 9600;
                break;

            case 2:
                tmp_sysparas.pcbaud = 19200;
                break;

            case 3:
                tmp_sysparas.pcbaud = 38400;
                break;

            case 4:
                tmp_sysparas.pcbaud = 115200;
                break;
        }

        globalvar.paraindex = 63;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.pcbaud, "后台波特率");
    FYD12864DispPrintfFlash(2, 1, "1.9600  2.19200");
    FYD12864DispPrintfFlash(3, 1, "3.38400 4.115200");
    TaskAdd(KEY_RET, DispCommPara, NULL);

    if(tmp != 0)
    {
        TaskAdd(KEY_OK, DispOK, SystemReStart);
    }
}

/***************************************************************************
**	函数名称：DispCommPara(void)
**	函数功能：通讯参数设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispCommPara(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.流量计波特率");
    FYD12864DispPrintfFlash(2, 1, "2.后台波特率");

    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetModbusBaud, NULL);
    TaskAdd(KEY_2, DispSetPCBaud, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/***************************************************************************
**	函数名称：SetPressureRatioEnter(void)
**	函数功能：压力调校
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SetPressureRatioEnter(void)
{
    uint32_t  pressurevalue, temp;
    char buf[10];
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    pressurevalue = atol(buf);
    temp = globalvar.U32Temp - tmp_sysparas.pzero;
    temp *= 100;
    tmp_sysparas.pcoeff = temp / pressurevalue;
    globalvar.paraindex = 33;
    FYD12864ClearLine(2);

    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        sysparas = tmp_sysparas;

        if(SaveSignalSysparas(globalvar.paraindex, 1))
        {
            FYD12864DispPrintfFlash(3, 1, "    设置成功");
        }
        else
        {
            FYD12864DispPrintfFlash(3, 1, "    设置失败");
        }
    }

    FYD12864DispPrintfFlash(4, 1, "返回请按【返回】");
    TaskAdd(KEY_RET, DispSetPressure, NULL);

}

/***************************************************************************
**	函数名称：SetPressureRatio(void)
**	函数功能：压力系数设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SetPressureRatio(void)
{
    uint32_t rd;

    tmp_sysparas = sysparas;
    globalvar.displayonce = 1;
    FYD12864DispPrintfFlash(1, 1, "  设置压力系数");
    rd = PlADCGetValue();
    globalvar.U32Temp = rd;

    FYD12864DispPrintfFlash(2, 1, "当前AD值:%ld", rd);

    FYD12864DispPrintfFlash(3, 1, "请输入");
    ScreenInput.inputmaxlen = 5;
    NumKeyHandler();

    FYD12864DispPrintfFlash(4, 1, "当前压力:%s", ScreenInput.array);

    TaskAdd(KEY_RET, DispSetPressure, NULL);
    TaskAdd(KEY_OK, SetPressureRatioEnter, NULL);
}

/***************************************************************************
**	函数名称：SetPressureZeroEnter(void)
**	函数功能：压力零点设置完成，设置结果提示界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SetPressureZeroEnter(void)
{
    FYD12864ClearLine(2);

    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        sysparas = tmp_sysparas;

        if(SaveSignalSysparas(globalvar.paraindex, 1))
        {
            FYD12864DispPrintfFlash(3, 1, "    设置成功");
        }
        else
        {
            FYD12864DispPrintfFlash(3, 1, "    设置失败");
        }
    }

    FYD12864DispPrintfFlash(4, 1, "返回请按【返回】");
    TaskAdd(KEY_RET, DispSetPressure, NULL);
}

/***************************************************************************
**	函数名称：SetPressureZero(void)
**	函数功能：压力零点设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SetPressureZero(void)
{
    uint32_t rd;

    tmp_sysparas = sysparas;
    FYD12864DispPrintfFlash(1, 1, "  设置压力零点");
    rd = PlADCGetValue();
    tmp_sysparas.pzero = rd;

    if(tmp_sysparas.pzero < 100)
    {
        FYD12864ClearLine(2);
        FYD12864DispPrintfFlash(3, 1, "压力异常");
    }
    else
    {
        globalvar.paraindex = 32;
        globalvar.displayonce = 1;
        FYD12864DispPrintfFlash(2, 1, "当前AD值:%ld", rd);
        FYD12864DispPrintfFlash(3, 1, "确认请按【确认】");
        TaskAdd(KEY_OK, SetPressureZeroEnter, NULL);
    }

    FYD12864DispPrintfFlash(4, 1, "返回请按【返回】");
    TaskAdd(KEY_RET, DispSetPressure, NULL);

}

/***************************************************************************
**	函数名称：DispSetPressure(void)
**	函数功能：压力相关参数设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPressure(void)
{
    FYD12864DispPrintfFlash(1, 1, "  校正压力参数");
    FYD12864DispPrintfFlash(2, 1, "1.设置压力零点");
    FYD12864DispPrintfFlash(3, 1, "2.设置压力系数");
    FYD12864ClearLine(4);
    TaskAdd(KEY_RET, DispCommonOpt, NULL);
    TaskAdd(KEY_1, SetPressureZero, NULL);
    TaskAdd(KEY_2, SetPressureRatio, NULL);
}

/***************************************************************************
**	函数名称：DispSetFuelMode(void)
**	函数功能：加气模式选择
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetFuelMode(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1) || (tmp == 2))
    {
        tmp_sysparas.fuelmode = tmp;
        globalvar.paraindex = 64;
        TaskAdd(KEY_OK, DispOK, DispCommonOpt1);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.fuelmode, "加气模式设置");
    FYD12864DispPrintfFlash(2, 1, "0.手动    1.后台", 1);
    FYD12864DispPrintfFlash(3, 1, "2.电子标签", 1);
    TaskAdd(KEY_RET, DispCommonOpt1, NULL);

}

/***************************************************************************
**	函数名称：DispSetDateTimeOK(void)
**	函数功能：设置日期和时间显示确认及设置结果提示界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetDateTimeOK(void)
{
    uint8_t tmp[6], i, ret = false;

    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;

        for(i = 0; i < 6; i++)
        {
            tmp[i] = AscToBcd(&ScreenInput.array[2 * i]);
        }

        ret = Ds3232SetTime(tmp);

        if(ret == true)
            FYD12864DispPrintfFlash(4, 1, "设置成功");
        else
            FYD12864DispPrintfFlash(4, 1, "设置失败");
    }

    TaskMessage(10, DispCommonOpt1);
}

/***************************************************************************
**	函数名称：DispSetDateTime(void)
**	函数功能：日期时间设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetDateTime(void)
{
    uint8_t *tmp, i, ret = true;

    Time    newtime = {0};

    if(globalvar.KeyPreValue != KEY_PT)
    {
        FYD12864DispPrintfFlash(1, 1, "日期时间设置");
        NumKeyHandler();
        ScreenInput.inputmaxlen = 12;
        tmp = (uint8_t*)&newtime;
        newtime = time;

        // i<6 为后续添加，没有此判断条件，可能会把newtime后面的地址中的数据修改 ADD BY LY
        for(i = 0; i < (ScreenInput.inputlen / 2 + 1) && i < 6; i++)
        {
            *(tmp + i) = AscToBcd(&ScreenInput.array[2 * i]);
        }

        FYD12864DispPrintfFlash(2, 1, "%02X年%02X月%02X日",
                                newtime.year,
                                newtime.month,
                                newtime.day);

        FYD12864DispPrintfFlash(3, 1, "%02X时%02X分%02X秒",
                                newtime.hour,
                                newtime.minute,
                                newtime.second);

        if(ScreenInput.inputlen > 5)
            ret = CheckDate(tmp);

        if(ScreenInput.inputlen > 9)
            ret = CheckTime(tmp + 3);

        if(ret == false)
        {
            FYD12864DispPrintfFlash(4, 1, "输入格式错误");
            return;
        }
        else
        {
            FYD12864ClearLine(4);
        }

        TaskAdd(KEY_RET, DispCommonOpt1, NULL);

        if(ScreenInput.inputlen > 9)   //输入完毕后才能按确认键
        {
            TaskAdd(KEY_OK, DispSetDateTimeOK, NULL);
        }
    }
}

/***************************************************************************
**	函数名称：DispSetPrintMode(void)
**	函数功能：小票打印方式设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPrintMode(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1))
    {
        tmp_sysparas.printmode = tmp;
        globalvar.paraindex = 65;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.printmode, "打印方式设置");
    FYD12864DispPrintfFlash(2, 1, "0.手动打印");
    FYD12864DispPrintfFlash(3, 1, "1.自动打印");
    TaskAdd(KEY_RET, DispSetPrint, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetPrint);
}

/***************************************************************************
**	函数名称：DispSetIntNum(void)
**	函数功能：是否取整设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetIntNum(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1))
    {
        tmp_sysparas.intnumflag = tmp;
        globalvar.paraindex = 79;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.intnumflag, "是否取整设置");
    FYD12864DispPrintfFlash(2, 1, "0.不能取整");
    FYD12864DispPrintfFlash(3, 1, "1.可以取整");
    TaskAdd(KEY_RET, DispCommonOpt, NULL);
    TaskAdd(KEY_OK, DispOK, DispCommonOpt);
}

//设置打印次数
void DispSetPrintTimes(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
	
    tmp = atol((char *)ScreenInput.array);

    if(tmp > 0)
    {
        tmp_sysparas.printtimes = tmp;
        globalvar.paraindex = 218;
   		TaskAdd(KEY_OK, DispOK, DispSetPrint);		
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }	
	
    DispSet(0, sysparas.printtimes, "打印次数设置");
	
    TaskAdd(KEY_RET, DispSetPrint, NULL);

}

//打印有关设置
void DispSetPrint(void)
{
    InputInit();

	FYD12864DispPrintfFlash(1, 1, "1.打印模式设置");
	FYD12864DispPrintfFlash(2, 1, "2.打印次数设置");
	FYD12864DispPrintfFlash(3, 1, "3.是否打印车牌");
	FYD12864ClearLine(4);

    TaskAdd(KEY_1, DispSetPrintMode, NULL);	
    TaskAdd(KEY_2, DispSetPrintTimes, NULL);
    TaskAdd(KEY_3, DispSetIsPrintCarnum, NULL);
    TaskAdd(KEY_RET, DispCommonOpt, NULL);	
}
/***************************************************************************
**	函数名称：DispCommonOpt(void)
**	函数功能：常用操作设置界面1
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispCommonOpt(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.屏幕选择");
//    FYD12864DispPrintfFlash( 2, 1, "2.联机模式");
    FYD12864DispPrintfFlash(2, 1, "2.是否取整");
    FYD12864DispPrintfFlash(3, 1, "3.打印设置");
    FYD12864DispPrintfFlash(4, 1, "4.压力调校");

    TaskAdd(KEY_1, DispSetPing, NULL);
//    TaskAdd( KEY_2, DispSetOffline, NULL ); //huanghao 20161025
    TaskAdd(KEY_2, DispSetIntNum, NULL);   //ycx20161202
    TaskAdd(KEY_3, DispSetPrint, NULL);
    TaskAdd(KEY_4, DispSetPressure, NULL);
    TaskAdd(KEY_DOWN, DispCommonOpt1, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/***************************************************************************
**	函数名称：DispSetOffline(void)
**	函数功能：脱机模式选择
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetOffline(void)  
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1))
    {
        tmp_sysparas.offlinemode = tmp;
        globalvar.paraindex = 76;

        TaskAdd(KEY_OK, DispOK, DispCommonOpt1);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.offlinemode, "联机模式设置");
    FYD12864DispPrintfFlash(2, 1, "0.在线模式");
    FYD12864DispPrintfFlash(3, 1, "1.脱机模式");


    if(tmp_sysparas.offlinemode && globalvar.KeyValue == KEY_OK)
        CloseUart2RcvIRQ();//关uart2 接收数据中断
    else
        OpenUart2RcvIRQ();//开uart2 接收数据中断

    TaskAdd(KEY_RET, DispCommonOpt1, NULL);
}


/***************************************************************************
**	函数名称：DispSetIsPrintCarnum(void)
**	函数功能：是否打印车牌号
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-10-11
***************************************************************************/
void DispSetIsPrintCarnum(void)  
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1))
    {
        tmp_sysparas.IsPrintCarnum = tmp;
        globalvar.paraindex = 85;
        TaskAdd(KEY_OK, DispOK, DispCommonOpt1);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.IsPrintCarnum, "是否打印车牌号");
    FYD12864DispPrintfFlash(2, 1, "0.不打印");
    FYD12864DispPrintfFlash(3, 1, "1.打印");
    TaskAdd(KEY_RET, DispSetPrint, NULL);
}


/***************************************************************************
**	函数名称：DispCommonOpt1(void)
**	函数功能：常用操作设置界面2
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispCommonOpt1(void)
{
	InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.加气模式设置");
    FYD12864DispPrintfFlash(2, 1, "2.日期时间设置");
    FYD12864DispPrintfFlash(3, 1, "3.联机模式设置");

    TaskAdd(KEY_1, DispSetFuelMode, NULL);
    TaskAdd(KEY_2, DispSetDateTime, NULL);
    TaskAdd(KEY_3, DispSetOffline, NULL);
    TaskAdd(KEY_4, DispSetIsPrintCarnum, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
    TaskAdd(KEY_UP, DispCommonOpt, NULL);
}

/***************************************************************************
**	函数名称：DispSetPing(void)
**	函数功能：客显屏选择
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPing(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);

    if((tmp == 0) || (tmp == 1))
    {
        tmp_sysparas.lcdtype = tmp;
        globalvar.paraindex = 77;

        //设置屏幕时，刷一次屏ycx20170330
        if(tmp == 0)
            OsplcShowB(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price, 0, 1);
        else OsplcShowN(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.lcdtype, "屏幕选择设置");
    FYD12864DispPrintfFlash(2, 1, "0.蓝屏");
    FYD12864DispPrintfFlash(3, 1, "1.大屏");
    TaskAdd(KEY_RET, DispCommonOpt, NULL);
    TaskAdd(KEY_OK, DispOK, DispCommonOpt);
}

/***************************************************************************
**	函数名称：DispMenu_2(void)
**	函数功能：菜单界面2
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispMenu_2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.通用信息设置");
    FYD12864DispPrintfFlash(2, 1, "2.控制参数设置");
    FYD12864DispPrintfFlash(3, 1, "3.通讯参数设置");
    FYD12864DispPrintfFlash(4, 1, "4.常用操作设置");


    TaskAdd(KEY_1, DispGeneralInfo, NULL);
    TaskAdd(KEY_2, DispSetControl1, NULL);
    TaskAdd(KEY_3, DispCommPara, NULL);
    TaskAdd(KEY_4, DispCommonOpt, NULL);
    TaskAdd(KEY_UP, DispMenu_1, NULL);
    TaskAdd(KEY_DOWN, DispMenu_3, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**	函数名称：DispMenu_3(void)
**	函数功能：菜单界面3
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispMenu_3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.版本编号设置");
    FYD12864DispPrintfFlash(2, 1, "2.密码信息设置");
    FYD12864DispPrintfFlash(3, 1, "3.模拟加气设置");
    FYD12864DispPrintfFlash(4, 1, "4.循环加气设置");

    TaskAdd(KEY_1, DispVerNum1, NULL);
    TaskAdd(KEY_2, DispOthMsg1, NULL);
    TaskAdd(KEY_3, SimulatorSet, NULL);
    TaskAdd(KEY_4, CirculateFuelingSet, NULL);
    TaskAdd(KEY_UP, DispMenu_2, NULL);
    //TaskAdd(KEY_DOWN, DispMenu_1,NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}


/***************************************************************************
**	函数名称：DispSetPriceVer(void)
**	函数功能：单价版本号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPriceVer(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.pricever = atol((char *)ScreenInput.array);
    globalvar.paraindex = 66;
    DispSet(0, sysparas.pricever, "单价版本号设置");
    TaskAdd(KEY_RET, DispVerNum1, NULL);
    TaskAdd(KEY_OK, DispOK, DispVerNum1);
}

/***************************************************************************
**	函数名称：DispSetCommInfoVer(void)
**	函数功能：通用信息版本号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetCommInfoVer(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.comminfover = atol((char *)ScreenInput.array);
    globalvar.paraindex = 68;
    DispSet(0, sysparas.comminfover, "通用信息版本号");
    TaskAdd(KEY_RET, DispVerNum1, NULL);
    TaskAdd(KEY_OK, DispOK, DispVerNum1);
}

/***************************************************************************
**	函数名称：DispSetLockVer(void)
**	函数功能：密钥信息版本号设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetLockVer(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.lockver = atol((char *)ScreenInput.array);
    globalvar.paraindex = 69;
    DispSet(0, sysparas.lockver, "密钥信息版本号");
    TaskAdd(KEY_RET, DispVerNum1, NULL);
    TaskAdd(KEY_OK, DispOK, DispVerNum1);
}


/***************************************************************************
**	函数名称：DispVerNum1(void)
**	函数功能：版本编号设置1
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispVerNum1(void)
{
    InputInit();

    FYD12864DispPrintfFlash(1, 1, "1.单价版本号");
    FYD12864DispPrintfFlash(2, 1, "2.通用信息版本号");
    FYD12864DispPrintfFlash(3, 1, "3.密钥信息版本号");
    FYD12864ClearLine(4);
    //FYD12864DispString( 4, 1, "2.黑名单版本号", 1 );

    TaskAdd(KEY_1, DispSetPriceVer, NULL);
    TaskAdd(KEY_2, DispSetCommInfoVer, NULL);
    TaskAdd(KEY_3, DispSetLockVer, NULL);
    //TaskAdd( KEY_4, DispSetBlackVer, NULL );
    TaskAdd(KEY_RET, DispMenu_3, NULL);
}

/***************************************************************************
**	函数名称：DispJudgePwd(void)
**	函数功能：输入密码判断
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispJudgePwd(void)
{
    InputInit();//为输入新密码做准备 ADD BY LY

    if(globalvar.U8Temp == 1)  //1.设置菜单密码 ADD BY LY
    {
        if(globalvar.U32Temp != sysparas.menupwd)
        {
            FYD12864DispPrintfFlash(4, 1, "密码错误");
            TaskAdd(KEY_CLR, DispInputOldPwd, NULL);
            TaskAdd(KEY_RET, DispInputOldPwd, NULL);
        }
        else
        {
            DispUI = DispSetMenuPwd;
        }
    }
    else if(globalvar.U8Temp == 2)  //2.设置质检密码 ADD BY LY
    {
        if(globalvar.U32Temp != sysparas.qcpwd)
        {
            FYD12864DispPrintfFlash(4, 1, "密码错误");
            TaskAdd(KEY_CLR, DispInputOldPwd, NULL);
            TaskAdd(KEY_RET, DispInputOldPwd, NULL);
        }
        else
        {
            DispUI = DispSetQcPwd;
        }
    }
    else if(globalvar.U8Temp == 3)  //3.设置维护密码 ADD BY LY
    {
        if(globalvar.U32Temp != sysparas.testpwd)
        {
            FYD12864DispPrintfFlash(4, 1, "密码错误");
            TaskAdd(KEY_CLR, DispInputOldPwd, NULL);
            TaskAdd(KEY_RET, DispInputOldPwd, NULL);
        }
        else
        {
            DispUI = DispSetTestPwd;
        }
    }
    else if(globalvar.U8Temp == 4)  //4.设置恢复出厂密码 ADD BY LY
    {
        if(globalvar.U32Temp != sysparas.resetpwd)
        {
            FYD12864DispPrintfFlash(4, 1, "密码错误");
            TaskAdd(KEY_CLR, DispInputOldPwd, NULL);
            TaskAdd(KEY_RET, DispInputOldPwd, NULL);
        }
        else
        {
            DispUI = DispSetResetPwd;
        }
    }
}

/***************************************************************************
**	函数名称：DispInputOldPwd(void)
**	函数功能：旧密码输入
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispInputOldPwd(void)
{
    char     buf[7] = {0}, i;

    ScreenInput.inputmaxlen = 6;
    NumKeyHandler();
    FYD12864ClearLine(1);
    FYD12864DispPrintfFlash(2, 1, "请输入旧密码");
    FYD12864ClearLine(4);

    for(i = 0; i < ScreenInput.inputlen; i++)
        buf[i] = '*';

    FYD12864DispPrintf(3, 1, buf);
    globalvar.U32Temp = atol((char *)ScreenInput.array);    //输入的旧密码 ADD BY LY
    TaskAdd(KEY_RET, DispOthMsg1, NULL);
    TaskAdd(KEY_OK, DispJudgePwd, NULL);
}

//判断两次密码是否一致
void JudgePwdTwice(void)
{
    uint32_t pwd;
    pwd = atol((char *)ScreenInput.array);

    if(pwd != globalvar.afterdex)//第一次输入密码时，已经赋值
    {
        FYD12864DispPrintfFlash(3, 1, "两次密码不一致");
        TaskMessage(10, DispOthMsg1);
    }
    else
    {
        globalvar.displayonce = 1;
        DispUI = DispOK;
    }
}

//再次输入新密码
void InputNewPwdAgain(void)
{
    char i, tmpbuf[7] = {0};

    if(globalvar.displayonce == 1)
    {
        memset(&ScreenInput, 0, sizeof(Screeninput));
        globalvar.displayonce = 0;
    }

    ScreenInput.inputmaxlen = 6;
    NumKeyHandler();
    FYD12864DispPrintfFlash(1, 1, "再次输入新密码:");

    for(i = 0; i < ScreenInput.inputlen; i++)
    {
        tmpbuf[i] = '*';
    }

    FYD12864DispPrintfFlash(2, 1, "%s", tmpbuf);
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0) //防止直接按了确认键
    {
        TaskAdd(KEY_OK, JudgePwdTwice, DispOthMsg1);
    }
}


/***************************************************************************
**	函数名称：DispSetMenuPwd(void)
**	函数功能：菜单密码设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetMenuPwd(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    tmp_sysparas.menupwd = atol((char *)ScreenInput.array);
    globalvar.paraindex = 70;
    DispSet(6, sysparas.menupwd, "菜单密码设置:");
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0)   //防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain, NULL);
    }
}

/***************************************************************************
**	函数名称：DispSetQcPwd(void)
**	函数功能：质检密码设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetQcPwd(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    tmp_sysparas.qcpwd = atol((char *)ScreenInput.array);
    globalvar.paraindex = 71;
    DispSet(6, sysparas.qcpwd, "质检密码设置:");
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0)   //防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain, NULL);
    }
}

/***************************************************************************
**	函数名称：DispSetTestPwd(void)
**	函数功能：维护密码设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetTestPwd(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    tmp_sysparas.testpwd = atol((char *)ScreenInput.array);
    globalvar.paraindex = 72;
    DispSet(6, sysparas.testpwd, "维护密码设置:");
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0)   //防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain, NULL);
    }
}

/***************************************************************************
**	函数名称：DispSetResetPwd(void)
**	函数功能：恢复出厂密码设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetResetPwd(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    tmp_sysparas.resetpwd = atol((char *)ScreenInput.array);
    globalvar.paraindex = 73;
    DispSet(6, sysparas.resetpwd, "恢复出厂密码设置");
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0)   //防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain, NULL);
    }
}

/***************************************************************************
**	函数名称：DispOthMsg1(void)
**	函数功能：其他信息设置界面1
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispOthMsg1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.菜单密码设置");
    FYD12864DispPrintfFlash(2, 1, "2.质检密码设置");
    FYD12864DispPrintfFlash(3, 1, "3.维护密码设置");
    FYD12864DispPrintfFlash(4, 1, "4.恢复出厂密码");

    if(globalvar.KeyValue == KEY_1)
    {
        globalvar.U8Temp = 1;
        TaskAdd(KEY_1, DispInputOldPwd, NULL);
    }

    if(globalvar.KeyValue == KEY_2)
    {
        globalvar.U8Temp = 2;
        TaskAdd(KEY_2, DispInputOldPwd, NULL);
    }

    if(globalvar.KeyValue == KEY_3)
    {
        globalvar.U8Temp = 3;
        TaskAdd(KEY_3, DispInputOldPwd, NULL);
    }

    if(globalvar.KeyValue == KEY_4)
    {
        globalvar.U8Temp = 4;
        TaskAdd(KEY_4, DispInputOldPwd, NULL);
    }

    //TaskAdd(KEY_UP,DispOthMsg2 ,NULL);
    TaskAdd( KEY_DOWN, DispOthMsg2, NULL );
    TaskAdd(KEY_RET, DispMenu_3, NULL);
}

/***************************************************************************
**	函数名称：DispOthMsg2(void)
**	函数功能：其他信息设置界面2
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispOthMsg2(void)
{
    
    InputInit();
	
    FYD12864DispPrintfFlash(1,1,"1.加密板设置");
    FYD12864ClearLine(2);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetPwdBoard,NULL);
    TaskAdd(KEY_UP,DispOthMsg1 ,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}

/***************************************************************************
**	函数名称：SetPwdBoard(void)
**	函数功能：设置加密板功能完成，显示设置结果，启用加密板功能
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SetPwdBoard(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        sysparas = tmp_sysparas;

        if(tmp_sysparas.usepwdboard == 0)
        {
        	//保存修改后的sysparas.usepwdboard ADD BY LY
            if(SaveSignalSysparas(globalvar.paraindex,1))
            {
                SetSuccess();
            }
            else
            {
                SetFault();
            }
        }
        else
        {
        	//获取加密数据，保存在外部EEPROM及加密板EERPOM中 ADD BY LY
            LockDataStore();
        }
    }

    if(DispNextUI != NULL)
    {
        TaskMessage(10, DispNextUI);
    }
}

/***************************************************************************
**	函数名称：DispSetPwdBoard(void)
**	函数功能：是否使用加密板功能选择
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetPwdBoard(void)
{
    uint8_t tmp;

    if(sysparas.usepwdboard == 1 && SI_INDATE != 0)     //使用加密板但处于正常模式
    {
        FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(2, 1, "加气机正常模式");		
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);

        TaskAdd(KEY_RET, DispOthMsg2, NULL);
    }
    else
    {
        tmp_sysparas = sysparas;
        ScreenInput.inputmaxlen = 1;
        tmp = atol((char *)ScreenInput.array);

        if((tmp == 0) || (tmp == 1))
        {
            tmp_sysparas.usepwdboard = tmp;
            globalvar.paraindex = 75;
        }
        else if(ScreenInput.inputlen > 0)
        {
            FYD12864DispPrintfFlash(4, 1, "输入错误");
            NumKeyHandler();
            return;
        }

        DispSet(5, sysparas.usepwdboard, "加密板设置");
        FYD12864DispPrintfFlash(2, 1, "0.不使用加密板");
        FYD12864DispPrintfFlash(3, 1, "1.使用加密板");
        TaskAdd(KEY_RET, DispOthMsg2, NULL);
        TaskAdd(KEY_OK, SetPwdBoard, DispOthMsg2);
    }
}

/***************************************************************************
**	函数名称：DispSetDensty(void)
**	函数功能：密度设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetDensty(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 10000);
    tmp_sysparas.densty = atol(buf);
    globalvar.paraindex = 30;

    if(tmp_sysparas.densty > 0 && tmp_sysparas.densty < 10000)
    {
        TaskAdd(KEY_OK, DispOK, DispQcPara);
    }
    else if((ScreenInput.inputlen > 5))
    {
        FYD12864DispPrintfFlash(4, 1, "密度超限");
        NumKeyHandler();
        return;
    }

    DispSet(4, sysparas.densty, "密度设置");
    TaskAdd(KEY_RET, DispQcPara, NULL);
}

/***************************************************************************
**	函数名称：DispSetCoeff(void)
**	函数功能：修正系数设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispSetCoeff(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 10000);
    tmp_sysparas.coeff = atol(buf);

    if(tmp_sysparas.coeff > 0 && tmp_sysparas.coeff <= 15000)
    {
        TaskAdd(KEY_OK, DispOK, DispQcPara);
    }
    else if((ScreenInput.inputlen > 5))
    {

        FYD12864DispPrintfFlash(4, 1, "修正系数超限");
        NumKeyHandler();
        return;
    }

    globalvar.paraindex = 31;
    DispSet(4, sysparas.coeff, "修正系数设置");
    TaskAdd(KEY_RET, DispQcPara, NULL);
    TaskAdd(KEY_OK, DispOK, DispQcPara);
}

/***************************************************************************
**	函数名称：DispQcPara(void)
**	函数功能：质检参数设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispQcPara(void)
{
	//使用加密板但出于正常模式
    if(sysparas.usepwdboard == 1 && SI_INDATE != 0)     
    {
        FYD12864ClearLine(1);
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
        FYD12864DispPrintfFlash(2, 1, "加气机在正常模式");
        TaskAdd(KEY_RET, DispFirst, NULL);
    }
	 //不使用加密板或使用加密板且数据正确
    else if(sysparas.usepwdboard == 0 || (sysparas.usepwdboard == 1 && LockDataCompare()))
    {
        InputInit();
        FYD12864DispPrintfFlash(1, 1, "1.密度设置");
        FYD12864DispPrintfFlash(2, 1, "2.修正系数设置");
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
        TaskAdd(KEY_1, DispSetDensty, NULL);
        TaskAdd(KEY_2, DispSetCoeff, NULL);
        TaskAdd(KEY_RET, DispFirst, NULL);
    }
    else
    {
        FYD12864ClearLine(1);
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
        FYD12864DispPrintfFlash(2, 1, "加密板不匹配！！");
        TaskAdd(KEY_RET, DispFirst, NULL);
    }
}

/***************************************************************************
**	函数名称：EraseFlash(void)
**	函数功能：擦除flash，全片擦除
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void EraseFlash(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        //添加flash全片擦除代码
        SetSuccess();
    }

    TaskMessage(10, DispEraseChip);
}

/***************************************************************************
**	函数名称：DispEraseFlash(void)
**	函数功能：擦除flash选择界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispEraseFlash(void)
{
    FYD12864DispPrintfFlash(1, 1, "确认擦除Flash");
    FYD12864DispPrintfFlash(2, 1, "【确认】  继续");
    FYD12864DispPrintfFlash(3, 1, "【返回】  取消");
    FYD12864ClearLine(4);

    TaskAdd(KEY_OK, EraseFlash, NULL);
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);
}

/***************************************************************************
**	函数名称：CirculateFuelingSet(void)
**	函数功能：循环加气设置
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void CirculateFuelingSet(void)
{
    uint8_t tmp;
    ScreenInput.inputmaxlen = 1;
    tmp = ScreenInput.array[0];

    if((tmp == '0') || (tmp == '1'))
    {
        globalvar.circulatefue = atol((char *)ScreenInput.array);

        if(globalvar.KeyValue == KEY_OK)
            DispUI = DispFirst;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, globalvar.circulatefue, "循环加气设置");
    FYD12864DispPrintfFlash(2, 1, "0.正常加气");
    FYD12864DispPrintfFlash(3, 1, "1.循环加气");
    TaskAdd(KEY_RET, DispMenu_3, NULL);
}

/***************************************************************************
**	函数名称：DispVerNum2(void)
**	函数功能：菜单界面2
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void DispVerNum2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.版本编号设置");
    FYD12864DispPrintfFlash(2, 1, "2.其它信息设置");
    FYD12864DispPrintfFlash(3, 1, "3.模拟加气设置");
    FYD12864DispPrintfFlash(4, 1, "4.循环加气设置");

    TaskAdd(KEY_1, DispVerNum1, NULL);
    TaskAdd(KEY_2, DispOthMsg1, NULL);
    TaskAdd(KEY_3, SimulatorSet, NULL);
    TaskAdd(KEY_4, CirculateFuelingSet, NULL);
    TaskAdd(KEY_UP, DispVerNum1, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**	函数名称：Diswork(void)
**	函数功能：1秒后返回主界面
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void Diswork(void)
{
    TaskMessage(10, DispFirst);
}

/***************************************************************************
**	函数名称：SimulatorSet(void)
**	函数功能：设置模拟加气
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2018-9-17
***************************************************************************/
void SimulatorSet(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = ScreenInput.array[0];

    if((tmp == '0') || (tmp == '1'))
    {
        sysparas.simulation = atol((char *)ScreenInput.array);
        globalvar.paraindex = 78;
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "输入错误");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.simulation, "模拟加气设置");
    FYD12864DispPrintfFlash(2, 1, "0.正常加气");
    FYD12864DispPrintfFlash(3, 1, "1.模拟加气");
    TaskAdd(KEY_RET, DispMenu_3, NULL);
    TaskAdd(KEY_OK, DispOK, DispMenu_3);
}
