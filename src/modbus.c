#include "kernel.h"


MassFlowModbus  modbus;
RealTimeData    realtime;

#if 0
uint16_t _MODBUS_G_AUTO_RM_BUF[] = {0/*����*/, 2/*����*/, 1/*�¶�*/, 3/*�ܶ�*/, 9/*ѹ��*/};
uint16_t _MODBUS_L_AUTO_RM_BUF[] = {0/*����*/, 2/*����*/, 1/*�¶�*/, 3/*�ܶ�*/, 47/*����*/};

//9503Ϊ����Ĵ�����ֱ�Ӷ��ķ�ΧΪ0-100��ͨ��ɨ��Ĵ�����ȡ��ΧΪ:0-0.1
uint16_t _MODBUS_G_AUTO_EH_BUF[] = {2007/*����*/, 2610/*����*/, 2017/*�¶�*/, 2013/*�ܶ�*/, 2089/*ѹ��*/};
uint16_t _MODBUS_L_AUTO_EH_BUF[] = {2007/*����*/, 2610/*����*/, 2017/*�¶�*/, 2013/*�ܶ�*/, 9503/*����*/};
#endif

/***************************************************************************
**	�������ƣ�ModbusCrc16(uint8_t *buf, uint16_t Len)
**	�������ܣ�������CRC16����
**	���������buf:��Ҫ����CRC������
			  len:��Ҫ��������ݳ���
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint16_t ModbusCrc16(uint8_t *buf, uint16_t Len)
{
    uint16_t IX, IY, CRC;
    CRC = 0xFFFF;

    if(Len <= 0)    return 0;

    for(IX = 0; IX < Len; IX++)
    {
        CRC = CRC ^ (uint16_t)(buf[IX]);

        for(IY = 0; IY < 8; IY++)
        {
            if((CRC & 1) != 0)
                CRC = (CRC >> 1) ^ 0xA001;
            else
                CRC = CRC >> 1;
        }
    }

    return CRC;
}

/***************************************************************************
**	�������ƣ�ModbusReadScanReg_L(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
**	�������ܣ���ȡɨ��Ĵ�����ֵ
**	���������modbus_addr:Һ���������豸��ַ
			  RegAddr:�Ĵ�����ַ
			  regnum:�Ĵ�������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadScanReg_L(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount = 0;
    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //���Ĵ����Ĺ��ܴ���
    modbus.sendbuf[2] = RegAddr >> 8;     //�Ĵ�����λ
    modbus.sendbuf[3] = RegAddr & 0xFF;   //�Ĵ�����λ
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16//����CRCУ��
    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //У��ֵ��λ
    modbus.sendbuf[7] = crc >> 8;

    OpenUart1RcvIRQ();//��uart1 ���������ж�
    Uart1SendStr(&modbus.sendbuf[0], 8);   //ͨ��uart1����ɨ��Ĵ���ֵ����͸�������

    while(timecount < MODBUS_TIMEOUT)
    {
        timecount++;

        if(modbus.flag == 1)
        {
            modbus.flag = 0;
            ModbusDataDisposs_L();
            return true;
        }
    }

    CloseUart1RcvIRQ();
    return false;

}

/***************************************************************************
**	�������ƣ�ModbusReadReg_L(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
**	�������ܣ���ȡָ���Ĵ�����ֵ
**	���������modbus_addr:Һ���������豸��ַ
			  RegAddr:�Ĵ�����ַ
			  regnum:�Ĵ�������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadReg_L(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //���Ĵ����Ĺ��ܴ���
    modbus.sendbuf[2] = RegAddr >> 8;     //�Ĵ�����λ
    modbus.sendbuf[3] = RegAddr & 0xFF;   //�Ĵ�����λ
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16//����CRCУ��
    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //У��ֵ��λ
    modbus.sendbuf[7] = crc >> 8;

    //OpenUartRcvIRQ(ModbusPort);
    OpenUart1RcvIRQ();//��uart1 ���������ж�
    Uart1SendStr(&modbus.sendbuf[0], 8);   //���Ͷ�ָ���Ĵ���ֵ����

    while(timecount < MODBUS_TIMEOUT)
    {
        timecount++;

        if(modbus.flag == 1)
        {
            modbus.flag = 0;
            globalvar.modbus_g_error = 0;
            return true;
        }
    }

    //CloseUartRcvIRQ(ModbusPort);
    CloseUart1RcvIRQ();//��uart1 ���������ж�
    return false;
}

/***************************************************************************
**	�������ƣ�ModbusWriteReg_L(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
**	�������ܣ���ָ����Һ�������ƼĴ�����д������
**	���������modbus_addr:Һ���������豸��ַ
			  modbus_coil:д�Ĵ���������
			  RegAddr:�Ĵ�����ַ
			  v_value:��Ҫд��Ĵ�����ֵ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusWriteReg_L(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
{
    uint32_t    timecount = 0;
    uint16_t    crc = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = modbus_coil;
    modbus.sendbuf[2] = RegAddr >> 8;       //�Ĵ�����λ
    modbus.sendbuf[3] = RegAddr & 0xFF;     //�Ĵ�����λ
    modbus.sendbuf[4] = v_value >> 8;       //ֵ��λ
    modbus.sendbuf[5] = v_value & 0x00FF;   //ֵ��λ

    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16У��

    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //У��ֵ��λ
    modbus.sendbuf[7] = crc >> 8;

    //OpenUartRcvIRQ(ModbusPort);
    OpenUart1RcvIRQ();//��uart1 ���������ж�
    Uart1SendStr(&modbus.sendbuf[0], 8);

    while(timecount < MODBUS_TIMEOUT)
    {
        timecount++;

        if(modbus.flag == 1)
        {
            modbus.flag = 0;
            return true;
        }
    }

    CloseUart1RcvIRQ();//��uart1 �����ж�
    return false;
}

/***************************************************************************
**	�������ƣ�ModbusReadDeviceAddress_L(uint8_t modbus_addr, uint16_t *rdaddress)
**	�������ܣ���ȡҺ���������豸��ַ������MODBUS����
**	���������modbus_addr:Һ���������豸��ַ
			  rdaddress: �����Ʒ��ص��豸��ַ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadDeviceAddress_L(uint8_t modbus_addr, uint16_t *rdaddress)
{
    uint8_t readresult;
    UnionU16 rdInt16reg;

    rdInt16reg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)   //��˹����������
    {
        readresult = ModbusReadReg_L(modbus_addr, 312, 1);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)   //E+H������
    {
        readresult = ModbusReadReg_L(modbus_addr, 4909, 1);
    }


    if(readresult != MODBUS_OK)
    {
        return 0;
    }

    rdInt16reg.rdcv[0] = modbus.recbuf[4];
    rdInt16reg.rdcv[1] = modbus.recbuf[3];

    *rdaddress = rdInt16reg.data;

    return MODBUS_OK ;
}

/***************************************************************************
**	�������ƣ�ModbusWriteDeviceAddress_L(uint8_t modbus_addr, uint16_t rdaddress)
**	�������ܣ�дҺ���������豸��ַ
**	���������modbus_addr:Һ���������豸��ַ
			  rdaddress:��д��������Ƶ����豸��ַ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusWriteDeviceAddress_L(uint8_t modbus_addr, uint16_t rdaddress)
{
    uint8_t readresult;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult = ModbusWriteReg_L(modbus_addr, 6, 312, rdaddress);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult = ModbusWriteReg_L(modbus_addr, 6, 4909, rdaddress);
    }

    if(readresult != MODBUS_OK)
    {
        return false;
    }

    return MODBUS_OK ;
}

/***************************************************************************
**	�������ƣ�ModbusReadTotal_L(uint8_t modbus_addr, float *total)
**	�������ܣ���ȡҺ�������Ƽ�������
**	���������modbus_addr:Һ���������豸��ַ
			  total:�����Ʒ��ص�����ֵ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadTotal_L(uint8_t modbus_addr, float *total)
{
    uint8_t readresult;
    UnionFloat rdFreg;
    rdFreg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult = ModbusReadReg_L(modbus_addr, 258, 2);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult = ModbusReadReg_L(modbus_addr, 2609, 2);
    }

    if(readresult != MODBUS_OK)
    {
        return false;
    }

    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    rdFreg.data = rdFreg.data + 0.005;
    *total = rdFreg.data;
    return MODBUS_OK ;
}

/***************************************************************************
**	�������ƣ�ModbusReadFlow_L(uint8_t modbus_addr, float *flow)
**	�������ܣ���ȡҺ������������
**	���������modbus_addr:Һ���������豸��ַ
			  flow:�����Ʒ��ص�����ֵ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadFlow_L(uint8_t modbus_addr, float *flow)
{
    uint8_t readresult;
    UnionFloat rdFreg;
    rdFreg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult = ModbusReadReg_L(modbus_addr, 246, 2);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult = ModbusReadReg_L(modbus_addr, 2006, 2);
    }

    if(readresult != MODBUS_OK)
    {
        return false;
    }

    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    rdFreg.data = rdFreg.data + 0.005;
    *flow = rdFreg.data;
    return MODBUS_OK ;
}

/***************************************************************************
**	�������ƣ�ModbusClearTotal_L(uint8_t modbus_addr)
**	�������ܣ���λҺ�������������Ĵ���
**	���������modbus_addr:Һ���������豸��ַ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusClearTotal_L(uint8_t modbus_addr)
{
    uint8_t rdresult = false;
    float   cngsum;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        rdresult = ModbusWriteReg_L(modbus_addr, 5, 55, 0xFF00);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        rdresult = ModbusWriteReg_L(modbus_addr, 6, 2607, 1);
    }

    if(rdresult)
    {
        rdresult = false;
        rdresult = ModbusReadTotal_L(modbus_addr, &cngsum);

        if(cngsum < 0.01)
        {
            return true;
        }
    }

    return false;
}

/***************************************************************************
**	�������ƣ� ModbusInitReg_L(void)
**	�������ܣ�Һ��������scan buffer�Ĵ�����ʼ��
**	�����������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusInitReg_L(void)
{
    uint8_t n = 0;
//    uint16_t AUTOSCAN_RM_BUF[] = {246/*����*/, 258/*����*/, 250/*�¶�*/, 248/*�ܶ�*/, 290/*����*/}; //��˹����ɨ��Ĵ���
    uint16_t AUTOSCAN_RM_BUF[] = {0/*����*/, 2/*����*/, 1/*�¶�*/, 3/*�ܶ�*/, 47/*����*/}; //��˹����ɨ��Ĵ���
    uint16_t AUTOSCAN_EH_BUF[] = {2007/*����*/, 2610/*����*/, 2017/*�¶�*/, 2013/*�ܶ�*/, 9503/*����*/}; //E+Hɨ��Ĵ���

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        for(n = 0; n < _MODBUS_L_AUTO_BUF_LEN; n++)
        {
            //WATCH_DOG();
            if(!ModbusWriteReg_L(_MODBUS_L_, 6, 750 + n, AUTOSCAN_RM_BUF[n]))
            {
                if(sysparas.simulation == 0)
                {
                    syserr |= 1 << SYSERR_MODBUS_L_AUTO_ERR; //Һ��������ͨѶ����ϵͳ����0x08��
                }

                return false;
            }

            _delay_ms(200);
        }
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        for(n = 0; n < _MODBUS_L_AUTO_BUF_LEN; n++)
        {
            //WATCH_DOG();
            if(!ModbusWriteReg_L(_MODBUS_L_, 6, 5000 + n, AUTOSCAN_EH_BUF[n]))
            {
                if(sysparas.simulation == 0)
                {
                    syserr |= 1 << SYSERR_MODBUS_L_AUTO_ERR; //Һ��������ͨѶ����ϵͳ����0x08��
                }

                return false;
            }

            _delay_ms(200);
        }
    }
    else
    {
        return false;
    }

    return true;
}

/***************************************************************************
**	�������ƣ� ModbusDataDisposs_L(void)
**	�������ܣ�Һ��������scan buffer���ݴ���
**	�����������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
void ModbusDataDisposs_L(void)
{
    UnionFloat rdFreg;
    rdFreg.data = 0;
    /*��������*/
    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    realtime.cur_flowrate_l = rdFreg.data;

    /*��������*/
    rdFreg.rdcv[0] =  modbus.recbuf[8];
    rdFreg.rdcv[1] =  modbus.recbuf[7];
    rdFreg.rdcv[2] =  modbus.recbuf[10];
    rdFreg.rdcv[3] =  modbus.recbuf[9];

    realtime.cur_totalize_l = rdFreg.data;

    /*�����¶�*/
    rdFreg.rdcv[0] =  modbus.recbuf[12];
    rdFreg.rdcv[1] =  modbus.recbuf[11];
    rdFreg.rdcv[2] =  modbus.recbuf[14];
    rdFreg.rdcv[3] =  modbus.recbuf[13];

    realtime.cur_temperature_l = rdFreg.data;

    /*�����ܶ�*/
    rdFreg.rdcv[0] =  modbus.recbuf[16];
    rdFreg.rdcv[1] =  modbus.recbuf[15];
    rdFreg.rdcv[2] =  modbus.recbuf[18];
    rdFreg.rdcv[3] =  modbus.recbuf[17];

    realtime.cur_density_l = rdFreg.data;

    /*��������*/
    rdFreg.rdcv[0] =  modbus.recbuf[20];
    rdFreg.rdcv[1] =  modbus.recbuf[19];
    rdFreg.rdcv[2] =  modbus.recbuf[22];
    rdFreg.rdcv[3] =  modbus.recbuf[21];

    if(rdFreg.data <= 0.1)
    {
        rdFreg.data = rdFreg.data * 1000;
    }
    else if(rdFreg.data > 99.9)   //����������Ϊ100.45��������ʾ����ʾ�ĸ�������
    {
        rdFreg.data = 99.9;
    }

    realtime.cur_gain_l = rdFreg.data;
    globalvar.modbus_l_error = 0;
    ClrBit(syserr, SYSERR_MODBUS_L_AUTO_ERR);   //���Һ��������ͨѶ����
}

/***************************************************************************
**	�������ƣ�ModbusReadScanBuffer_L(uint8_t modbus_addr)
**	�������ܣ���ȡҺ��������ɨ��Ĵ����е�ֵ
**	�����������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
void ModbusReadScanBuffer_L(uint8_t modbus_addr)
{
    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        ModbusReadScanReg_L(modbus_addr, 782, 10);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        ModbusReadScanReg_L(modbus_addr, 5050, 10);
    }
}

/***************************************************************************
**	�������ƣ�ModbusComTest_L(uint8_t modbus_addr)
**	�������ܣ���ȡ�����Ƶ��豸��ַ������������ͨѶ����
**	���������modbus_addr��Һ���������豸��ַ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusComTest_L(uint8_t modbus_addr)
{
    uint16_t deviceaddr;

    if(ModbusReadDeviceAddress_L(modbus_addr, &deviceaddr))
    {
        if(deviceaddr == modbus_addr)
        {
            return true;
        }
    }

    return false;
}

/***************************************************************************
**	�������ƣ�ModbusRecDataCheck_L(void)
**	�������ܣ�Һ�������ƽ������ݴ���
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
void ModbusRecDataCheck_L(void)
{
    uint32_t    len, i;
    uint16_t    CRC;

    //���ݴ���
    if(((uart1.recbuf[0] == _MODBUS_G_) || (uart1.recbuf[0] == _MODBUS_L_)) &&
            ((uart1.recbuf[1] == 0x03) || (uart1.recbuf[1] == 0x06) || (uart1.recbuf[1] == 0x05)))          //������Ч���ж�
    {
        if((uart1.recbuf[1] == 0x06) || ((uart1.recbuf[1] == 0x05)))         //д�Ĵ���
        {
            len = 8;
        }
        else
        {
            len = uart1.recbuf[2] + 5; //���ݳ���
        }

        if(len < 40)
        {
            if(uart1.reclen >= len)
            {
                CloseUart1RcvIRQ();
                CRC = ModbusCrc16((uint8_t *)uart1.recbuf, len - 2);

                if((uart1.recbuf[len - 2] == ((uint8_t)(CRC & 0x00FF))) &&
                        (uart1.recbuf[len - 1] == ((uint8_t)(CRC >> 8))))
                {
                    for(i = 0; i < len; i++)
                    {
                        modbus.recbuf[i] = uart1.recbuf[i] ;
                    }

                    modbus.flag = 1;//���յ�����ͨ��CRC��֤�����ý���������ȷ��־  BY LY
                }

                uart1.reclen = 0;
            }
        }
        else
        {
            OpenUart1RcvIRQ();
            uart1.reclen = 0;
        }
    }
    else
    {
        OpenUart1RcvIRQ();
        uart1.reclen = 0;
    }
}

/*****************************************����Ϊ���������Ƴ���*******************************************/
/***************************************************************************
**	�������ƣ�ModbusReadScanReg_G(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
**	�������ܣ���ȡ����������ɨ��Ĵ�����ֵ
**	���������modbus_addr:�����������豸��ַ
			  RegAddr :  �Ĵ�����ַ
			  regnum  : �Ĵ�������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadScanReg_G(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //���Ĵ����Ĺ��ܴ���
    modbus.sendbuf[2] = RegAddr >> 8;     //�Ĵ�����λ
    modbus.sendbuf[3] = RegAddr & 0xFF;   //�Ĵ�����λ
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16//����CRCУ��
    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //У��ֵ��λ
    modbus.sendbuf[7] = crc >> 8;

    OpenUart3RcvIRQ();//��uart3 �����ж�
    Uart3SendStr(&modbus.sendbuf[0], 8);

    while(timecount < MODBUS_TIMEOUT)
    {
        timecount++;

        if(modbus.flag == 1)
        {
            modbus.flag = 0;
            ModbusDataDisposs_G();
            return true;
        }
    }

    CloseUart3RcvIRQ();
    return false;
}

/***************************************************************************
**	�������ƣ�ModbusReadReg_G(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
**	�������ܣ���ȡ����������ָ���Ĵ�����ֵ
**	���������modbus_addr:�����������豸��ַ
			  RegAddr :  �Ĵ�����ַ
			  regnum  : �Ĵ�������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadReg_G(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //���Ĵ����Ĺ��ܴ���
    modbus.sendbuf[2] = RegAddr >> 8;     //�Ĵ�����λ
    modbus.sendbuf[3] = RegAddr & 0xFF;   //�Ĵ�����λ
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16//����CRCУ��
    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //У��ֵ��λ
    modbus.sendbuf[7] = crc >> 8;

    OpenUart3RcvIRQ();//��uart3 �����ж�
    Uart3SendStr(&modbus.sendbuf[0], 8);

    while(timecount < MODBUS_TIMEOUT)
    {
        timecount++;

        if(modbus.flag == 1)
        {
            modbus.flag = 0;
            globalvar.modbus_g_error = 0;
            return true;
        }
    }

    CloseUart3RcvIRQ();
    return false;
}

/***************************************************************************
**	�������ƣ�ModbusWriteReg_G(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
**	�������ܣ�������������ָ���Ĵ���д������
**	���������modbus_addr:�����������豸��ַ
			  modbus_coil:д�Ĵ���������
			  RegAddr:�Ĵ�����ַ
			  v_value:��Ҫд��Ĵ�����ֵ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusWriteReg_G(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
{
    uint32_t    timecount = 0;
    uint16_t    crc = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = modbus_coil;
    modbus.sendbuf[2] = RegAddr >> 8;       //�Ĵ�����λ
    modbus.sendbuf[3] = RegAddr & 0xFF;     //�Ĵ�����λ
    modbus.sendbuf[4] = v_value >> 8;       //ֵ��λ
    modbus.sendbuf[5] = v_value & 0x00FF;   //ֵ��λ

    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16У��

    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //У��ֵ��λ
    modbus.sendbuf[7] = crc >> 8;

    OpenUart3RcvIRQ();//��uart3 �����ж�
    Uart3SendStr(&modbus.sendbuf[0], 8);

    while(timecount < MODBUS_TIMEOUT)
    {
        timecount++;

        if(modbus.flag == 1)
        {
            modbus.flag = 0;
            return true;
        }
    }

    CloseUart3RcvIRQ();
    return false;
}

/***************************************************************************
**	�������ƣ�ModbusReadDeviceAddress_G(uint8_t modbus_addr, uint16_t *rdaddress)
**	�������ܣ���ȡ�����������豸��ַ������MODBUS����
**	���������modbus_addr:�����������豸��ַ
			  rdaddress	:���������Ʒ��ص��豸��ַ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadDeviceAddress_G(uint8_t modbus_addr, uint16_t *rdaddress)
{
    uint8_t readresult;
    UnionU16 rdInt16reg;
    rdInt16reg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult = ModbusReadReg_G(modbus_addr, 312, 1);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult = ModbusReadReg_G(modbus_addr, 4909, 1);
    }


    if(readresult != MODBUS_OK)
    {
        return 0;
    }

    rdInt16reg.rdcv[0] = modbus.recbuf[4];
    rdInt16reg.rdcv[1] = modbus.recbuf[3];

    *rdaddress = rdInt16reg.data;

    return MODBUS_OK ;
}

/***************************************************************************
**	�������ƣ�ModbusWriteDeviceAddress(uint8_t modbus_addr, uint16_t rdaddress)
**	�������ܣ���ȡ�����������豸��ַ������MODBUS����
**	���������modbus_addr:�����������豸��ַ
			  rdaddress:��д�����������Ƶ����豸��ַ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusWriteDeviceAddress(uint8_t modbus_addr, uint16_t rdaddress)
{
    uint8_t readresult;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult = ModbusWriteReg_G(modbus_addr, 6, 312, rdaddress);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult = ModbusWriteReg_G(modbus_addr, 6, 4909, rdaddress);
    }

    if(readresult != MODBUS_OK)
    {
        return false;
    }

    return MODBUS_OK ;
}

/***************************************************************************
**	�������ƣ�ModbusReadTotal_G(uint8_t modbus_addr, float *total)
**	�������ܣ���ȡ���������Ƽ�������
**	���������modbus_addr:�����������豸��ַ
			  total:���������Ʒ��ص�����ֵ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadTotal_G(uint8_t modbus_addr, float *total)
{
    uint8_t readresult;
    UnionFloat rdFreg;
    rdFreg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult = ModbusReadReg_G(modbus_addr, 258, 2);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult = ModbusReadReg_G(modbus_addr, 2609, 2);
    }

    if(readresult != MODBUS_OK)
    {
        return false;
    }

    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    rdFreg.data = rdFreg.data + 0.005;
    *total = rdFreg.data;
    return MODBUS_OK ;
}

/***************************************************************************
**	�������ƣ�ModbusReadFlow_G(uint8_t modbus_addr, float *flow)
**	�������ܣ���ȡ��������������
**	���������modbus_addr:�����������豸��ַ
			  flow:�����Ʒ��ص�����ֵ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusReadFlow_G(uint8_t modbus_addr, float *flow)
{
    uint8_t readresult;
    UnionFloat rdFreg;
    rdFreg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult = ModbusReadReg_G(modbus_addr, 246, 2);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult = ModbusReadReg_G(modbus_addr, 2006, 2);
    }

    if(readresult != MODBUS_OK)
    {
        return false;
    }

    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    rdFreg.data = rdFreg.data + 0.005;
    *flow = rdFreg.data;
    return MODBUS_OK ;
}

/***************************************************************************
**	�������ƣ�ModbusClearTotal_G(uint8_t modbus_addr)
**	�������ܣ���λ���������������Ĵ���
**	���������modbus_addr:�����������豸��ַ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusClearTotal_G(uint8_t modbus_addr)
{
    uint8_t rdresult = false;
    float   cngsum;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        rdresult = ModbusWriteReg_G(modbus_addr, 5, 55, 0xFF00);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        rdresult = ModbusWriteReg_G(modbus_addr, 6, 2607, 1);
    }

    if(rdresult)
    {
        rdresult = false;
        rdresult = ModbusReadTotal_G(modbus_addr, &cngsum);

        if(cngsum < 0.01)
        {
            return true;
        }
    }

    return false;
}

/***************************************************************************
**	�������ƣ�ModbusInitReg_G(void)
**	�������ܣ�����������scan buffer�Ĵ�����ʼ��
**	�����������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusInitReg_G(void)
{
    uint8_t n = 0;
//    uint16_t AUTOSCAN_RM_BUF[] = {246/*����*/, 258/*����*/, 250/*�¶�*/, 248/*�ܶ�*/, 290/*����*/}; //��˹����ɨ��Ĵ���
    uint16_t AUTOSCAN_RM_BUF[] = {0/*����*/, 2/*����*/, 1/*�¶�*/, 3/*�ܶ�*/, 47/*����*/}; //��˹����ɨ��Ĵ���
    uint16_t AUTOSCAN_EH_BUF[] = {2007/*����*/, 2610/*����*/, 2017/*�¶�*/, 2013/*�ܶ�*/, 9503/*����*/}; //E+Hɨ��Ĵ���

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        for(n = 0; n < _MODBUS_G_AUTO_BUF_LEN; n++)
        {
            //WATCH_DOG();
            if(!ModbusWriteReg_G(_MODBUS_G_, 6, 750 + n, AUTOSCAN_RM_BUF[n]))
            {
                if(sysparas.simulation == 0)
                {
                    syserr |= 1 << SYSERR_MODBUS_G_AUTO_ERR; //����������ͨѶ����ϵͳ����0x04��
                }

                return false;
            }

            _delay_ms(200);
        }
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        for(n = 0; n < _MODBUS_G_AUTO_BUF_LEN; n++)
        {
            //WATCH_DOG();
            if(!ModbusWriteReg_G(_MODBUS_G_, 6, 5000 + n, AUTOSCAN_EH_BUF[n]))
            {
                if(sysparas.simulation == 0)
                {
                    syserr |= 1 << SYSERR_MODBUS_G_AUTO_ERR; //����������ͨѶ����ϵͳ����0x04��
                }

                return false;
            }

            _delay_ms(200);
        }

    }
    else
    {
        return false;
    }

    return true;
}

/***************************************************************************
**	�������ƣ�ModbusDataDisposs_G(void)
**	�������ܣ�����������scan buffer���ݴ���
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
void ModbusDataDisposs_G(void)
{
    UnionFloat rdFreg;
    rdFreg.data = 0;

    /*��������*/
    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    realtime.cur_flowrate_g = rdFreg.data;

    /*��������*/
    rdFreg.rdcv[0] =  modbus.recbuf[8];
    rdFreg.rdcv[1] =  modbus.recbuf[7];
    rdFreg.rdcv[2] =  modbus.recbuf[10];
    rdFreg.rdcv[3] =  modbus.recbuf[9];

    realtime.cur_totalize_g = rdFreg.data;

    /*�����¶�*/
    rdFreg.rdcv[0] =  modbus.recbuf[12];
    rdFreg.rdcv[1] =  modbus.recbuf[11];
    rdFreg.rdcv[2] =  modbus.recbuf[14];
    rdFreg.rdcv[3] =  modbus.recbuf[13];

    realtime.cur_temperature_g = rdFreg.data;

    /*�����ܶ�*/
    rdFreg.rdcv[0] =  modbus.recbuf[16];
    rdFreg.rdcv[1] =  modbus.recbuf[15];
    rdFreg.rdcv[2] =  modbus.recbuf[18];
    rdFreg.rdcv[3] =  modbus.recbuf[17];


    realtime.cur_density_g = rdFreg.data;

    /*��������*/
    rdFreg.rdcv[0] =  modbus.recbuf[20];
    rdFreg.rdcv[1] =  modbus.recbuf[19];
    rdFreg.rdcv[2] =  modbus.recbuf[22];
    rdFreg.rdcv[3] =  modbus.recbuf[21];


    if(rdFreg.data <= 0.1)
    {
        rdFreg.data = rdFreg.data * 1000;
    }
    else if(rdFreg.data > 99.9)   //����������Ϊ100.45��������ʾ����ʾ�ĸ�������
    {
        rdFreg.data = 99.9;
    }

    realtime.cur_gain_g = rdFreg.data;
    globalvar.modbus_g_error = 0; //��������ʱ����һ�ζ�Һ��������
    ClrBit(syserr, SYSERR_MODBUS_G_AUTO_ERR);   //�������������ͨѶ����
}

/***************************************************************************
**	�������ƣ�ModbusReadScanBuffer_G(uint8_t modbus_addr)
**	�������ܣ���ȡ����������ɨ��Ĵ����е�ֵ
**	���������modbus_addr:�����������豸��ַ
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
void ModbusReadScanBuffer_G(uint8_t modbus_addr)
{
    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        ModbusReadScanReg_G(modbus_addr, 782, 10);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        ModbusReadScanReg_G(modbus_addr, 5050, 10);
    }
}

/***************************************************************************
**	�������ƣ�ModbusComTest_G(uint8_t modbus_addr)
**	�������ܣ���ȡ���������Ƶ��豸��ַ������������ͨѶ����
**	���������modbus_addr:�����������豸��ַ
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
uint8_t ModbusComTest_G(uint8_t modbus_addr)
{
    uint16_t deviceaddr;

    if(ModbusReadDeviceAddress_G(modbus_addr, &deviceaddr))
    {
        if(deviceaddr == modbus_addr)
        {
            return true;
        }
    }

    return false;
}

/***************************************************************************
**	�������ƣ�ModbusRecDataCheck_G(void)
**	�������ܣ����������ƽ������ݴ���
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
***************************************************************************/
void ModbusRecDataCheck_G(void)
{
    uint32_t    len, i;
    uint16_t    CRC;

    //���ݴ���
    if(((uart3.recbuf[0] == _MODBUS_G_) || (uart3.recbuf[0] == _MODBUS_L_)) &&
            ((uart3.recbuf[1] == 0x03) || (uart3.recbuf[1] == 0x06) || (uart3.recbuf[1] == 0x05)))          //������Ч���ж�
    {
        if((uart3.recbuf[1] == 0x06) || ((uart3.recbuf[1] == 0x05)))         //д�Ĵ���
        {
            len = 8;
        }
        else
        {
            len = uart3.recbuf[2] + 5; //���ݳ���
        }

        if(len < 40)
        {
            if(uart3.reclen >= len)
            {
                CloseUart3RcvIRQ();
                CRC = ModbusCrc16((uint8_t *)uart3.recbuf, len - 2);

                if((uart3.recbuf[len - 2] == ((uint8_t)(CRC & 0x00FF))) &&
                        (uart3.recbuf[len - 1] == ((uint8_t)(CRC >> 8))))
                {
                    for(i = 0; i < len; i++)
                    {
                        modbus.recbuf[i] = uart3.recbuf[i] ;
                    }

                    modbus.flag = 1;//���յ�����ͨ��CRC��֤�����ý���������ȷ��־  BY LY
                }

                uart3.reclen = 0;
            }
        }
        else
        {
            OpenUart3RcvIRQ();
            uart3.reclen = 0;
        }
    }
    else
    {
        OpenUart3RcvIRQ();
        uart3.reclen = 0;
        memset(uart3.recbuf, 0, BUFSIZE);   //�����������㡣
    }
}
