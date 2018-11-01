#include "kernel.h"

/************************************************************************
**	��������:	PumpSwitch(uint8_t state)
**	��������:	���ݼ�������״̬�򿪺͹ر���Ӧ�ķ��ź�PLC�����ź�
**	�������: state:��������״̬
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-4-1
*************************************************************************/
/*PCL1��PCL2�����̵������ڸ�PLC���俪��������ͨ��PLC���Ʊ�  BY  LY*/
/*���������̵������ڿ��Ƹߡ��С���������ŷ�  BY  LY*/
void PumpSwitch(uint8_t state)
{
    switch(state)
    {
        case StaClose://ͣ��
            PUMPCTRL_HIGH_CLOSE();  //�ø߹�
            PUMPCTRL_LOW_CLOSE();   //�õ͹�
            VCTRL_CLOSEKEY();       //������
            VCTRL_MID_CLOSE();      //�й�
            VCTRL_HIGH_CLOSE();     //�߹�
            break;

        case StaOtherFuel://�Է���Һ
            PUMPCTRL_LOW_OPEN();    //�õͿ�
            PUMPCTRL_HIGH_CLOSE();  //�ø߹�
            VCTRL_OPENKEY();        //������
            VCTRL_MID_CLOSE();      //�й�
            VCTRL_HIGH_CLOSE();     //�߹�
            break;

        case StaSPrecool://Сѭ��Ԥ��
            AB_OUT_LOW();//һ��˫����� ADD BY LY
            PUMPCTRL_HIGH_OPEN();
            PUMPCTRL_LOW_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();
            VCTRL_HIGH_CLOSE();
            break;

        case StaBPrecool://��ѭ��Ԥ��
            AB_OUT_LOW();//һ��˫����� ADD BY LY
            PUMPCTRL_HIGH_OPEN();
            PUMPCTRL_LOW_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();
            VCTRL_HIGH_OPEN();
            break;

        case StaFuel://��Һ
            AB_OUT_LOW();//һ��˫����� ADD BY LY
            PUMPCTRL_LOW_OPEN();    //PLC
            PUMPCTRL_HIGH_OPEN();   //PLC
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();
            VCTRL_HIGH_OPEN();
            break;

        case StaIdle://����
        default:
            AB_OUT_HIGH();//һ��˫����� ADD BY LY
            PUMPCTRL_LOW_OPEN();
            PUMPCTRL_HIGH_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();//����ʱ������ѹ��������Сѭ��״̬����Ϊ�˷�ֹ��ѹ����Ϊ��·����LNGҺ���¶����ߺ󣬻�����������ѹ�� ADD BY LY
            VCTRL_HIGH_CLOSE();
            break;
    }
}

/************************************************************************
**	��������:	PumpSwitch(uint8_t state)
**	��������:	����������ȡ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-12-2
*************************************************************************/
void GetIntNum(void)
{
    if(globalvar.KeyValue == KEY_PT)   //��С����ȡ����������
    {
        globalvar.fix = 2;        //����
        globalvar.fixmoney = ((fuelrecordinfo.money + 100) / 100) * 100;
    }
}

/************************************************************************
**	��������:	JudgeStopConditions(void)
**	��������:	ͣ�������ж�
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-5-10
*************************************************************************/
void JudgeStopConditions(void)
{
    double money_tmp;
    double price_tmp;
    double volume_tmp;
	
    if(globalvar.timeout > (uint32_t)(sysparas.delaystoptime))       //ͣ����ʱ������ж�ͣ������
    {
        if(realtime.cur_flowrate_l < (float)sysparas.lowratestop)     //������ͣ��
        {
            globalvar.stoptimes++;
            stopreason = 8;
        }

        if(realtime.cur_flowrate_l > (float)sysparas.highratestop)     //������ͣ��
        {
            globalvar.stoptimes++;
            stopreason = 9;
        }

        if(realtime.cur_pressure > ((float)sysparas.pressurestop))       //��ѹͣ��??
        {
            globalvar.stoptimes++;
            stopreason = 12;
        }

        if(realtime.cur_density_g > ((float)sysparas.backdenstystop / 10000.0))       //�����ܶ�ͣ��
        {
            globalvar.stoptimes++;
            stopreason = 10;
        }

        if(realtime.cur_gain_g > ((float)sysparas.backgainstop))       //��������ͣ��
        {
            globalvar.stoptimes++;
            stopreason = 11;
        }
    }

    if(globalvar.stoptimes > 100)
    {
        globalvar.KeyValue = KEY_STOP;
    }
    else
    {
        stopreason = 0;
    }

    if(m1card.existflag == 1)
    {
        if(m1card.cardtype_another == 3)   //���˿���
        {
            if((fuelrecordinfo.money + m1card.cardbalance_another) >= sysparas.accountcardmaxyue)     //������ޡ�
            {
                money_tmp = (double)(fuelrecordinfo.money);
                price_tmp = (double)(realtime.price);
                fuelrecordinfo.volume = (uint32_t)(money_tmp / price_tmp * 100 + 0.5);
                stopreason = 14;
                globalvar.KeyValue = KEY_STOP;
            }
        }
        else if((m1card.cardtype_another != 3) && (m1card.cardtype_another != 7))       //�Ǽ��˿�
        {
            if((fuelrecordinfo.money + sysparas.cardminyue) >= m1card.cardbalance_another)
            {
                money_tmp = (double)(fuelrecordinfo.money);
                price_tmp = (double)(realtime.price);
                fuelrecordinfo.volume = (uint32_t)(money_tmp / price_tmp * 100 + 0.5);
                stopreason = 15;
                globalvar.KeyValue = KEY_STOP;
            }
        }
    }

    if(sysparas.intnumflag == 1)   //�ж��Ƿ���Ҫȡ��
    {
        GetIntNum();
    }

    if(globalvar.fix == 1)   //��������
    {
        if(fuelrecordinfo.volume + 10 >= (globalvar.fixnum))     //��ǰ0.1kg�ط�
        {
            fuelrecordinfo.volume = globalvar.fixnum;
            volume_tmp = (double)fuelrecordinfo.volume / 100.0;
            fuelrecordinfo.money = (uint32_t)(volume_tmp * realtime.price + 0.5);
            stopreason = 6;
            globalvar.KeyValue = KEY_STOP;
        }
    }

    if(globalvar.fix == 2)   //����
    {
        if(fuelrecordinfo.money + 5 >= (globalvar.fixmoney))     //��ǰ0.5Ԫ�ط�
        {
            fuelrecordinfo.money = (globalvar.fixmoney);
            money_tmp = (double)(globalvar.fixmoney);
            price_tmp = (double)(realtime.price);
            fuelrecordinfo.volume = (uint32_t)(money_tmp * 100 / price_tmp + 0.5);
            stopreason = 7;
            globalvar.KeyValue = KEY_STOP;
        }
    }

    if((m1card.existflag == 0) && (m1card.cardtype_another != 0))       //�п�����ʱ����;�ο�
    {
        globalvar.KeyValue = KEY_STOP;
        stopreason = 13;
    }
}

/************************************************************************
**	��������:	StartPrecooling(void)
**	��������:	����ǰ��Ԥ��
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-5-10
*************************************************************************/
void StartPrecooling(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
		//����ǰ����ʵʱ����һ��ycx
        memset(&realtime.lcng, 0, sizeof(RealTimeData));     
    }

    ReadModbusAndPressure();//��ȡ�����ƺ�ѹ����������ֵ ADD BY LY

	//ģ���Һ���ݴ��� 
    if(sysparas.simulation)
    {
        realtime.cur_flowrate_l += 10;    //����
        realtime.cur_temperature_l -= 10; //Ԥ���¶�
        realtime.cur_gain_l += 1;        //Ԥ������

        if(realtime.cur_gain_l >= sysparas.precoolgain)
        {
            realtime.cur_gain_l--;
        }

        realtime.cur_density_l += 1;     //Ԥ���ܶ�

    }
	
	//ֻԤ�䡢��Ϊ��ѭ��Ԥ��
    if((DispNextUI == DispFirst) && (sysparas.precoolmode == 1))       
    {
    	//�򿪷��ţ������ѭ��
        PumpSwitch(StaBPrecool);
        globalvar.workstate = StaBPrecool;
    }
    else
    {
    	//�򿪷��ţ�����Сѭ������ҺǰԤ�䣬ΪСѭ��Ԥ�䣩ADD BY LY
        PumpSwitch(StaSPrecool);
        globalvar.workstate = StaSPrecool;
    }

	//�嵱ǰҺ������
    if(realtime.cur_totalize_l > 20000)
    {
        ModbusClearTotal_L(_MODBUS_L_);
        memset(&realtime.lcng, 0, sizeof(RealTimeData));     //�����ۺ�������¶�ȡ������
    }
	
	//�嵱ǰ��������
    if(realtime.cur_totalize_g > 20000)
    {
        ModbusClearTotal_G(_MODBUS_G_);
        memset(&realtime.lcng, 0, sizeof(RealTimeData));     //�����ۺ�������¶�ȡ������
    }

    FYD12864DispPrintfFlash(1, 1, "����Ԥ��%d", globalvar.timeout);
    FYD12864DispPrintfFlash(2, 1, "����:%.02f", realtime.cur_flowrate_l);
    FYD12864DispPrintfFlash(3, 1, "�¶�:%.02f", realtime.cur_temperature_l);
    FYD12864DispPrintfFlash(4, 1, "����:%.02f", realtime.cur_gain_l);

    OsplcShow(0, 0, 0, 0, 0);

    //Ԥ�䳬ʱ�ж� ÿ��1s���ۼ�1�� ADD BY LY
    if(globalvar.timeout > sysparas.precooltimeout)   //Ԥ�䳬ʱ
    {
        if(globalvar.otherstate == 1)   //�Է�δ��Һ
        {
            PumpSwitch(StaIdle);
        }
        else
        {
            PumpSwitch(StaOtherFuel);
        }

        DispUI = DispFirst;
    }
    else//�ﵽԤ��Ҫ��
    {
        if(((realtime.cur_temperature_l < ((float)(0 - (int)sysparas.precooltemp)))             //Ԥ���¶�
                && (realtime.cur_density_l > ((float)(sysparas.precooldensty) / 10000.0))         //Ԥ���ܶ�
                && (realtime.cur_gain_l < ((float)(sysparas.precoolgain))))          //Ԥ������
                || (globalvar.precoolfinished == 1))    //֮ǰ�Ѿ����Ԥ�䣬���ڴ���ֱ�Ӱ���Һ��
        {
            globalvar.precoolfinished = 1; //Ԥ����ɽ���
			//�п����ҿ���֤ͨ������Һ
            if((m1card.existflag == 1) && (m1card.checkresult == 0) && (DispNextUI != DispFirst))
            {
            	//��VIP����Ҫ�û�
                if(m1card.cardtype_another != 7)   
                {
                	//��״̬����Ϊ�����루�ñ�־��ʱû�ã�
                    fuelrecordinfo.cardstate = 1;
					//�û�
                    DispUI = GreyLock;
                }
				//VIP ��
                else if(m1card.cardtype_another == 7)
                {
                    fuelrecordinfo.cardstate = 1;
					//ֱ�Ӽ�Һ
                    DispUI = StartFueling;
                }
            }
			//�޿���Ԥ�䡢��Һ��
            else if(DispNextUI != NULL)
            {
                DispUI = DispNextUI;
				//һ��˫�������ж�
                if(globalvar.otherstate == 1)   
                {
                	//�Է�����������Сѭ��
                    PumpSwitch(StaSPrecool);
                }
                else
                {
                	//�Է���Һ���رշ���
                    PumpSwitch(StaOtherFuel);
                }
            }
            else
            {
                DispUI = DispFirst;
            }
        }
    }
	
	//Ԥ���Ҳͣ����ʱ��
    globalvar.stoppumptime = sysparas.stoppumptime + 2; 
    
	//�������쳣��ѹ���������쳣(globalvar.stoptimes = 1000)
    if(globalvar.stoptimes > 100)   
    {
        globalvar.KeyValue = KEY_STOP;
    }
	//��ͣ�������ص�������
    TaskAdd(KEY_STOP, DispFirst, NULL);
}

/************************************************************************
**	��������:	StartFueling(void)
**	��������:	��ʼ����,Ԥ����ɺ�ʼ��Һǰ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-5-10
*************************************************************************/
void StartFueling(void)
{
    uint32_t  availmoney;     //IC�����ü������
    
	//����ˮ�ṹ��
    memset(&fuelrecordinfo, 0, sizeof(FuelRecordInfo));   
	
	//���ã��������״̬
    PumpSwitch(StaFuel);	
	
	//��������1
    globalvar.fuelstep = 1;         
	//����״̬
    globalvar.workstate = StaFuel;  
	
    globalvar.timeout = 0;
    globalvar.stoptimes = 0;
    stopreason = 0;
    globalvar.displayonce = 1;

    //������ʼʱ��
    memcpy(&fuelrecordinfo.starttime[0], &time.year, 6); 

    //�п�������������ˮ���뿨��صĲ�����ֵ ADD BY LY
    if(m1card.existflag == 1)
    {
        //����
        fuelrecordinfo.usernum = m1card.cardnum_another;

        //������
        fuelrecordinfo.cardtype = m1card.cardtype_another;

        //������
        fuelrecordinfo.price = sysparas.price[m1card.pricelevel_another];//���տ������ͣ�������Ӧ���� ADD BY LY

		//���ƺ�
		fuelrecordinfo.carnum_another = m1card.carnum_another;
		
        //��VIP����������п������������ ADD BY LY
        if(m1card.cardtype_another != 7)
        {
            fuelrecordinfo.greyflag = 0x01;  //������ˮ�еĻұ��
            //������ǰ���
            fuelrecordinfo.beforebalance = m1card.cardbalance_another;
            //�����������
            fuelrecordinfo.afterbalance = m1card.cardbalance_another;
            //���ڳ����
            fuelrecordinfo.InitialMoney = m1card.cardinitmoney;
            //���������
            fuelrecordinfo.CalMoney = m1card.cardcalmoney;
            //���ۼƳ�ֵ���
            fuelrecordinfo.allrechargemoney = m1card.sumchargemoney;
            //���ۼƳ�ֵ����
            fuelrecordinfo.allrechargetimes = m1card.sumchargetimes;
            //���ۻ���������
            fuelrecordinfo.allconsumptionnum = m1card.sumvolme;
            //���ۻ����ѽ��
            fuelrecordinfo.allconsumptionmoney = m1card.summoney;
            //���ۼ����Ѵ���
            fuelrecordinfo.allconsumptiontimes = m1card.sumtimes;


            //�����
            if(m1card.cardtype_another != 3)   //�Ǽ��˿� ADD BY LY
            {
                //���������е�Ǯ�����ǰ�������ȡ����ģ�ֻ������ʾ��ʱ�򣬱��Ԫ����ʾ���ͻ����� ADD BY LY
                availmoney = m1card.cardbalance_another - sysparas.cardminyue;//����IC�����ü������ ADD BY LY
            }
            else if(m1card.cardtype_another == 3)   //���˿�
            {
                availmoney = sysparas.accountcardmaxyue - m1card.cardbalance_another;
            }
			
			//���Ƕ�������Ҳ���Ƕ�����������������ֱ�ӽ���������Ϊ�������������Ϊ���ý�� ADD BY LY
            if(globalvar.fix == 0)            
            {
                //������Ƕ����������򰴶������������û�е�����������ް���ֹͣ����ֹͣ������ﵽ����������ޣ���δ��ֹͣ������������ﵽ��������Զ�ͣ�� ADD BY LY
                globalvar.fix = 2;
                globalvar.fixmoney = availmoney;
            }		
        }
    }
	//�޿�����
    else
    {
        //Ĭ�Ͻ��׼۸�
        fuelrecordinfo.price = sysparas.price[0];
    }

    //ʵʱ����
    realtime.price = fuelrecordinfo.price;		//��ǰ���׼۸� ADD BY LY
    //���
    fuelrecordinfo.classnum = sysparas.shiftnum;
    //����ˮ��
    fuelrecordinfo.classid = sysparas.shiftransnum;
    //վ���
    fuelrecordinfo.stationcode = sysparas.stationcode;
    //����ǰѹ��
    fuelrecordinfo.beforepressure = realtime.cur_pressure;
    //Һ������������
    globalvar.starttotalizeL = realtime.cur_totalize_l;
    //����ǰǹ��
    fuelrecordinfo.beforegunsum = sysparas.totvolume;
    //������ǹ��
    fuelrecordinfo.aftergunsum = sysparas.totvolume;
    //Һ����������һ������
    globalvar.pretotalL = realtime.cur_totalize_l;

    //��������
    if(sysparas.backgascalc == 1)
    {
        globalvar.starttotalizeG = realtime.cur_totalize_g;
        globalvar.pretotalG = realtime.cur_totalize_g;
    }

    //�豸����
    fuelrecordinfo.devicetype = sysparas.devicetype;

    DispUI = DispStartFueling;	//��ʼ���� ADD BY LY
    realtime.powerdown = 0;     //δ�����־                                            
}



//��Һ��ʾ
double	 lngvolume = 0; //��Һ��
double   cngvolume = 0; //������
void DispStartFueling(void)
{
	//��������2
    globalvar.fuelstep = 2; 
	//����״̬
    globalvar.workstate = StaFuel;    

	//ģ�����
    if(sysparas.simulation)
    {
        realtime.cur_totalize_l += 0.11;
        realtime.cur_flowrate_l++;
        realtime.cur_pressure += 1;

        if(realtime.cur_pressure > 150)
        {
            realtime.cur_pressure = 150;
        }

        if(realtime.cur_flowrate_l > 400)
        {
            realtime.cur_flowrate_l = 400;
        }
    }
	//��������
	//�����У����϶�ȡ��������ѹ����������ֵ ADD BY LY
    ReadModbusAndPressure();
    
	//Һ�������쳣 fabs()�󸡵����ľ���ֵ BY LY
    if(fabs(realtime.cur_totalize_l - globalvar.pretotalL) > 2)     
    {
        globalvar.KeyValue = KEY_STOP;
        stopreason = 3;
    }
	//Һ����������
    else
    {
        //��Һ������
        lngvolume = realtime.cur_totalize_l - globalvar.starttotalizeL;
        globalvar.pretotalL = realtime.cur_totalize_l; //���˴�ʵʱ���۸�ֵ�������´��ж�˲ʱ����
    }
	
	//��������
    if(sysparas.backgascalc == 1)   
    {
        if(globalvar.starttotalizeG < 0.1)   //��ֹ��һ��û�ж������������Ƶ���ʼ��
        {
            globalvar.starttotalizeG = realtime.cur_totalize_g;//��������ֵ��ʼ�� ADD BY LY
        }
        else
        {
        	//���������쳣
            if(fabs(realtime.cur_totalize_g - globalvar.pretotalG) > 1)     
            {
                globalvar.KeyValue = KEY_STOP;
                stopreason = 3;
            }
			//������������
            else
            {
                //����������
                cngvolume = realtime.cur_totalize_g - globalvar.starttotalizeG;
                globalvar.pretotalG = realtime.cur_totalize_g; //���˴�ʵʱ���۸�ֵ�������´��ж�˲ʱ����

            }
        }
    }
	//������������ֱ������Ϊ 0 ADD BY LY
    else
    {
        cngvolume = 0;
    }
	
	//���귽����
    if(sysparas.unit == 2)   
    {
        lngvolume = lngvolume / ((double)sysparas.densty / 10000.0);
        cngvolume = cngvolume / ((double)sysparas.densty / 10000.0);
    }

    //��ǰ������������
    lngvolume = (lngvolume - cngvolume) * ((double)sysparas.coeff / 10000.0);
    //��ǰ������
    fuelrecordinfo.backgas = (uint32_t)(cngvolume * 100.0);
    //��ǰ��������
    fuelrecordinfo.volume = (uint32_t)(lngvolume * 100.0);
    
    lngvolume = (double)fuelrecordinfo.volume / 100.0;
    //��ǰ���׽�����
    fuelrecordinfo.money = (uint32_t)(lngvolume * realtime.price + 0.5);
	
	//�ж�ͣ������
    JudgeStopConditions();
	
    realtime.volume = fuelrecordinfo.volume;
    realtime.money = fuelrecordinfo.money;

    //ÿ1s�ӣ����̨����һ�μ���ʵʱ��ˮ ADD BY LY
    if(timer1.tick_100ms % 10 == 0)
    {
    	//�������ϴ�ʵʱ��ˮ
        UpLoad_RunState(2);
    }

    FYD12864DispPrintfFlash(1, 1, "״̬: ���ڼ�Һ");
    FYD12864DispPrintfFlash(2, 1, "����:%ld.%02d", fuelrecordinfo.volume / 100, fuelrecordinfo.volume % 100);
    FYD12864DispPrintfFlash(3, 1, "���:%ld.%02d", fuelrecordinfo.money / 100, fuelrecordinfo.money % 100);
    FYD12864DispPrintfFlash(4, 1, "V:%.01f T:%.01f ", realtime.cur_flowrate_l, realtime.cur_temperature_l);

	//��ֹͣ����ֹͣ����
    TaskAdd(KEY_STOP, StopFueling, NULL);

	//δ�����־
    realtime.powerdown = 0;    
	
    //��̬��ʾ˲ʱ��������
    OsplcShow(fuelrecordinfo.volume, fuelrecordinfo.money, realtime.price, 0, 0);

    //�ж��Ƿ����
    if(PowerCheckIn == 1)
    {
    	//��20ms���ٴμ���Ƿ��ѵ���
        _delay_ms(20);
        if(PowerCheckIn == 1)
        {
        	//�ѵ��磨�ñ�־��ʱδʹ�ã�
            realtime.powerdown = 1;   
			
			//�������ʵʱ����realtime
            SaveRealtime();
			
			//�رá�����������ʾ
            PowerDown();
        }
    }
}

/************************************************************************
**	��������:	StartFueling(void)
**	��������:	ֹͣ����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-5-10
*************************************************************************/
void StopFueling(void)
{
	//��������3
    globalvar.fuelstep = 3; 		
	//����״̬
    globalvar.workstate = StaPay;
	
    FYD12864DispPrintfFlash(1, 1, "״̬����Һ����");

    if(globalvar.displayonce == 1)
    {
		//�ط�ǰ�������ж�
		//�Է�����
        if(globalvar.otherstate == 1)   
        {	
        	//����Сѭ��
            PumpSwitch(StaSPrecool);
        }
		//�Է���Һ
        else
        {
        	//�ر���ѹ����ѹ����
            PumpSwitch(StaOtherFuel);
        }

        globalvar.displayonce = 0;
		
		//��Һ������������������
        lngvolume = 0;                                  
		//��Һ��ͣ����ʱ
        globalvar.stoppumptime = sysparas.stoppumptime + 2; 
		
        //������ˮ��
        sysparas.transnum++;
        fuelrecordinfo.transnum = sysparas.transnum;
        //ǹ������
        sysparas.totvolume += fuelrecordinfo.volume;
        //ǹ�ܽ��
        sysparas.totmoney += fuelrecordinfo.money;
        //���ۼ�����(ͨ����ˮ�ۼӼ���)
        sysparas.shiftotvol += fuelrecordinfo.volume;
        //���ۼƽ�� (ͨ����ˮ�ۼӼ���)
        sysparas.shiftotmon += fuelrecordinfo.money;
        //���������
        sysparas.shiftfueltimes++;
		
        //����ϵͳ����
        StopFuelingSaveParas();

		//��VIP������
        if((fuelrecordinfo.cardtype != 0) && (fuelrecordinfo.cardtype != 7))
        {
            //���˿����������	�������
            if(fuelrecordinfo.cardtype == 3)
            {
                fuelrecordinfo.afterbalance = fuelrecordinfo.afterbalance + fuelrecordinfo.money;
                fuelrecordinfo.CalMoney = fuelrecordinfo.CalMoney + fuelrecordinfo.money;
            }
			//��VIP�������˿�����Ŀ� �������
            else
            {
                fuelrecordinfo.afterbalance = fuelrecordinfo.afterbalance - fuelrecordinfo.money;
                fuelrecordinfo.CalMoney = fuelrecordinfo.CalMoney - fuelrecordinfo.money;
            }

            //���ۼ����Ѵ���
            fuelrecordinfo.allconsumptiontimes = m1card.sumtimes + 1;
            //���ۼ����ѽ��
            fuelrecordinfo.allconsumptionmoney += fuelrecordinfo.money;
            //���ۻ���������
            fuelrecordinfo.allconsumptionnum += fuelrecordinfo.volume;
        }

        //��������ʱ��
        memcpy(&fuelrecordinfo.endtime[0], &time.year, 6);
        //������ǹ��
        fuelrecordinfo.aftergunsum = fuelrecordinfo.beforegunsum + fuelrecordinfo.volume;
        //ǹ�����ۼ�
        fuelrecordinfo.allgunnum = sysparas.totvolume;
        //ǹ����ۼ�
        fuelrecordinfo.allgunmoney = sysparas.totmoney;
        //������ѹ��
        fuelrecordinfo.afterpressure = realtime.cur_pressure;
        //����
        fuelrecordinfo.curnum = sysparas.coeff;
        //������λ
        fuelrecordinfo.measuretype = sysparas.unit;//0-�귽��1-Kg��
        //�ܶ�
        fuelrecordinfo.density = (uint32_t)realtime.cur_density_l;
        //����ˮ��
        fuelrecordinfo.classid = sysparas.shiftransnum;
        //ͣ��ԭ��
        realtime.stopreason = stopreason;
        fuelrecordinfo.stopreason = realtime.stopreason;
        //������ʽ
        fuelrecordinfo.fuelmode = sysparas.fuelmode;
        //�����������������ʾ
        OsplcShow(fuelrecordinfo.volume, fuelrecordinfo.money, realtime.price, 0, 1);
        //�������10�οۿ�
        m1card.debittimes = 10;
    }

	 //�ÿ�����������δ����
    if((m1card.existflag == 1) && (PowerCheckIn == 0))      
    {
    	//�з�VIP���ҿ�ͨ����֤
        if((m1card.checkresult == 0x00) && (m1card.cardtype_another != 7))       
        {
            FYD12864DispPrintfFlash(1, 1, "״̬�����ڿۿ�..");
			//�ۿ�
            if(IcCardPayMoney())
            {
                FYD12864DispPrintfFlash(1, 1, "״̬���ۿ�ɹ�");
				//���
                GreyUnLock();
            }
            else
            {
                FYD12864DispPrintfFlash(1, 1, "״̬���ۿ�ʧ��");
            }
        }

        globalvar.displayonce = 1;
        DispUI = FuelEndDisplay;
    }
	//�޿�����������δ����
    else if(PowerCheckIn == 0)   
    {
        globalvar.displayonce = 1;
        DispUI = FuelEndDisplay;
    }

    realtime.powerdown = 0;        //δ�����־��

    if(PowerCheckIn == 1)          //�ж��Ƿ���硣
    {
        _delay_ms(20);

        if(PowerCheckIn == 1)
        {
            realtime.powerdown = 1; //�ѵ��硣
            SaveRealtime();         //�������ʵʱ���� realtime
            PowerDown();
        }

    }
}

/************************************************************************
**	��������:	FuelEndDisplay(void)
**	��������:	��Һ��Ϻ�洢��ˮ�ʹ�ӡ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-5-10
*************************************************************************/
void FuelEndDisplay(void)
{	
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;

        //crc���� �ŵ���ˮ������ǰ ���������ٱ��޸� ADD BY LY
        fuelrecordinfo.crc = ModbusCrc16((uint8_t*)(&fuelrecordinfo.transnum), (sizeof(FuelRecordInfo) - 2));           //changed by LY
		//���������ˮ
        if(SaveFuelRecord(0))
        {
            FYD12864DispPrintfFlash(1, 1, "״̬: ����ɹ�");
        }
        else
        {
            FYD12864DispPrintfFlash(1, 1, "״̬: ����ʧ��");
        }

		//�����������������������ͼ�������״̬����̨ ADD BY LY
        UpLoad_RunState(0);   

		//�ж��Ƿ��Զ���ӡһ��
        if(sysparas.printmode == 1)  
        {
        	Printid();
        }

		//����״̬
        globalvar.workstate = StaIdle;

		//��ֹ��ε��ú���TaskMessage����������
        globalvar.MsgFlag = 0;
        globalvar.MsgTime = 0; 
    }

    FYD12864DispPrintfFlash(4, 1, "ͣ��: %s", stopreasonbuf[stopreason]);

	
    TaskAdd(KEY_RET, DispFirst, NULL);//�����ؼ����ص�������
    TaskAdd(KEY_OK, StopPrint, NULL); //��ȷ�ϼ�����ӡ��ǰ��ˮ
    TaskMessage(50, DispFirst); 	  //û���κβ�����3s�󷵻�������  
}

/************************************************************************
**	��������:	DispCardError(void)
**	��������:	IC��A�ܻ�B����֤������ʾ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCX
**	��������:	2016-5-10
*************************************************************************/
void DispCardError(void)
{
    FYD12864DispPrintfFlash(2, 1, "��������");

    if(mt318reader.recbuf[6] == 0x33)
    {
        if(mt318reader.recbuf[4] == KEYA)
            FYD12864DispPrintfFlash(3, 1, "A ����֤ʧ��");
        else if(mt318reader.recbuf[4] == KEYB)
            FYD12864DispPrintfFlash(3, 1, "B ����֤ʧ��");
    }

    if(m1card.existflag == 0)   //�ο�ֱ�ӷ���������
    {
        DispUI = DispFirst;
    }

    else
        TaskMessage(100, DispFirst);   // 5s�󷵻�������
}

/************************************************************************
**	��������:	GetCardData(void)
**	��������:	�ȶ�IC����Կ����ȡ����������Ϣ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void GetCardData(void)
{
    uint8_t tmp = 0;                            //������������
    uint8_t keyb[6];
    uint8_t ntry = 2;

    FYD12864ClearLine(1);
    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "    ������");
    FYD12864DispPrintfFlash(3, 1, "    ���Ժ�...");

	//�ÿ����ڱ�־
    m1card.existflag = 1;
	
	//��ʼ������֤�����־ 0xFF��ʾ�ȴ���֤
    m1card.checkresult = 0xFF;  
	
	//���κΰ���������Ч
    globalvar.KeyValue = 0;                      

    //��EEPROM��ȥ������B�ܣ�����keyb[]��	ADD BY LY
    EepromInterReadBytes(LOCKVER_ADDR, keyb, 6);
    memcpy(&m1card.keyB[0], keyb, 6);  //��B�����¸�ֵ

    /**** ���� 1��2��4��5��6��9��10��13��14��16��17��18�����ݶ�������m1card�ṹ���� ADD BY LY *****/

    //��1���������޸ĳ����ڶ��������£��ٶ�һ�Σ�ADD BY LY
    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 1, &m1card.backUp0[0]))     //����1��
        {
            tmp++;
        }
        else
        {
            tmp = 0;
            break;
        }
    }

	//tmpΪ�� 0����ʾ����ʧ�� ADD BY LY
    if(tmp > 0)
    {
        if(m1card.checkresult == 0xFF)
            m1card.checkresult = 0x01;

        DispUI = CardDataError;
        return;
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, &m1card.card[0]))     //����2��
        {
            tmp++;
            m1card.checkresult = 0x02;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 4, &m1card.balancecrc[0]))     //����4��
        {
            tmp++;
            m1card.checkresult = 0x03;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 5, &m1card.lastBalanceCrc[0]))     //����5��
        {
            tmp++;
            m1card.checkresult = 0x04;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 6, (uint8_t *)&m1card.payMoneyCrc[0]))       //����6��
        {
            tmp++;
            m1card.checkresult = 0x05;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 9, (uint8_t *)&m1card.block9[0]))       //����9��
        {
            tmp++;
            m1card.checkresult = 0x06;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 10, (uint8_t *)&m1card.block10[0]))       //����10��
        {
            tmp++;
            m1card.checkresult = 0x07;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 13, &m1card.cylindersdate[0]))     //����13��
        {
            tmp++;
            m1card.checkresult = 0x08;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 14, &m1card.backUp5[0]))     //����14��
        {
            tmp++;
            m1card.checkresult = 0x09;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 16, &m1card.block16[0]))     //����16��
        {
            tmp++;
            m1card.checkresult = 0x0A;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 17, &m1card.block17[0]))     //����17��
        {
            tmp++;
            m1card.checkresult = 0x0B;
            DispUI = CardDataError;
            return;
        }
    }

    if(tmp == 0)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 18, &m1card.block18[0]))     //����18��
        {
            tmp++;
            m1card.checkresult = 0x0C;
            DispUI = CardDataError;
            return;
        }
    }

    /**************************************************************************************************/
    if(tmp == 0)                                               //�����ɹ���
    {
        DispUI = IcCardPasswd;
    }
}

/************************************************************************
**	��������:	IcCardPasswd(void)
**	��������:	�����ɹ����жϿ������Ƿ�Ϊ0����Ϊ0����Ҫ��������
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void IcCardPasswd(void)
{
    uint8_t tmpbuffer[6], count = 0;
    uint32_t tmp1, tmp2;

    //���ƺţ���M1�����룺0000
    for(count = 0; count < 2; count++)
    {
    	//4���ֽڵ�ASCII ת���� 2���ֽڵ�BCD
        tmpbuffer[count] = AscToBcd(&m1card.carnum[count * 2]);   
    }
	//2���ֽڵ�BCD ת���� һ��32λ��HEX
    m1card.carnum_another = BcdbufToHex(&tmpbuffer[0], 2);   

    m1card.key = m1card.carnum_another;//���ƺż�������
    
	//����Ҫ�����ƿ����
    if(sysparas.cardcheck == 0)                  
    {
    	//������Ϊ0������������
        if(m1card.key == 0)                     
        {
        	//����Ϣ��֤
            DispUI = CardMessageCheck;
        }
		//�����벻Ϊ0������������
        else                                     
        {
        	InputInit();
            DispUI = InputIccardPasswd;
        }
    }
	 //��Ҫ�����ƿ����
    else if(sysparas.cardcheck == 1)            
    {
    	//ASCII ת ʮ���� ADD BY LY
        tmpbuffer[0] = (m1card.cylindersdate[0] - 0x30) * 10 + (m1card.cylindersdate[1] - 0x30);     
        tmpbuffer[1] = (m1card.cylindersdate[2] - 0x30) * 10 + (m1card.cylindersdate[3] - 0x30);
        tmpbuffer[2] = (m1card.cylindersdate[4] - 0x30) * 10 + (m1card.cylindersdate[5] - 0x30);

		//��֤���ڵķ�����ת���������Ƚ�
        tmp1 = tmpbuffer[0] * 365 + tmpbuffer[1] * 30 + tmpbuffer[2];
        tmp2 = time.year * 365 + time.month * 30 + time.day;

		//��ƿδ�������� ADD BY LY
        if(tmp1 > tmp2)		
        {
            if(m1card.key == 0)                 //������Ϊ0��ֱ��������
            {
                DispUI = CardMessageCheck;
            }
            else                                 //�����벻Ϊ0�����������롣
            {
                DispUI = InputIccardPasswd;
            }
        }
        else
        {
            DispUI = DispDateOver;//�����ûң���ʾ��ƿ���� ADD BY LY
        }
    }
}

/************************************************************************
**	��������:	DispDateOver(void)
**	��������:	��ƿ������ʾ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void DispDateOver(void)
{
    uint8_t tmpbuffer[3];

    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "��ƿ�Ѿ�����");

	//ASCII ת ʮ���� ADD BY LY
    tmpbuffer[0] = (m1card.cylindersdate[0] - 0x30) * 10 + (m1card.cylindersdate[1] - 0x30);     
    tmpbuffer[1] = (m1card.cylindersdate[2] - 0x30) * 10 + (m1card.cylindersdate[3] - 0x30);
    tmpbuffer[2] = (m1card.cylindersdate[4] - 0x30) * 10 + (m1card.cylindersdate[5] - 0x30);

	//��ʾ��ƿ�������� ֻȡ�����գ���ȡ����λ��
    FYD12864DispPrintfFlash(3, 1, "%02d-%02d-%02d", tmpbuffer[0], tmpbuffer[1], tmpbuffer[2]);   

	//�����ؼ�����������
    TaskAdd(KEY_RET, DispFirst, NULL);

	//�������кţ�ʵ���Ǽ�⿨�Ƿ񻹴��� ADD BY LY
    if(Mt318GetCardSerialNum() != true)
    {
        DispUI = DispFirst;
    }
}

/************************************************************************
**	��������:	InputIccardPasswd(void)
**	��������:	����IC������ҳ����ʾ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-7-25
*************************************************************************/
void InputIccardPasswd(void)
{
    DispInputPwd("�����뿨����", ComparePasswd);

    if(m1card.existflag == 0)
    {
        DispUI = DispFirst;
    }
}

/************************************************************************
**	��������:	ComparePasswd(void)
**	��������:	IC�������ж�
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	YCK
**	��������:	2016-7-25
*************************************************************************/
void ComparePasswd(void)
{
    if(globalvar.U32Temp != m1card.key)
    {
        FYD12864DispPrintfFlash(4, 1, "�������");
        TaskAdd(KEY_CLR, InputIccardPasswd, NULL);
        TaskAdd(KEY_RET, InputIccardPasswd, NULL);
    }
    else
    {
        DispUI = CardMessageCheck;
    }
}

/************************************************************************
**	��������:	DispCardInfo(void)
**	��������:	�����ɹ�����ʾ����Ϣ���жϿ���;
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void DispCardInfo(void)
{
    globalvar.timeout = 0;
	//��ʾ����
    FYD12864DispPrintfFlash(1, 1, "����: %ld", m1card.cardnum_another);

	//��ʾ������
    switch(m1card.cardtype_another)
    {
        case 0x01:
            FYD12864DispPrintfFlash(2, 1, "����: �û���");
            break;

        case 0x02:
            FYD12864DispPrintfFlash(2, 1, "����: Ա����");
            break;

        case 0x03:
            FYD12864DispPrintfFlash(2, 1, "����: ���˿�");
            break;

        case 0x04:
            FYD12864DispPrintfFlash(2, 1, "����: ά����");
            break;

        case 0x05:
            FYD12864DispPrintfFlash(2, 1, "����: �ڲ���");
            break;

        case 0x06:
            FYD12864DispPrintfFlash(2, 1, "����: ���ֿ�");
            break;

        case 0x07:
            FYD12864DispPrintfFlash(2, 1, "����: VIP ��");
            break;
    }
	//��ʾ�����
    FYD12864DispPrintfFlash(3, 1, "���: %ld.%02d", m1card.cardbalance_another / 100, m1card.cardbalance_another % 100);
	//��ʾ����Ӧ����
	FYD12864DispPrintfFlash(4, 1, "����: %.02f", (double)m1card.currentprice / 100);

	//�ֶ�����
    if(sysparas.fuelmode == 0)	    
    {
        globalvar.displayonce = 1;

		//�˴�����TaskAdd����ȻNextUi = NULL��������StartPrecooling�У���⵽�п��������ͽ���GreyLock
        TaskAdd(KEY_START, StartPrecooling, NULL);    //������������ʼԤ�䣬Ȼ��ʼ����
        TaskAdd(KEY_BACK, StartPrecooling, DispFirst);//�����Լ�������Ԥ�䣬Ȼ�󷵻�������
    }
	
    InputInit();
    TaskAdd(KEY_FIX, FixGas, NULL); 		 //������������������
    TaskAdd(KEY_QRY, DispQuery1, NULL);		 //����ѯ���������ѯ����	
    TaskAdd(KEY_SET, DispInputMenuPwd, NULL);//���˵������������ù���

    if(m1card.existflag == 0)   //�����γ�
    {
        DispUI = DispFirst;
    }
}

/************************************************************************
**	��������:	CardMessageCheck(void)
**	��������:	Check������Ϣ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void CardMessageCheck(void)
{
    uint8_t count;
    uint8_t tmpbuffer[6];
    uint32_t tmp = 0;
    UnionU32 trans;

    //����
    for(count = 0; count < 4; count++)
    {
    	// 4�ֽ� ASCII to 2�ֽ� BCD
        tmpbuffer[count] = AscToBcd(&m1card.cardnum[count * 2]);   
    }
	//2�ֽ� BCD to 1��32λ HEX
    m1card.cardnum_another = BcdbufToHex(tmpbuffer, 4);   

    //������ˮ
    for(count = 0; count < 4; count++)
    {
        //M1���е���ˮ�ţ��͵�ַ����Ǹ�λ������CPU��͵�ַ�Ǵ��ڵ�λ(С��)�ģ��ʸ�ֵʱ����Ҫ�ߵ�˳�� ADD BY LY
        trans.rdcv[count] = m1card.transnum[4 - count - 1]; //HEX
    }

    m1card.transnum_another = trans.data;//HEX
    //������ ��2�еĿ�����
    tmp = AscToBcd(&m1card.cardtype[0]);   //2�ֽ�ASCIIת����1�ֽ�BCD
    m1card.cardtype_another = (uint8_t)BcdToHex(tmp);     //HEX

    //�����۵ȼ�
    //tmp = AscToBcd(&m1card.pricelevel[0]);//BCD /* ��̨δд���۵ȼ� ycx.modify 2018-09-11 11:15:26 */
    m1card.pricelevel_another = m1card.cardtype_another;//HEX

    //�����
    for(count = 0; count < 4; count++)
    {
        tmpbuffer[count] = AscToBcd(&m1card.cardbalance[count * 2]);   //BCD
    }

    m1card.cardbalance_another = BcdbufToHex(tmpbuffer, 4);   //HEX

    //���������
    for(count = 0; count < 4; count++)
    {
        tmpbuffer[count] = AscToBcd(&m1card.payMoney[count * 2]);   //BCD
    }

    m1card.trademoney = BcdbufToHex(tmpbuffer, 4);   //HEX
    //�ۻ����Ѵ���
    m1card.sumtimes = BcdbufToHex(&m1card.block16[0], 4);
    //�ۼ����ѽ��
    m1card.summoney = BcdbufToHex(&m1card.block16[4], 6);   //HEX
    //�ۻ���������
    m1card.sumvolme = BcdbufToHex(&m1card.block16[12], 4);
    //�������
    memset(&tmpbuffer[0], 0, sizeof(tmpbuffer));     //M1���п��ڳ����ΪBCD�� ADD BY LY
    memcpy(&tmpbuffer[1], &m1card.block17[0], 5);   //BCD ��Ϊ���ڳ����ֻ��5λ�����Դ�tmpbuffer[1]��ʼ���� ADD BY LY
    m1card.cardinitmoney = BcdbufToHex(&tmpbuffer[0], 6);   //HEX
    //�������
    memset(&tmpbuffer[0], 0, sizeof(tmpbuffer));
    memcpy(&tmpbuffer[1], &m1card.block17[11], 5);   //BCD ��Ϊ�������ֻ��5λ�����Դ�tmpbuffer[1]��ʼ���� ADD BY LY
    m1card.cardcalmoney = BcdbufToHex(&tmpbuffer[0], 6);   //HEX
    //��ֵ�������
    m1card.sumchargemoney = BcdbufToHex(&m1card.block18[0], 6);   //HEX M1�����ۻ���ֵ���ΪBCD�� ADD BY LY
    //�ۻ���ֵ����
    m1card.sumchargetimes = BcdbufToHex(&m1card.block18[6], 4);

    //У��1
    if(m1card.cardbalance_another != m1card.cardcalmoney)
    {
        if(m1card.greyFlag[1] != 0x32)   //����ǻҿ�����������Ԥ���ۿ�ʧ�ܣ��޷������ۿ� ADD BY LY
        {
            m1card.checkresult = 0x0F;//��������
            DispUI = CardDataError;
            return;
        }
    }

    //У��2
    if(m1card.cardtype_another == 3)   //���˿���
    {
        if(m1card.cardbalance_another >= sysparas.accountcardmaxyue)   //�ж�����Ƿ�������ޡ�
        {
            m1card.checkresult = 0x10;//����
            DispUI = CardDataError;
            return;
        }
    }
    else if(m1card.cardtype_another != 7)   //�����
    {
        if(m1card.cardbalance_another <= sysparas.cardminyue)   //�ж�����Ƿ�����������
        {
            m1card.checkresult = 0x11;//����
            DispUI = CardDataError;
            return;
        }
    }

    //Ч��3 �������У��
    if(m1card.cardtype_another == 3)   //���˿� ADD BY LY
    {
        if(m1card.cardcalmoney != (m1card.cardinitmoney + m1card.sumchargemoney + m1card.summoney))     //���˿� ����Ǽ�
        {
            if(m1card.greyFlag[1] != 0x32)   //����ǻҿ�����������Ԥ���ۿ�ʧ�ܣ��޷������ۿ� ADD BY LY
            {
                m1card.checkresult = 0x12;//����������
                DispUI = CardDataError;
                return;
            }
        }
    }
    else if(m1card.cardtype_another != 7)   //����VIP��
    {
        if((m1card.cardcalmoney + m1card.summoney) != (m1card.sumchargemoney + m1card.cardinitmoney))
        {
            if(m1card.greyFlag[1] != 0x32)
            {
                m1card.checkresult = 0x13;//���������������������
                DispUI = CardDataError;
                return;
            }
        }
    }

    //������
    m1card.currentprice = sysparas.price[m1card.pricelevel_another];

    if(m1card.currentprice == 0)
    {
        m1card.checkresult = 0x14;
        DispUI = CardDataError;
        return;
    }

    //��ѯ��ʧ������IC����ʧ��־ ADD BY LY
    if(ReadLossCard(m1card.cardnum_another) == false)
    {
        globalvar.displayonce = 1;

        if(globalvar.displayonce == 1)
        {
            if(m1card.lossFlag[1] != 0x32)
            {
                if(!WriteLossCardFlag())	//�ù�ʧ����� ADD BY LY
                {
                    carderror = 19;
                    DispUI = IcCardOperaError;
                    return;
                }
                else
                {
//					FYD12864DispPrintfFlash(1,1,"�ù�ʧ����־�ɹ�");
//					FYD12864DispPrintfFlash(2,1,"���¶�����...");
                    DispUI = GetCardData;
                    return;
                }
            }

            globalvar.displayonce = 0;
        }
    }
	
	//0x32 Ϊ"2"��ASCII�� 2��ʾ�ҿ� ADD BY LY
    if(m1card.greyFlag[1] == 0x32)   
    {
        m1card.checkresult = 0x0D;
        DispUI = CardDataError;
        return;
    }
	
	//0x32 Ϊ"2"��ASCII�� 2��ʾ��ʧ����ֻ���ڿ���ȥ��̨д���Ժ󣬸ñ�־�Ż�ı䣩ADD BY LY
    else if(m1card.lossFlag[1] == 0x32)   
    {
        m1card.checkresult = 0x0E;
        DispUI = CardDataError;
        return;
    }
    else
    {
    	//�ñ�����ʱδʹ�� ADD BY LY
        m1card.checkflag = 1;
		
        //M1����֤ͨ��
        m1card.checkresult = 0;
        globalvar.displayonce = 0;
    }

    if(m1card.existflag == 0)   //�����γ�
    {
        DispUI = DispFirst;
    }
    else
    {
        DispUI = DispCardInfo;
    }
}

/************************************************************************
**	��������:	CardDataError(void)
**	��������:	IC����֤ʧ�ܣ���Ϣ��ʾ
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void CardDataError(void)
{
    if(globalvar.displayonce == 1)
    {
        FYD12864ClearLine(1);
        FYD12864ClearLine(4);
        globalvar.displayonce = 0;
    }

    switch(m1card.checkresult)
    {
        case 0xFF:
            FYD12864DispPrintfFlash(2, 1, "���ڵȴ���֤");
            FYD12864DispPrintfFlash(3, 1, "���Ժ�...");
            break;

        case 0x00:
            DispUI = DispCardInfo;
            globalvar.displayonce = 1;
            break;

        case 0x01:
            FYD12864DispPrintfFlash(2, 1, "����01ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x02:
            FYD12864DispPrintfFlash(2, 1, "����02ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x03:
            FYD12864DispPrintfFlash(2, 1, "����04ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x04:
            FYD12864DispPrintfFlash(2, 1, "����05ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x05:
            FYD12864DispPrintfFlash(2, 1, "����06ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x06:
            FYD12864DispPrintfFlash(2, 1, "����09ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x07:
            FYD12864DispPrintfFlash(2, 1, "����10ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x08:
            FYD12864DispPrintfFlash(2, 1, "����13ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x09:
            FYD12864DispPrintfFlash(2, 1, "����14ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x0A:
            FYD12864DispPrintfFlash(2, 1, "����16ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x0B:
            FYD12864DispPrintfFlash(2, 1, "����17ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x0C:
            FYD12864DispPrintfFlash(2, 1, "����18ʧ��");
            FYD12864ClearLine(3);
            break;

        case 0x0D:
            FYD12864DispPrintfFlash(2, 1, "�ҿ�");
            FYD12864DispPrintfFlash(3, 1, "�����...");
            DispUI = GreyDataDeal;
            break;

        case 0x0E:
            FYD12864DispPrintfFlash(2, 1, "��ʧ��");
            FYD12864DispPrintfFlash(3, 1, "���˿�...");
            break;

        case 0x0F:
            FYD12864DispPrintfFlash(2, 1, "��������");
            FYD12864ClearLine(3);
            break;

        case 0x10:
            FYD12864DispPrintfFlash(2, 1, "���: %ld.%02d", m1card.cardbalance_another / 100, m1card.cardbalance_another % 100);
            FYD12864DispPrintfFlash(3, 1, "���ޣ��븶��");
            break;

        case 0x11:
            FYD12864DispPrintfFlash(2, 1, "���: %ld.%02d", m1card.cardbalance_another / 100, m1card.cardbalance_another % 100);
            FYD12864DispPrintfFlash(3, 1, "���㣬���ֵ");
            break;

        case 0x12:
            FYD12864DispPrintfFlash(2, 1, "���˼���������");
            FYD12864ClearLine(3);
            break;

        case 0x13:
            FYD12864DispPrintfFlash(2, 1, "�������������");
            FYD12864ClearLine(3);
            break;

        case 0x14:
            FYD12864DispPrintfFlash(2, 1, "����Ϊ0");
            FYD12864ClearLine(3);
            break;

        case 0x16:
            FYD12864DispPrintfFlash(2, 1, "B �ܴ���");
            FYD12864ClearLine(3);
            break;

    }

    if(m1card.existflag == 0)
        DispUI = DispFirst; //�����γ�
}

/************************************************************************
**	��������:	Card_WriteGunandTran(uint8_t gunnumber, uint32_t transum, uint8_t stationid)
**	��������:	����IC���е�ǹ�š���ˮ�š�վ���
**	�������: gunnumber: ��д��IC���е�ǹ��
			  transum��	 ��д��IC���еĽ�����ˮ��
			  stationid����д��IC���е�վ����
**	����ֵ  	: flag��1��д��ɹ���0��д��ʧ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
uint8_t Card_WriteGunandTran(uint8_t gunnumber, uint32_t transum, uint8_t stationid)
{
    uint8_t 	buf[16];//����������� ADD BY LY
    uint8_t   bufbak[16];//��д����ٴζ��������� ADD BY LY
    uint8_t 	ntry = 2; //����д��ʱ����д�Ĵ��� ADD BY LY
    uint8_t	flag = false;
    //���¿�14����
    /*********************************************************/
    memcpy(buf, &m1card.backUp5[0], 16);
    //��14��ǰ[0-5]�ֽ�=����ʼ����Ϣ;
    //[6]�ֽ� =ǹ�ţ�
    //[7-10]�ֽ�=���һ����ˮ�ţ�ADD BY LY
    //[11]�ֽ� = ����������վ���룻
    //[12-15]=���� ADD BY LY

    //ǹ�� ADD BY LY
    buf[6] = gunnumber;
    //buf[7-10] ���һ�ν�����ˮ��
    buf[7] = (uint8_t)(transum >> 24);
    buf[8] = (uint8_t)(transum >> 16);
    buf[9] = (uint8_t)(transum >> 8);
    buf[10] = (uint8_t)transum;
    //���һ�μ���������վ���� LY
    buf[11] = stationid;

    //д��14�� ADD BY LY
    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 14, buf))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //д��14ʧ��
    if(flag == false)
    {
        carderror = 20;//ADD BY LAST
        DispUI = IcCardOperaError;
        return flag;
    }

    //����14
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 14, bufbak))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //����14ʧ��
    if(flag == false)
    {
        carderror = 21;//ADD BY LAST
        DispUI = IcCardOperaError;
        return flag;
    }

    //У��д�������
    if(memcmp(buf, bufbak, 16) != 0)
    {
        //дǹ�š���ˮ�š�����վ���ʧ��
        flag = false;
        carderror = 22;
        DispUI = IcCardOperaError;
        return flag;
    }

    //дǹ�š���ˮ�š�����վ��ųɹ�
    DispUI = StartFueling;
    return flag;
}

/************************************************************************
**	��������:	GreyLock(void)
**	��������:	IC���ûң�ͬʱ���öԿ���������Ϊ������
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void GreyLock(void)
{
    uint8_t buf[16];
    uint8_t bufbak[16];
    uint8_t step = 0;
    uint8_t ntry = 2;

    FYD12864ClearLine(1);
    FYD12864DispPrintfFlash(2, 1, "�����û�...");
    FYD12864DispPrintfFlash(3, 1, "���Ժ�");
    FYD12864ClearLine(4);

    m1card.greyFlag[0] = 0x30;//�ҿ���־
    m1card.greyFlag[1] = 0x32;

    //д��ұ��
    while((ntry--) && (step == 0))
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 2, &m1card.card[0]))     //�û� ADD BY LY
        {
            continue;
        }
        else
        {
            step = 1;
            break;
        }
    }

    if(step != 1)
    {
        carderror = 23;//ADD BY LAST
        DispUI = IcCardOperaError;
        return;
    }

    //�����ո�д���2������
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, bufbak))
        {
            continue;
        }
        else
        {
            step = 2;
            break;
        }
    }

    if(step != 2)
    {
        carderror = 24;//ADD BY LAST
        DispUI = IcCardOperaError;
        return;
    }

    //��������Ƿ�д����ȷ
    memcpy(buf,  &m1card.card[0], 16);

    if(memcmp(buf, bufbak, 16) != 0)
    {
        //�û�ʧ��
        carderror = 25;//ADD BY LAST
        DispUI = IcCardOperaError;
        return;
    }
    else
    {
        //�ûҳɹ�
        //дǹ�š�������ˮ�š�վ���
        if(Mt318ReadM1Card(KEYB, &m1card.keyB[0], 14, buf))     //�û� ADD BY LY
        {
            m1card.gunnum = sysparas.gunnum;;	//	ǹ��
            m1card.transnum_another = sysparas.transnum + 1;//������ˮ��
            m1card.laststationcode = (uint8_t)sysparas.stationcode;   //վ����

            if(Card_WriteGunandTran(m1card.gunnum, m1card.transnum_another, m1card.laststationcode))
            {
                //���¶Կ��Ĳ��� ADD BY LY
                m1card.operateType[0] = 0x30;
                m1card.operateType[1] = 0x31;

                if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 6, &m1card.payMoneyCrc[0]))
                {
                    carderror = 26;//ADD BY LAST
                    DispUI = IcCardOperaError;
                    return;
                }

                DispUI = StartFueling;
                return;
            }
            else
            {
                carderror = 27;//ADD BY LAST
                DispUI = IcCardOperaError;
                return;
            }

        }
        else
        {
            carderror = 28;
            DispUI = IcCardOperaError;
            return;
        }

    }
}

/************************************************************************
**	��������:	GreyUnLock(void)
**	��������:	IC�����
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void GreyUnLock(void)
{
    uint8_t step = 0;
    uint8_t buf[16];
    uint8_t bufbak[16];
    uint8_t ntry = 2;

    fuelrecordinfo.greyflag = 0x00; //������ˮ�еĻұ�����
    
    //���ý�ұ��
    m1card.greyFlag[0] = 0x30;
    m1card.greyFlag[1] = 0x31;

    //д��ұ��
    while((ntry--) && (step == 0))
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 2, &m1card.card[0]))     //д��2 ADD BY LY
        {
            continue;
        }
        else
        {
            step = 1;
            break;
        }
    }

    if(step != 1)
    {
        carderror = 29;
        DispUI = IcCardOperaError;
        return ;
    }

    //�����ո�д������� ADD BY LY
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, bufbak))     //����2
        {
            continue;
        }
        else
        {
            step = 2;
            break;
        }
    }

    if(step != 2)
    {
        carderror = 30;
        DispUI = IcCardOperaError;
        return ;
    }

    //У���Ƿ�д����ȷ
    memcpy(buf,  &m1card.card[0], 16);

    if(memcmp(buf, bufbak, 16) != 0)
    {
        //���ʧ��
        carderror = 31;
        DispUI = IcCardOperaError;
        return ;

    }
    else
    {
        //��ҳɹ�
        DispUI = DispFirst;//�������Ժ��޸�ΪDispFirst
    }

    return;
}

/************************************************************************
**	��������:	GreyDataDeal(void)
**	��������:	IC�������ݴ���
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-5-10
*************************************************************************/
void GreyDataDeal(void)
{
    uint8_t 		buf[16];
    uint8_t 		bsecond = 0;
    uint8_t 		bthird = 0;
    uint8_t 		bforth = 0;
    uint8_t 		bfifth = 0;
    uint8_t 		bsixth = 0;
    uint8_t 		bfirst = false;

    //վ����ȶ� ADD BY LY
    if(m1card.laststationcode  == (uint8_t)(sysparas.stationcode))
    {
        //ǹ�űȶ� ADD BY LY
        if(m1card.gunnum == (uint8_t)sysparas.gunnum)
        {
            //ǹ�űȶ�ͨ�� ADD BY LY
            bfirst = true;
        }
        else
        {
            //ǹ�űȶ�ʧ�� ADD BY LY
            carderror = 32;
            DispUI = IcCardOperaError;
            return;
        }
    }
    else
    {
        //վ����ȶ�ʧ�� ADD BY LY
        carderror = 33;
        DispUI = IcCardOperaError;
        return;
    }

    //�Աȿ��� ADD BY LY
    if(bfirst)
    {
        //���ݿ�����ˮ�Ŷ�ȡ��ˮ����fuelrecordinfo�� ADD BY LY
        if(ReadFuelRecord(m1card.transnum_another - 1, 0))
        {
            //�ȶ�IC���� ADD BY LY
            if(fuelrecordinfo.usernum == m1card.cardnum_another)
            {
                bsecond = true;
            }
            else
            {
                carderror = 34;
                DispUI = IcCardOperaError;
                return;
            }
        }
        else
        {
            //��ˮ��ȡʧ�� ADD BY LY
            carderror = 35;
            DispUI = IcCardOperaError;
            return;
        }
    }

    //���¿�45
    if(bsecond)
    {
        //����������ǰ������ ��ʾδ�ۿ����IC�� ADD BY LY
        if(m1card.cardbalance_another == fuelrecordinfo.beforebalance)
        {
            if(Card_UpdateSec45())
            {
                bthird = true;
            }
            else
            {
                carderror = 36;
                DispUI = IcCardOperaError;
                return;
            }
        }		
        else if(m1card.cardbalance_another == fuelrecordinfo.afterbalance)
        {
            bthird = true;
        }
		else
		{	
            carderror = 41;
            DispUI = IcCardOperaError;
            return;		
		}		
    }

    //���¿�6
    if(bthird)
    {
        if(Card_UpdateSec6(6))
        {
            bforth = true;
        }
        else
        {
            carderror = 37;
            DispUI = IcCardOperaError;
            return;
        }
    }

    //���¿�16��17
    if(bforth)
    {
        //���¿�16
        memset(buf, 0, sizeof(buf));
        HexToBcdbuf(fuelrecordinfo.allconsumptiontimes, &buf[0], 4);
        HexToBcdbuf(fuelrecordinfo.allconsumptionmoney, &buf[4], 6);
        HexToBcdbuf(fuelrecordinfo.allconsumptionnum, &buf[12], 4);

        if(Mt318WriteM1Card(KEYB, &m1card.keyB[0], 16, buf))
        {
            bfifth = true;
        }
        else
        {
            carderror = 38;
            DispUI = IcCardOperaError;
            return;
        }

        //���¿�17
        memset(buf, 0, sizeof(buf));
        HexToBcdbuf(fuelrecordinfo.InitialMoney, &buf[0], 5);
        memcpy(&buf[5], &m1card.block17[5], 6);   //�ڳ�ʱ�䲻��
        HexToBcdbuf(fuelrecordinfo.CalMoney, &buf[11], 5);

        if(Mt318WriteM1Card(KEYB, &m1card.keyB[0], 17, buf))
        {
            bsixth = true;
        }
        else
        {
            carderror = 39;
            DispUI = IcCardOperaError;
            return;
        }

        //IC ȫ��������ɣ����
        if((bfifth == true) && (bsixth == true))
        {
            GreyUnLock();
            DispUI = DispFirst;
        }
        else
        {
            carderror = 40;
            DispUI = IcCardOperaError;
            return;
        }
		
    }
	
	if(m1card.existflag == 0)   //�����γ�
	{
		DispUI = DispFirst;
	}
}

/************************************************************************
**	��������:	IcCardOperaError(void)
**	��������:	��ʾ��IC�������йصĴ���
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	LY
**	��������:	2018-9-17
*************************************************************************/
void IcCardOperaError(void)
{
    switch(carderror)
    {
        case 1:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "д��5ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 2:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "����5ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 3:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "�����ϴ�����");
            FYD12864DispPrintfFlash(3, 1, "��ʹ��ʱ��ʧ��");
            FYD12864ClearLine(4);
            break;

        case 4:

            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "д��4ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 5:

            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "����4ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 6:
	        FYD12864ClearLine(1);		
            FYD12864DispPrintfFlash(2, 1, "���¿������");
            FYD12864DispPrintfFlash(3, 1, "ʹ��ʱ��ʧ��");
  
            FYD12864ClearLine(4);
            break;

        case 7:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "д��6ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 8:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "����6ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 9:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "�������������");
            FYD12864DispPrintfFlash(3, 1, "ֵ������ʧ��");
            FYD12864ClearLine(4);
            break;

        case 10:

            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "д��16ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 11:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "����16ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 12:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "�����ۻ����Ѵ���");
            FYD12864DispPrintfFlash(3, 1, "������ʧ��");
            FYD12864ClearLine(4);
            break;

        case 13:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "д��17ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 14:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "����17ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;

        case 15:
            FYD12864ClearLine(1);
            FYD12864DispPrintfFlash(2, 1, "���¼������ʧ��");		
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);

        case 19:
            FYD12864DispPrintfFlash(1, 1, "��ʧ��");
            FYD12864DispPrintfFlash(2, 1, "�ù�ʧ����־ʧ��");
		    FYD12864ClearLine(3);
            FYD12864DispPrintfFlash(4, 1, "���˿�...");
            break;
			
        case 25:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "�ûҿ�ʧ��");
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;
        case 31:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "���ʧ��");
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;
        case 32:
            FYD12864ClearLine(1);			
            FYD12864DispPrintfFlash(2, 1, "�뵽%02d��ǹ���", m1card.gunnum);
            FYD12864ClearLine(3);
            FYD12864ClearLine(4);
            break;
        case 33:
            FYD12864DispPrintfFlash(1, 1, "�뵽վ����Ϊ:");
            FYD12864DispPrintfFlash(2, 1, "%ld", m1card.laststationcode);
            FYD12864DispPrintfFlash(3, 1, "�ļ���վ���");
            FYD12864ClearLine(4);
            break;
		case 34:	
			//��������IC��������ˮ�ѱ��棬�ӿ�����ϵͳFLASH��������÷Ǳ��ſ����޿�����������ˮ�ţ��ٲ���Ŀ���� ADD BY LY
			//����취����̨���
			FYD12864ClearLine(1);
			FYD12864DispPrintfFlash(2, 1, "���Ų�һ��");		
			FYD12864ClearLine(3);
			FYD12864ClearLine(4);
			break;
		case 35:
			//��������IC���ûҳɹ����Ѹ�д��ˮ�ţ��˿���ϵͳ���Ҳ�����Ӧ��ˮ��¼ ADD BY LY
			//����취������
			FYD12864ClearLine(1);			
			FYD12864DispPrintfFlash(2, 1, "����ˮ����");
			FYD12864ClearLine(3);
			FYD12864ClearLine(4);
			break;			
        default:
            FYD12864DispPrintfFlash(1, 1, "������ʧ��");
            FYD12864DispPrintfFlash(2, 1, "�������: %d", carderror);
            FYD12864ClearLine(3);
            FYD12864DispPrintfFlash(4, 1, "���˿�...");
            break;
    }

    //�˿��������׽��� ADD BY LY
    if(m1card.existflag == 0)
    {
        DispUI = DispFirst;
    }

}

/***************************************************************************
**	�������ƣ�Card_UpdateSec45(void)
**	�������ܣ�����IC����4����5������
**	�����������
**	�� �� ֵ��true:	���¿�4��5�ɹ�
			  false:���¿�4��5ʧ��

**	�� �� �ߣ�LY
**	�������ڣ�2018-9-17
***************************************************************************/
uint8_t Card_UpdateSec45(void)
{
    uint8_t 	rdbuf5[16];
    uint8_t 	rdbuf4[16];
    uint8_t 	rdbufbak[16];
    uint8_t 	n = 3;
    uint8_t 	tmp;
    uint8_t 	ntry = 2;
    uint8_t		flag = false; //true: ��ʾ��֤ͨ��

    //���¿�5����
    /*****************************************************************/
    //���������ʹ��ʱ�丳ֵ��rdbuf5����14�ֽ�
    memcpy(&rdbuf5[2], &m1card.cardbalance[0], 14);

    //���¼��㿨�ϴ�������У�����ASCII����ֵ ADD BY LY
    tmp = 0;

    for(n = 0; n < 4; n++)
    {
        tmp ^= AscToBcd(&rdbuf5[2 + n * 2]);
    }

    rdbuf5[0] = tmp / 0x10 + 0x30;
    rdbuf5[1] = tmp % 0x10 + 0x30;

    /******************************************************************/
    //д��5
    while(ntry--)
    {
    	//дʧ�ܣ���дһ�Σ����д2�� ADD BY LY
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 5, rdbuf5))
        {
            continue;
        }
        else
        {
        	//д��5�ɹ����˳�ѭ�� ADD BY LY
            flag = true;
            break;
        }
    }

    //д��5ʧ�ܣ���ʾ�������
    if(flag == false)
    {
        carderror = 1;
        DispUI = IcCardOperaError;
        return flag;
    }

    //д��5�ɹ���У������
    flag = false;
	//����5
    while(ntry--)
    {
    	//����5ʧ�ܣ��ٶ�һ�Σ�����2��
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 5, rdbufbak))     
        {
            continue;
        }
        else
        {
        	//����5�ɹ����˳�ѭ�� ADD BY LY
            flag = true;
            break;
        }
    }

    //����5ʧ��
    if(flag == false)
    {
        carderror = 2;
        DispUI = IcCardOperaError;
        return flag;
    }

    //����5�ɹ����ȶ�д�������������
    if(memcmp(rdbuf5, rdbufbak, 16) != 0)   
    {
        //�ȶ�ʧ��
        flag = false;
        carderror = 3;
        DispUI = IcCardOperaError;
        return flag;
    }

    /*************************************************************/
    //���¿�4����
    flag = false;//���־
    
	//����4�����ݴ���rdbuf4[]��
    memcpy(&rdbuf4[0], &m1card.balancecrc[0], 16);
	
    //��������ת����ASCII ����rdbuf[2]���8���ֽ� ADD BY LY
    sprintf_P((char*)&rdbuf4[2], "%08ld", fuelrecordinfo.afterbalance);

    //�����ʹ��ʱ�����
    if(Ds3232ReadTime())   //��ȡ��ǰʱ��
    {
        //��
        rdbuf4[10] = time.year / 0x10 + 0x30; //Ds3232ReadTime()������ʮ�����ƣ���ת����ASCII ADD BY LY
        rdbuf4[11] = time.year % 0x10 + 0x30;
        //��
        rdbuf4[12] = time.month / 0x10 + 0x30;
        rdbuf4[13] = time.month % 0x10 + 0x30;
        //��
        rdbuf4[14] = time.day / 0x10 + 0x30;
        rdbuf4[15] = time.day % 0x10 + 0x30;
    }

    //���¼��㿨������У�����ASCII����ֵ ADD BY LY
    tmp = 0;

    for(n = 0; n < 4; n++)
    {
        tmp ^= AscToBcd(&rdbuf4[2 + n * 2]);
    }

    rdbuf4[0] = tmp / 0x10 + 0x30; //�����У���� 2�ֽ� ADD BY LY
    rdbuf4[1] = tmp % 0x10 + 0x30;
    /*************************************************************/
	
    //���д������ʼ��
    ntry = 2;
	
	//д��4
    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 4, rdbuf4))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //д��4ʧ��
    if(flag == false)
    {
        carderror = 4;
        DispUI = IcCardOperaError;
        return flag;
    }

    //����4
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 4, rdbufbak))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //����4ʧ��
    if(flag == false)
    {
        carderror = 5;
        DispUI = IcCardOperaError;
        return flag;
    }

    //�ȶ�д�������������
    if(memcmp(rdbuf4, rdbufbak, 16) != 0)
    {
        //�ȶ�ʧ��
        flag = false;
        carderror = 6;
        DispUI = IcCardOperaError;
        return flag;
    }

    return flag;
}

/***************************************************************************
**	�������ƣ�Card_UpdateSec6(void)
**	�������ܣ�����IC����6������
**	�����������
**	�� �� ֵ��true:	���¿�6�ɹ�
			  false:���¿�6ʧ��

**	�� �� �ߣ�LY
**	�������ڣ�2018-9-17
***************************************************************************/
uint8_t Card_UpdateSec6(uint8_t optype)
{
    uint8_t tmp, n;
    uint8_t flag = false;
    uint8_t rdbuf6[16];
    uint8_t rdbufbak[16];
    uint8_t ntry = 2;

    //���¿�6����
    /************************************************************************/
    //����6�е����ݸ�ֵ��rdbuf6��
    memcpy(rdbuf6, &m1card.payMoneyCrc[0], 16);
    //�������������ֵ���ת����ASCII ADD BY LY
    sprintf_P((char*)&rdbuf6[2], "%08ld", fuelrecordinfo.money);
    //���ò�������
    rdbuf6[10] = '0'; //�������͸��£�01������

    if((optype > 0) && (optype < 9))
    {
        tmp = 0x30 + optype;
    }

    rdbuf6[11] = tmp;
    //���¼������������������У�����ASCII����ֵ ADD BY LY
    tmp = 0;

    for(n = 0; n < 4; n++)
    {
        tmp ^= AscToBcd(&rdbuf6[2 + n * 2]);   //���������ֵ��� ��ASCIIת����BCD �ټ���У��ֵ����tmp�� ADD BY LY
    }

    rdbuf6[0] = tmp / 0x10 + 0x30; //У��ֵ��ֵ ADD BY LY
    rdbuf6[1] = tmp % 0x10 + 0x30;
    /************************************************************************/
    //д��6
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 6, rdbuf6))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //д��6ʧ��
    if(flag == false)
    {
        carderror = 7;
        DispUI = IcCardOperaError;
        return flag;
    }

    //����6
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 6, rdbufbak))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //����6ʧ��
    if(flag == false)
    {
        carderror = 8;
        DispUI = IcCardOperaError;
        return flag;
    }

    //д������У��
    if(memcmp(rdbuf6, rdbufbak, 16) != 0)     //�ȶ�
    {
        carderror = 9;
        DispUI = IcCardOperaError;
        return flag;
    }

    return flag;
}

/***************************************************************************
**	�������ƣ�Card_UpdateSec1_67(void)
**	�������ܣ�����IC����16��17������
**	�����������
**	�� �� ֵ��true:	���¿�6�ɹ�
			  false:���¿�6ʧ��

**	�� �� �ߣ�LY
**	�������ڣ�2018-9-17
***************************************************************************/
uint8_t Card_UpdateSec1_67(void)
{
    uint8_t 	rdbuf16[16];
    uint8_t 	rdbuf17[16];
    uint8_t 	tmpbuf[6];
    uint8_t 	rdbufbak[16];
    uint8_t 	ntry = 2;
    uint8_t  	flag = false;
	
	//����16�����ݸ�ֵ��rdbuf16[]��
    memcpy(rdbuf16, &m1card.block16, 16);
	
    //���ۼ����Ѵ��� ����
    HexToBcdbuf(fuelrecordinfo.allconsumptiontimes, &rdbuf16[0], 4);
    //���ۻ����ѽ�� ����
    HexToBcdbuf(fuelrecordinfo.allconsumptionmoney / 10000, &rdbuf16[4], 4);
    HexToBcdbuf(fuelrecordinfo.allconsumptionmoney % 10000, &rdbuf16[8], 2);
    //�������������� ����
    HexToBcdbuf(fuelrecordinfo.allconsumptionnum, &rdbuf16[12], 4);

    //���¿��ۻ����Ѵ������ۻ����ѽ��ۻ��������� ADD BY LY
    flag = 0;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 16, rdbuf16))     //д��16
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //д��16ʧ��
    if(flag == false)
    {
        carderror = 10;
        DispUI = IcCardOperaError;
        return flag;
    }

    //����16
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 16, rdbufbak))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //����16ʧ��
    if(flag == false)
    {
        carderror = 11;
        DispUI = IcCardOperaError;
        return flag;
    }

    //д������У��
    if(memcmp(rdbuf16, rdbufbak, 16) != 0)     //�ȶ�
    {
        flag = false;
        carderror = 12;
        DispUI = IcCardOperaError;
        return flag;
    }

    //���¿�17
    memcpy(rdbuf17, &m1card.block17, 16);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    HexToBcdbuf(fuelrecordinfo.CalMoney / 10000, &tmpbuf[0], 4);
    HexToBcdbuf(fuelrecordinfo.CalMoney % 10000, &tmpbuf[4], 2);
    memcpy(&rdbuf17[11], &tmpbuf[1], 5);

    //д��17
    flag = false;
    ntry = 3;

    while(ntry--)
    {
        if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 17, rdbuf17))
        {
            continue;
        }
        else
        {
            flag = true;
            break;
        }
    }

    //д��17ʧ��
    if(flag == false)
    {
        carderror = 13;
        DispUI = IcCardOperaError;
        return flag;
    }

    //����17
    flag = false;
    ntry = 2;

    while(ntry--)
    {
        //����18 ȡ���ۻ���ֵ���ۻ���ֵ���� ADD BY LY
        if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 17, rdbufbak))
        {
            continue;
        }
        else
        {
            flag =  true;
            break;
        }
    }

    //����17ʧ��
    if(flag == false)
    {
        carderror = 14;
        DispUI = IcCardOperaError;
        return flag;
    }

    //д������У��
    if(memcmp(rdbuf17, rdbufbak, 16) != 0)     //�ȶ�
    {
        flag = false;
        carderror = 15;
        DispUI = IcCardOperaError;
        return flag;
    }

    return flag;
}

/***************************************************************************
**	�������ƣ�IcCardPayMoney(void)
**	�������ܣ��ÿ������������������ۿ�
**	�����������
**	�� �� ֵ��true:	�ۿ�ɹ�
			  false:�ۿ�ʧ��

**	�� �� �ߣ�LY
**	�������ڣ�2018-9-17
***************************************************************************/
uint8_t IcCardPayMoney(void)
{
    uint8_t ret = false;

    //	����������ǰ�����ȣ�˵��δ�ۿ� ADD BY LY
    if(m1card.cardbalance_another == fuelrecordinfo.beforebalance)
    {
    	//���¿�4����5
        ret = Card_UpdateSec45();
    }
	// �����������������ȣ�˵���ѿۿ�
    else if(m1card.cardbalance_another == fuelrecordinfo.afterbalance)
    {
        ret = true;
    }

    //���¿�45ʧ��
    if(ret == false)
    {
        carderror = 16;
        DispUI = IcCardOperaError;
        return ret;
    }

    //���¿�45�ɹ����ٸ��¿�6
    ret = false;

	//���¿�6
    if(Card_UpdateSec6(3))
    {
        ret = true;
    }

    //���¿�6ʧ��
    if(ret == false)
    {
        carderror = 17;
        DispUI = IcCardOperaError;
        return ret;
    }

    //���¿�6�ɹ����ٸ��¿�16��17
    ret = false;
	
	//���¿�16��17
    if(Card_UpdateSec1_67())
    {
        ret = true;
    }
    else
    {
        carderror = 18;
        DispUI = IcCardOperaError;
        return ret;
    }

    return ret;
}

/***************************************************************************
**	�������ƣ�PowerDown(void)
**	�������ܣ�������ʾ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCK
**	�������ڣ�2016-7-25
***************************************************************************/
void PowerDown(void)
{
	//ͣ��
    PumpSwitch(StaClose);     
	
    FYD12864DispPrintfFlash(1, 1, "ϵͳ�ѵ���...");
    FYD12864DispPrintfFlash(4, 1, "�����ѱ���");
    DispUI = PowerDown;

	//�����ؼ�������ϵͳ
    TaskAdd(KEY_RET, SoftReset, NULL);
}

/***************************************************************************
**	�������ƣ�SaveRealtime(void)
**	�������ܣ��������ʵʱ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�LY
**	�������ڣ�2018-9-17
***************************************************************************/
void SaveRealtime(void)
{
    //�豸����
    realtime.lcng = sysparas.devicetype;
    //��ǰ������ˮ�� �������ֵ ADD BY LY
    //����������״̬
    realtime.workstatus = globalvar.workstate;
    //����
    realtime.price = fuelrecordinfo.price;
    //������
    realtime.backgas = fuelrecordinfo.backgas;
    //��ǰ������
    realtime.volume = fuelrecordinfo.volume;
    //��ǰ�������
    realtime.money = fuelrecordinfo.money;
    //��ǰ���
    realtime.shiftnum = sysparas.shiftnum;
    //IC���� ���п����渳ֵ ADD BY LY
    //��ǰ���ƺ�
    realtime.carnum = m1card.carnum_another;
    //������ ���п����渳ֵ ADD BY LY
    //������ʼʱ��
    memcpy(&realtime.starttime[0], &fuelrecordinfo.starttime[0], 6);
    //��������ʱ��
    realtime.endtime[0] = 0x20;
    memcpy(&realtime.endtime[0], &time.year, 6);

    /********************���뿨�ж�����**********************************/
    //����ǰ���
    //���������
    //�ۼƳ�ֵ����
    //�ۼƳ�ֵ���
    //�ۻ����Ѵ���
    //�ۻ���������
    //�ۻ����ѽ��
    //�������
    /********************************************************************/
    //����ǰѹ��
    realtime.beforepressure = fuelrecordinfo.beforepressure;
    //������ѹ��
    realtime.afterpressure = realtime.cur_pressure;
    //����
    realtime.curnum = sysparas.coeff;
    //������λ
    realtime.measuretype = sysparas.unit;
    //ͣ��ԭ��
    realtime.stopreason = 5;
    //��������ˮ��
    realtime.jhid = 0;
    //����ˮ��
    realtime.classid = sysparas.shiftransnum;
    //����ģʽ
    realtime.fuelmode = sysparas.fuelmode;

    if(globalvar.fuelstep != 3)
    {
        //��StopFueling() ��ǰ���� ���ȼ��㣬�ٱ��� ADD BY LY
        globalvar.stoppumptime = sysparas.stoppumptime + 2; //��Һ��ͣ����ʱ
        //������ˮ��
        sysparas.transnum++;
        fuelrecordinfo.transnum = sysparas.transnum;
        //ǹ������
        sysparas.totvolume += fuelrecordinfo.volume;
        //ǹ�ܽ��
        sysparas.totmoney += fuelrecordinfo.money;
        //���ۼ�����(ͨ����ˮ�ۼӼ���)
        sysparas.shiftotvol += fuelrecordinfo.volume;
        //���ۼƽ�� (ͨ����ˮ�ۼӼ���)
        sysparas.shiftotmon += fuelrecordinfo.money;
        //���������
        sysparas.shiftfueltimes++;
		
        //��������
        StopFuelingSaveParas();

        //�뿨�йصļ��� ADD BY LY
        if((fuelrecordinfo.cardtype != 0) && (fuelrecordinfo.cardtype != 7))
        {
            //�����������	�������
            if(fuelrecordinfo.cardtype == 3)
            {
                fuelrecordinfo.afterbalance = fuelrecordinfo.afterbalance + fuelrecordinfo.money;
                fuelrecordinfo.CalMoney = fuelrecordinfo.CalMoney + fuelrecordinfo.money;
            }
            else
            {
                fuelrecordinfo.afterbalance = fuelrecordinfo.afterbalance - fuelrecordinfo.money;
                fuelrecordinfo.CalMoney = fuelrecordinfo.CalMoney - fuelrecordinfo.money;
            }

            //���ۼ����Ѵ���
            fuelrecordinfo.allconsumptiontimes = m1card.sumtimes + 1;
            //���ۼ����ѽ��
            fuelrecordinfo.allconsumptionmoney += fuelrecordinfo.money;
            //���ۻ���������
            fuelrecordinfo.allconsumptionnum += fuelrecordinfo.volume;
        }
    }

    //�뿨�й�ʵʱ���ݸ�ֵ
    if(fuelrecordinfo.cardtype != 0)
    {
        //��ǰ�û�����
        realtime.cardnum = fuelrecordinfo.usernum;
        //������
        realtime.cardtype = m1card.cardtype_another;
        //�ҿ����
        realtime.greyflag = fuelrecordinfo.greyflag;
        //���������
        realtime.afterbalance = fuelrecordinfo.afterbalance;
        //�������
        realtime.calmoney = fuelrecordinfo.CalMoney;
        //����ǰ���
        realtime.beforebalance = fuelrecordinfo.beforebalance;
        //�ۻ���ֵ����
        realtime.allrechargetimes = fuelrecordinfo.allrechargetimes;
        //�ۼƳ�ֵ���
        realtime.allrechargemoney = fuelrecordinfo.allrechargemoney;
        //ʵʱ�ۼ����Ѵ�����ֵ
        realtime.allconsumptiontimes = fuelrecordinfo.allconsumptiontimes;
        //ʵʱ�ۼ�����������ֵ
        realtime.allconsumptionnum = fuelrecordinfo.allconsumptionnum;
        //ʵʱ�ۼ����ѽ�ֵ
        realtime.allconsumptionmoney = fuelrecordinfo.allconsumptionmoney;
    }

    //������ˮ��
    realtime.transnum = sysparas.transnum;
    //����ʵʱ��ˮ
    EepromInterWriteBytes(REALTIMEDATA_ADDR, (uint8_t *)(&realtime.lcng), sizeof(RealTimeData));         //�洢����ʵʱ��ˮ��EEPROM ��
}

/***************************************************************************
**	�������ƣ�PowerDownRealDate(void)
**	�������ܣ��������ݻָ�
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�LY
**	�������ڣ�2018-9-17
***************************************************************************/
void PowerDownRealDate(void)
{
    uint8_t *tmp;

    fuelrecordinfo.beforegunsum = sysparas.totvolume - realtime.volume;	//����ǰǹ��
    fuelrecordinfo.aftergunsum = sysparas.totvolume;					//������ǹ��

    fuelrecordinfo.transnum = realtime.transnum;                        //������ˮ��
    fuelrecordinfo.classnum = sysparas.shiftnum;                        //���
	fuelrecordinfo.carnum_another = realtime.carnum;					//���ƺ�
    fuelrecordinfo.usernum = realtime.cardnum;							//�û�����
    fuelrecordinfo.cardtype = realtime.cardtype;                        //������
    memcpy(&fuelrecordinfo.starttime[0], &realtime.starttime[0], 6);    //������ʼʱ��
    memcpy(&fuelrecordinfo.endtime[0], &realtime.endtime[0], 6);        //��������ʱ��

    fuelrecordinfo.price = realtime.price;                              //����
    fuelrecordinfo.volume = realtime.volume;                            //����
    fuelrecordinfo.money = realtime.money;                              //���
    fuelrecordinfo.backgas = realtime.backgas;                          //������

    fuelrecordinfo.allrechargetimes = realtime.allrechargetimes;        //�ۼƳ�ֵ����
    fuelrecordinfo.allrechargemoney = realtime.allrechargemoney;        //�ۼƳ�ֵ���

    fuelrecordinfo.beforebalance = realtime.beforebalance;              //����ǰ���
    fuelrecordinfo.afterbalance = realtime.afterbalance;                //���������

    fuelrecordinfo.allconsumptiontimes = realtime.allconsumptiontimes;  //�ۼ����Ѵ���
    fuelrecordinfo.allconsumptionnum = realtime.allconsumptionnum;      //�ۼ���������
    fuelrecordinfo.allconsumptionmoney = realtime.allconsumptionmoney;  //�ۼ����ѽ��
    fuelrecordinfo.CalMoney = realtime.calmoney;						//�������

    fuelrecordinfo.allgunnum = sysparas.totvolume;                      //ǹ������
    fuelrecordinfo.allgunmoney = sysparas.totmoney;                     //ǹ�۽��
    fuelrecordinfo.beforepressure = realtime.beforepressure;            //����ǰѹ��
    fuelrecordinfo.afterpressure = realtime.afterpressure;              //������ѹ��
    fuelrecordinfo.density = (uint32_t)(realtime.cur_density_g * 10000 + 0.5);     //�ܶ�
    fuelrecordinfo.curnum = realtime.curnum;                            //����
    fuelrecordinfo.devicetype = realtime.lcng;                          //��Ʒ����
    fuelrecordinfo.measuretype = realtime.measuretype;                  //������λ
    fuelrecordinfo.stopreason = realtime.stopreason;                    //ͣ��ԭ��
    //fuelrecordinfo.jhid = realtime.jhid;                              //��������ˮ��
    fuelrecordinfo.classid = realtime.classid;                          //����ˮ��
    fuelrecordinfo.fuelmode = realtime.fuelmode;						//����ģʽ
    fuelrecordinfo.greyflag = realtime.greyflag;						//�ұ��
    tmp = (uint8_t *)(&fuelrecordinfo.transnum);
    fuelrecordinfo.crc = ModbusCrc16(tmp, sizeof(FuelRecordInfo) - 2);		//changed by LY

	//�������󣬻ָ�����ˮ
    if(SaveFuelRecord(0))
    {
        FYD12864DispPrintfFlash(4, 1, "�����ѻָ�");
		//��ӡ��ˮ
        Printid();
    }
    else
    {
        FYD12864DispPrintfFlash(4, 1, "���ݻָ�ʧ��");
    }
}

/***************************************************************************
**	�������ƣ�FixGas(void)
**	�������ܣ�����������ѡ��ҳ��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCK
**	�������ڣ�2016-7-25
***************************************************************************/
void FixGas(void)
{
    FYD12864ClearLine(1);
    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "  1.������");
    FYD12864DispPrintfFlash(3, 1, "  2.�����");

    TaskAdd(KEY_RET, DispFirst, NULL);	//�����ؼ�����������
    TaskAdd(KEY_1, FixNum, NULL);		//����1�������붨��������
    TaskAdd(KEY_2, FixMoney, NULL);		//����2�������붨������
	
	//�����γ�
    if((m1card.cardtype_another != 0) && (m1card.existflag == 0))       
    {
        DispUI = DispFirst;
    }

    globalvar.fixnum = 0;
    globalvar.fixmoney = 0;
    globalvar.displayonce = 1;
	
    InputInit();//����ṹ���ʼ��
}

/***************************************************************************
**	�������ƣ�FixNum(void)
**	�������ܣ�����������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCK
**	�������ڣ�2016-7-25
***************************************************************************/
void FixNum(void)
{
    char buf[16] = {0};
    uint32_t tmp_fixmoney = 0;
    uint32_t  availmoney;     //IC�����ü������

    ScreenInput.inputmaxlen = 5;//��������99999

    if(globalvar.KeyValue == KEY_CLR)
    {
    	//�������������
        FYD12864ClearLine(4);
    }

    NumKeyHandler();
    globalvar.fix = 1;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    globalvar.fixnum = atol(buf);
    FYD12864ClearLine(3);
    FYD12864DispPrintfFlash(2, 1, "������:%ld.%02d", globalvar.fixnum / 100, globalvar.fixnum % 100);

	TaskAdd(KEY_RET, DispFirst, NULL);//�������ء�����������

	//�����������������㶨�Ľ��
    tmp_fixmoney = (globalvar.fixnum * fuelrecordinfo.price) / 100 ;
	
	//���������������1L��1kg
	if((globalvar.fixnum < 100) && (globalvar.KeyValue == KEY_OK))
    {
        FYD12864DispPrintfFlash(4, 1, "����������С");
    }
	//��VIP ����������������
    else if((m1card.existflag == 1) && (m1card.checkresult == 0)&&(m1card.cardtype_another != 7))
    {
    	//�Ǽ��˿�������� ADD BY LY
        if(m1card.cardtype_another != 3)   
        {
            //���������е�Ǯ�����ǰ�������ȡ������ģ�ֻ������ʾ��ʱ�򣬱��Ԫ����ʾ���ͻ����� ADD BY LY
            availmoney = m1card.cardbalance_another - sysparas.cardminyue;//����IC�����ü������ ADD BY LY
        }
		//���˿��������
        else if(m1card.cardtype_another == 3)   
        {
            availmoney = sysparas.accountcardmaxyue - m1card.cardbalance_another;
        }
		
		//��������������������ޱ���
        if(tmp_fixmoney > availmoney)
        {
            FYD12864DispPrintfFlash(4, 1, "�������������޶�");
        }
		//�п��ֶ�������������ȷ�Ͽ�ʼԽ�����  ADD BY LY
        else if((sysparas.fuelmode == 0)&& (globalvar.KeyValue == KEY_OK))   
        {
            FYD12864ClearLine(4);
            TaskAdd(KEY_OK, StartPrecooling, NULL);
        }
    }
	//�޿��ֶ���������
    else
    {
        if((sysparas.fuelmode == 0) && (sysparas.usecard != 1))       
        {
        	//������󣬰���ȷ�ϡ�����ʼԤ�䡢����
            TaskAdd(KEY_OK, StartPrecooling, StartFueling);
        }
    }

}

/***************************************************************************
**	�������ƣ�FixMoney(void)
**	�������ܣ���������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCK
**	�������ڣ�2016-7-25
***************************************************************************/
void FixMoney(void)
{
    char buf[16] = {0};
    uint32_t  availmoney;     //IC�����ü������

    ScreenInput.inputmaxlen = 5;//��������99999

    //����������Сʱ��ɾ��ԭ�����룬�������룬ͬʱ���������ʾ����NumKeyHandler()��ɾ������ֵ���˴���
    //��Ҫ��NumKeyHandler()������ǰ�����д�����ʾ��������� ADD BY LY
    if(globalvar.KeyValue == KEY_CLR)
    {
    	//�����������ʾ
        FYD12864ClearLine(4);
    }

    NumKeyHandler();
    globalvar.fix = 2;
	//�������Ԫ������ʹ�õ��Ƿ֣�����Ҫ����100 ADD BY LY
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);      
    globalvar.fixmoney = atol(buf);
    FYD12864ClearLine(3);
    FYD12864DispPrintfFlash(2, 1, "�����:%ld.%02d", globalvar.fixmoney / 100, globalvar.fixmoney % 100);

	//�������ء����ص�������
	TaskAdd(KEY_RET, DispFirst, NULL);

	//������С����С��Ĭ�ϵ���
	if((globalvar.fixmoney < sysparas.price[0]) && (globalvar.KeyValue == KEY_OK))
    {
        FYD12864DispPrintfFlash(4, 1, "��������С");
    }
    else if((m1card.existflag == 1) && (m1card.checkresult == 0) && (m1card.cardtype_another != 7))
    {	
    	//�Ǽ��˿� ADD BY LY
        if(m1card.cardtype_another != 3)   
        {
            //���������е�Ǯ�����ǰ�������ȡ������ģ�ֻ������ʾ��ʱ�򣬱��Ԫ����ʾ���ͻ����� ADD BY LY
            availmoney = m1card.cardbalance_another - sysparas.cardminyue;//����IC�����ü������ ADD BY LY
        }
		//���˿�
        else if(m1card.cardtype_another == 3)   
        {
            availmoney = sysparas.accountcardmaxyue - m1card.cardbalance_another;
        }
		
		//��������������ޱ���
        if(globalvar.fixmoney > availmoney)
        {
            FYD12864DispPrintfFlash(4, 1, "�����������");
        }
		//�������δ���ޣ��ֶ�����ģʽ��������󣬰�ȷ������ʼԤ�����
        else if((sysparas.fuelmode == 0)&& (globalvar.KeyValue == KEY_OK)) 
        {
            FYD12864ClearLine(4);
            TaskAdd(KEY_OK, StartPrecooling, NULL);
        }
    }
	//�޿��ֶ�����
    else
    {
        if((sysparas.fuelmode == 0) && (sysparas.usecard != 1))       
        {
        	//������󣬰���ȷ�ϡ���ʼԤ�䡢����
            TaskAdd(KEY_OK, StartPrecooling, StartFueling);
        }
    }
}

