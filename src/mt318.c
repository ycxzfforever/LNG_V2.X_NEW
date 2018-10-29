#include "kernel.h"

Mt318Reader mt318reader;
//M1Card m1card;    //定义M1卡块结构体。//huanghao20161026

/***************************************************************************
**	函数名称：Mt318CrcVerify(uint8_t *array, uint8_t length)
**	函数功能：MT318读卡器校验算法
**	输入参数：array:需要校验的数据
			  length:校验数据的长度
**	返 回 值：计算后的BCC校验值

**	创 建 者：YCX
**	创建日期：2016-3-30
***************************************************************************/
uint8_t Mt318CrcVerify(uint8_t *array, uint8_t length)
{
    uint8_t crcresult = 0;
    uint8_t tmp;
    crcresult = *array;

    for(tmp = 1; tmp < length; tmp++)
        crcresult ^= *(array + tmp);

    return(crcresult);
}

/***************************************************************************
**	函数名称：Mt318GetCardStatus(void)
**	函数功能：通过MT318获取卡的状态
**	输入参数：array:需要校验的数据
			  length:校验数据的长度
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-4-7
***************************************************************************/
uint8_t Mt318GetCardStatus(void)
{
    //0x02 0x00 0x02 0x31 0x41 0x03 BCC
    uint8_t     str[7] = {0x02, 0x00, 0x02, 0x31, 0x41, 0x03};
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = false;

    str[6] = Mt318CrcVerify(str, 6);   //最后一位是CRC
    mt318reader.err_code = 0;

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));
    OpenUart0RcvIRQ();//开uart0接收中断

    SELECT_CARD();
    Uart0SendStr(str, 7);

    timeout =  CARD_RESPONSE_MAX_TIME;

    while(--timeout)
    {
        if(mt318reader.recbuf[5] == 0x31)
        {
            ret = false;
            //FYD12864DispString( 2, 1, "机内无卡", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x30)
        {
            ret = false;
            //FYD12864DispString( 2, 1, "寻不到射频卡", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x32)
        {
            ret = true;
            //FYD12864DispString( 2, 1, "CPU B", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x33)
        {
            ret = true;
            //FYD12864DispString( 2, 1, "CPU A", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x34)
        {
            ret = true;
            //FYD12864DispString( 2, 1, "M1卡", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[6];
    }

    if(timeout == 0) ret = false;

    return  ret;
}

/***************************************************************************
**	函数名称：Mt318GetCardSerialNum(void)
**	函数功能：通过MT318获取卡的序列号
**	输入参数：无
**	返 回 值：true:成功，false:失败

**	创 建 者：YCX
**	创建日期：2016-4-7
***************************************************************************/
uint8_t Mt318GetCardSerialNum(void)  
{
    uint8_t     str[7] = {0x02, 0x00, 0x02, 0x34, 0x31, 0x03};//获取M1卡序列号命令
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;//寻卡失败时，最大寻卡次数 ADD BY LY
    uint8_t     ret = false;

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));

    str[6] = Mt318CrcVerify(str, 6);//异或校验码，获取M1卡序列号命令的最后一字节
    mt318reader.err_code = 0;


    SELECT_CARD();
    OpenUart0RcvIRQ();//开uart0接收中断

	//发送获取M1卡序列号命令
    Uart0SendStr(str, 7);   

    while(--timeout)
    {	
    	//mt318reader.recbuf[5] 为读卡器回复的：操作状态P 参看规格书 ADD BY LY
        if(mt318reader.recbuf[5] == 0x59 && mt318reader.recbuf[10] == 0x03)
        {
            ret = true;
            //FYD12864DispString( 3, 1, "卡序列号", 1 );

            //sprintf_P( str1, "%02X%02X%02X%02X", mt318reader.recbuf[6], mt318reader.recbuf[7],
            //           mt318reader.recbuf[8], mt318reader.recbuf[9] );
            //FYD12864DispPrintf( 3, 1, str1 );
            break;
        }
        else if(mt318reader.recbuf[5] == 0x4e)
        {
            ret = false;
            //FYD12864DispString( 3, 1, "寻卡不成功", 1 );
            break;
        }
        else if(mt318reader.recbuf[5] == 0x45)
        {
            ret = false;
            //FYD12864DispString( 3, 1, "机内无卡", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[5];
    }

	//获取卡序列号超时 ADD BY LY
    if(timeout == 0)
        ret = false;

    return  ret;
}

/***************************************************************************
**	函数名称：Mt318VerifyKey(uint8_t keyAB, uint8_t * key, uint8_t secter_num)
**	函数功能：验证第secter_num 扇区的B密
**	输入参数：key:密码
			  secter_num:扇区号(取值范围：0-63)
              keyAB：选择验证A密还是B 密；0x32 验证A密；0x39 验证B密
**	返 回 值：true:验证B密成功，false:验证B密失败

**	创 建 者：YCX
**	创建日期：2016-4-7
***************************************************************************/
uint8_t Mt318VerifyKey(uint8_t keyAB, uint8_t * key, uint8_t secter_num)
{
    uint8_t     i, str[14] = {0x02, 0x00, 0x09, 0x34, 0x39, 0xFF, '2', '2', '2', '2', '2', '2'};//str[0-5]验证B密命令ID ADD BY LY
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;//验证失败时，最大验证次数 ADD BY LY
    uint8_t     ret = false;//B密验证结果

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));

    /*************************** 验证AB密 指令填充 ****************************/

    str[4] = keyAB;		//选择验证 A密还是 B密 0x32=A密 0x39=B密 ADD BY LY
    str[5] = secter_num;//扇区号 ADD BY LY

    for(i = 0; i < 6; i++)   //6字节密码 ADD BY LY
    {
        str[6 + i] = key[i]; //需要写入的B密 ADD BY LY
    }

    str[12] = 0x03;
    str[13] = Mt318CrcVerify(str, 13);   //写入CRC值 ADD BY LY

    /**************************************************************************/

    mt318reader.err_code = 0;
    SELECT_CARD();//4052 选择读卡器功能 ADD BY LY
    OpenUart0RcvIRQ();//开uart0接收中断 ADD BY LY
    Uart0SendStr(str, 14);   //发送验证AB密指令 ADD BY LY

    while(--timeout)   //验证结果判断 mt318reader.recbuf[6]为B密验证结果
    {
        if(mt318reader.recbuf[6] == 0x59)		//B密验证成功 ADD BY LY
        {
            ret = true;
            //FYD12864DispString( 2, 1, "密码验证成功", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x30)  //寻不到射频卡 ADD BY LY
        {
            ret = false;
            //FYD12864DispString( 2, 1, "寻不到射频卡", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x33)  //密码错误 ADD BY LY
        {
            ret = false;

            if(mt318reader.recbuf[4] == KEYA)
            {
                //FYD12864DispString( 3, 1, "A 密验证失败", 1 );
            }

            else if(mt318reader.recbuf[4] == KEYB)
            {
                //FYD12864DispString( 3, 1, "B 密验证失败", 1 );
            }


            break;
        }
        else if(mt318reader.recbuf[6] == 0x45)  //卡机器内无卡 ADD BY LY
        {
            ret = false;
            //FYD12864DispString( 2, 1, "机内无卡", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[6];//记录验证结果 ADD BY LY
    }

    if(timeout == 0)
        ret = false;

    return  ret;
}

/***************************************************************************
**	函数名称：Mt318ReadM1Card(uint8_t keyAB, uint8_t * key, uint8_t blocks, uint8_t* rdbuf)
**	函数功能：读取M1卡扇区块中的内容
**	输入参数：keyAB:验证密码时采用A密验证还是B密验证
			  key:密码
              blocks:块号（取值0-63）
              rdbuf:将读取的数据内容装入该buf
**	返 回 值：true:读扇区成功，false:读扇区失败

**	创 建 者：YCX
**	创建日期：2016-4-7
***************************************************************************/
uint8_t Mt318ReadM1Card(uint8_t keyAB, uint8_t * key, uint8_t blocks, uint8_t* rdbuf)
{
    uint8_t     secter = 0;
    uint8_t     str[10] = {0x02, 0x00, 0x04, 0x34, 0x33};//读扇区块命令ID ADD BY LY
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = false;

// WATCH_DOG();
    secter = blocks / 4; //取扇区号 因为每个扇区有4块，但是块号是连续编码 ADD BY LY

    if(Mt318VerifyKey(keyAB, key, secter) == false)     // AB密验证失败
    {
        m1card.checkresult = 0x16;
        return false;
    }

    /*************************** 验证AB密 指令填充 ****************************/

    str[5] = secter;				//扇区号 ADD BY LY
    str[6] = blocks - 4 * secter;	//块号   ADD BY LY
    str[7] = 0x03;
    str[8] = Mt318CrcVerify(str, 8);

    /**************************************************************************/

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));

    mt318reader.err_code = 0;

    SELECT_CARD();
    OpenUart0RcvIRQ();//开uart0接收中断 ADD BY LY
    Uart0SendStr(str, 9);   //发送读扇区块数据命令 ADD BY LY

    while(--timeout)
    {
        if((mt318reader.recbuf[7] == 0x59) && (mt318reader.recbuf[24] == 0x03))
        {
            ret = true;
            memcpy(rdbuf, (uint8_t*)&mt318reader.recbuf[8], 16);    //数据读取成功，将数据copy到mt318reader.recbuf中 ADD BY LY
            //FYD12864DispPrintf(4,1,"读块成功:%d,%x",blocks,mt318reader.recbuf[11]);
            //sprintf_P( str1, "读块成功:%d,%x", blocks, mt318reader.recbuf[11] );
            //FYD12864DispPrintf( 4, 1, str1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x30)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "寻不到射频卡", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x33)
        {
            ret = false;
            //FYD12864DispPrintf(4,1,"密码验证错误%x,%x",mt318reader.recbuf[5],mt318reader.recbuf[6]);
            //sprintf_P( str1, "密码验证错误%x,%x", mt318reader.recbuf[5], mt318reader.recbuf[6] );
            //FYD12864DispPrintf( 4, 1, str1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x34)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "读数据错", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x45)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "机内无卡", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[7];
    }

    if(timeout == 0)
        ret = false;

    return  ret;
}

/***************************************************************************
**	函数名称：Mt318WriteM1Card(uint8_t keyAB, uint8_t * key, uint8_t blocks, uint8_t *wtbuf)
**	函数功能：向M1卡中写入数据
**	输入参数：keyAB:验证密码时采用A密验证还是B密验证
              key:密码
              blocks:块号（取值0-63）
              wdbuf:将wtbuf的数据写入m1卡中
**	返 回 值：true:写扇区成功，false:写扇区失败

**	创 建 者：YCX
**	创建日期：2016-4-7
***************************************************************************/
uint8_t Mt318WriteM1Card(uint8_t keyAB, uint8_t * key, uint8_t blocks, uint8_t *wtbuf)
{

    uint8_t     secter = 0;
    uint8_t     str[26] = {0x02, 0x00, 0x14, 0x34, 0x34};
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = false;

//    WATCH_DOG();

    secter = blocks / 4;

    if(Mt318VerifyKey(keyAB, key, secter) == false)
    {
        return false;
    }

    /*************************** 验证AB密 指令填充 ****************************/
    str[5] = secter;
    str[6] = blocks - 4 * secter;
    memcpy((uint8_t *)&str[7], wtbuf, 16);
    str[23] =  0x03;
    str[24] = Mt318CrcVerify(str, 24);

    /**************************************************************************/

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));
    mt318reader.err_code = 0;
    SELECT_CARD();
    OpenUart0RcvIRQ();//开uart0接收中断
    Uart0SendStr(str, 25);

    while(--timeout)
    {
        if(mt318reader.recbuf[7] == 0x59)
        {
            ret = true;
            //sprintf_P( str1, "写块成功:%d", blocks );
            //FYD12864DispPrintf( 4, 1, str1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x30)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "寻不到射频卡", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x31)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "操作扇区号错", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x33)
        {
            ret = false;
            //sprintf_P( str1, "密码验证错%x,%x", mt318reader.recbuf[5], mt318reader.recbuf[6] );
            //FYD12864DispPrintf( 4, 1, str1 );

            break;
        }
        else if(mt318reader.recbuf[7] == 0x34)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "校验写入块数据错", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x45)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "机内无卡", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[7];//写操作状态字 ADD BY LY
    }

    if(timeout == 0)
        ret = false;

    return  ret;

}

/***************************************************************************
**	函数名称：Mt318RecDataCheck(void)
**	函数功能：中断处理函数中检测接收到读卡器的数据是否正确
**	输入参数：无
**	返 回 值：无

**	创 建 者：YCX
**	创建日期：2016-4-7
***************************************************************************/
void Mt318RecDataCheck(void)
{
    uint32_t    len, i;
    uint32_t    datalen;//接收mt318数据帧中的数据长度，除开帧头、帧尾、BCC
    uint8_t     crc;

    if(uart0.recbuf[0] == 0x02)   //帧头
    {
        if((uart0.reclen > 2) && ((uart0.recbuf[1] != 0) || (uart0.recbuf[2] != 0)))           //接收数据有效，求出数据长度 ADD BY LY
        {
            datalen = (uint32_t)uart0.recbuf[1] << 8 | uart0.recbuf[2];
            len = 5 + datalen;

        }
        else if((uart0.recbuf[1] == 0) && (uart0.recbuf[2] == 0))
        {
            //可能是MT318bug，当获取序列号时，若机内无卡，则返回的长度为0
            len = 12;
        }

        if((uart0.reclen >= len) && (uart0.reclen > 2))       //将uart0接收的数据复制到mt318reader接收缓冲区 ADD BY LY
        {
            crc = Mt318CrcVerify((uint8_t*)uart0.recbuf, len - 1);

            if(crc == uart0.recbuf[len - 1])
            {
                CloseUart0RcvIRQ();//关uart0接收中断(CardPort);

                for(i = 0; i < len; i++)
                {
                    mt318reader.recbuf[i] = uart0.recbuf[i] ;
                }

                mt318reader.flag = 1;//置位mt318reader读取数据标志，表示接收数据有效
            }

            uart0.reclen = 0;
        }
    }
    else if(uart0.recbuf[0] == 0x06)   //如果接收到正确的ACK   ADD BY LY
    {
        Uart0SendByte(0x05);   //主机发送0x05，请求应答    ADD BY LY
        uart0.reclen = 0; //为接收报告做准备 ADD BY LY
    }
    else
    {
        CloseUart0RcvIRQ();//开uart0接收中断;
        uart0.reclen = 0;
    }
}


