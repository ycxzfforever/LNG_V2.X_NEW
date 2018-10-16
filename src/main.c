#include "kernel.h"

GlobalVar        globalvar;			//ȫ�ֱ���
ShiftRecordInfo  shiftrecordinfo;	//���°��¼��Ϣ
FuelRecordInfo   fuelrecordinfo;	//������ˮ��¼��Ϣ
FuelRecordInfo   fuelrecordinfo_tmp;//������ˮ��¼��Ϣ
ErrorRecordInfo	 errorRecordinfo;	//������ˮ��Ϣ
Sysparas         sysparas;			//ϵͳ����
Sysparas         tmp_sysparas;		//�û��޸�ϵͳ����ʱ����ʱ�洢��Ҫ�޸ĵĲ���
M1carddata       m1card;    		//���ṹ���ݡ�

uint32_t         syserr;       		//ϵͳ�쳣
uint32_t         stopreason;   		//ͣ��ԭ��
uint32_t		 carderror;	   		//��IC�������йصĴ���

uint32_t    	 SYSPARA_ADDR = SYSPARA_ADDR1;
uint8_t			 _MODBUS_G_ = 1;
uint8_t			 _MODBUS_L_ = 1;

//syserr 32λ������32���쳣��ĳλΪ1��ʾ1���쳣
__farflash char ErrorInfo[][16] =
{
    "FLASH ���ʧ��",//0
    "FLASH дʧ��",
    "FLASH ��ʧ��",
    "д�ڲ�EEPROM����",
    "���ڲ�EEPROM����",
    "д�ⲿEEPROM����",//5
    "���ⲿEEPROM����",
    "����EE���ݲ�һ��",
    "����������ͨѶ��",
    "Һ��������ͨѶ��",// 9
    "ѹ���������쳣",
};


//�����ÿ�����ʱ��δ�忨��ɨ���˵��ӱ�ǩ
void DispInserCard(void)
{
    FYD12864ClearLine(1);

    if(sysparas.offlinemode == 1)
    {
        FYD12864DispPrintfFlash(2, 1, "�ѻ�ģʽ�����ÿ�");
        FYD12864ClearLine(3);
    }
    else
    {
        FYD12864DispPrintfFlash(2, 1, "�����IC������");
        FYD12864DispPrintfFlash(3, 1, "����������");
    }

    FYD12864ClearLine(4);
    TaskAdd(KEY_RET, DispFirst, NULL);
}


void DispErrorInfo()
{
    uint8_t row = 0, num = 0, i = 4;
    uint32_t tmpsyserror = syserr;

    globalvar.workstate = StaFault; //���������ϡ�

    for(row = 0; row < 4;)
    {
        num = GetOnePosition(tmpsyserror);   //����Ϊ1��λ��λ�� ADD BY LY

        if((num == 9) || (num == 10) || (num == 11))        //���9��10��11�е�����λΪ1����ʾ�����ƺ�ѹ������������ ADD BY LY
            ReadModbusAndPressure();//�������ƺ�ѹ�������� ADD BY LY

        if(num > 0)
        {
            ClrBit(tmpsyserror, num - 1);   //�ҵ�1��λ�ú������ ADD BY LY
            row++;
            FYD12864DispString(row, 1, ErrorInfo[num - 1], 1);   //��ʾ��Ӧ���� ADD BY LY
        }
        else
            break;
    }

    for(i = 4; i > row; i--)
    {
        FYD12864ClearLine(i);
    }

    if(sysparas.lcdtype == 1)   //��Ҫ���ڴ�����ʾ����
    {
        OsplcShowN(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price);
    }
}

void ClearSyserror(void)
{
    syserr = 0;
    DispUI = DispFirst;
}


void DispFirst(void)
{
    globalvar.timeout = 0;           				//��ʱʱ�����㡣
    globalvar.modbus_g_error = 0;    				//�����������ƴ�������㡣
    globalvar.modbus_l_error = 0;    				//��Һ�������ƴ�������㡣
    globalvar.fuelstep = 0;         				//�������衣
    globalvar.fix = 0;               				//������־��
    globalvar.fixnum = 0;
    globalvar.fixmoney = 0;
    globalvar.U8Temp = 1;
    globalvar.devicetest = 0;
    globalvar.stoptimes = 0;
    globalvar.workstate = StaIdle;
	
    InputInit();
    memset(&m1card.serialnumber[0], 0, sizeof(M1carddata));   	//�����ݽṹ�����㡣
    AB_OUT_HIGH();

    if(syserr)
    {
        DispErrorInfo();//��ȻЭ�������б�����ˮ��ʵ��û��ʹ�� ����ʱ��д�ϴ����󱨾��Ĳ��� ADD BY LY
    }
    else
    {
        globalvar.workstate = StaIdle;			
		if(globalvar.eepromflag == 1)
        {
        	FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "EEPROM��������");
            FYD12864DispPrintfFlash(3, 1, "�뼰ʱ��������");
            FYD12864ClearLine(4);
        }
        else
        {
        	FYD12864DispPrintfFlash(1, 1, "ǹ��:%02d ���:%02d", sysparas.gunnum, sysparas.shiftnum);
            FYD12864DispPrintfFlash(2, 1, "����:%ld.%02d", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);
            FYD12864DispPrintfFlash(3, 1, "���:%ld.%02d", fuelrecordinfo.money / 100, fuelrecordinfo.money % 100);
			if((globalvar.comfailcount > 10) && (sysparas.offlinemode == 0))       //��������������ģʽ�������ӳ�ʱ
	        {
	            FYD12864DispPrintfFlash(4, 1, "�������ѵ���");
	        }
	        else
	        {
	            DispDateTime();
	        }
        }
        
        TaskAdd(KEY_BACK, StartPrecooling, DispFirst);			  //ֻ��Ԥ��
        if((sysparas.fuelmode == 0) && (sysparas.usecard != 1))	 //�ֶ�������
        {
            TaskAdd(KEY_START, StartPrecooling, StartFueling);    //Ԥ����Һ��
        }

        if(sysparas.usecard != 1)
        {
            TaskAdd(KEY_FIX, FixGas, NULL);	//����������
        }
        else
        {
            TaskAdd(KEY_START, DispInserCard, NULL);
            TaskAdd(KEY_FIX, DispInserCard, NULL);
        }
    }

    if(sysparas.simulation == 1)
    {
        TaskAdd(KEY_RET, ClearSyserror, NULL);
    }

    TaskAdd(KEY_QRY, DispQuery1, NULL);								//��ѯ��
    TaskAdd(KEY_SET, DispInputMenuPwd, NULL);						//���á�
    TaskAdd(KEY_COMB_SBACK, DispInputQcPwd, NULL);					//ֹͣ+���� �޸��ʼ����	(��������ʼ�ģʽ�޸�)
    TaskAdd(KEY_COMB_SQRY, DispInputTestPwd, NULL);					//ֹͣ+��ѯ Ӳ�����ԡ�
    TaskAdd(KEY_COMB_SSET, DispInputResetPwd, NULL); 				//ֹͣ+�˵� ������λ���ʼ�ģʽ���ָ��������á�
    TaskAdd(KEY_COMB_SRETPOINT, SystemReStartToBootLoader, NULL);   //������BootLoader
    TaskAdd(KEY_COMB_SRETCLR, SystemReStartToAPP, NULL);   			//������APP
}

//ѭ����������ʱ������������
void CirculateFueling(void)
{
    globalvar.fixnum = 100 + time.day + time.hour + time.minute + time.second;
    globalvar.fix = 1;  	 //����

    if((m1card.existflag == 0) && (sysparas.usecard != 1)) 		//�޿�����
    {
        DispUI = StartPrecooling;
        DispNextUI = StartFueling;
    }
    else if((m1card.existflag == 1) && (sysparas.usecard != 2))       //�п�����
    {
        DispUI = StartPrecooling;
        DispNextUI = GreyLock;
    }
    else
    {
        DispUI = DispFirst;
    }

    globalvar.displayonce = 1;
}

/*************************************************************************
** �������ƣ�Task100msProcess
** �������ܣ�100ms������
** ��ڲ�������
** �� �� ֵ����
** �� �� �ߣ����
** ����ʱ�䣺2016-12-7 13:21:02

** �� �� �ߣ�
** �޸����ڣ�
** �޸����ݣ�
*************************************************************************/
void Task100msProcess(void)
{
    timer1.flag_100ms = 0;

    if((globalvar.BeepCount--) == 0)
    {
        BEEP_OFF();
    }

    if(globalvar.MsgTime > 0)
        globalvar.MsgTime--;

    RecBlckDataRplyCheck();//��̨����������Ч���ж�

    DispUI();// DispUI()�ڲ�ͬʱ�䣬ִ�еĲ�ͬ�������ڵ�ǰUI��ʾ���Ժ󣬶�DispUI���и�ֵ��������ʾDispFirst() BY  LY
    globalvar.KeyValue = 0; //����ˢ�º��轫��ֵ����

}

/*************************************************************************
** �������ƣ�Task500msProcess
** �������ܣ�500ms������
** ��ڲ�������
** �� �� ֵ����
** �� �� �ߣ����
** ����ʱ�䣺2016-12-7 13:21:02

** �� �� �ߣ�
** �޸����ڣ�
** �޸����ݣ�
*************************************************************************/
void Task500msProcess(void)
{
    timer1.flag_500ms = 0;
    Ds3232ReadTime();

    if(LED1_RUN == 0)
        LED1_ON();
    else
        LED1_OFF();

    if(BA == 1)   //һ��˫����Һʱ���ж�״̬
    {
        globalvar.otherstate = 1; //�Է�����
    }
    else
    {
        globalvar.otherstate = 0;//�Է���Һ
    }
}

/*************************************************************************
** �������ƣ�Task1sProcess
** �������ܣ�1s������
** ��ڲ�������
** �� �� ֵ����
** �� �� �ߣ����
** ����ʱ�䣺2016-12-7 13:21:02

** �� �� �ߣ�
** �޸����ڣ�
** �޸����ݣ�
*************************************************************************/
void Task1sProcess(void)
{
    timer1.flag_1s = 0;

    if((globalvar.devicetest == 0) && ((globalvar.workstate == StaIdle) || (globalvar.workstate == StaFault)))           //�°࣬���У�������ʵʱ�жϣ���ŷ�����ʱ�����ж�
    {
        if(globalvar.otherstate == 0)  		   //�Է���Һ
        {
            //PumpSwitch(StaOtherFuel);
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();      //�й�
            VCTRL_HIGH_CLOSE();     //�߹�
        }
        else if(globalvar.otherstate == 1)   //�Է�����
        {
            //PumpSwitch(StaIdle);
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();      //�п�
            VCTRL_HIGH_CLOSE();    //�߹�
        }
    }

    if(globalvar.stoppumptime > 0)
    {
        globalvar.stoppumptime--;//��ʱͣ�õ���ʱ
    }

    if((globalvar.devicetest == 0) && (globalvar.stoppumptime == 0) && (globalvar.workstate != StaFuel))       //��Һ����ʱͣ��
    {
        if(globalvar.otherstate == 0) 		   //�Է���Һ
        {
            PumpSwitch(StaOtherFuel);
        }
        else if(globalvar.otherstate == 1)   //�Է�����
        {
            PumpSwitch(StaIdle);
        }
    }

    if((m1card.existflag == 0) && (globalvar.workstate == StaIdle) && (sysparas.usecard != 2))         //�޿�״̬���ڿ����°�ʱѰ����
    {
        if(Mt318GetCardSerialNum())                          //Ѱ����
        {
            DispUI = GetCardData;                            //������
        }
    }

    if(m1card.existflag == 1)                                //ycx20161125���п���ʵʱ��⿨״̬
    {
        if(Mt318GetCardStatus() == false)
        {
            if(Mt318GetCardStatus() == false)                //�����ж϶�û�п���֤�������γ�
            {
                m1card.existflag = 0;
                m1card.checkresult = 0xFF;
            }
        }
    }

    if((globalvar.circulatefue == 1) &&
            (globalvar.workstate == StaIdle) &&
            (globalvar.displayonce == 1) &&
            (((sysparas.usecard != 2) && (m1card.existflag == 1) && (m1card.checkresult == 0)) ||          //�п�����
             ((sysparas.usecard != 1) && (m1card.existflag == 0))) &&        //�޿�����
            ((DispUI == DispFirst) || (DispUI == DispCardInfo))       //ֻ���׽���Ϳ���������ж��Ƿ�����Զ�����
      ) //�Ƿ�ѭ��������
    {
        globalvar.displayonce = 0;
        TaskMessage(20, CirculateFueling);
    }

}

/*************************************************************************
** �������ƣ�HW_Initial
** �������ܣ���Ӳ����ʼ��
** ��ڲ�������
** �� �� ֵ����
*************************************************************************/
void HW_Initial(void)
{
    _delay_ms(3000);
    OS_ENTER_CRITICAL();								//��ȫ���ж�

    memset(&globalvar.U8Temp, 0, sizeof(GlobalVar));    //ȫ�ֱ�����ʼ����  �˴�Ҳ����д�� memset(&globalvaribles.U8Temp,0,sizeof(struct GlobalVar)); ADD BY LY �ṹ������ֶ���
    memset(&m1card, 0, sizeof(M1carddata));     		//M1���ṹ���ʼ�� ADD BY LY

    PowerCheck();				//������
    SI_INDATE_IN();				//���ܰ�
    SI_INDATE_H();				//���ü��ܰ�����Ϊ����
    BA_IN();					//����
    BA_OUT_HIGH();				//����BAΪ�������� ADD BY LY

    AB_OUT() ;					//����
    AB_OUT_HIGH();

    LED1_OUT();					//����ָʾ��

    LedInit();
    FYD12864Init();				//��ʼ������
    Uart0Init();				//uart0 ��ʼ�� ���ڴ�ӡ���������
    KeyBoardInit();				//���̳�ʼ��

    JS28F128FlashInit();
    I2cInit();					//I2C��ʼ�� ����DS3232 �� �ⲿ EEPROM

    PgADCUinit();				//ADC��ʼ�� ����ѹ����������ʼ��
    PlADCUinit();				//ADC��ʼ�� Һ��ѹ����������ʼ��
    ValveInit();				//��ŷ���ʼ��

    SELECT_PRINT();				//uart0 ѡ��Ϊ��ӡ��
    WHPrinterInit();			//��ӡ����ʼ����
    WHPrinterNormalCmd();
    SELECT_CARD();				//uart0 ѡ��Ϊ������
    JS28F128FlashInit();		//�ⲿflash IO ��ʼ��

    Timer1Init();				//Timer1 ��ʼ��
    //Timer3Init();				//Timer3 ��ʼ��
    OS_EXIT_CRITICAL();			//��ȫ���ж�
}



/*************************************************************************
** �������ƣ�HW_Test
** �������ܣ���Ӳ�����ܲ���
** ��ڲ�������
** �� �� ֵ����
*************************************************************************/
void HW_Test(void)
{
    if(!EepromDeviceTest())   ////EEPROM����
    {
        FYD12864ClrScreen();
        FYD12864DispPrintfFlash(2, 1, "��EERPOM Fail");
        SetBit(syserr, SYSERR_W_IN_EEPROM);
        TaskMessage(50, DispFirst);
    }
    else
    {
        FYD12864ClrScreen();
        FYD12864DispPrintfFlash(2, 1, "��EERPOM Pass");
        ClrBit(syserr, SYSERR_W_IN_EEPROM);

    }

    if(!JS28F128FlashCheckId())   //NORflash����
    {
        FYD12864ClrScreen();
        FYD12864DispPrintfFlash(2, 1, "FLASH ���ʧ��");
        SetBit(syserr, SYSERR_FLASH_CHK);
        TaskMessage(50, DispFirst);
    }
    else
    {
        FYD12864ClrScreen();
        FYD12864DispPrintfFlash(2, 1, "FLASH ���ɹ�");
        ClrBit(syserr, SYSERR_FLASH_CHK);
    }

    Mt318GetCardSerialNum();
}


void main(void)
{
    HW_Initial();
    Timer1On();
    //�ж��Ƿ����,��ֹ�������ݵ�ѹ���ٽ��ʱʹcpu��λ��������ݴ���
    if(PowerCheckIn == 1)
    {
        _delay_ms(20);
        if(PowerCheckIn == 1)
        {
 			return;
        }
    }
    //��ȡʱ��
    Ds3232ReadTime();
    //�������ʼ��
    DispUI = DispFirst;
    globalvar.comreceiveflag = 0;
    PumpSwitch(StaIdle);
    //��ϵͳ����
    ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(sysparas));
			
    //�ж�EEPROM��д�Ƿ񳬳�����
    if(sysparas.writeE2num >= EEPROM_W_TIMES)
    {
        SYSPARA_ADDR = SYSPARA_ADDR2;
        ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //��֤����������һ���ϵͳ����

        if(sysparas.writeE2num >= EEPROM_W_TIMES)
        {
            SYSPARA_ADDR = SYSPARA_ADDR3;
            ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //��֤����������һ���ϵͳ����

            if(sysparas.writeE2num >= EEPROM_W_TIMES)
            {
                SYSPARA_ADDR = SYSPARA_ADDR4;
                ReadSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas));       //��֤����������һ���ϵͳ����

                if(sysparas.writeE2num >= EEPROM_W_TIMES)
                {
                    globalvar.eepromflag = 1; //��ʾ����eeprom
                }
                else
                    globalvar.eepromflag = 0;
            }
        }
    }
    
	//���Գ�ʼ��
    OsplcInit();	

    //�ȶ�ȡϵͳ�������ٸ���ϵͳ������ʼ��������
    Uart1Init(sysparas.modbusbaud);   //Һ�������ƶ˿ڳ�ʼ�� 19200
    Uart3Init(sysparas.modbusbaud);   //���������ƶ˿ڳ�ʼ����
    Uart2Init(sysparas.pcbaud);   //��̨ͨѶ 485 �˿ڳ�ʼ��

    //�����һ��ʵʱ��ˮ
    EepromInterReadBytes(REALTIMEDATA_ADDR, (uint8_t*)&realtime, sizeof(realtime));      //�����һ��ʵʱ��ˮ

    //�жϼ��������һ�εĹ���״̬
    if((realtime.workstatus == StaFuel) || (realtime.workstatus == StaPay))       //����ϴμ��������ڼ������߿۷ѹ����е��硣
    {
        //�ָ��������� ADD BY LY
        PowerDownRealDate();
        memset(&realtime.lcng, 0, sizeof(RealTimeData));     //ycx����������ʵʱ���ݡ�
        EepromInterWriteBytes(REALTIMEDATA_ADDR, (uint8_t*)&realtime, sizeof(realtime));      //������������ʵʱ��ˮ���ݣ�Ϊ��һ�μ�����׼��
    }
    else
    {
        //�����״μ�������ȡ�ϴ���ˮ��¼
        if(sysparas.transnum > 0)
            ReadFuelRecord(sysparas.transnum - 1, 0);   //���������ˮ�Ŵ���0�����ȡ��һ����ˮBY  LY
    }    
	OsplcShow(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price, 0, 1);

    //������������ȡ�����Ƶ�ֵ
    if(sysparas.simulation == 0)
    {
        _MODBUS_G_ = sysparas.modbusaddr_g;
        _MODBUS_L_ = sysparas.modbusaddr_l;
        ModbusInitReg_L();

        if(sysparas.backgascalc == 1)   //��������ʱ�ų�ʼ������������
        {
            ModbusInitReg_G();
        }
    }

    while(1)
    {
        realtime.workstatus = globalvar.workstate;
        KeyBoardReadKeyValue();
#if 0

        if(modbus.flag == 1)   //���ò�ѯ��־�ķ�ʽִ����صĳ���
        {
            modbus.flag = 0;
            ModbusDataDisposs_G();
            ModbusDataDisposs_L();
        }

#endif

        if(timer1.flag_100ms == 1)
        {
            Task100msProcess();
        }

        if(timer1.flag_500ms == 1)
        {
            Task500msProcess();
        }

        if(timer1.flag_1s == 1)
        {
            Task1sProcess();
        }
    }
}


