#include "kernel.h"

void ValveTestLow(void);
void ValveTestMid(void);
void ValveTestHigh(void);
void ValveTestAll(void);
void MassmeterTest(void);
/************************************************************************
**	函数名称:	KeyBoard_Test(void)
**	函数功能:	键盘测试
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HHM
**	创建日期:	2016-4-13
*************************************************************************/

void KeyBoard_Test(void)
{
    FYD12864DispPrintfFlash(1, 1, "键盘测试");
    FYD12864DispPrintfFlash(2, 1, "请按按键测试");
    FYD12864DispPrintfFlash(4, 1, "【返回】停止测试");

    TaskAdd(KEY_RET, DeviceTestMenu, NULL);

    switch(globalvar.KeyValue)
    {
        case KEY_0:
            FYD12864DispPrintfFlash(3, 1, "数字按键：0");
            break;

        case KEY_1:
            FYD12864DispPrintfFlash(3, 1, "数字按键：1");
            break;

        case KEY_2:
            FYD12864DispPrintfFlash(3, 1, "数字按键：2");
            break;

        case KEY_3:
            FYD12864DispPrintfFlash(3, 1, "数字按键：3");
            break;

        case KEY_4:
            FYD12864DispPrintfFlash(3, 1, "数字按键：4");
            break;

        case KEY_5:
            FYD12864DispPrintfFlash(3, 1, "数字按键：5");
            break;

        case KEY_6:
            FYD12864DispPrintfFlash(3, 1, "数字按键：6");
            break;

        case KEY_7:
            FYD12864DispPrintfFlash(3, 1, "数字按键：7");
            break;

        case KEY_8:
            FYD12864DispPrintfFlash(3, 1, "数字按键：8");
            break;

        case KEY_9:
            FYD12864DispPrintfFlash(3, 1, "数字按键：9");
            break;

        case KEY_PT:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 小数点");
            break;

        case KEY_FIX:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 定量");
            break;

        case KEY_OK:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 确认");
            break;

        case KEY_BACK:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 回显");
            break;

        case KEY_START:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 加气");
            break;

        case KEY_SET:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 菜单");
            break;

        case KEY_QRY:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 查询");
            break;

        case KEY_CLR:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 清除");
            break;

        case KEY_STOP:
            FYD12864DispPrintfFlash(3, 1, "功能按键: 停止");
            break;

        default:
            break;
    }
}

/************************************************************************
**	函数名称:	KeyBoardTest(void)
**	函数功能:	键盘测试
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-4-13
*************************************************************************/
void KeyBoardTest(void)
{
    FYD12864ClrScreen();
    DispUI = KeyBoard_Test;
}

/************************************************************************
**	函数名称:	ValveTestStop(void)
**	函数功能:	关闭全部电磁阀
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HHM
**	创建日期:	2016-04-25
*************************************************************************/
void ValveTestStop(void)
{
    VCTRL_MID_CLOSE();
    _delay_us(10);
    VCTRL_HIGH_CLOSE();
    _delay_us(10);
    VCTRL_LOW_CLOSE();
    _delay_us(10);
    VCTRL_CLOSEKEY();
    _delay_us(10);
    DispUI = DeviceTestMenu;
}

/************************************************************************
**	函数名称:	ValveTestLow(void)
**	函数功能:	打开低压阀
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void ValveTestLow(void)
{
    FYD12864DispPrintfFlash(2, 1, "低  中  高  总");
    FYD12864DispPrintfFlash(3, 1, "●  ○  ○  ○");

    TaskAdd(KEY_2, ValveTestMid, NULL);
    TaskAdd(KEY_3, ValveTestHigh, NULL);
    TaskAdd(KEY_4, ValveTestAll, NULL);
    TaskAdd(KEY_RET, ValveTestStop, NULL);
    VCTRL_OPENKEY();
    _delay_us(10);
    VCTRL_MID_CLOSE();
    _delay_us(10);
    VCTRL_HIGH_CLOSE();
    _delay_us(10);
    VCTRL_LOW_OPEN();
    _delay_us(10);
}
/************************************************************************
**	函数名称:	ValveTestMid(void)
**	函数功能:	打开中压阀
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void ValveTestMid(void)
{
    FYD12864DispPrintfFlash(2, 1, "低  中  高  总");
    FYD12864DispPrintfFlash(3, 1, "○  ●  ○  ○");

    TaskAdd(KEY_1, ValveTestLow, NULL);
    TaskAdd(KEY_3, ValveTestHigh, NULL);
    TaskAdd(KEY_4, ValveTestAll, NULL);
    TaskAdd(KEY_RET, ValveTestStop, NULL);
    VCTRL_OPENKEY();
    _delay_us(10);
    VCTRL_LOW_CLOSE();
    _delay_us(10);
    VCTRL_HIGH_CLOSE();
    _delay_us(10);
    VCTRL_MID_OPEN();
    _delay_us(10);
}

/************************************************************************
**	函数名称:	ValveTestHigh(void)
**	函数功能:	打开高压阀
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void ValveTestHigh(void)
{
    FYD12864DispPrintfFlash(2, 1, "低  中  高  总");
    FYD12864DispPrintfFlash(3, 1, "○  ○  ●  ○");

    TaskAdd(KEY_1, ValveTestLow, NULL);
    TaskAdd(KEY_2, ValveTestMid, NULL);
    TaskAdd(KEY_4, ValveTestAll, NULL);
    TaskAdd(KEY_RET, ValveTestStop, NULL);
    VCTRL_OPENKEY();
    _delay_us(10);
    VCTRL_MID_CLOSE();
    _delay_us(10);
    VCTRL_HIGH_OPEN();
    _delay_us(10);
    VCTRL_LOW_CLOSE();
    _delay_us(10);
}

/************************************************************************
**	函数名称:	ValveTestAll(void)
**	函数功能:	打开全部阀门
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void ValveTestAll(void)
{
    FYD12864DispPrintfFlash(2, 1, "低  中  高  总");
    FYD12864DispPrintfFlash(3, 1, "●  ●  ●  ●");

    TaskAdd(KEY_1, ValveTestLow, NULL);
    TaskAdd(KEY_2, ValveTestMid, NULL);
    TaskAdd(KEY_3, ValveTestHigh, NULL);
    TaskAdd(KEY_RET, ValveTestStop, NULL);
    VCTRL_OPENKEY();
    _delay_us(10);
    VCTRL_LOW_OPEN();
    _delay_us(10);
    VCTRL_MID_OPEN();
    _delay_us(10);
    VCTRL_HIGH_OPEN();
    _delay_us(10);
}

/************************************************************************
**	函数名称:	ValveTest(void)
**	函数功能:	电磁阀测试菜单
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-04-25
*************************************************************************/
void ValveTest(void)
{
    globalvar.devicetest = 1;

    FYD12864DispPrintfFlash(1, 1, "  电磁阀测试  ");
    FYD12864DispPrintfFlash(2, 1, "低  中  高  总");
    FYD12864DispPrintfFlash(3, 1, "○  ○  ○  ○");
    FYD12864DispPrintfFlash(4, 1, "【返回】停止测试");

    TaskAdd(KEY_1, ValveTestLow, NULL);
    TaskAdd(KEY_2, ValveTestMid, NULL);
    TaskAdd(KEY_3, ValveTestHigh, NULL);
    TaskAdd(KEY_4, ValveTestAll, NULL);
    TaskAdd(KEY_RET, ValveTestStop, NULL);
}

/************************************************************************
**	函数名称:	PressureWork(void)
**	函数功能:	压力测试
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void PressureWork(void)
{
    uint16_t pzero;
    uint16_t pratio;
    uint32_t curad;

    pzero = sysparas.pzero;
    pratio = sysparas.pcoeff;

    if(pzero == 0)
    {
        FYD12864DispPrintfFlash(2, 1, "压力零点错");
        FYD12864DispPrintfFlash(3, 1, "请按【返回】停止");
    }
    else
    {
        FYD12864DispPrintfFlash(2, 1, "零点:%d", pzero);
        curad = PlADCGetValue();
        FYD12864DispPrintfFlash(3, 1, "当前AD值:%d", curad);

        curad = curad;
        pzero = pzero;

        if(curad <= pzero)
        {
            curad = 0;
        }
        else
        {
            curad = curad - pzero;
        }

        FYD12864DispPrintfFlash(4, 1, "当前压力:%.02f", (double)curad / pratio);
    }

    TaskAdd(KEY_RET, DeviceTestMenu, NULL);
}

/************************************************************************
**	函数名称:	PressureTest(void)
**	函数功能:	压力测试界面
**	输入参数: 无
**	返回值		：无

**	创 建 者:	LY
**	创建日期:	2018-04-25
*************************************************************************/
void PressureTest(void)
{
    FYD12864ClrScreen();
    FYD12864DispPrintfFlash(1, 1, "  压力测试");
    DispUI = PressureWork;
}

/************************************************************************
**	函数名称:	GasMassmeterTest(void)
**	函数功能:	气相流量计测试
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-25
*************************************************************************/
void GasMassmeterTest(void)
{
    ModbusReadScanBuffer_G(_MODBUS_G_);
    globalvar.modbus_g_error++;

    if(globalvar.modbus_g_error > 5)
    {
        SetBit(syserr, SYSERR_MODBUS_G_AUTO_ERR);   //液相流量计通讯错误。
    }

    FYD12864DispPrintfFlash(1, 1, "流速:%.02f", realtime.cur_flowrate_g);
    FYD12864DispPrintfFlash(2, 1, "温度:%.02f", realtime.cur_temperature_g);
    FYD12864DispPrintfFlash(3, 1, "总累:%.02f", realtime.cur_totalize_g);
    FYD12864DispPrintfFlash(4, 1, "增益:%.02f", realtime.cur_gain_g);
    TaskAdd(KEY_RET, MassmeterTest, NULL);
    TaskAdd(KEY_STOP, MassmeterTest, NULL);
#if 0
    uint8_t  result;
    uint16_t deviceadd;//存储气相流量计返回的流量计地址 ADD BY LY

    FYD12864DispPrintfFlash(1, 1, "  气相流量计");
    FYD12864ClearLine(2);

    result = ModbusReadDeviceAddress_G(_MODBUS_G_, &deviceadd);

    if(result == true)
    {
        if(deviceadd == _MODBUS_G_)  //返回地址与实际地址相同则表示流量计通信成功 ADD BY LY
        {
            FYD12864DispPrintfFlash(3, 1, "  流量计连接正常");
        }
        else
        {
            FYD12864DispPrintfFlash(3, 1, "  流量计连接异常");
        }
    }
    else
    {
        FYD12864DispPrintfFlash(3, 1, "  读流量计失败");
    }

    FYD12864DispPrintfFlash(4, 1, "请按【返回】返回");
    TaskAdd(KEY_RET, MassmeterTest, NULL);
#endif
}

/************************************************************************
**	函数名称:	LiquidMassmeterTest(void)
**	函数功能:	液相流量计测试
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-25
*************************************************************************/
void LiquidMassmeterTest(void)
{
    ModbusReadScanBuffer_L(_MODBUS_L_);
    globalvar.modbus_l_error++;

    if(globalvar.modbus_l_error > 5)
    {
        SetBit(syserr, SYSERR_MODBUS_L_AUTO_ERR);   //液相流量计通讯错误。
    }

    FYD12864DispPrintfFlash(1, 1, "流速:%.02f", realtime.cur_flowrate_l);
    FYD12864DispPrintfFlash(2, 1, "温度:%.02f", realtime.cur_temperature_l);
    FYD12864DispPrintfFlash(3, 1, "总累:%.02f", realtime.cur_totalize_l);
    FYD12864DispPrintfFlash(4, 1, "增益:%.02f", realtime.cur_gain_l);
    TaskAdd(KEY_RET, MassmeterTest, NULL);
    TaskAdd(KEY_STOP, MassmeterTest, NULL);
#if 0
    uint8_t  result;
    uint16_t deviceadd;//存储液相流量计返回的流量计地址 ADD BY LY

    FYD12864DispPrintfFlash(1, 1, "  液相流量计");
    FYD12864ClearLine(2);

    result = ModbusReadDeviceAddress_L(_MODBUS_L_, &deviceadd);
    _delay_ms(100);

    if(result == true)
    {
        if(deviceadd == _MODBUS_L_)
        {
            FYD12864DispPrintfFlash(3, 1, "  流量计连接正常");
        }
        else
        {
            FYD12864DispPrintfFlash(3, 1, "  流量计连接异常");
        }
    }
    else
    {
        FYD12864DispPrintfFlash(3, 1, "  读流量计失败");
    }

    FYD12864DispPrintfFlash(4, 1, "请按【返回】返回");
    TaskAdd(KEY_RET, MassmeterTest, NULL);
#endif
}

/************************************************************************
**	函数名称:	GasMassmeterClear(void)
**	函数功能:	气相数据清零
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-25
*************************************************************************/
void GasMassmeterClear(void)
{
	if(globalvar.displayonce == 1)
	{
		globalvar.displayonce = 0;
		FYD12864ClrScreen();
		if(ModbusClearTotal_G(_MODBUS_G_))
		{
			FYD12864DispPrintfFlash(2,1,"气相清零成功");			
		}
		else
		{
			FYD12864DispPrintfFlash(2,1,"气相清零失败");
		}		
	}
	TaskMessage(300, MassmeterTest);
	TaskAdd(KEY_RET, MassmeterTest, NULL);
}

/************************************************************************
**	函数名称:	LiquidMassmeterClear(void)
**	函数功能:	液相数据清零
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-25
*************************************************************************/
void LiquidMassmeterClear(void)
{
	if(globalvar.displayonce == 1)
	{
		globalvar.displayonce = 0;
		FYD12864ClrScreen();
		if(ModbusClearTotal_L(_MODBUS_L_))
		{
			FYD12864DispPrintfFlash(2,1,"液相清零成功");			
		}
		else
		{
			FYD12864DispPrintfFlash(2,1,"液相清零失败");
		}
	}
	TaskMessage(300, MassmeterTest);
	TaskAdd(KEY_RET, MassmeterTest, NULL);
}

/************************************************************************
**	函数名称:	MassmeterTest(void)
**	函数功能:	流量计测试界面
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-25
*************************************************************************/
void MassmeterTest(void)
{
	globalvar.displayonce = 1;
    FYD12864DispPrintfFlash(1, 1, "1.气相数据信息");
    FYD12864DispPrintfFlash(2, 1, "2.液相数据信息");
    FYD12864DispPrintfFlash(3, 1, "3.气相总累清零");
	FYD12864DispPrintfFlash(4, 1, "4.液相总累清零");

	TaskAdd(KEY_1, GasMassmeterTest, NULL);
    TaskAdd(KEY_2, LiquidMassmeterTest, NULL);
    TaskAdd(KEY_3, GasMassmeterClear, NULL);
    TaskAdd(KEY_4, LiquidMassmeterClear, NULL);
    TaskAdd(KEY_STOP, DeviceTestMenu, NULL);
    TaskAdd(KEY_RET, DeviceTestMenu, NULL);    
}

/************************************************************************
**	函数名称:	PrinterTest(void)
**	函数功能:	打印机测试
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HHM
**	创建日期:	2016-04-25
*************************************************************************/
void PrinterTest(void)
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("       打印机测试\r\n");
    WHPrinterFreeRow(1);
    WHPrintfString("---------------------------\r\n");
    WHPrinterNormalCmd();
    WHPrintfString("   谢谢使用金星产品\r\n");
    WHPrinterFreeRow(6);
    SELECT_CARD();
    DispUI = DeviceTestMenu;
}

/************************************************************************
**	函数名称:	SoftReset(void)
**	函数功能:	软件重启
**	输入参数: 无
**	返回值		：无

**	创 建 者:	YCX
**	创建日期:	2018-09-25
*************************************************************************/
void SoftReset(void)
{
    (*(void(*)(void))0x00)();
}

/************************************************************************
**	函数名称:	CopyExToIn(void)
**	函数功能:	将数据从外部EEPROM拷贝到内部EEPROM
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void CopyExToIn(void)
{
    uint32_t    addr = 0, num, i;
    uint8_t     tmp1[100], tmp2[100], ret = false ;
    num = sizeof(tmp1);

    if(globalvar.displayonce == 1)
    {
        while(1)
        {
            //WATCH_DOG();
            if(EepromExternalReadBytes(addr, tmp1, num))     //从外部EEPROM读取数据放入tmp1中
            {
                if(EepromInterWriteBytes(addr, tmp1, num))     //将tmp1中的数据写入内部EEPROM
                {
                    if(EepromInterReadBytes(addr, tmp2, num))     //将刚刚写入内部EEPROM的数据读出，放入tmp2中
                    {
                        for(i = 0; i < num; i++)
                        {
                            if(tmp1[i] != tmp2[i])   //比对写入的数据与读出的数据是否一致，不一致跳出
                                break;
                        }

                        if(i == num)   //如果一致，表示copy成功
                            ret = true;
                        else
                            ret = false;
                    }
                    else
                    {
                        ret = false;
                    }
                }
                else
                {
                    ret = false;
                }
            }
            else
            {
                ret = false;
            }

            addr += num;

            if(addr >= 4000) break;

            if(ret == false) break;
        }

        globalvar.displayonce = 0;

        if(ret == true)
            SetSuccess();
        else
            SetFault();
    }

    TaskMessage(10, DispEepromCopy);
}

/************************************************************************
**	函数名称:	DispCopyExToIn(void)
**	函数功能:	将数据从外部EEPROM拷贝到内部EEPROM显示界面
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void DispCopyExToIn(void)
{
    FYD12864DispPrintfFlash(1, 1, "是否拷贝  外->内");
    FYD12864DispPrintfFlash(2, 1, "【确认】  继续");
    FYD12864DispPrintfFlash(3, 1, "【返回】  取消");
    FYD12864ClearLine(4);

    TaskAdd(KEY_OK, CopyExToIn, NULL);
    TaskAdd(KEY_RET, DispEepromCopy, NULL);
}

/************************************************************************
**	函数名称:	CopyInToEx(void)
**	函数功能:	将数据从内部EEPROM拷贝到外部EEPROM
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void CopyInToEx(void)
{
    uint32_t    addr = 0, num, i;
    uint8_t     tmp1[100], tmp2[100], ret = false;
    num = sizeof(tmp1);

    if(globalvar.displayonce == 1)
    {
        while(1)
        {
            //WATCH_DOG();
            if(EepromInterReadBytes(addr, tmp1, num))     //从内部EEPROM读取num字节，放入tmp1
            {
                if(EepromExternalWriteBytes(addr, tmp1, num))     //将tmp1中的num个字节，写入外部EEPROM
                {
                    if(EepromExternalReadBytes(addr, tmp2, num))     //将刚刚写入的num个数据从外部EEPROM读出到tmp2
                    {
                        for(i = 0; i < num; i++)
                        {
                            if(tmp1[i] != tmp2[i])   //比对外部EEPROM写入的tmp1和读出的tmp2中的数据
                                break;
                        }

                        if(i == num)
                            ret = true;
                        else
                            ret = false;
                    }
                    else
                    {
                        ret = false;
                    }
                }
                else
                {
                    ret = false;
                }
            }
            else
            {
                ret = false;
            }

            addr += num;

            if(addr >= 4000) break;

            if(ret == false) break;
        }

        globalvar.displayonce = 0;

        if(ret == true)
            SetSuccess();
        else
            SetFault();
    }

    TaskMessage(10, DispEepromCopy);
}


/************************************************************************
**	函数名称:	DispCopyInToEx(void)
**	函数功能:	将数据从内部EEPROM拷贝到外部EEPROM界面
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void DispCopyInToEx(void)
{
    FYD12864DispPrintfFlash(1, 1, "是否拷贝  内->外");
    FYD12864DispPrintfFlash(2, 1, "【确认】  继续");
    FYD12864DispPrintfFlash(3, 1, "【返回】  取消");
    FYD12864ClearLine(4);

    TaskAdd(KEY_OK, CopyInToEx, NULL);
    TaskAdd(KEY_RET, DispEepromCopy, NULL);
}

/************************************************************************
**	函数名称:	DispEepromCopy(void)
**	函数功能:	EEPROM 拷贝选择界面
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void DispEepromCopy(void)
{
    globalvar.displayonce = 1;
    FYD12864DispPrintfFlash(1, 1, "    EEPROM拷贝  ");
    FYD12864DispPrintfFlash(2, 1, "1.将内部拷到外部");
    FYD12864DispPrintfFlash(3, 1, "2.将外部拷到内部");


    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispCopyInToEx, NULL);
    TaskAdd(KEY_2, DispCopyExToIn, NULL);
    TaskAdd(KEY_RET, DeviceTestMenu, NULL);
}

/************************************************************************
**	函数名称:	DispLCDTest(void)
**	函数功能:	客显屏测试界面
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void DispLCDTest(void)
{
    FYD12864ClearLine(1);
    FYD12864DispPrintfFlash(2, 1, "    客显测试  ");
    FYD12864DispPrintfFlash(3, 1, "停止请按【返回】");
    FYD12864DispPrintfFlash(4, 1, "键不放，直到退出");

    OsplcHardwarechk();
    TaskAdd(KEY_RET, DeviceTestMenu, NULL);
}

/************************************************************************
**	函数名称:	DeviceTestMenu(void)
**	函数功能:	设备硬件测试界面
**	输入参数: 无
**	返回值		：无

**	创 建 者:	HMM
**	创建日期:	2016-04-25
*************************************************************************/
void DeviceTestMenu(void)
{
    FYD12864DispPrintfFlash(1, 1, "  硬件维护测试");
    FYD12864DispPrintfFlash(2, 1, "1.键盘  2.电磁阀");
    FYD12864DispPrintfFlash(3, 1, "3.压力  4.流量计");
    FYD12864DispPrintfFlash(4, 1, "5.客显  6.打印机");

    TaskAdd(KEY_1, KeyBoardTest, NULL);
    TaskAdd(KEY_2, ValveTest, NULL);
    TaskAdd(KEY_3, PressureTest, NULL);
    TaskAdd(KEY_4, MassmeterTest, NULL);
    TaskAdd(KEY_5, DispLCDTest, NULL);
    TaskAdd(KEY_6, PrinterTest, NULL);
    TaskAdd(KEY_8, DispEepromCopy, NULL);
    TaskAdd(KEY_COMB_SFIX, SoftReset, NULL);   //停止加定量 软件重启 ADD BY LY
    TaskAdd(KEY_RET, DispFirst, NULL);
}

