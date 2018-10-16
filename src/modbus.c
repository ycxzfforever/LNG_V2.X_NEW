#include "kernel.h"


MassFlowModbus  modbus;
RealTimeData    realtime;

#if 0
uint16_t _MODBUS_G_AUTO_RM_BUF[] = {0/*流速*/, 2/*总量*/, 1/*温度*/, 3/*密度*/, 9/*压力*/};
uint16_t _MODBUS_L_AUTO_RM_BUF[] = {0/*流速*/, 2/*总量*/, 1/*温度*/, 3/*密度*/, 47/*增益*/};

//9503为增益寄存器，直接读的范围为0-100，通过扫描寄存器读取则范围为:0-0.1
uint16_t _MODBUS_G_AUTO_EH_BUF[] = {2007/*流速*/, 2610/*总量*/, 2017/*温度*/, 2013/*密度*/, 2089/*压力*/};
uint16_t _MODBUS_L_AUTO_EH_BUF[] = {2007/*流速*/, 2610/*总量*/, 2017/*温度*/, 2013/*密度*/, 9503/*增益*/};
#endif

/***************************************************************************
**	函数名称：ModbusCrc16(uint8_t *buf, uint16_t Len)
**	函数功能：流量计CRC16计算
**	输入参数：buf:需要计算CRC的数据
			  len:需要计算的数据长度
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusReadScanReg_L(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
**	函数功能：读取扫描寄存器的值
**	输入参数：modbus_addr:液相流量计设备地址
			  RegAddr:寄存器地址
			  regnum:寄存器数量
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusReadScanReg_L(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount = 0;
    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //读寄存器的功能代码
    modbus.sendbuf[2] = RegAddr >> 8;     //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;   //寄存器低位
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16//进行CRC校验
    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //校验值低位
    modbus.sendbuf[7] = crc >> 8;

    OpenUart1RcvIRQ();//开uart1 接收数据中断
    Uart1SendStr(&modbus.sendbuf[0], 8);   //通过uart1将读扫描寄存器值命令发送给流量计

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
**	函数名称：ModbusReadReg_L(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
**	函数功能：读取指定寄存器的值
**	输入参数：modbus_addr:液相流量计设备地址
			  RegAddr:寄存器地址
			  regnum:寄存器数量
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusReadReg_L(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //读寄存器的功能代码
    modbus.sendbuf[2] = RegAddr >> 8;     //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;   //寄存器低位
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16//进行CRC校验
    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //校验值低位
    modbus.sendbuf[7] = crc >> 8;

    //OpenUartRcvIRQ(ModbusPort);
    OpenUart1RcvIRQ();//开uart1 接收数据中断
    Uart1SendStr(&modbus.sendbuf[0], 8);   //发送读指定寄存器值命令

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
    CloseUart1RcvIRQ();//关uart1 接收数据中断
    return false;
}

/***************************************************************************
**	函数名称：ModbusWriteReg_L(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
**	函数功能：向指定的液相流量计寄存器中写入数据
**	输入参数：modbus_addr:液相流量计设备地址
			  modbus_coil:写寄存器功能码
			  RegAddr:寄存器地址
			  v_value:需要写入寄存器的值
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusWriteReg_L(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
{
    uint32_t    timecount = 0;
    uint16_t    crc = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = modbus_coil;
    modbus.sendbuf[2] = RegAddr >> 8;       //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;     //寄存器低位
    modbus.sendbuf[4] = v_value >> 8;       //值高位
    modbus.sendbuf[5] = v_value & 0x00FF;   //值低位

    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16校验

    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //校验值低位
    modbus.sendbuf[7] = crc >> 8;

    //OpenUartRcvIRQ(ModbusPort);
    OpenUart1RcvIRQ();//开uart1 接收数据中断
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

    CloseUart1RcvIRQ();//关uart1 接收中断
    return false;
}

/***************************************************************************
**	函数名称：ModbusReadDeviceAddress_L(uint8_t modbus_addr, uint16_t *rdaddress)
**	函数功能：读取液相流量计设备地址，测试MODBUS总线
**	输入参数：modbus_addr:液相流量计设备地址
			  rdaddress: 流量计返回的设备地址
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusReadDeviceAddress_L(uint8_t modbus_addr, uint16_t *rdaddress)
{
    uint8_t readresult;
    UnionU16 rdInt16reg;

    rdInt16reg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)   //罗斯蒙特流量计
    {
        readresult = ModbusReadReg_L(modbus_addr, 312, 1);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)   //E+H流量计
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
**	函数名称：ModbusWriteDeviceAddress_L(uint8_t modbus_addr, uint16_t rdaddress)
**	函数功能：写液相流量计设备地址
**	输入参数：modbus_addr:液相流量计设备地址
			  rdaddress:待写入的流量计的新设备地址
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusReadTotal_L(uint8_t modbus_addr, float *total)
**	函数功能：读取液相流量计加气总量
**	输入参数：modbus_addr:液相流量计设备地址
			  total:流量计返回的总累值
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusReadFlow_L(uint8_t modbus_addr, float *flow)
**	函数功能：读取液相流量计流速
**	输入参数：modbus_addr:液相流量计设备地址
			  flow:流量计返回的流速值
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusClearTotal_L(uint8_t modbus_addr)
**	函数功能：复位液相流量计总量寄存器
**	输入参数：modbus_addr:液相流量计设备地址
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称： ModbusInitReg_L(void)
**	函数功能：液相流量计scan buffer寄存器初始化
**	输入参数：无
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusInitReg_L(void)
{
    uint8_t n = 0;
//    uint16_t AUTOSCAN_RM_BUF[] = {246/*流速*/, 258/*总量*/, 250/*温度*/, 248/*密度*/, 290/*增益*/}; //罗斯蒙特扫描寄存器
    uint16_t AUTOSCAN_RM_BUF[] = {0/*流速*/, 2/*总量*/, 1/*温度*/, 3/*密度*/, 47/*增益*/}; //罗斯蒙特扫描寄存器
    uint16_t AUTOSCAN_EH_BUF[] = {2007/*流速*/, 2610/*总量*/, 2017/*温度*/, 2013/*密度*/, 9503/*增益*/}; //E+H扫描寄存器

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        for(n = 0; n < _MODBUS_L_AUTO_BUF_LEN; n++)
        {
            //WATCH_DOG();
            if(!ModbusWriteReg_L(_MODBUS_L_, 6, 750 + n, AUTOSCAN_RM_BUF[n]))
            {
                if(sysparas.simulation == 0)
                {
                    syserr |= 1 << SYSERR_MODBUS_L_AUTO_ERR; //液相流量计通讯错误。系统错误0x08。
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
                    syserr |= 1 << SYSERR_MODBUS_L_AUTO_ERR; //液相流量计通讯错误。系统错误0x08。
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
**	函数名称： ModbusDataDisposs_L(void)
**	函数功能：液相流量计scan buffer数据处理
**	输入参数：无
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
void ModbusDataDisposs_L(void)
{
    UnionFloat rdFreg;
    rdFreg.data = 0;
    /*计算流速*/
    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    realtime.cur_flowrate_l = rdFreg.data;

    /*计算总量*/
    rdFreg.rdcv[0] =  modbus.recbuf[8];
    rdFreg.rdcv[1] =  modbus.recbuf[7];
    rdFreg.rdcv[2] =  modbus.recbuf[10];
    rdFreg.rdcv[3] =  modbus.recbuf[9];

    realtime.cur_totalize_l = rdFreg.data;

    /*计算温度*/
    rdFreg.rdcv[0] =  modbus.recbuf[12];
    rdFreg.rdcv[1] =  modbus.recbuf[11];
    rdFreg.rdcv[2] =  modbus.recbuf[14];
    rdFreg.rdcv[3] =  modbus.recbuf[13];

    realtime.cur_temperature_l = rdFreg.data;

    /*计算密度*/
    rdFreg.rdcv[0] =  modbus.recbuf[16];
    rdFreg.rdcv[1] =  modbus.recbuf[15];
    rdFreg.rdcv[2] =  modbus.recbuf[18];
    rdFreg.rdcv[3] =  modbus.recbuf[17];

    realtime.cur_density_l = rdFreg.data;

    /*计算增益*/
    rdFreg.rdcv[0] =  modbus.recbuf[20];
    rdFreg.rdcv[1] =  modbus.recbuf[19];
    rdFreg.rdcv[2] =  modbus.recbuf[22];
    rdFreg.rdcv[3] =  modbus.recbuf[21];

    if(rdFreg.data <= 0.1)
    {
        rdFreg.data = rdFreg.data * 1000;
    }
    else if(rdFreg.data > 99.9)   //增益最大可以为100.45，但是显示屏显示的个数有限
    {
        rdFreg.data = 99.9;
    }

    realtime.cur_gain_l = rdFreg.data;
    globalvar.modbus_l_error = 0;
    ClrBit(syserr, SYSERR_MODBUS_L_AUTO_ERR);   //清除液相流量计通讯错误。
}

/***************************************************************************
**	函数名称：ModbusReadScanBuffer_L(uint8_t modbus_addr)
**	函数功能：读取液相流量计扫描寄存器中的值
**	输入参数：无
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusComTest_L(uint8_t modbus_addr)
**	函数功能：读取流量计的设备地址，用于流量计通讯测试
**	输入参数：modbus_addr：液相流量计设备地址
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusRecDataCheck_L(void)
**	函数功能：液相流量计接收数据处理
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
void ModbusRecDataCheck_L(void)
{
    uint32_t    len, i;
    uint16_t    CRC;

    //数据处理
    if(((uart1.recbuf[0] == _MODBUS_G_) || (uart1.recbuf[0] == _MODBUS_L_)) &&
            ((uart1.recbuf[1] == 0x03) || (uart1.recbuf[1] == 0x06) || (uart1.recbuf[1] == 0x05)))          //数据有效性判断
    {
        if((uart1.recbuf[1] == 0x06) || ((uart1.recbuf[1] == 0x05)))         //写寄存器
        {
            len = 8;
        }
        else
        {
            len = uart1.recbuf[2] + 5; //数据长度
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

                    modbus.flag = 1;//接收的数据通过CRC验证，设置接收数据正确标志  BY LY
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

/*****************************************以下为气相流量计程序*******************************************/
/***************************************************************************
**	函数名称：ModbusReadScanReg_G(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
**	函数功能：读取气相流量计扫描寄存器的值
**	输入参数：modbus_addr:气相流量计设备地址
			  RegAddr :  寄存器地址
			  regnum  : 寄存器数量
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusReadScanReg_G(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //读寄存器的功能代码
    modbus.sendbuf[2] = RegAddr >> 8;     //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;   //寄存器低位
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16//进行CRC校验
    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //校验值低位
    modbus.sendbuf[7] = crc >> 8;

    OpenUart3RcvIRQ();//开uart3 接收中断
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
**	函数名称：ModbusReadReg_G(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
**	函数功能：读取气相流量计指定寄存器的值
**	输入参数：modbus_addr:气相流量计设备地址
			  RegAddr :  寄存器地址
			  regnum  : 寄存器数量
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusReadReg_G(uint8_t modbus_addr, uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //读寄存器的功能代码
    modbus.sendbuf[2] = RegAddr >> 8;     //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;   //寄存器低位
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16//进行CRC校验
    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //校验值低位
    modbus.sendbuf[7] = crc >> 8;

    OpenUart3RcvIRQ();//开uart3 接收中断
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
**	函数名称：ModbusWriteReg_G(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
**	函数功能：向气相流量计指定寄存器写入数据
**	输入参数：modbus_addr:气相流量计设备地址
			  modbus_coil:写寄存器功能码
			  RegAddr:寄存器地址
			  v_value:需要写入寄存器的值
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusWriteReg_G(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
{
    uint32_t    timecount = 0;
    uint16_t    crc = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = modbus_coil;
    modbus.sendbuf[2] = RegAddr >> 8;       //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;     //寄存器低位
    modbus.sendbuf[4] = v_value >> 8;       //值高位
    modbus.sendbuf[5] = v_value & 0x00FF;   //值低位

    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16校验

    modbus.sendbuf[6] = (uint8_t)(crc & 0x00FF);     //校验值低位
    modbus.sendbuf[7] = crc >> 8;

    OpenUart3RcvIRQ();//开uart3 接收中断
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
**	函数名称：ModbusReadDeviceAddress_G(uint8_t modbus_addr, uint16_t *rdaddress)
**	函数功能：读取气相流量计设备地址，测试MODBUS总线
**	输入参数：modbus_addr:气相流量计设备地址
			  rdaddress	:气相流量计返回的设备地址
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusWriteDeviceAddress(uint8_t modbus_addr, uint16_t rdaddress)
**	函数功能：读取气相流量计设备地址，测试MODBUS总线
**	输入参数：modbus_addr:气相流量计设备地址
			  rdaddress:待写入气相流量计的新设备地址
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusReadTotal_G(uint8_t modbus_addr, float *total)
**	函数功能：读取气相流量计加气总量
**	输入参数：modbus_addr:气相流量计设备地址
			  total:气相流量计返回的总累值
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusReadFlow_G(uint8_t modbus_addr, float *flow)
**	函数功能：读取气相流量计流速
**	输入参数：modbus_addr:气相流量计设备地址
			  flow:流量计返回的流速值
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusClearTotal_G(uint8_t modbus_addr)
**	函数功能：复位气相流量计总量寄存器
**	输入参数：modbus_addr:气相流量计设备地址
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusInitReg_G(void)
**	函数功能：气相流量计scan buffer寄存器初始化
**	输入参数：无
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t ModbusInitReg_G(void)
{
    uint8_t n = 0;
//    uint16_t AUTOSCAN_RM_BUF[] = {246/*流速*/, 258/*总量*/, 250/*温度*/, 248/*密度*/, 290/*增益*/}; //罗斯蒙特扫描寄存器
    uint16_t AUTOSCAN_RM_BUF[] = {0/*流速*/, 2/*总量*/, 1/*温度*/, 3/*密度*/, 47/*增益*/}; //罗斯蒙特扫描寄存器
    uint16_t AUTOSCAN_EH_BUF[] = {2007/*流速*/, 2610/*总量*/, 2017/*温度*/, 2013/*密度*/, 9503/*增益*/}; //E+H扫描寄存器

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        for(n = 0; n < _MODBUS_G_AUTO_BUF_LEN; n++)
        {
            //WATCH_DOG();
            if(!ModbusWriteReg_G(_MODBUS_G_, 6, 750 + n, AUTOSCAN_RM_BUF[n]))
            {
                if(sysparas.simulation == 0)
                {
                    syserr |= 1 << SYSERR_MODBUS_G_AUTO_ERR; //气相流量计通讯错误。系统错误0x04。
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
                    syserr |= 1 << SYSERR_MODBUS_G_AUTO_ERR; //气相流量计通讯错误。系统错误0x04。
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
**	函数名称：ModbusDataDisposs_G(void)
**	函数功能：气相流量计scan buffer数据处理
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
void ModbusDataDisposs_G(void)
{
    UnionFloat rdFreg;
    rdFreg.data = 0;

    /*计算流速*/
    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    realtime.cur_flowrate_g = rdFreg.data;

    /*计算总量*/
    rdFreg.rdcv[0] =  modbus.recbuf[8];
    rdFreg.rdcv[1] =  modbus.recbuf[7];
    rdFreg.rdcv[2] =  modbus.recbuf[10];
    rdFreg.rdcv[3] =  modbus.recbuf[9];

    realtime.cur_totalize_g = rdFreg.data;

    /*计算温度*/
    rdFreg.rdcv[0] =  modbus.recbuf[12];
    rdFreg.rdcv[1] =  modbus.recbuf[11];
    rdFreg.rdcv[2] =  modbus.recbuf[14];
    rdFreg.rdcv[3] =  modbus.recbuf[13];

    realtime.cur_temperature_g = rdFreg.data;

    /*计算密度*/
    rdFreg.rdcv[0] =  modbus.recbuf[16];
    rdFreg.rdcv[1] =  modbus.recbuf[15];
    rdFreg.rdcv[2] =  modbus.recbuf[18];
    rdFreg.rdcv[3] =  modbus.recbuf[17];


    realtime.cur_density_g = rdFreg.data;

    /*计算增益*/
    rdFreg.rdcv[0] =  modbus.recbuf[20];
    rdFreg.rdcv[1] =  modbus.recbuf[19];
    rdFreg.rdcv[2] =  modbus.recbuf[22];
    rdFreg.rdcv[3] =  modbus.recbuf[21];


    if(rdFreg.data <= 0.1)
    {
        rdFreg.data = rdFreg.data * 1000;
    }
    else if(rdFreg.data > 99.9)   //增益最大可以为100.45，但是显示屏显示的个数有限
    {
        rdFreg.data = 99.9;
    }

    realtime.cur_gain_g = rdFreg.data;
    globalvar.modbus_g_error = 0; //回气计量时，下一次读液象流量计
    ClrBit(syserr, SYSERR_MODBUS_G_AUTO_ERR);   //清除气相流量计通讯错误。
}

/***************************************************************************
**	函数名称：ModbusReadScanBuffer_G(uint8_t modbus_addr)
**	函数功能：读取气相流量计扫描寄存器中的值
**	输入参数：modbus_addr:气相流量计设备地址
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusComTest_G(uint8_t modbus_addr)
**	函数功能：读取气相流量计的设备地址，用于流量计通讯测试
**	输入参数：modbus_addr:气相流量计设备地址
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-3-30
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
**	函数名称：ModbusRecDataCheck_G(void)
**	函数功能：气相流量计接收数据处理
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
void ModbusRecDataCheck_G(void)
{
    uint32_t    len, i;
    uint16_t    CRC;

    //数据处理
    if(((uart3.recbuf[0] == _MODBUS_G_) || (uart3.recbuf[0] == _MODBUS_L_)) &&
            ((uart3.recbuf[1] == 0x03) || (uart3.recbuf[1] == 0x06) || (uart3.recbuf[1] == 0x05)))          //数据有效性判断
    {
        if((uart3.recbuf[1] == 0x06) || ((uart3.recbuf[1] == 0x05)))         //写寄存器
        {
            len = 8;
        }
        else
        {
            len = uart3.recbuf[2] + 5; //数据长度
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

                    modbus.flag = 1;//接收的数据通过CRC验证，设置接收数据正确标志  BY LY
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
        memset(uart3.recbuf, 0, BUFSIZE);   //发送数组清零。
    }
}
