#include "kernel.h"

void ValveTestLow(void);
void ValveTestMid(void);
void ValveTestHigh(void);
void ValveTestAll(void);
void MassmeterTest(void);
/************************************************************************
**	��������:	KeyBoard_Test(void)
**	��������:	���̲���
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HHM
**	��������:	2016-4-13
*************************************************************************/

void KeyBoard_Test(void)
{
    FYD12864DispPrintfFlash(1, 1, "���̲���");
    FYD12864DispPrintfFlash(2, 1, "�밴��������");
    FYD12864DispPrintfFlash(4, 1, "�����ء�ֹͣ����");

    TaskAdd(KEY_RET, DeviceTestMenu, NULL);

    switch(globalvar.KeyValue)
    {
        case KEY_0:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����0");
            break;

        case KEY_1:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����1");
            break;

        case KEY_2:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����2");
            break;

        case KEY_3:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����3");
            break;

        case KEY_4:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����4");
            break;

        case KEY_5:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����5");
            break;

        case KEY_6:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����6");
            break;

        case KEY_7:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����7");
            break;

        case KEY_8:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����8");
            break;

        case KEY_9:
            FYD12864DispPrintfFlash(3, 1, "���ְ�����9");
            break;

        case KEY_PT:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: С����");
            break;

        case KEY_FIX:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: ����");
            break;

        case KEY_OK:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: ȷ��");
            break;

        case KEY_BACK:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: ����");
            break;

        case KEY_START:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: ����");
            break;

        case KEY_SET:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: �˵�");
            break;

        case KEY_QRY:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: ��ѯ");
            break;

        case KEY_CLR:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: ���");
            break;

        case KEY_STOP:
            FYD12864DispPrintfFlash(3, 1, "���ܰ���: ֹͣ");
            break;

        default:
            break;
    }
}

/************************************************************************
**	��������:	KeyBoardTest(void)
**	��������:	���̲���
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-4-13
*************************************************************************/
void KeyBoardTest(void)
{
    FYD12864ClrScreen();
    DispUI = KeyBoard_Test;
}

/************************************************************************
**	��������:	ValveTestStop(void)
**	��������:	�ر�ȫ����ŷ�
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HHM
**	��������:	2016-04-25
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
**	��������:	ValveTestLow(void)
**	��������:	�򿪵�ѹ��
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void ValveTestLow(void)
{
    FYD12864DispPrintfFlash(2, 1, "��  ��  ��  ��");
    FYD12864DispPrintfFlash(3, 1, "��  ��  ��  ��");

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
**	��������:	ValveTestMid(void)
**	��������:	����ѹ��
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void ValveTestMid(void)
{
    FYD12864DispPrintfFlash(2, 1, "��  ��  ��  ��");
    FYD12864DispPrintfFlash(3, 1, "��  ��  ��  ��");

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
**	��������:	ValveTestHigh(void)
**	��������:	�򿪸�ѹ��
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void ValveTestHigh(void)
{
    FYD12864DispPrintfFlash(2, 1, "��  ��  ��  ��");
    FYD12864DispPrintfFlash(3, 1, "��  ��  ��  ��");

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
**	��������:	ValveTestAll(void)
**	��������:	��ȫ������
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void ValveTestAll(void)
{
    FYD12864DispPrintfFlash(2, 1, "��  ��  ��  ��");
    FYD12864DispPrintfFlash(3, 1, "��  ��  ��  ��");

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
**	��������:	ValveTest(void)
**	��������:	��ŷ����Բ˵�
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-04-25
*************************************************************************/
void ValveTest(void)
{
    globalvar.devicetest = 1;

    FYD12864DispPrintfFlash(1, 1, "  ��ŷ�����  ");
    FYD12864DispPrintfFlash(2, 1, "��  ��  ��  ��");
    FYD12864DispPrintfFlash(3, 1, "��  ��  ��  ��");
    FYD12864DispPrintfFlash(4, 1, "�����ء�ֹͣ����");

    TaskAdd(KEY_1, ValveTestLow, NULL);
    TaskAdd(KEY_2, ValveTestMid, NULL);
    TaskAdd(KEY_3, ValveTestHigh, NULL);
    TaskAdd(KEY_4, ValveTestAll, NULL);
    TaskAdd(KEY_RET, ValveTestStop, NULL);
}

/************************************************************************
**	��������:	PressureWork(void)
**	��������:	ѹ������
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
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
        FYD12864DispPrintfFlash(2, 1, "ѹ������");
        FYD12864DispPrintfFlash(3, 1, "�밴�����ء�ֹͣ");
    }
    else
    {
        FYD12864DispPrintfFlash(2, 1, "���:%d", pzero);
        curad = PlADCGetValue();
        FYD12864DispPrintfFlash(3, 1, "��ǰADֵ:%d", curad);

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

        FYD12864DispPrintfFlash(4, 1, "��ǰѹ��:%.02f", (double)curad / pratio);
    }

    TaskAdd(KEY_RET, DeviceTestMenu, NULL);
}

/************************************************************************
**	��������:	PressureTest(void)
**	��������:	ѹ�����Խ���
**	�������: ��
**	����ֵ		����

**	�� �� ��:	LY
**	��������:	2018-04-25
*************************************************************************/
void PressureTest(void)
{
    FYD12864ClrScreen();
    FYD12864DispPrintfFlash(1, 1, "  ѹ������");
    DispUI = PressureWork;
}

/************************************************************************
**	��������:	GasMassmeterTest(void)
**	��������:	���������Ʋ���
**	�������: ��
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2018-09-25
*************************************************************************/
void GasMassmeterTest(void)
{
    ModbusReadScanBuffer_G(_MODBUS_G_);
    globalvar.modbus_g_error++;

    if(globalvar.modbus_g_error > 5)
    {
        SetBit(syserr, SYSERR_MODBUS_G_AUTO_ERR);   //Һ��������ͨѶ����
    }

    FYD12864DispPrintfFlash(1, 1, "����:%.02f", realtime.cur_flowrate_g);
    FYD12864DispPrintfFlash(2, 1, "�¶�:%.02f", realtime.cur_temperature_g);
    FYD12864DispPrintfFlash(3, 1, "����:%.02f", realtime.cur_totalize_g);
    FYD12864DispPrintfFlash(4, 1, "����:%.02f", realtime.cur_gain_g);
    TaskAdd(KEY_RET, MassmeterTest, NULL);
    TaskAdd(KEY_STOP, MassmeterTest, NULL);
#if 0
    uint8_t  result;
    uint16_t deviceadd;//�洢���������Ʒ��ص������Ƶ�ַ ADD BY LY

    FYD12864DispPrintfFlash(1, 1, "  ����������");
    FYD12864ClearLine(2);

    result = ModbusReadDeviceAddress_G(_MODBUS_G_, &deviceadd);

    if(result == true)
    {
        if(deviceadd == _MODBUS_G_)  //���ص�ַ��ʵ�ʵ�ַ��ͬ���ʾ������ͨ�ųɹ� ADD BY LY
        {
            FYD12864DispPrintfFlash(3, 1, "  ��������������");
        }
        else
        {
            FYD12864DispPrintfFlash(3, 1, "  �����������쳣");
        }
    }
    else
    {
        FYD12864DispPrintfFlash(3, 1, "  ��������ʧ��");
    }

    FYD12864DispPrintfFlash(4, 1, "�밴�����ء�����");
    TaskAdd(KEY_RET, MassmeterTest, NULL);
#endif
}

/************************************************************************
**	��������:	LiquidMassmeterTest(void)
**	��������:	Һ�������Ʋ���
**	�������: ��
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2018-09-25
*************************************************************************/
void LiquidMassmeterTest(void)
{
    ModbusReadScanBuffer_L(_MODBUS_L_);
    globalvar.modbus_l_error++;

    if(globalvar.modbus_l_error > 5)
    {
        SetBit(syserr, SYSERR_MODBUS_L_AUTO_ERR);   //Һ��������ͨѶ����
    }

    FYD12864DispPrintfFlash(1, 1, "����:%.02f", realtime.cur_flowrate_l);
    FYD12864DispPrintfFlash(2, 1, "�¶�:%.02f", realtime.cur_temperature_l);
    FYD12864DispPrintfFlash(3, 1, "����:%.02f", realtime.cur_totalize_l);
    FYD12864DispPrintfFlash(4, 1, "����:%.02f", realtime.cur_gain_l);
    TaskAdd(KEY_RET, MassmeterTest, NULL);
    TaskAdd(KEY_STOP, MassmeterTest, NULL);
#if 0
    uint8_t  result;
    uint16_t deviceadd;//�洢Һ�������Ʒ��ص������Ƶ�ַ ADD BY LY

    FYD12864DispPrintfFlash(1, 1, "  Һ��������");
    FYD12864ClearLine(2);

    result = ModbusReadDeviceAddress_L(_MODBUS_L_, &deviceadd);
    _delay_ms(100);

    if(result == true)
    {
        if(deviceadd == _MODBUS_L_)
        {
            FYD12864DispPrintfFlash(3, 1, "  ��������������");
        }
        else
        {
            FYD12864DispPrintfFlash(3, 1, "  �����������쳣");
        }
    }
    else
    {
        FYD12864DispPrintfFlash(3, 1, "  ��������ʧ��");
    }

    FYD12864DispPrintfFlash(4, 1, "�밴�����ء�����");
    TaskAdd(KEY_RET, MassmeterTest, NULL);
#endif
}

/************************************************************************
**	��������:	GasMassmeterClear(void)
**	��������:	������������
**	�������: ��
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2018-09-25
*************************************************************************/
void GasMassmeterClear(void)
{
	if(globalvar.displayonce == 1)
	{
		globalvar.displayonce = 0;
		FYD12864ClrScreen();
		if(ModbusClearTotal_G(_MODBUS_G_))
		{
			FYD12864DispPrintfFlash(2,1,"��������ɹ�");			
		}
		else
		{
			FYD12864DispPrintfFlash(2,1,"��������ʧ��");
		}		
	}
	TaskMessage(300, MassmeterTest);
	TaskAdd(KEY_RET, MassmeterTest, NULL);
}

/************************************************************************
**	��������:	LiquidMassmeterClear(void)
**	��������:	Һ����������
**	�������: ��
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2018-09-25
*************************************************************************/
void LiquidMassmeterClear(void)
{
	if(globalvar.displayonce == 1)
	{
		globalvar.displayonce = 0;
		FYD12864ClrScreen();
		if(ModbusClearTotal_L(_MODBUS_L_))
		{
			FYD12864DispPrintfFlash(2,1,"Һ������ɹ�");			
		}
		else
		{
			FYD12864DispPrintfFlash(2,1,"Һ������ʧ��");
		}
	}
	TaskMessage(300, MassmeterTest);
	TaskAdd(KEY_RET, MassmeterTest, NULL);
}

/************************************************************************
**	��������:	MassmeterTest(void)
**	��������:	�����Ʋ��Խ���
**	�������: ��
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2018-09-25
*************************************************************************/
void MassmeterTest(void)
{
	globalvar.displayonce = 1;
    FYD12864DispPrintfFlash(1, 1, "1.����������Ϣ");
    FYD12864DispPrintfFlash(2, 1, "2.Һ��������Ϣ");
    FYD12864DispPrintfFlash(3, 1, "3.������������");
	FYD12864DispPrintfFlash(4, 1, "4.Һ����������");

	TaskAdd(KEY_1, GasMassmeterTest, NULL);
    TaskAdd(KEY_2, LiquidMassmeterTest, NULL);
    TaskAdd(KEY_3, GasMassmeterClear, NULL);
    TaskAdd(KEY_4, LiquidMassmeterClear, NULL);
    TaskAdd(KEY_STOP, DeviceTestMenu, NULL);
    TaskAdd(KEY_RET, DeviceTestMenu, NULL);    
}

/************************************************************************
**	��������:	PrinterTest(void)
**	��������:	��ӡ������
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HHM
**	��������:	2016-04-25
*************************************************************************/
void PrinterTest(void)
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintfString("       ��ӡ������\r\n");
    WHPrinterFreeRow(1);
    WHPrintfString("---------------------------\r\n");
    WHPrinterNormalCmd();
    WHPrintfString("   ллʹ�ý��ǲ�Ʒ\r\n");
    WHPrinterFreeRow(6);
    SELECT_CARD();
    DispUI = DeviceTestMenu;
}

/************************************************************************
**	��������:	SoftReset(void)
**	��������:	�������
**	�������: ��
**	����ֵ		����

**	�� �� ��:	YCX
**	��������:	2018-09-25
*************************************************************************/
void SoftReset(void)
{
    (*(void(*)(void))0x00)();
}

/************************************************************************
**	��������:	CopyExToIn(void)
**	��������:	�����ݴ��ⲿEEPROM�������ڲ�EEPROM
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
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
            if(EepromExternalReadBytes(addr, tmp1, num))     //���ⲿEEPROM��ȡ���ݷ���tmp1��
            {
                if(EepromInterWriteBytes(addr, tmp1, num))     //��tmp1�е�����д���ڲ�EEPROM
                {
                    if(EepromInterReadBytes(addr, tmp2, num))     //���ո�д���ڲ�EEPROM�����ݶ���������tmp2��
                    {
                        for(i = 0; i < num; i++)
                        {
                            if(tmp1[i] != tmp2[i])   //�ȶ�д�������������������Ƿ�һ�£���һ������
                                break;
                        }

                        if(i == num)   //���һ�£���ʾcopy�ɹ�
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
**	��������:	DispCopyExToIn(void)
**	��������:	�����ݴ��ⲿEEPROM�������ڲ�EEPROM��ʾ����
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void DispCopyExToIn(void)
{
    FYD12864DispPrintfFlash(1, 1, "�Ƿ񿽱�  ��->��");
    FYD12864DispPrintfFlash(2, 1, "��ȷ�ϡ�  ����");
    FYD12864DispPrintfFlash(3, 1, "�����ء�  ȡ��");
    FYD12864ClearLine(4);

    TaskAdd(KEY_OK, CopyExToIn, NULL);
    TaskAdd(KEY_RET, DispEepromCopy, NULL);
}

/************************************************************************
**	��������:	CopyInToEx(void)
**	��������:	�����ݴ��ڲ�EEPROM�������ⲿEEPROM
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
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
            if(EepromInterReadBytes(addr, tmp1, num))     //���ڲ�EEPROM��ȡnum�ֽڣ�����tmp1
            {
                if(EepromExternalWriteBytes(addr, tmp1, num))     //��tmp1�е�num���ֽڣ�д���ⲿEEPROM
                {
                    if(EepromExternalReadBytes(addr, tmp2, num))     //���ո�д���num�����ݴ��ⲿEEPROM������tmp2
                    {
                        for(i = 0; i < num; i++)
                        {
                            if(tmp1[i] != tmp2[i])   //�ȶ��ⲿEEPROMд���tmp1�Ͷ�����tmp2�е�����
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
**	��������:	DispCopyInToEx(void)
**	��������:	�����ݴ��ڲ�EEPROM�������ⲿEEPROM����
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void DispCopyInToEx(void)
{
    FYD12864DispPrintfFlash(1, 1, "�Ƿ񿽱�  ��->��");
    FYD12864DispPrintfFlash(2, 1, "��ȷ�ϡ�  ����");
    FYD12864DispPrintfFlash(3, 1, "�����ء�  ȡ��");
    FYD12864ClearLine(4);

    TaskAdd(KEY_OK, CopyInToEx, NULL);
    TaskAdd(KEY_RET, DispEepromCopy, NULL);
}

/************************************************************************
**	��������:	DispEepromCopy(void)
**	��������:	EEPROM ����ѡ�����
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void DispEepromCopy(void)
{
    globalvar.displayonce = 1;
    FYD12864DispPrintfFlash(1, 1, "    EEPROM����  ");
    FYD12864DispPrintfFlash(2, 1, "1.���ڲ������ⲿ");
    FYD12864DispPrintfFlash(3, 1, "2.���ⲿ�����ڲ�");


    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispCopyInToEx, NULL);
    TaskAdd(KEY_2, DispCopyExToIn, NULL);
    TaskAdd(KEY_RET, DeviceTestMenu, NULL);
}

/************************************************************************
**	��������:	DispLCDTest(void)
**	��������:	���������Խ���
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void DispLCDTest(void)
{
    FYD12864ClearLine(1);
    FYD12864DispPrintfFlash(2, 1, "    ���Բ���  ");
    FYD12864DispPrintfFlash(3, 1, "ֹͣ�밴�����ء�");
    FYD12864DispPrintfFlash(4, 1, "�����ţ�ֱ���˳�");

    OsplcHardwarechk();
    TaskAdd(KEY_RET, DeviceTestMenu, NULL);
}

/************************************************************************
**	��������:	DeviceTestMenu(void)
**	��������:	�豸Ӳ�����Խ���
**	�������: ��
**	����ֵ		����

**	�� �� ��:	HMM
**	��������:	2016-04-25
*************************************************************************/
void DeviceTestMenu(void)
{
    FYD12864DispPrintfFlash(1, 1, "  Ӳ��ά������");
    FYD12864DispPrintfFlash(2, 1, "1.����  2.��ŷ�");
    FYD12864DispPrintfFlash(3, 1, "3.ѹ��  4.������");
    FYD12864DispPrintfFlash(4, 1, "5.����  6.��ӡ��");

    TaskAdd(KEY_1, KeyBoardTest, NULL);
    TaskAdd(KEY_2, ValveTest, NULL);
    TaskAdd(KEY_3, PressureTest, NULL);
    TaskAdd(KEY_4, MassmeterTest, NULL);
    TaskAdd(KEY_5, DispLCDTest, NULL);
    TaskAdd(KEY_6, PrinterTest, NULL);
    TaskAdd(KEY_8, DispEepromCopy, NULL);
    TaskAdd(KEY_COMB_SFIX, SoftReset, NULL);   //ֹͣ�Ӷ��� ������� ADD BY LY
    TaskAdd(KEY_RET, DispFirst, NULL);
}

