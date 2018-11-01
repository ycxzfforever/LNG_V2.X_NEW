#include "kernel.h"

fun         DispUI;         //����ˢ�´�����
fun         DispNextUI;     //������Ϣ��ʾ�����һ������

/***************************************************************************
**	�������ƣ�TaskAdd(uint32_t keyvalue, fun funname1, fun funname2)
**	�������ܣ��¼�����
**	���������keyvalue ������ֵ
         	  funname1 ����Ӧ������Ҫִ�е�����
         	  funname2 ������������Ӧ֮�󣬽�������Ҫִ�е����飬����Ϊ NULL
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2016-7-25
***************************************************************************/
void TaskAdd(uint32_t keyvalue, fun funname1, fun funname2)
{
	//����ֵ�����趨ֵ��ִ��funname1�����funname2��ΪNULL������ִ��funname2
    if(globalvar.KeyValue == keyvalue)
    {
        globalvar.KeyValue = 0;
        DispUI = funname1; //����ˢ�¸�ֵ BY LY

        if(funname2 != NULL)
            DispNextUI = funname2; //����ˢ�¸�ֵ BY LY
        else
            DispNextUI = NULL;
    }
}

/***************************************************************************
**	�������ƣ�TaskMessage(uint32_t ms_100, fun funname)
**	�������ܣ��ȴ�һ��ʱ�� ms_100��ִ��ĳ���¼� funname
**	���������ms_100 ����Ҫ�ȴ���ʱ�䣬������λ�� 100ms
         	  funname�� �ȴ���ô��ʱ���Ժ���Ҫִ�еĺ���
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2016-7-25
***************************************************************************/
void TaskMessage(uint32_t ms_100, fun funname)
{
    if((globalvar.MsgTime == 0) && (globalvar.MsgFlag == 0))       //��һ�����msg����
    {
        globalvar.MsgFlag = 1;
        globalvar.MsgTime = ms_100;
    }

    if((globalvar.MsgTime == 0) && (globalvar.MsgFlag == 1))       //ʱ�䵽�˹�����msg����
    {
        globalvar.MsgFlag = 0;
        DispUI = funname;
    }
}

/***************************************************************************
**	�������ƣ�SetSuccess(void)
**	�������ܣ����óɹ���ʾ
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2016-7-25
***************************************************************************/
void SetSuccess(void)
{
    FYD12864DispPrintfFlash(4, 1, "���óɹ�");
}

/***************************************************************************
**	�������ƣ�SetFault(void)
**	�������ܣ�����ʧ����ʾ
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2016-7-25
***************************************************************************/
void SetFault(void)
{
    FYD12864DispPrintfFlash(4, 1, "����ʧ��");
}

/***************************************************************************
**	�������ƣ�DispSet(uint8_t numpoint, uint32_t data, char __farflash * buf)
**	�������ܣ����ò���
**	���������numpoint��С��������<5,��ʾ�����С��������5��ʾ����ķ�С����6:���룻7������
		   	  data��	 �������ֵ
		      buf��	 "��Ҫ���õ�����"������flash��
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSet(uint8_t numpoint, uint32_t data, char __farflash * buf)
{
    char buff[16];
    char tmpbuf[7] = {0};//6���������"*",һ�����ַ���������
    char i;

    NumKeyHandler();
    globalvar.pointnum = 0;
	
	//�������ò���ԭ����ֵ
    globalvar.beforedex = data;

    if(numpoint < 5)   //��С��������(С��������λ��Ϊ0,2,4)
    {
		//��ʾ��������
		FYD12864DispPrintfFlash(1, 1, buf);

		//���������û��С������������С�������п��ַ����� ADD BY LY
        if(numpoint == 0)  
        {
            if(ScreenInput.array[ScreenInput.inputlen - 1] == 0x2E)
            {
                ScreenInput.array[ScreenInput.inputlen - 1] = 0x20;
                ScreenInput.inputlen--;
            }

            globalvar.pointnum = numpoint; //��Ҫ�������������ʱ��С�����Ƿ���ʾ���ж� BY LY
            //�������ò������ڵ�ֵ
            globalvar.afterdex = atol((char *)ScreenInput.array);
			//û��С�����֣�ֱ����ʾԭ����ֵ
            FYD12864DispPrintfFlash(2, 1, "��ֵ:%ld", data);
        }
        else if(numpoint == 2)
        {
            globalvar.pointnum = numpoint;
            sprintf_P(buff, "%f", atof((char *)ScreenInput.array) * 100);
            globalvar.afterdex = atol(buff);
			//��ʾԭ����ֵ��2λС���� �Ƚ�С������ʾ�������ٷֱ���ʾС����ǰ��С�������� BY LY
            FYD12864DispPrintfFlash(2, 1, "��ֵ:%ld.%02ld", data / 100, data % 100);   
        }
        else if(numpoint == 4)
        {
            globalvar.pointnum = numpoint;
			
            sprintf_P(buff, "%f", atof((char *)ScreenInput.array) * 10000);
            globalvar.afterdex = atol(buff);
			//��ʾԭ����ֵ��4λС���� �Ƚ�С������ʾ�������ٷֱ���ʾС����ǰ��С�������� BY LY
            FYD12864DispPrintfFlash(2, 1, "��ֵ:%ld.%04ld", data / 10000, data % 10000);
        }
		//��ʾ��ֵ
        FYD12864DispPrintfFlash(3, 1, "��ֵ:%s", ScreenInput.array);
        FYD12864ClearLine(4);
    }
	
	//ѡ������ ֻ����ѡ�û��С���㣬�ʶ�С��������Ч���� BY LY
    else  if(numpoint == 5)   
    {
        if(ScreenInput.array[ScreenInput.inputlen - 1] == 0x2E)
        {
            ScreenInput.array[ScreenInput.inputlen - 1] = 0x20;
            ScreenInput.inputlen--;
        }
		//�������ò������ڵ�ֵ
        globalvar.afterdex = atol((char *)ScreenInput.array);

		//��ʾ��ʾ��Ϣ
        strcpy_P(buff, buf);   //��far flash char*ת����char* ADD BY LY
        FYD12864DispPrintfFlash(1, 1, "%s %ld", buff, data);
		//��ʾ���ڵ�ֵ
        FYD12864DispPrintfFlash(4, 1, "��ֵ:%s", ScreenInput.array);
    }
	//�������� ���������С���㣬��ͨ����Ļ������������ʾ BY LY
    else  if(numpoint == 6)   
    {
    	//��ʾ��ʾ��Ϣ
        FYD12864DispPrintfFlash(1, 1, buf);   
    	//�������ò������ڵ�ֵ
        globalvar.afterdex = atol((char *)ScreenInput.array);

		//���벻ֱ����ʾ����"*"������ʾ
        for(i = 0; i < ScreenInput.inputlen; i++)
        {
            tmpbuf[i] = '*';

        }
		//��ʾ����"*"
        FYD12864DispPrintfFlash(2, 1, "������:%s", tmpbuf);
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
    }
	//������ʾ �����С�������룬��ͨ����Ļ������������ʾ ADD BY LY
    else  if(numpoint == 7)   
    {
    	//��ʾ��ʾ��Ϣ
        FYD12864DispString(1, 1, buf, 1);    
		//�������ò�������ֵ
        globalvar.afterdex = atol((char *)ScreenInput.array);
		//��ʾ���ò���ԭ����ֵ
        FYD12864DispPrintfFlash(2, 1, "��ֵ:-%ld", data);
		//��ʾ���ò����µ�ֵ
        FYD12864DispPrintfFlash(3, 1, "��ֵ:%s", ScreenInput.array);
        FYD12864ClearLine(4);
    }
}

/***************************************************************************
**	�������ƣ�InputInit(void)
**	�������ܣ���ʼ����������BUF
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void InputInit(void)
{
    memset(&ScreenInput, 0, sizeof(Screeninput));     //��ʼ��ScreenInput �ṹ��Ϊ 0
    globalvar.displayonce = 1;
}

//ȷ�ϴ���
void DispOK(void)
{
    if(ScreenInput.inputlen > 0)
    {
        if(globalvar.displayonce == 1)
        {
            sysparas = tmp_sysparas;

            if(globalvar.paraindex == 0)   //ֻ��Իָ��������ù��� ADD BY LY
            {
                if(SaveSysparas(SYSPARA_ADDR, (uint8_t*)&sysparas.head, sizeof(Sysparas)))
                {
                    FYD12864DispPrintfFlash(4, 1, "�ָ��������óɹ�");
                }
                else
                {
                    FYD12864DispPrintfFlash(4, 1, "�ָ���������ʧ��");
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
**	�������ƣ�DispInputPwd(char __farflash* pwdbuf, fun funname)
**	�������ܣ���ʾ�����������
**	���������pwbuf : ����ʾ���������йص���ʾ������"�˵����롢�������롢�ָ���������"�ȣ��ں���������ʱ��ֵ
	          funname :���������Ժ󣬰�ȷ��������Ҫִ�еĺ���
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispInputPwd(char __farflash* pwdbuf, fun funname)
{
    char  buf[7] = {0};//buf���ڴ����������� BY LY
    uint8_t i;

    ScreenInput.inputmaxlen = 6; //���볤��Ϊ 6

    NumKeyHandler(); //���ݴ���

    FYD12864ClearLine(1);          //���һ�С�
    FYD12864DispPrintfFlash(2, 1, pwdbuf);   //��ʾ pwdbuf��ָ���ַ����������
    FYD12864ClearLine(4);          //������С�

    for(i = 0; i < ScreenInput.inputlen; i++)
    {
        buf[i] = '*'; //����inputlen���������ݵĸ�������ʾ'*'�ĸ���
    }

    FYD12864DispPrintf(3, 1, buf);
    //FYD12864DispPrintfFlash( 3, 1,buf); //��ʾ buf�е�����

    globalvar.U32Temp = atol((char *)ScreenInput.array);     //��ScreenInput.array�е��ַ���ת���ɳ��������ݣ������globalvar.U32Temp �� Ϊ����������֤��׼�� ADD BY LY

    TaskAdd(KEY_RET, DispFirst, NULL);   //�����ؼ����ص�DsipFirst����
    TaskAdd(KEY_OK, funname, NULL);   //��ȷ�ϼ�ִ��funname��ָ����
}

/***************************************************************************
**	�������ƣ�DispInputMenuPwd(void)
**	�������ܣ���ʾ����˵�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispInputMenuPwd(void)
{
    DispInputPwd("������˵�����", DispJudgeMenuPwd);
}

/***************************************************************************
**	�������ƣ�DispInputQcPwd(void)
**	�������ܣ���ʾ�����ʼ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispInputQcPwd(void)
{
    DispInputPwd("�������ʼ�����", DispJudgeQcPwd);
}

/***************************************************************************
**	�������ƣ�DispInputTestPwd(void)
**	�������ܣ���ʾ����ά������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispInputTestPwd(void)
{
    DispInputPwd("������ά������", DispJudgeTestPwd);
}

/***************************************************************************
**	�������ƣ�DispInputResetPwd(void)
**	�������ܣ���ʾ����ָ���������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispInputResetPwd(void)
{
    DispInputPwd("������ָ�����", DispJudgeResetPwd);
}

/***************************************************************************
**	�������ƣ�DispJudgeMenuPwd(void)
**	�������ܣ��˵�������֤
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispJudgeMenuPwd(void)
{
    if(globalvar.U32Temp != sysparas.menupwd)   //��������������ϵͳ�����е����벻һ�£�����ʾ�������  ADD BY LY
    {
        FYD12864DispPrintfFlash(4, 1, "�������");
        TaskAdd(KEY_CLR, DispInputMenuPwd, NULL);
        TaskAdd(KEY_RET, DispInputMenuPwd, NULL);
    }
    else
    {
        DispUI = DispMenu_1; //����������ȷ��������һ���˵�
    }
}

/***************************************************************************
**	�������ƣ�DispJudgeQcPwd(void)
**	�������ܣ��ʼ�������֤
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispJudgeQcPwd(void)
{
    if(globalvar.U32Temp != sysparas.qcpwd)
    {
        FYD12864DispPrintfFlash(4, 1, "�������");
        TaskAdd(KEY_CLR, DispInputQcPwd, NULL);
        TaskAdd(KEY_RET, DispInputQcPwd, NULL);
    }
    else
    {
        DispUI = DispQcPara;
    }
}

/***************************************************************************
**	�������ƣ�DispJudgeTestPwd(void)
**	�������ܣ�ά��������֤
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispJudgeTestPwd(void)
{
    if(globalvar.U32Temp != sysparas.testpwd)
    {
        FYD12864DispPrintfFlash(4, 1, "�������");
        TaskAdd(KEY_CLR, DispInputTestPwd, NULL);
        TaskAdd(KEY_RET, DispInputTestPwd, NULL);
    }
    else
    {
        DispUI = DeviceTestMenu;
    }
}

/***************************************************************************
**	�������ƣ�DispJudgeResetPwd(void)
**	�������ܣ��ָ���������������֤
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispJudgeResetPwd(void)
{
    if((globalvar.U32Temp != sysparas.resetpwd) && (globalvar.U32Temp != 760103))       //�˴���������������ϵͳ�еĻָ�����������ͬ����������Ϊ760103�����ɻָ���������
    {
        FYD12864DispPrintfFlash(4, 1, "�������");
        TaskAdd(KEY_CLR, DispInputResetPwd, NULL);
        TaskAdd(KEY_RET, DispInputResetPwd, NULL);
    }
    else
    {
        DispUI = DispUnitQcReset;
    }
}

/***************************************************************************
**	�������ƣ�DispMenu_1(void)
**	�������ܣ��˵���ʾ����1
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispMenu_1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.��������");
    FYD12864DispPrintfFlash(2, 1, "2.ϵͳ��������");
    FYD12864DispPrintfFlash(3, 1, "3.������Ϣ����");
    FYD12864DispPrintfFlash(4, 1, "4.IC����������");

    TaskAdd(KEY_1, DispSetPrice, NULL);
    TaskAdd(KEY_2, DispSysSet1, NULL);
    TaskAdd(KEY_3, DispSetShiftInfo1, NULL);
    TaskAdd(KEY_4, DispSetICInfo1, NULL);
    TaskAdd(KEY_DOWN, DispMenu_2, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetDefPrice(void)
**	�������ܣ�Ĭ�ϵ�������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetDefPrice(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 5;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);       //��atof()�е����ݰ��չ涨�ĸ�ʽ��ӡ��buf������ BY LY
    tmp_sysparas.price[0] = atol(buf);   //atol()���ַ���ת���ɳ������� BY LY
    globalvar.paraindex = 7;

    if(tmp_sysparas.price[0] > 10000)
    {
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[0], "Ĭ�ϵ�������");
    TaskAdd(KEY_RET, DispSetPrice, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetPrice);
}

//��������
/***************************************************************************
**	�������ƣ�DispSetPrice(void)
**	�������ܣ���������ѡ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetPrice(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.Ĭ�ϵ�������");
    FYD12864DispPrintfFlash(2, 1, "2.�Żݵ�������");
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetDefPrice, NULL);
    TaskAdd(KEY_2, DispSetOthPrice1, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetPrice1(void)
**	�������ܣ��Żݵ���1����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[1], "�û�������");
    TaskAdd(KEY_RET, DispSetOthPrice1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice1);
}

/***************************************************************************
**	�������ƣ�DispSetPrice2(void)
**	�������ܣ��Żݵ���2����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[2], "Ա��������");
    TaskAdd(KEY_RET, DispSetOthPrice1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice1);
}

/***************************************************************************
**	�������ƣ�DispSetPrice3(void)
**	�������ܣ��Żݵ���3����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[3], "���˿�����");
    TaskAdd(KEY_RET, DispSetOthPrice1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice1);
}

/***************************************************************************
**	�������ƣ�DispSetPrice4(void)
**	�������ܣ��Żݵ���4����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[4], "ά��������");
    globalvar.paraindex = 11;
    TaskAdd(KEY_RET, DispSetOthPrice1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice1);
}

/***************************************************************************
**	�������ƣ�DispSetPrice5(void)
**	�������ܣ��Żݵ���5����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[5], "�ڲ�������");
    globalvar.paraindex = 12;
    TaskAdd(KEY_RET, DispSetOthPrice2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice2);
}

/***************************************************************************
**	�������ƣ�DispSetPrice6(void)
**	�������ܣ��Żݵ���6����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[6], "���ֿ�����");
    globalvar.paraindex = 13;
    TaskAdd(KEY_RET, DispSetOthPrice2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice2);
}

/***************************************************************************
**	�������ƣ�DispSetPrice7(void)
**	�������ܣ��Żݵ���7����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[7], "VIP ������");
    globalvar.paraindex = 14;
    TaskAdd(KEY_RET, DispSetOthPrice2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice2);
}

/***************************************************************************
**	�������ƣ�DispSetPrice8(void)
**	�������ܣ��Żݵ���8����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[8], "���ÿ�2 ����");
    globalvar.paraindex = 15;
    TaskAdd(KEY_RET, DispSetOthPrice2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice2);
}

/***************************************************************************
**	�������ƣ�DispSetPrice9(void)
**	�������ܣ��Żݵ���9����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���۳���");
        NumKeyHandler();
        return;
    }

    DispSet(2, sysparas.price[9], "���ÿ�3 ����");
    globalvar.paraindex = 16;
    TaskAdd(KEY_RET, DispSetOthPrice3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetOthPrice3);
}


/***************************************************************************
**	�������ƣ�DispSetOthPrice1(void)
**	�������ܣ��Żݵ�������ѡ�����1
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetOthPrice1(void)
{
    InputInit();

    FYD12864DispPrintfFlash(1, 1, "1.�û��� %d.%02d", sysparas.price[1] / 100, sysparas.price[1] % 100);
    FYD12864DispPrintfFlash(2, 1, "2.Ա���� %d.%02d", sysparas.price[2] / 100, sysparas.price[2] % 100);
    FYD12864DispPrintfFlash(3, 1, "3.���˿� %d.%02d", sysparas.price[3] / 100, sysparas.price[3] % 100);
    FYD12864DispPrintfFlash(4, 1, "4.ά���� %d.%02d", sysparas.price[4] / 100, sysparas.price[4] % 100);

    TaskAdd(KEY_1, DispSetPrice1, NULL);
    TaskAdd(KEY_2, DispSetPrice2, NULL);
    TaskAdd(KEY_3, DispSetPrice3, NULL);
    TaskAdd(KEY_4, DispSetPrice4, NULL);
    TaskAdd(KEY_DOWN, DispSetOthPrice2, NULL);
    TaskAdd(KEY_RET, DispSetPrice, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetOthPrice2(void)
**	�������ܣ��Żݵ�������ѡ�����2
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetOthPrice2(void)
{
    InputInit();

    FYD12864DispPrintfFlash(1, 1, "1.�ڲ���  %d.%02d", sysparas.price[5] / 100, sysparas.price[5] % 100);
    FYD12864DispPrintfFlash(2, 1, "2.���ֿ�  %d.%02d", sysparas.price[6] / 100, sysparas.price[6] % 100);
    FYD12864DispPrintfFlash(3, 1, "3.VIP ��  %d.%02d", sysparas.price[7] / 100, sysparas.price[7] % 100);
    FYD12864DispPrintfFlash(4, 1, "4.���ÿ�2 %d.%02d", sysparas.price[8] / 100, sysparas.price[8] % 100);

    TaskAdd(KEY_1, DispSetPrice5, NULL);
    TaskAdd(KEY_2, DispSetPrice6, NULL);
    TaskAdd(KEY_3, DispSetPrice7, NULL);
    TaskAdd(KEY_4, DispSetPrice8, NULL);
    TaskAdd(KEY_UP, DispSetOthPrice1, NULL);
    TaskAdd(KEY_DOWN, DispSetOthPrice3, NULL);
    TaskAdd(KEY_RET, DispSetPrice, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetOthPrice3(void)
**	�������ܣ��Żݵ�������ѡ�����3
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetOthPrice3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.���ÿ�3 %d.%02d", sysparas.price[9] / 100, sysparas.price[9] % 100);
    FYD12864ClearLine(2);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetPrice9, NULL);
    TaskAdd(KEY_UP, DispSetOthPrice2, NULL);
    TaskAdd(KEY_RET, DispSetPrice, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetUnit(void)
**	�������ܣ�������λ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.unit, "������λ����");
    FYD12864DispPrintfFlash(2, 1, "1.����");
    FYD12864DispPrintfFlash(3, 1, "2.�귽");
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);

}

/***************************************************************************
**	�������ƣ�DispSetQCMode(void)
**	�������ܣ��ʼ�ģʽ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.qcmode, "�ʼ�ģʽ����");

    FYD12864DispPrintfFlash(2, 1, "0.����  1.�ֳֻ�");
    FYD12864DispPrintfFlash(3, 1, "2.IC��  3.���ܰ�");
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);

}

/***************************************************************************
**	�������ƣ�DispEraseChip(void)
**	�������ܣ�Flash�洢оƬ����ѡ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispEraseChip(void)
{
    globalvar.displayonce = 1;

    FYD12864DispPrintfFlash(1, 1, "��������FLASH");
    FYD12864DispPrintfFlash(2, 1, "��ȷ�ϡ�  ����");
    FYD12864DispPrintfFlash(3, 1, "�����ء�  ȡ��");
    FYD12864ClearLine(4);

    TaskAdd(KEY_OK, JS28F128FlashEraseAll, NULL);   //��Ҫ��Ӳ���flash�Ĵ��� ADD BY LY
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);
}

/***************************************************************************
**	�������ƣ�DispFactoryReset(void)
**	�������ܣ��ָ���������ѡ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispFactoryReset(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "  �ָ���������");
    FYD12864DispPrintfFlash(2, 1, "��ȷ�ϡ�  ����");
    FYD12864DispPrintfFlash(3, 1, "�����ء�  ȡ��");
    FYD12864ClearLine(4);
    FactoryReset();
    globalvar.paraindex = 0;

    if(globalvar.KeyValue == KEY_OK)
        ScreenInput.inputlen = 1;

    TaskAdd(KEY_OK, DispOK, SystemReStart);
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);
}

/***************************************************************************
**	�������ƣ�DispUnitQcReset(void)
**	�������ܣ�������λ���ʼ�ģʽ���ָ���������ѡ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispUnitQcReset(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.������λ����");
    FYD12864DispPrintfFlash(2, 1, "2.�ʼ�ģʽ����");
    FYD12864DispPrintfFlash(3, 1, "3.�洢оƬ����");
    FYD12864DispPrintfFlash(4, 1, "0.�ָ���������");
    TaskAdd(KEY_1, DispSetUnit, NULL);
    TaskAdd(KEY_2, DispSetQCMode, NULL);
    TaskAdd(KEY_3, DispEraseChip, NULL);
    TaskAdd(KEY_0, DispFactoryReset, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetGunNum(void)
**	�������ܣ�ǹ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetGunNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 2;
    tmp_sysparas.gunnum = atol((char *)ScreenInput.array);

    if((tmp_sysparas.gunnum > 0) && (tmp_sysparas.gunnum < 100))       //ǹ��ֻ��ȡ1-99 ADD BY LY
    {
        TaskAdd(KEY_OK, DispOK, DispSysSet1);
    }

    DispSet(0, sysparas.gunnum, "ǹ������");
    globalvar.paraindex = 2;
    TaskAdd(KEY_RET, DispSysSet1, NULL);
    //TaskAdd( KEY_OK, DispOK, DispSysSet1 );
}

/***************************************************************************
**	�������ƣ�DispSetTotvolume(void)
**	�������ܣ�����������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetTotvolume(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.totvolume = atol(buf);
    globalvar.paraindex = 5;
    DispSet(2, sysparas.totvolume, "����������");
    TaskAdd(KEY_RET, DispSysSet1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet1);
}

/***************************************************************************
**	�������ƣ�DispSetTotmoney(void)
**	�������ܣ��ܽ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetTotmoney(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.totmoney = atol(buf);
    globalvar.paraindex = 6;
    DispSet(2, sysparas.totmoney, "�ܽ������");
    TaskAdd(KEY_RET, DispSysSet1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet1);
}

/***************************************************************************
**	�������ƣ�DispSetModbusType(void)
**	�������ܣ���������������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.modbustype, "��������������");
    FYD12864DispPrintfFlash(2, 1, "1.��˹����");
    FYD12864DispPrintfFlash(3, 1, "2.E+H");
    TaskAdd(KEY_RET, DispSysSet1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet1);
}

/***************************************************************************
**	�������ƣ�DispSysSet1(void)
**	�������ܣ�ϵͳ�������ý���1
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSysSet1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.ǹ��");
    FYD12864DispPrintfFlash(2, 1, "2.������");
    FYD12864DispPrintfFlash(3, 1, "3.�ܽ��");
    FYD12864DispPrintfFlash(4, 1, "4.����������");

    TaskAdd(KEY_1, DispSetGunNum, NULL);
    TaskAdd(KEY_2, DispSetTotvolume, NULL);
    TaskAdd(KEY_3, DispSetTotmoney, NULL);
    TaskAdd(KEY_4, DispSetModbusType, NULL);
    TaskAdd(KEY_DOWN, DispSysSet2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetTransnum(void)
**	�������ܣ���ˮ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetTransnum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    tmp_sysparas.transnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 3;
    DispSet(0, sysparas.transnum, "��ˮ������");
    TaskAdd(KEY_RET, DispSysSet2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet2);
}

/***************************************************************************
**	�������ƣ�DispSetUpTransnum(void)
**	�������ܣ��Ѵ���ˮ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetUpTransnum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    tmp_sysparas.uptransnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 4;
    DispSet(0, sysparas.uptransnum, "�Ѵ���ˮ������");
    TaskAdd(KEY_RET, DispSysSet2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet2);
}

/***************************************************************************
**	�������ƣ�DispSetModnum(void)
**	�������ܣ��޸ļ�¼��ˮ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetModnum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    tmp_sysparas.modnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 34;
    DispSet(0, sysparas.modnum, "�޸���ˮ������");
    TaskAdd(KEY_RET, DispSysSet2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet2);
}

/***************************************************************************
**	�������ƣ�DispSetUpModnum(void)
**	�������ܣ��Ѵ������޸���ˮ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetUpModnum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    ScreenInput.pointnum = 0;
    tmp_sysparas.upmodnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 35;
    DispSet(0, sysparas.upmodnum, "�Ѵ��޸ĺ�����");
    TaskAdd(KEY_RET, DispSysSet2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet2);
}

/***************************************************************************
**	�������ƣ�DispSysSet2(void)
**	�������ܣ�ϵͳ�������ý���2
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSysSet2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.��ˮ��");
    FYD12864DispPrintfFlash(2, 1, "2.�Ѵ���ˮ��");
    FYD12864DispPrintfFlash(3, 1, "3.�޸ļ�¼��ˮ��");
    FYD12864DispPrintfFlash(4, 1, "4.�Ѵ��޸���ˮ��");

    TaskAdd(KEY_1, DispSetTransnum, NULL);
    TaskAdd(KEY_2, DispSetUpTransnum, NULL);
    TaskAdd(KEY_3, DispSetModnum, NULL);
    TaskAdd(KEY_4, DispSetUpModnum, NULL);
    TaskAdd(KEY_UP, DispSysSet1, NULL);
    TaskAdd(KEY_DOWN, DispSysSet3, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetModbusAdr_G(void)
**	�������ܣ����������Ƶ�ַ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(0, sysparas.modbusaddr_g, "���������Ƶ�ַ");

    TaskAdd(KEY_RET, DispSysSet3, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetModbusAdr_L(void)
**	�������ܣ�Һ�������Ƶ�ַ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(0, sysparas.modbusaddr_l, "Һ�������Ƶ�ַ");

    TaskAdd(KEY_RET, DispSysSet3, NULL);

}

/***************************************************************************
**	�������ƣ�DispSetModbusRS485(void)
**	�������ܣ������Ƶ�ַ��������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.modbusRS485, "��������������");
    FYD12864DispPrintfFlash(2, 1, "1.��Һ��ϲ�");
    FYD12864DispPrintfFlash(3, 1, "2.��Һ��ֿ�");
    TaskAdd(KEY_RET, DispSysSet3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSysSet3);
}

/***************************************************************************
**	�������ƣ�DispSysSet3(void)
**	�������ܣ�ϵͳ�������ý���4
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSysSet3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.���������Ƶ�ַ");
    FYD12864DispPrintfFlash(2, 1, "2.Һ�������Ƶ�ַ");
    FYD12864DispPrintfFlash(3, 1, "3.�����Ƶ�ַ����");
    FYD12864ClearLine(4);

    TaskAdd(KEY_1, DispSetModbusAdr_G, NULL);
    TaskAdd(KEY_2, DispSetModbusAdr_L, NULL);
    TaskAdd(KEY_3, DispSetModbusRS485, NULL);
    TaskAdd(KEY_UP, DispSysSet2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetShiftNum(void)
**	�������ܣ��������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.shiftnum = atol((char *)ScreenInput.array);

    //����İ������0��shiftmaxnum֮�� ADD BY LY
    if((tmp_sysparas.shiftnum > 0) && (tmp_sysparas.shiftnum <= tmp_sysparas.shiftmaxnum))
    {
    	globalvar.U8Temp = sysparas.shiftnum;
        TaskAdd(KEY_OK, DispOK, DispSetShiftInfo1);
    }
    else if(tmp_sysparas.shiftnum > tmp_sysparas.shiftmaxnum)
    {
        FYD12864DispPrintfFlash(4, 1, "��ų���");
        NumKeyHandler();
        return;
    }

    globalvar.paraindex = 37;
    DispSet(0, sysparas.shiftnum, "�������");
    TaskAdd(KEY_RET, DispSetShiftInfo1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetShiftMaxNum(void)
**	�������ܣ����������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
    DispSet(0, sysparas.shiftmaxnum, "���������");
    TaskAdd(KEY_RET, DispSetShiftInfo1, NULL);
}


/***************************************************************************
**	�������ƣ�DispSetShiftTransNum(void)
**	�������ܣ�����ˮ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftTransNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    tmp_sysparas.shiftransnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 38;
    DispSet(0, sysparas.shiftransnum, "����ˮ������");
    TaskAdd(KEY_RET, DispSetShiftInfo1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo1);
}

/***************************************************************************
**	�������ƣ�DispSetShiftUpTransNum(void)
**	�������ܣ��Ѵ�����ˮ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftUpTransNum(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    tmp_sysparas.shiftuptransnum = atol((char *)ScreenInput.array);
    globalvar.paraindex = 39;
    DispSet(0, sysparas.shiftuptransnum, "�Ѵ�����ˮ������");
    TaskAdd(KEY_RET, DispSetShiftInfo1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo1);
}

/***************************************************************************
**	�������ƣ�DispSetShiftInfo1(void)
**	�������ܣ�������Ϣ���ý���1
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftInfo1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.���");
    FYD12864DispPrintfFlash(2, 1, "2.�����");
    FYD12864DispPrintfFlash(3, 1, "3.����ˮ��");
    FYD12864DispPrintfFlash(4, 1, "4.�Ѵ�����ˮ��");
    TaskAdd(KEY_1, DispSetShiftNum, NULL);
    TaskAdd(KEY_2, DispSetShiftMaxNum, NULL);
    TaskAdd(KEY_3, DispSetShiftTransNum, NULL);
    TaskAdd(KEY_4, DispSetShiftUpTransNum, NULL);
    //TaskAdd(KEY_UP, DispSetShiftInfo3,NULL);
    TaskAdd(KEY_DOWN, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetShiftStartvol(void)
**	�������ܣ��࿪ʼǹ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftStartvol(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.shiftstartvol = atol(buf);
    globalvar.paraindex = 42;
    DispSet(2, sysparas.shiftstartvol, "�࿪ʼǹ������");
    TaskAdd(KEY_RET, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo2);
}

/***************************************************************************
**	�������ƣ�DispSetShiftStartmon(void)
**	�������ܣ��࿪ʼ�������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftStartmon(void)
{
    char buf[16] = {0};

    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.shiftstartmon = atol(buf);
    globalvar.paraindex = 44;
    DispSet(2, sysparas.shiftstartmon, "�࿪ʼ�������");

    TaskAdd(KEY_RET, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo2);
}

/***************************************************************************
**	�������ƣ�DispSetShiftTotvol(void)
**	�������ܣ����ۼ���������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftTotvol(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.shiftotvol = atol(buf);
    globalvar.paraindex = 46;
    DispSet(2, sysparas.shiftotvol, "���ۼ���������");
    TaskAdd(KEY_RET, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo2);
}

/***************************************************************************
**	�������ƣ�DispSetShiftTotmon(void)
**	�������ܣ����ۼƽ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftTotmon(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.shiftotmon = atol(buf);
    globalvar.paraindex = 47;
    DispSet(2, sysparas.shiftotmon, "���ۼƽ������");
    TaskAdd(KEY_RET, DispSetShiftInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo2);
}

/***************************************************************************
**	�������ƣ�DispSetShiftInfo2(void)
**	�������ܣ�������Ϣ���ý���2
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftInfo2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.�࿪ʼǹ��");
    FYD12864DispPrintfFlash(2, 1, "2.�࿪ʼ���");
    FYD12864DispPrintfFlash(3, 1, "3.���ۼ�����");
    FYD12864DispPrintfFlash(4, 1, "4.���ۼƽ��");

    TaskAdd(KEY_1, DispSetShiftStartvol, NULL);
    TaskAdd(KEY_2, DispSetShiftStartmon, NULL);
    TaskAdd(KEY_3, DispSetShiftTotvol, NULL);
    TaskAdd(KEY_4, DispSetShiftTotmon, NULL);
    TaskAdd(KEY_UP, DispSetShiftInfo1, NULL);
    TaskAdd(KEY_DOWN, DispSetShiftInfo3, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetShiftFuelTimes(void)
**	�������ܣ��������������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftFuelTimes(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    tmp_sysparas.shiftfueltimes = atol((char *)ScreenInput.array);
    globalvar.paraindex = 49;
    DispSet(0, sysparas.shiftfueltimes, "�������������");
    TaskAdd(KEY_RET, DispSetShiftInfo3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetShiftInfo3);
}

/***************************************************************************
**	�������ƣ�DispSetShiftStartDate(void)
**	�������ܣ��࿪ʼ��������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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

        FYD12864DispPrintfFlash(1, 1, "�࿪ʼ��������");
        FYD12864DispPrintfFlash(2, 1, "��ֵ: %02X��%02X��%02X��",
                                sysparas.shiftstarttime[0],
                                sysparas.shiftstarttime[1],
                                sysparas.shiftstarttime[2]);


        FYD12864DispPrintfFlash(3, 1, "��ֵ: %02X��%02X��%02X��",
                                tmp_sysparas.shiftstarttime[0],
                                tmp_sysparas.shiftstarttime[1],
                                tmp_sysparas.shiftstarttime[2]);

        if(ScreenInput.inputlen > 5) ret = CheckDate(tmp);

        if(ret == false)
        {
            FYD12864DispPrintfFlash(4, 1, "�����ʽ����");
            return;
        }
        else
        {
            FYD12864ClearLine(4);
        }

        TaskAdd(KEY_RET, DispSetShiftInfo3, NULL);

        if(ScreenInput.inputlen == 6)   //������Ϻ���ܰ�ȷ�ϼ�
        {
            globalvar.paraindex = 40;
            TaskAdd(KEY_OK, DispOK, DispSetShiftInfo3);
        }
    }
}

/***************************************************************************
**	�������ƣ�DispSetShiftStartTime(void)
**	�������ܣ��࿪ʼʱ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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

        FYD12864DispPrintfFlash(1, 1, "�࿪ʼʱ������");
        FYD12864DispPrintfFlash(2, 1, "��ֵ: %02X:%02X:%02X",
                                sysparas.shiftstarttime[3],
                                sysparas.shiftstarttime[4],
                                sysparas.shiftstarttime[5]);
        FYD12864DispPrintfFlash(3, 1, "��ֵ: %02X:%02X:%02X",
                                tmp_sysparas.shiftstarttime[3],
                                tmp_sysparas.shiftstarttime[4],
                                tmp_sysparas.shiftstarttime[5]);



        if(ScreenInput.inputlen > 5) ret = CheckTime(tmp);

        if(ret == false)
        {
            FYD12864DispPrintfFlash(4, 1, "�����ʽ����");
            return;
        }
        else
        {
            FYD12864ClearLine(4);
        }

        TaskAdd(KEY_RET, DispSetShiftInfo3, NULL);

        if(ScreenInput.inputlen == 6)   //������Ϻ���ܰ�ȷ�ϼ�
        {
            globalvar.paraindex = 40;
            TaskAdd(KEY_OK, DispOK, DispSetShiftInfo3);
        }
    }
}

/***************************************************************************
**	�������ƣ�DispSetShiftInfo3(void)
**	�������ܣ�������Ϣ���ý���3
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetShiftInfo3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.���������");
    FYD12864DispPrintfFlash(2, 1, "2.�࿪ʼ����");
    FYD12864DispPrintfFlash(3, 1, "3.�࿪ʼʱ��");
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetShiftFuelTimes, NULL);
    TaskAdd(KEY_2, DispSetShiftStartDate, NULL);
    TaskAdd(KEY_3, DispSetShiftStartTime, NULL);
    TaskAdd(KEY_UP, DispSetShiftInfo2, NULL);
    //TaskAdd(KEY_DOWN, DispSetShiftInfo1,NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetCardMaxYuE(void)
**	�������ܣ���������������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    globalvar.paraindex = 55;
    DispSet(2, sysparas.cardmaxyue, "�������������");
    TaskAdd(KEY_RET, DispSetICInfo1, NULL);

}

/***************************************************************************
**	�������ƣ�DispSetCardMinYuE(void)
**	�������ܣ���������������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetCardMinYuE(void)
{
    char buf[16] = {0};
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 16;
    sprintf_P(buf, "%f", atof((char *)ScreenInput.array) * 100);
    tmp_sysparas.cardminyue = atol(buf);
    globalvar.paraindex = 56;
    DispSet(2, sysparas.cardminyue, "�������������");
    TaskAdd(KEY_RET, DispSetICInfo1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo1);
}

/***************************************************************************
**	�������ƣ�DispSetAccountCardMaxYuE(void)
**	�������ܣ����˿������������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "���˿�����");
        NumKeyHandler();
        return;
    }

    globalvar.paraindex = 57;
    DispSet(2, sysparas.accountcardmaxyue, "���˿�����������");
    TaskAdd(KEY_RET, DispSetICInfo1, NULL);

}

/***************************************************************************
**	�������ƣ�DispSetAccountCardMaxYuE(void)
**	�������ܣ�PSAM����ѡ��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(0, sysparas.cardpsamnum, "PSAM����ѡ�� 1-4");
    TaskAdd(KEY_RET, DispSetICInfo3, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo3);
}

/***************************************************************************
**	�������ƣ�DispSetCardMaxDay(void)
**	�������ܣ�IC��δʹ�������ʱ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetCardMaxDay(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.cardmaxday = atol((char *)ScreenInput.array);
    globalvar.paraindex = 59;
    DispSet(0, sysparas.cardmaxday, "���������");
    TaskAdd(KEY_RET, DispSetICInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo2);
}

/***************************************************************************
**	�������ƣ�DispSetUseCard(void)
**	�������ܣ��Ƿ��ÿ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.usecard, "�Ƿ��ÿ�");
    FYD12864DispPrintfFlash(2, 1, "0.�ɲ�ʹ�ÿ�");
    FYD12864DispPrintfFlash(3, 1, "1.�ÿ�  2.�޿�");

    TaskAdd(KEY_RET, DispSetICInfo1, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo1);
}

/***************************************************************************
**	�������ƣ�DispSetICInfo1(void)
**	�������ܣ�IC����Ϣ���ý���1
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetICInfo1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.����������");
    FYD12864DispPrintfFlash(2, 1, "2.����������");
    FYD12864DispPrintfFlash(3, 1, "3.���˿��������");
    FYD12864DispPrintfFlash(4, 1, "4.�Ƿ��ÿ�");

    TaskAdd(KEY_1, DispSetCardMaxYuE, NULL);
    TaskAdd(KEY_2, DispSetCardMinYuE, NULL);
    TaskAdd(KEY_3, DispSetAccountCardMaxYuE, NULL);   //ycx20161202
    TaskAdd(KEY_4, DispSetUseCard, NULL);
    TaskAdd(KEY_DOWN, DispSetICInfo2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetCardCheck(void)
**	�������ܣ��Ƿ�����ƿѡ��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.cardcheck, "�Ƿ�����ƿ");
    FYD12864DispPrintfFlash(2, 1, "0.������");
    FYD12864DispPrintfFlash(3, 1, "1.����");
    TaskAdd(KEY_RET, DispSetICInfo2, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetICInfo2);
}

/***************************************************************************
**	�������ƣ�DispSetKeybOK(void)
**	�������ܣ����ò�����ɣ����ý����ʾ
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetKeybOK(void)
{
    if(EepromInterWriteBytes(LOCKVER_ADDR, &m1card.newkeyb[0], 6))     //��LOCKVER_ADDRλ�ñ����µ�B�� ADD BY LY
    {
        FYD12864DispPrintfFlash(4, 1, "B �����óɹ�");
    }
    else
    {
        FYD12864DispPrintfFlash(4, 1, "B ������ʧ��");
    }

    if(DispNextUI != NULL)
    {
        TaskMessage(10, DispNextUI);
    }
}

/***************************************************************************
**	�������ƣ�DispSetKeyB(void)
**	�������ܣ�IC��B������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�LY
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetKeyB(void)
{
	
    ScreenInput.inputmaxlen = 6;
    FYD12864DispPrintfFlash(1, 1, "�����뿨B ��:");

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
        FYD12864DispPrintfFlash(4, 1, "�����������");
        NumKeyHandler();
        return;
    }

    FYD12864DispPrintfFlash(2, 1, "����:%s", m1card.newkeyb);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_RET, DispSetICInfo2, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetLossCard(void)
**	�������ܣ�������й�ʧ�����ѡ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�LY
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetLossCard(void)
{

    FYD12864DispPrintfFlash(1, 1, "����ȷ�ϡ������");
    FYD12864DispPrintfFlash(2, 1, "�й�ʧ����¼");
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);


    TaskAdd(KEY_OK, ClearLossCard, NULL);
    TaskAdd(KEY_RET, DispSetICInfo2, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetICInfo2(void)
**	�������ܣ�IC����Ϣ���ý���2
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetICInfo2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.�Ƿ�����ƿ");
    FYD12864DispPrintfFlash(2, 1, "2.���������");
    FYD12864DispPrintfFlash(3, 1, "3.������й�ʧ��");
    FYD12864DispPrintfFlash(4, 1, "4.B ������");

    TaskAdd(KEY_1, DispSetCardCheck, NULL);
    TaskAdd(KEY_2, DispSetCardMaxDay, NULL);
    TaskAdd(KEY_3, DispSetLossCard, NULL);
    TaskAdd(KEY_4, DispSetKeyB, NULL);
    TaskAdd(KEY_UP, DispSetICInfo1, NULL);
    TaskAdd(KEY_DOWN, DispSetICInfo3, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetICInfo3(void)
**	�������ܣ�IC����Ϣ���ý���4
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetICInfo3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.PSAM����ѡ��");
    FYD12864ClearLine(2);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);

    TaskAdd(KEY_1, DispSetPsamNum, NULL);
    TaskAdd(KEY_UP, DispSetICInfo2, NULL);
    TaskAdd(KEY_RET, DispMenu_1, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetShengCode(void)
**	�������ܣ�ʡ��������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(1, 1, "ʡ��������");
        FYD12864DispPrintfFlash(2, 1, "��ֵ: %02X%02X", sysparas.shengcode[0], sysparas.shengcode[1]);
        globalvar.beforedex = BcdbufToHex(sysparas.shengcode, 2);
        FYD12864DispPrintfFlash(3, 1, "��ֵ: %02X%02X", tmp_sysparas.shengcode[0], tmp_sysparas.shengcode[1]);
        globalvar.afterdex = BcdbufToHex(tmp_sysparas.shengcode, 2);
        globalvar.pointnum = 0;
        FYD12864ClearLine(4);
        TaskAdd(KEY_RET, DispGeneralInfo, NULL);
        TaskAdd(KEY_OK, DispOK, DispGeneralInfo);
    }
}

/***************************************************************************
**	�������ƣ�DispSetStationCode(void)
**	�������ܣ�վ��������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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

        FYD12864DispPrintfFlash(1, 1, "վ��������");
        FYD12864DispPrintfFlash(2, 1, "��ֵ: %ld",sysparas.stationcode);
        FYD12864DispPrintfFlash(3, 1, "��ֵ: %ld",tmp_sysparas.stationcode);
        FYD12864ClearLine(4);
        
        TaskAdd(KEY_RET, DispGeneralInfo, NULL);
        TaskAdd(KEY_OK, DispOK, DispGeneralInfo);
    }

}

/***************************************************************************
**	�������ƣ�DispSetCityCode(void)
**	�������ܣ����б�������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(1, 1, "���б�������");

        FYD12864DispPrintfFlash(2, 1, "��ֵ: %02X%02X",
                                sysparas.citycode[0],
                                sysparas.citycode[1]);
        globalvar.beforedex = BcdbufToHex(sysparas.citycode, 2);

        FYD12864DispPrintfFlash(3, 1, "��ֵ: %02X%02X",
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
**	�������ƣ�DispSetParentCode(void)
**	�������ܣ��ϼ���λ��������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(1, 1, "�ϼ���λ��������", 1);

        FYD12864DispPrintfFlash(2, 1, "��ֵ: %02X%02X%02X%02X",
                                sysparas.parentcode[0],
                                sysparas.parentcode[1],
                                sysparas.parentcode[2],
                                sysparas.parentcode[3]);

        globalvar.beforedex = BcdbufToHex(sysparas.parentcode, 4);

        FYD12864DispPrintfFlash(3, 1, "��ֵ: %02X%02X%02X%02X",
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
**	�������ƣ�DispGeneralInfo(void)
**	�������ܣ�ͨ����Ϣ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispGeneralInfo(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.ʡ����");
    FYD12864DispPrintfFlash(2, 1, "2.վ����");
    FYD12864DispPrintfFlash(3, 1, "3.���д���");
    FYD12864DispPrintfFlash(4, 1, "4.�ϼ���λ����");

    TaskAdd(KEY_1, DispSetShengCode, NULL);
    TaskAdd(KEY_2, DispSetStationCode, NULL);
    TaskAdd(KEY_3, DispSetCityCode, NULL);
    TaskAdd(KEY_4, DispSetParentCode, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/***************************************************************************
**	�������ƣ�SystemReStart(void)
**	�������ܣ�ϵͳ��0��ַ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void SystemReStart(void)
{
    FYD12864ClearLine(1);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);

    FYD12864DispPrintfFlash(2, 1, "ϵͳ������");
    _delay_ms(500);
    //DispUI = NULL; 				//������Ӧ�ó���
    asm("jmp 0");					//������Ӧ�ó���
    //asm("jmp 0x1e000");
    //__indirect_jump_to(0x1e000);	//������BootLoader����
    //(*(void(*)())0x1E000)();		//������BootLoader����
    //((void(*)(void))0X1E000)();	//������BootLoader����
}

/***************************************************************************
**	�������ƣ�SystemReStartToBootLoader(void)
**	�������ܣ�ϵͳ��BootLoader����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void SystemReStartToBootLoader(void)
{
    FYD12864ClearLine(1);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "ϵͳ������");
    _delay_ms(500);
    asm("jmp 0x1e000");			//������BootLoader����
}

/***************************************************************************
**	�������ƣ�SystemReStartToAPP(void)
**	�������ܣ�ϵͳ��ָ��Ӧ�õ�ַ������0��ַ��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void SystemReStartToAPP(void)
{
    FYD12864ClearLine(1);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    FYD12864DispPrintfFlash(2, 1, "ϵͳ������");
    _delay_ms(500);
    asm("jmp 0");				//������Ӧ�ó���
}

/***************************************************************************
**	�������ƣ�DispSetModbusBaud(void)
**	�������ܣ������Ʋ���������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������", 1);
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.modbusbaud, "�����Ʋ�����");
    FYD12864DispPrintfFlash(2, 1, "1.9600  2.19200", 1);
    FYD12864DispPrintfFlash(3, 1, "3.38400 4.115200", 1);
    TaskAdd(KEY_RET, DispCommPara, NULL);

    if(tmp != 0)
    {
        TaskAdd(KEY_OK, DispOK, SystemReStart);
    }
}

/***************************************************************************
**	�������ƣ�DispSetPCBaud(void)
**	�������ܣ���̨����������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetPCBaud(void)
{
    uint8_t tmp;
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 1;
    tmp = atol((char *)ScreenInput.array);    //�����ֵ

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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.pcbaud, "��̨������");
    FYD12864DispPrintfFlash(2, 1, "1.9600  2.19200");
    FYD12864DispPrintfFlash(3, 1, "3.38400 4.115200");
    TaskAdd(KEY_RET, DispCommPara, NULL);

    if(tmp != 0)
    {
        TaskAdd(KEY_OK, DispOK, SystemReStart);
    }
}

/***************************************************************************
**	�������ƣ�DispCommPara(void)
**	�������ܣ�ͨѶ��������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispCommPara(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.�����Ʋ�����");
    FYD12864DispPrintfFlash(2, 1, "2.��̨������");

    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetModbusBaud, NULL);
    TaskAdd(KEY_2, DispSetPCBaud, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/***************************************************************************
**	�������ƣ�SetPressureRatioEnter(void)
**	�������ܣ�ѹ����У
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
            FYD12864DispPrintfFlash(3, 1, "    ���óɹ�");
        }
        else
        {
            FYD12864DispPrintfFlash(3, 1, "    ����ʧ��");
        }
    }

    FYD12864DispPrintfFlash(4, 1, "�����밴�����ء�");
    TaskAdd(KEY_RET, DispSetPressure, NULL);

}

/***************************************************************************
**	�������ƣ�SetPressureRatio(void)
**	�������ܣ�ѹ��ϵ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void SetPressureRatio(void)
{
    uint32_t rd;

    tmp_sysparas = sysparas;
    globalvar.displayonce = 1;
    FYD12864DispPrintfFlash(1, 1, "  ����ѹ��ϵ��");
    rd = PlADCGetValue();
    globalvar.U32Temp = rd;

    FYD12864DispPrintfFlash(2, 1, "��ǰADֵ:%ld", rd);

    FYD12864DispPrintfFlash(3, 1, "������");
    ScreenInput.inputmaxlen = 5;
    NumKeyHandler();

    FYD12864DispPrintfFlash(4, 1, "��ǰѹ��:%s", ScreenInput.array);

    TaskAdd(KEY_RET, DispSetPressure, NULL);
    TaskAdd(KEY_OK, SetPressureRatioEnter, NULL);
}

/***************************************************************************
**	�������ƣ�SetPressureZeroEnter(void)
**	�������ܣ�ѹ�����������ɣ����ý����ʾ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
            FYD12864DispPrintfFlash(3, 1, "    ���óɹ�");
        }
        else
        {
            FYD12864DispPrintfFlash(3, 1, "    ����ʧ��");
        }
    }

    FYD12864DispPrintfFlash(4, 1, "�����밴�����ء�");
    TaskAdd(KEY_RET, DispSetPressure, NULL);
}

/***************************************************************************
**	�������ƣ�SetPressureZero(void)
**	�������ܣ�ѹ���������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void SetPressureZero(void)
{
    uint32_t rd;

    tmp_sysparas = sysparas;
    FYD12864DispPrintfFlash(1, 1, "  ����ѹ�����");
    rd = PlADCGetValue();
    tmp_sysparas.pzero = rd;

    if(tmp_sysparas.pzero < 100)
    {
        FYD12864ClearLine(2);
        FYD12864DispPrintfFlash(3, 1, "ѹ���쳣");
    }
    else
    {
        globalvar.paraindex = 32;
        globalvar.displayonce = 1;
        FYD12864DispPrintfFlash(2, 1, "��ǰADֵ:%ld", rd);
        FYD12864DispPrintfFlash(3, 1, "ȷ���밴��ȷ�ϡ�");
        TaskAdd(KEY_OK, SetPressureZeroEnter, NULL);
    }

    FYD12864DispPrintfFlash(4, 1, "�����밴�����ء�");
    TaskAdd(KEY_RET, DispSetPressure, NULL);

}

/***************************************************************************
**	�������ƣ�DispSetPressure(void)
**	�������ܣ�ѹ����ز�������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetPressure(void)
{
    FYD12864DispPrintfFlash(1, 1, "  У��ѹ������");
    FYD12864DispPrintfFlash(2, 1, "1.����ѹ�����");
    FYD12864DispPrintfFlash(3, 1, "2.����ѹ��ϵ��");
    FYD12864ClearLine(4);
    TaskAdd(KEY_RET, DispCommonOpt, NULL);
    TaskAdd(KEY_1, SetPressureZero, NULL);
    TaskAdd(KEY_2, SetPressureRatio, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetFuelMode(void)
**	�������ܣ�����ģʽѡ��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.fuelmode, "����ģʽ����");
    FYD12864DispPrintfFlash(2, 1, "0.�ֶ�    1.��̨", 1);
    FYD12864DispPrintfFlash(3, 1, "2.���ӱ�ǩ", 1);
    TaskAdd(KEY_RET, DispCommonOpt1, NULL);

}

/***************************************************************************
**	�������ƣ�DispSetDateTimeOK(void)
**	�������ܣ��������ں�ʱ����ʾȷ�ϼ����ý����ʾ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
            FYD12864DispPrintfFlash(4, 1, "���óɹ�");
        else
            FYD12864DispPrintfFlash(4, 1, "����ʧ��");
    }

    TaskMessage(10, DispCommonOpt1);
}

/***************************************************************************
**	�������ƣ�DispSetDateTime(void)
**	�������ܣ�����ʱ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetDateTime(void)
{
    uint8_t *tmp, i, ret = true;

    Time    newtime = {0};

    if(globalvar.KeyPreValue != KEY_PT)
    {
        FYD12864DispPrintfFlash(1, 1, "����ʱ������");
        NumKeyHandler();
        ScreenInput.inputmaxlen = 12;
        tmp = (uint8_t*)&newtime;
        newtime = time;

        // i<6 Ϊ������ӣ�û�д��ж����������ܻ��newtime����ĵ�ַ�е������޸� ADD BY LY
        for(i = 0; i < (ScreenInput.inputlen / 2 + 1) && i < 6; i++)
        {
            *(tmp + i) = AscToBcd(&ScreenInput.array[2 * i]);
        }

        FYD12864DispPrintfFlash(2, 1, "%02X��%02X��%02X��",
                                newtime.year,
                                newtime.month,
                                newtime.day);

        FYD12864DispPrintfFlash(3, 1, "%02Xʱ%02X��%02X��",
                                newtime.hour,
                                newtime.minute,
                                newtime.second);

        if(ScreenInput.inputlen > 5)
            ret = CheckDate(tmp);

        if(ScreenInput.inputlen > 9)
            ret = CheckTime(tmp + 3);

        if(ret == false)
        {
            FYD12864DispPrintfFlash(4, 1, "�����ʽ����");
            return;
        }
        else
        {
            FYD12864ClearLine(4);
        }

        TaskAdd(KEY_RET, DispCommonOpt1, NULL);

        if(ScreenInput.inputlen > 9)   //������Ϻ���ܰ�ȷ�ϼ�
        {
            TaskAdd(KEY_OK, DispSetDateTimeOK, NULL);
        }
    }
}

/***************************************************************************
**	�������ƣ�DispSetPrintMode(void)
**	�������ܣ�СƱ��ӡ��ʽ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.printmode, "��ӡ��ʽ����");
    FYD12864DispPrintfFlash(2, 1, "0.�ֶ���ӡ");
    FYD12864DispPrintfFlash(3, 1, "1.�Զ���ӡ");
    TaskAdd(KEY_RET, DispSetPrint, NULL);
    TaskAdd(KEY_OK, DispOK, DispSetPrint);
}

/***************************************************************************
**	�������ƣ�DispSetIntNum(void)
**	�������ܣ��Ƿ�ȡ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.intnumflag, "�Ƿ�ȡ������");
    FYD12864DispPrintfFlash(2, 1, "0.����ȡ��");
    FYD12864DispPrintfFlash(3, 1, "1.����ȡ��");
    TaskAdd(KEY_RET, DispCommonOpt, NULL);
    TaskAdd(KEY_OK, DispOK, DispCommonOpt);
}

//���ô�ӡ����
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }	
	
    DispSet(0, sysparas.printtimes, "��ӡ��������");
	
    TaskAdd(KEY_RET, DispSetPrint, NULL);

}

//��ӡ�й�����
void DispSetPrint(void)
{
    InputInit();

	FYD12864DispPrintfFlash(1, 1, "1.��ӡģʽ����");
	FYD12864DispPrintfFlash(2, 1, "2.��ӡ��������");
	FYD12864DispPrintfFlash(3, 1, "3.�Ƿ��ӡ����");
	FYD12864ClearLine(4);

    TaskAdd(KEY_1, DispSetPrintMode, NULL);	
    TaskAdd(KEY_2, DispSetPrintTimes, NULL);
    TaskAdd(KEY_3, DispSetIsPrintCarnum, NULL);
    TaskAdd(KEY_RET, DispCommonOpt, NULL);	
}
/***************************************************************************
**	�������ƣ�DispCommonOpt(void)
**	�������ܣ����ò������ý���1
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispCommonOpt(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.��Ļѡ��");
//    FYD12864DispPrintfFlash( 2, 1, "2.����ģʽ");
    FYD12864DispPrintfFlash(2, 1, "2.�Ƿ�ȡ��");
    FYD12864DispPrintfFlash(3, 1, "3.��ӡ����");
    FYD12864DispPrintfFlash(4, 1, "4.ѹ����У");

    TaskAdd(KEY_1, DispSetPing, NULL);
//    TaskAdd( KEY_2, DispSetOffline, NULL ); //huanghao 20161025
    TaskAdd(KEY_2, DispSetIntNum, NULL);   //ycx20161202
    TaskAdd(KEY_3, DispSetPrint, NULL);
    TaskAdd(KEY_4, DispSetPressure, NULL);
    TaskAdd(KEY_DOWN, DispCommonOpt1, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetOffline(void)
**	�������ܣ��ѻ�ģʽѡ��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.offlinemode, "����ģʽ����");
    FYD12864DispPrintfFlash(2, 1, "0.����ģʽ");
    FYD12864DispPrintfFlash(3, 1, "1.�ѻ�ģʽ");


    if(tmp_sysparas.offlinemode && globalvar.KeyValue == KEY_OK)
        CloseUart2RcvIRQ();//��uart2 ���������ж�
    else
        OpenUart2RcvIRQ();//��uart2 ���������ж�

    TaskAdd(KEY_RET, DispCommonOpt1, NULL);
}


/***************************************************************************
**	�������ƣ�DispSetIsPrintCarnum(void)
**	�������ܣ��Ƿ��ӡ���ƺ�
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-10-11
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.IsPrintCarnum, "�Ƿ��ӡ���ƺ�");
    FYD12864DispPrintfFlash(2, 1, "0.����ӡ");
    FYD12864DispPrintfFlash(3, 1, "1.��ӡ");
    TaskAdd(KEY_RET, DispSetPrint, NULL);
}


/***************************************************************************
**	�������ƣ�DispCommonOpt1(void)
**	�������ܣ����ò������ý���2
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispCommonOpt1(void)
{
	InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.����ģʽ����");
    FYD12864DispPrintfFlash(2, 1, "2.����ʱ������");
    FYD12864DispPrintfFlash(3, 1, "3.����ģʽ����");

    TaskAdd(KEY_1, DispSetFuelMode, NULL);
    TaskAdd(KEY_2, DispSetDateTime, NULL);
    TaskAdd(KEY_3, DispSetOffline, NULL);
    TaskAdd(KEY_4, DispSetIsPrintCarnum, NULL);
    TaskAdd(KEY_RET, DispMenu_2, NULL);
    TaskAdd(KEY_UP, DispCommonOpt, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetPing(void)
**	�������ܣ�������ѡ��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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

        //������Ļʱ��ˢһ����ycx20170330
        if(tmp == 0)
            OsplcShowB(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price, 0, 1);
        else OsplcShowN(fuelrecordinfo.volume, fuelrecordinfo.money, fuelrecordinfo.price);
    }
    else if(ScreenInput.inputlen > 0)
    {
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.lcdtype, "��Ļѡ������");
    FYD12864DispPrintfFlash(2, 1, "0.����");
    FYD12864DispPrintfFlash(3, 1, "1.����");
    TaskAdd(KEY_RET, DispCommonOpt, NULL);
    TaskAdd(KEY_OK, DispOK, DispCommonOpt);
}

/***************************************************************************
**	�������ƣ�DispMenu_2(void)
**	�������ܣ��˵�����2
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispMenu_2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.ͨ����Ϣ����");
    FYD12864DispPrintfFlash(2, 1, "2.���Ʋ�������");
    FYD12864DispPrintfFlash(3, 1, "3.ͨѶ��������");
    FYD12864DispPrintfFlash(4, 1, "4.���ò�������");


    TaskAdd(KEY_1, DispGeneralInfo, NULL);
    TaskAdd(KEY_2, DispSetControl1, NULL);
    TaskAdd(KEY_3, DispCommPara, NULL);
    TaskAdd(KEY_4, DispCommonOpt, NULL);
    TaskAdd(KEY_UP, DispMenu_1, NULL);
    TaskAdd(KEY_DOWN, DispMenu_3, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**	�������ƣ�DispMenu_3(void)
**	�������ܣ��˵�����3
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispMenu_3(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.�汾�������");
    FYD12864DispPrintfFlash(2, 1, "2.������Ϣ����");
    FYD12864DispPrintfFlash(3, 1, "3.ģ���������");
    FYD12864DispPrintfFlash(4, 1, "4.ѭ����������");

    TaskAdd(KEY_1, DispVerNum1, NULL);
    TaskAdd(KEY_2, DispOthMsg1, NULL);
    TaskAdd(KEY_3, SimulatorSet, NULL);
    TaskAdd(KEY_4, CirculateFuelingSet, NULL);
    TaskAdd(KEY_UP, DispMenu_2, NULL);
    //TaskAdd(KEY_DOWN, DispMenu_1,NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}


/***************************************************************************
**	�������ƣ�DispSetPriceVer(void)
**	�������ܣ����۰汾������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetPriceVer(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.pricever = atol((char *)ScreenInput.array);
    globalvar.paraindex = 66;
    DispSet(0, sysparas.pricever, "���۰汾������");
    TaskAdd(KEY_RET, DispVerNum1, NULL);
    TaskAdd(KEY_OK, DispOK, DispVerNum1);
}

/***************************************************************************
**	�������ƣ�DispSetCommInfoVer(void)
**	�������ܣ�ͨ����Ϣ�汾������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetCommInfoVer(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.comminfover = atol((char *)ScreenInput.array);
    globalvar.paraindex = 68;
    DispSet(0, sysparas.comminfover, "ͨ����Ϣ�汾��");
    TaskAdd(KEY_RET, DispVerNum1, NULL);
    TaskAdd(KEY_OK, DispOK, DispVerNum1);
}

/***************************************************************************
**	�������ƣ�DispSetLockVer(void)
**	�������ܣ���Կ��Ϣ�汾������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetLockVer(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 3;
    tmp_sysparas.lockver = atol((char *)ScreenInput.array);
    globalvar.paraindex = 69;
    DispSet(0, sysparas.lockver, "��Կ��Ϣ�汾��");
    TaskAdd(KEY_RET, DispVerNum1, NULL);
    TaskAdd(KEY_OK, DispOK, DispVerNum1);
}


/***************************************************************************
**	�������ƣ�DispVerNum1(void)
**	�������ܣ��汾�������1
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispVerNum1(void)
{
    InputInit();

    FYD12864DispPrintfFlash(1, 1, "1.���۰汾��");
    FYD12864DispPrintfFlash(2, 1, "2.ͨ����Ϣ�汾��");
    FYD12864DispPrintfFlash(3, 1, "3.��Կ��Ϣ�汾��");
    FYD12864ClearLine(4);
    //FYD12864DispString( 4, 1, "2.�������汾��", 1 );

    TaskAdd(KEY_1, DispSetPriceVer, NULL);
    TaskAdd(KEY_2, DispSetCommInfoVer, NULL);
    TaskAdd(KEY_3, DispSetLockVer, NULL);
    //TaskAdd( KEY_4, DispSetBlackVer, NULL );
    TaskAdd(KEY_RET, DispMenu_3, NULL);
}

/***************************************************************************
**	�������ƣ�DispJudgePwd(void)
**	�������ܣ����������ж�
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispJudgePwd(void)
{
    InputInit();//Ϊ������������׼�� ADD BY LY

    if(globalvar.U8Temp == 1)  //1.���ò˵����� ADD BY LY
    {
        if(globalvar.U32Temp != sysparas.menupwd)
        {
            FYD12864DispPrintfFlash(4, 1, "�������");
            TaskAdd(KEY_CLR, DispInputOldPwd, NULL);
            TaskAdd(KEY_RET, DispInputOldPwd, NULL);
        }
        else
        {
            DispUI = DispSetMenuPwd;
        }
    }
    else if(globalvar.U8Temp == 2)  //2.�����ʼ����� ADD BY LY
    {
        if(globalvar.U32Temp != sysparas.qcpwd)
        {
            FYD12864DispPrintfFlash(4, 1, "�������");
            TaskAdd(KEY_CLR, DispInputOldPwd, NULL);
            TaskAdd(KEY_RET, DispInputOldPwd, NULL);
        }
        else
        {
            DispUI = DispSetQcPwd;
        }
    }
    else if(globalvar.U8Temp == 3)  //3.����ά������ ADD BY LY
    {
        if(globalvar.U32Temp != sysparas.testpwd)
        {
            FYD12864DispPrintfFlash(4, 1, "�������");
            TaskAdd(KEY_CLR, DispInputOldPwd, NULL);
            TaskAdd(KEY_RET, DispInputOldPwd, NULL);
        }
        else
        {
            DispUI = DispSetTestPwd;
        }
    }
    else if(globalvar.U8Temp == 4)  //4.���ûָ��������� ADD BY LY
    {
        if(globalvar.U32Temp != sysparas.resetpwd)
        {
            FYD12864DispPrintfFlash(4, 1, "�������");
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
**	�������ƣ�DispInputOldPwd(void)
**	�������ܣ�����������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispInputOldPwd(void)
{
    char     buf[7] = {0}, i;

    ScreenInput.inputmaxlen = 6;
    NumKeyHandler();
    FYD12864ClearLine(1);
    FYD12864DispPrintfFlash(2, 1, "�����������");
    FYD12864ClearLine(4);

    for(i = 0; i < ScreenInput.inputlen; i++)
        buf[i] = '*';

    FYD12864DispPrintf(3, 1, buf);
    globalvar.U32Temp = atol((char *)ScreenInput.array);    //����ľ����� ADD BY LY
    TaskAdd(KEY_RET, DispOthMsg1, NULL);
    TaskAdd(KEY_OK, DispJudgePwd, NULL);
}

//�ж����������Ƿ�һ��
void JudgePwdTwice(void)
{
    uint32_t pwd;
    pwd = atol((char *)ScreenInput.array);

    if(pwd != globalvar.afterdex)//��һ����������ʱ���Ѿ���ֵ
    {
        FYD12864DispPrintfFlash(3, 1, "�������벻һ��");
        TaskMessage(10, DispOthMsg1);
    }
    else
    {
        globalvar.displayonce = 1;
        DispUI = DispOK;
    }
}

//�ٴ�����������
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
    FYD12864DispPrintfFlash(1, 1, "�ٴ�����������:");

    for(i = 0; i < ScreenInput.inputlen; i++)
    {
        tmpbuf[i] = '*';
    }

    FYD12864DispPrintfFlash(2, 1, "%s", tmpbuf);
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0) //��ֱֹ�Ӱ���ȷ�ϼ�
    {
        TaskAdd(KEY_OK, JudgePwdTwice, DispOthMsg1);
    }
}


/***************************************************************************
**	�������ƣ�DispSetMenuPwd(void)
**	�������ܣ��˵���������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetMenuPwd(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    tmp_sysparas.menupwd = atol((char *)ScreenInput.array);
    globalvar.paraindex = 70;
    DispSet(6, sysparas.menupwd, "�˵���������:");
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0)   //��ֱֹ�Ӱ���ȷ�ϼ�
    {
        TaskAdd(KEY_OK, InputNewPwdAgain, NULL);
    }
}

/***************************************************************************
**	�������ƣ�DispSetQcPwd(void)
**	�������ܣ��ʼ���������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetQcPwd(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    tmp_sysparas.qcpwd = atol((char *)ScreenInput.array);
    globalvar.paraindex = 71;
    DispSet(6, sysparas.qcpwd, "�ʼ���������:");
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0)   //��ֱֹ�Ӱ���ȷ�ϼ�
    {
        TaskAdd(KEY_OK, InputNewPwdAgain, NULL);
    }
}

/***************************************************************************
**	�������ƣ�DispSetTestPwd(void)
**	�������ܣ�ά����������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetTestPwd(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    tmp_sysparas.testpwd = atol((char *)ScreenInput.array);
    globalvar.paraindex = 72;
    DispSet(6, sysparas.testpwd, "ά����������:");
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0)   //��ֱֹ�Ӱ���ȷ�ϼ�
    {
        TaskAdd(KEY_OK, InputNewPwdAgain, NULL);
    }
}

/***************************************************************************
**	�������ƣ�DispSetResetPwd(void)
**	�������ܣ��ָ�������������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetResetPwd(void)
{
    tmp_sysparas = sysparas;
    ScreenInput.inputmaxlen = 6;
    tmp_sysparas.resetpwd = atol((char *)ScreenInput.array);
    globalvar.paraindex = 73;
    DispSet(6, sysparas.resetpwd, "�ָ�������������");
    TaskAdd(KEY_RET, DispOthMsg1, NULL);

    if(ScreenInput.inputlen > 0)   //��ֱֹ�Ӱ���ȷ�ϼ�
    {
        TaskAdd(KEY_OK, InputNewPwdAgain, NULL);
    }
}

/***************************************************************************
**	�������ƣ�DispOthMsg1(void)
**	�������ܣ�������Ϣ���ý���1
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispOthMsg1(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.�˵���������");
    FYD12864DispPrintfFlash(2, 1, "2.�ʼ���������");
    FYD12864DispPrintfFlash(3, 1, "3.ά����������");
    FYD12864DispPrintfFlash(4, 1, "4.�ָ���������");

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
**	�������ƣ�DispOthMsg2(void)
**	�������ܣ�������Ϣ���ý���2
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispOthMsg2(void)
{
    
    InputInit();
	
    FYD12864DispPrintfFlash(1,1,"1.���ܰ�����");
    FYD12864ClearLine(2);
    FYD12864ClearLine(3);
    FYD12864ClearLine(4);
    TaskAdd(KEY_1, DispSetPwdBoard,NULL);
    TaskAdd(KEY_UP,DispOthMsg1 ,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}

/***************************************************************************
**	�������ƣ�SetPwdBoard(void)
**	�������ܣ����ü��ܰ幦����ɣ���ʾ���ý�������ü��ܰ幦��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void SetPwdBoard(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        sysparas = tmp_sysparas;

        if(tmp_sysparas.usepwdboard == 0)
        {
        	//�����޸ĺ��sysparas.usepwdboard ADD BY LY
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
        	//��ȡ�������ݣ��������ⲿEEPROM�����ܰ�EERPOM�� ADD BY LY
            LockDataStore();
        }
    }

    if(DispNextUI != NULL)
    {
        TaskMessage(10, DispNextUI);
    }
}

/***************************************************************************
**	�������ƣ�DispSetPwdBoard(void)
**	�������ܣ��Ƿ�ʹ�ü��ܰ幦��ѡ��
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispSetPwdBoard(void)
{
    uint8_t tmp;

    if(sysparas.usepwdboard == 1 && SI_INDATE != 0)     //ʹ�ü��ܰ嵫��������ģʽ
    {
        FYD12864ClearLine(1);
        FYD12864DispPrintfFlash(2, 1, "����������ģʽ");		
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
            FYD12864DispPrintfFlash(4, 1, "�������");
            NumKeyHandler();
            return;
        }

        DispSet(5, sysparas.usepwdboard, "���ܰ�����");
        FYD12864DispPrintfFlash(2, 1, "0.��ʹ�ü��ܰ�");
        FYD12864DispPrintfFlash(3, 1, "1.ʹ�ü��ܰ�");
        TaskAdd(KEY_RET, DispOthMsg2, NULL);
        TaskAdd(KEY_OK, SetPwdBoard, DispOthMsg2);
    }
}

/***************************************************************************
**	�������ƣ�DispSetDensty(void)
**	�������ܣ��ܶ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�ܶȳ���");
        NumKeyHandler();
        return;
    }

    DispSet(4, sysparas.densty, "�ܶ�����");
    TaskAdd(KEY_RET, DispQcPara, NULL);
}

/***************************************************************************
**	�������ƣ�DispSetCoeff(void)
**	�������ܣ�����ϵ������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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

        FYD12864DispPrintfFlash(4, 1, "����ϵ������");
        NumKeyHandler();
        return;
    }

    globalvar.paraindex = 31;
    DispSet(4, sysparas.coeff, "����ϵ������");
    TaskAdd(KEY_RET, DispQcPara, NULL);
    TaskAdd(KEY_OK, DispOK, DispQcPara);
}

/***************************************************************************
**	�������ƣ�DispQcPara(void)
**	�������ܣ��ʼ��������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispQcPara(void)
{
	//ʹ�ü��ܰ嵫��������ģʽ
    if(sysparas.usepwdboard == 1 && SI_INDATE != 0)     
    {
        FYD12864ClearLine(1);
        FYD12864ClearLine(3);
        FYD12864ClearLine(4);
        FYD12864DispPrintfFlash(2, 1, "������������ģʽ");
        TaskAdd(KEY_RET, DispFirst, NULL);
    }
	 //��ʹ�ü��ܰ��ʹ�ü��ܰ���������ȷ
    else if(sysparas.usepwdboard == 0 || (sysparas.usepwdboard == 1 && LockDataCompare()))
    {
        InputInit();
        FYD12864DispPrintfFlash(1, 1, "1.�ܶ�����");
        FYD12864DispPrintfFlash(2, 1, "2.����ϵ������");
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
        FYD12864DispPrintfFlash(2, 1, "���ܰ岻ƥ�䣡��");
        TaskAdd(KEY_RET, DispFirst, NULL);
    }
}

/***************************************************************************
**	�������ƣ�EraseFlash(void)
**	�������ܣ�����flash��ȫƬ����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void EraseFlash(void)
{
    if(globalvar.displayonce == 1)
    {
        globalvar.displayonce = 0;
        //���flashȫƬ��������
        SetSuccess();
    }

    TaskMessage(10, DispEraseChip);
}

/***************************************************************************
**	�������ƣ�DispEraseFlash(void)
**	�������ܣ�����flashѡ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispEraseFlash(void)
{
    FYD12864DispPrintfFlash(1, 1, "ȷ�ϲ���Flash");
    FYD12864DispPrintfFlash(2, 1, "��ȷ�ϡ�  ����");
    FYD12864DispPrintfFlash(3, 1, "�����ء�  ȡ��");
    FYD12864ClearLine(4);

    TaskAdd(KEY_OK, EraseFlash, NULL);
    TaskAdd(KEY_RET, DispUnitQcReset, NULL);
}

/***************************************************************************
**	�������ƣ�CirculateFuelingSet(void)
**	�������ܣ�ѭ����������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, globalvar.circulatefue, "ѭ����������");
    FYD12864DispPrintfFlash(2, 1, "0.��������");
    FYD12864DispPrintfFlash(3, 1, "1.ѭ������");
    TaskAdd(KEY_RET, DispMenu_3, NULL);
}

/***************************************************************************
**	�������ƣ�DispVerNum2(void)
**	�������ܣ��˵�����2
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void DispVerNum2(void)
{
    InputInit();
    FYD12864DispPrintfFlash(1, 1, "1.�汾�������");
    FYD12864DispPrintfFlash(2, 1, "2.������Ϣ����");
    FYD12864DispPrintfFlash(3, 1, "3.ģ���������");
    FYD12864DispPrintfFlash(4, 1, "4.ѭ����������");

    TaskAdd(KEY_1, DispVerNum1, NULL);
    TaskAdd(KEY_2, DispOthMsg1, NULL);
    TaskAdd(KEY_3, SimulatorSet, NULL);
    TaskAdd(KEY_4, CirculateFuelingSet, NULL);
    TaskAdd(KEY_UP, DispVerNum1, NULL);
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**	�������ƣ�Diswork(void)
**	�������ܣ�1��󷵻�������
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
***************************************************************************/
void Diswork(void)
{
    TaskMessage(10, DispFirst);
}

/***************************************************************************
**	�������ƣ�SimulatorSet(void)
**	�������ܣ�����ģ�����
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2018-9-17
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
        FYD12864DispPrintfFlash(4, 1, "�������");
        NumKeyHandler();
        return;
    }

    DispSet(5, sysparas.simulation, "ģ���������");
    FYD12864DispPrintfFlash(2, 1, "0.��������");
    FYD12864DispPrintfFlash(3, 1, "1.ģ�����");
    TaskAdd(KEY_RET, DispMenu_3, NULL);
    TaskAdd(KEY_OK, DispOK, DispMenu_3);
}
