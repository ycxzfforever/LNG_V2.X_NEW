#include "kernel.h"

Mt318Reader mt318reader;
//M1Card m1card;    //����M1����ṹ�塣//huanghao20161026

/***************************************************************************
**	�������ƣ�Mt318CrcVerify(uint8_t *array, uint8_t length)
**	�������ܣ�MT318������У���㷨
**	���������array:��ҪУ�������
			  length:У�����ݵĳ���
**	�� �� ֵ��������BCCУ��ֵ

**	�� �� �ߣ�YCX
**	�������ڣ�2016-3-30
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
**	�������ƣ�Mt318GetCardStatus(void)
**	�������ܣ�ͨ��MT318��ȡ����״̬
**	���������array:��ҪУ�������
			  length:У�����ݵĳ���
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-4-7
***************************************************************************/
uint8_t Mt318GetCardStatus(void)
{
    //0x02 0x00 0x02 0x31 0x41 0x03 BCC
    uint8_t     str[7] = {0x02, 0x00, 0x02, 0x31, 0x41, 0x03};
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = false;

    str[6] = Mt318CrcVerify(str, 6);   //���һλ��CRC
    mt318reader.err_code = 0;

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));
    OpenUart0RcvIRQ();//��uart0�����ж�

    SELECT_CARD();
    Uart0SendStr(str, 7);

    timeout =  CARD_RESPONSE_MAX_TIME;

    while(--timeout)
    {
        if(mt318reader.recbuf[5] == 0x31)
        {
            ret = false;
            //FYD12864DispString( 2, 1, "�����޿�", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x30)
        {
            ret = false;
            //FYD12864DispString( 2, 1, "Ѱ������Ƶ��", 1 );
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
            //FYD12864DispString( 2, 1, "M1��", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[6];
    }

    if(timeout == 0) ret = false;

    return  ret;
}

/***************************************************************************
**	�������ƣ�Mt318GetCardSerialNum(void)
**	�������ܣ�ͨ��MT318��ȡ�������к�
**	�����������
**	�� �� ֵ��true:�ɹ���false:ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-4-7
***************************************************************************/
uint8_t Mt318GetCardSerialNum(void)  
{
    uint8_t     str[7] = {0x02, 0x00, 0x02, 0x34, 0x31, 0x03};//��ȡM1�����к�����
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;//Ѱ��ʧ��ʱ�����Ѱ������ ADD BY LY
    uint8_t     ret = false;

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));

    str[6] = Mt318CrcVerify(str, 6);//���У���룬��ȡM1�����к���������һ�ֽ�
    mt318reader.err_code = 0;


    SELECT_CARD();
    OpenUart0RcvIRQ();//��uart0�����ж�

	//���ͻ�ȡM1�����к�����
    Uart0SendStr(str, 7);   

    while(--timeout)
    {	
    	//mt318reader.recbuf[5] Ϊ�������ظ��ģ�����״̬P �ο������ ADD BY LY
        if(mt318reader.recbuf[5] == 0x59 && mt318reader.recbuf[10] == 0x03)
        {
            ret = true;
            //FYD12864DispString( 3, 1, "�����к�", 1 );

            //sprintf_P( str1, "%02X%02X%02X%02X", mt318reader.recbuf[6], mt318reader.recbuf[7],
            //           mt318reader.recbuf[8], mt318reader.recbuf[9] );
            //FYD12864DispPrintf( 3, 1, str1 );
            break;
        }
        else if(mt318reader.recbuf[5] == 0x4e)
        {
            ret = false;
            //FYD12864DispString( 3, 1, "Ѱ�����ɹ�", 1 );
            break;
        }
        else if(mt318reader.recbuf[5] == 0x45)
        {
            ret = false;
            //FYD12864DispString( 3, 1, "�����޿�", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[5];
    }

	//��ȡ�����кų�ʱ ADD BY LY
    if(timeout == 0)
        ret = false;

    return  ret;
}

/***************************************************************************
**	�������ƣ�Mt318VerifyKey(uint8_t keyAB, uint8_t * key, uint8_t secter_num)
**	�������ܣ���֤��secter_num ������B��
**	���������key:����
			  secter_num:������(ȡֵ��Χ��0-63)
              keyAB��ѡ����֤A�ܻ���B �ܣ�0x32 ��֤A�ܣ�0x39 ��֤B��
**	�� �� ֵ��true:��֤B�ܳɹ���false:��֤B��ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-4-7
***************************************************************************/
uint8_t Mt318VerifyKey(uint8_t keyAB, uint8_t * key, uint8_t secter_num)
{
    uint8_t     i, str[14] = {0x02, 0x00, 0x09, 0x34, 0x39, 0xFF, '2', '2', '2', '2', '2', '2'};//str[0-5]��֤B������ID ADD BY LY
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;//��֤ʧ��ʱ�������֤���� ADD BY LY
    uint8_t     ret = false;//B����֤���

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));

    /*************************** ��֤AB�� ָ����� ****************************/

    str[4] = keyAB;		//ѡ����֤ A�ܻ��� B�� 0x32=A�� 0x39=B�� ADD BY LY
    str[5] = secter_num;//������ ADD BY LY

    for(i = 0; i < 6; i++)   //6�ֽ����� ADD BY LY
    {
        str[6 + i] = key[i]; //��Ҫд���B�� ADD BY LY
    }

    str[12] = 0x03;
    str[13] = Mt318CrcVerify(str, 13);   //д��CRCֵ ADD BY LY

    /**************************************************************************/

    mt318reader.err_code = 0;
    SELECT_CARD();//4052 ѡ����������� ADD BY LY
    OpenUart0RcvIRQ();//��uart0�����ж� ADD BY LY
    Uart0SendStr(str, 14);   //������֤AB��ָ�� ADD BY LY

    while(--timeout)   //��֤����ж� mt318reader.recbuf[6]ΪB����֤���
    {
        if(mt318reader.recbuf[6] == 0x59)		//B����֤�ɹ� ADD BY LY
        {
            ret = true;
            //FYD12864DispString( 2, 1, "������֤�ɹ�", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x30)  //Ѱ������Ƶ�� ADD BY LY
        {
            ret = false;
            //FYD12864DispString( 2, 1, "Ѱ������Ƶ��", 1 );
            break;
        }
        else if(mt318reader.recbuf[6] == 0x33)  //������� ADD BY LY
        {
            ret = false;

            if(mt318reader.recbuf[4] == KEYA)
            {
                //FYD12864DispString( 3, 1, "A ����֤ʧ��", 1 );
            }

            else if(mt318reader.recbuf[4] == KEYB)
            {
                //FYD12864DispString( 3, 1, "B ����֤ʧ��", 1 );
            }


            break;
        }
        else if(mt318reader.recbuf[6] == 0x45)  //���������޿� ADD BY LY
        {
            ret = false;
            //FYD12864DispString( 2, 1, "�����޿�", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[6];//��¼��֤��� ADD BY LY
    }

    if(timeout == 0)
        ret = false;

    return  ret;
}

/***************************************************************************
**	�������ƣ�Mt318ReadM1Card(uint8_t keyAB, uint8_t * key, uint8_t blocks, uint8_t* rdbuf)
**	�������ܣ���ȡM1���������е�����
**	���������keyAB:��֤����ʱ����A����֤����B����֤
			  key:����
              blocks:��ţ�ȡֵ0-63��
              rdbuf:����ȡ����������װ���buf
**	�� �� ֵ��true:�������ɹ���false:������ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-4-7
***************************************************************************/
uint8_t Mt318ReadM1Card(uint8_t keyAB, uint8_t * key, uint8_t blocks, uint8_t* rdbuf)
{
    uint8_t     secter = 0;
    uint8_t     str[10] = {0x02, 0x00, 0x04, 0x34, 0x33};//������������ID ADD BY LY
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = false;

// WATCH_DOG();
    secter = blocks / 4; //ȡ������ ��Ϊÿ��������4�飬���ǿ������������ ADD BY LY

    if(Mt318VerifyKey(keyAB, key, secter) == false)     // AB����֤ʧ��
    {
        m1card.checkresult = 0x16;
        return false;
    }

    /*************************** ��֤AB�� ָ����� ****************************/

    str[5] = secter;				//������ ADD BY LY
    str[6] = blocks - 4 * secter;	//���   ADD BY LY
    str[7] = 0x03;
    str[8] = Mt318CrcVerify(str, 8);

    /**************************************************************************/

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));

    mt318reader.err_code = 0;

    SELECT_CARD();
    OpenUart0RcvIRQ();//��uart0�����ж� ADD BY LY
    Uart0SendStr(str, 9);   //���Ͷ��������������� ADD BY LY

    while(--timeout)
    {
        if((mt318reader.recbuf[7] == 0x59) && (mt318reader.recbuf[24] == 0x03))
        {
            ret = true;
            memcpy(rdbuf, (uint8_t*)&mt318reader.recbuf[8], 16);    //���ݶ�ȡ�ɹ���������copy��mt318reader.recbuf�� ADD BY LY
            //FYD12864DispPrintf(4,1,"����ɹ�:%d,%x",blocks,mt318reader.recbuf[11]);
            //sprintf_P( str1, "����ɹ�:%d,%x", blocks, mt318reader.recbuf[11] );
            //FYD12864DispPrintf( 4, 1, str1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x30)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "Ѱ������Ƶ��", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x33)
        {
            ret = false;
            //FYD12864DispPrintf(4,1,"������֤����%x,%x",mt318reader.recbuf[5],mt318reader.recbuf[6]);
            //sprintf_P( str1, "������֤����%x,%x", mt318reader.recbuf[5], mt318reader.recbuf[6] );
            //FYD12864DispPrintf( 4, 1, str1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x34)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "�����ݴ�", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x45)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "�����޿�", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[7];
    }

    if(timeout == 0)
        ret = false;

    return  ret;
}

/***************************************************************************
**	�������ƣ�Mt318WriteM1Card(uint8_t keyAB, uint8_t * key, uint8_t blocks, uint8_t *wtbuf)
**	�������ܣ���M1����д������
**	���������keyAB:��֤����ʱ����A����֤����B����֤
              key:����
              blocks:��ţ�ȡֵ0-63��
              wdbuf:��wtbuf������д��m1����
**	�� �� ֵ��true:д�����ɹ���false:д����ʧ��

**	�� �� �ߣ�YCX
**	�������ڣ�2016-4-7
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

    /*************************** ��֤AB�� ָ����� ****************************/
    str[5] = secter;
    str[6] = blocks - 4 * secter;
    memcpy((uint8_t *)&str[7], wtbuf, 16);
    str[23] =  0x03;
    str[24] = Mt318CrcVerify(str, 24);

    /**************************************************************************/

    memset((uint8_t*)mt318reader.recbuf, 0, sizeof(mt318reader.recbuf));
    mt318reader.err_code = 0;
    SELECT_CARD();
    OpenUart0RcvIRQ();//��uart0�����ж�
    Uart0SendStr(str, 25);

    while(--timeout)
    {
        if(mt318reader.recbuf[7] == 0x59)
        {
            ret = true;
            //sprintf_P( str1, "д��ɹ�:%d", blocks );
            //FYD12864DispPrintf( 4, 1, str1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x30)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "Ѱ������Ƶ��", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x31)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "���������Ŵ�", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x33)
        {
            ret = false;
            //sprintf_P( str1, "������֤��%x,%x", mt318reader.recbuf[5], mt318reader.recbuf[6] );
            //FYD12864DispPrintf( 4, 1, str1 );

            break;
        }
        else if(mt318reader.recbuf[7] == 0x34)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "У��д������ݴ�", 1 );
            break;
        }
        else if(mt318reader.recbuf[7] == 0x45)
        {
            ret = false;
            //FYD12864DispString( 4, 1, "�����޿�", 1 );
            break;
        }

        mt318reader.err_code = mt318reader.recbuf[7];//д����״̬�� ADD BY LY
    }

    if(timeout == 0)
        ret = false;

    return  ret;

}

/***************************************************************************
**	�������ƣ�Mt318RecDataCheck(void)
**	�������ܣ��жϴ������м����յ��������������Ƿ���ȷ
**	�����������
**	�� �� ֵ����

**	�� �� �ߣ�YCX
**	�������ڣ�2016-4-7
***************************************************************************/
void Mt318RecDataCheck(void)
{
    uint32_t    len, i;
    uint32_t    datalen;//����mt318����֡�е����ݳ��ȣ�����֡ͷ��֡β��BCC
    uint8_t     crc;

    if(uart0.recbuf[0] == 0x02)   //֡ͷ
    {
        if((uart0.reclen > 2) && ((uart0.recbuf[1] != 0) || (uart0.recbuf[2] != 0)))           //����������Ч��������ݳ��� ADD BY LY
        {
            datalen = (uint32_t)uart0.recbuf[1] << 8 | uart0.recbuf[2];
            len = 5 + datalen;

        }
        else if((uart0.recbuf[1] == 0) && (uart0.recbuf[2] == 0))
        {
            //������MT318bug������ȡ���к�ʱ���������޿����򷵻صĳ���Ϊ0
            len = 12;
        }

        if((uart0.reclen >= len) && (uart0.reclen > 2))       //��uart0���յ����ݸ��Ƶ�mt318reader���ջ����� ADD BY LY
        {
            crc = Mt318CrcVerify((uint8_t*)uart0.recbuf, len - 1);

            if(crc == uart0.recbuf[len - 1])
            {
                CloseUart0RcvIRQ();//��uart0�����ж�(CardPort);

                for(i = 0; i < len; i++)
                {
                    mt318reader.recbuf[i] = uart0.recbuf[i] ;
                }

                mt318reader.flag = 1;//��λmt318reader��ȡ���ݱ�־����ʾ����������Ч
            }

            uart0.reclen = 0;
        }
    }
    else if(uart0.recbuf[0] == 0x06)   //������յ���ȷ��ACK   ADD BY LY
    {
        Uart0SendByte(0x05);   //��������0x05������Ӧ��    ADD BY LY
        uart0.reclen = 0; //Ϊ���ձ�����׼�� ADD BY LY
    }
    else
    {
        CloseUart0RcvIRQ();//��uart0�����ж�;
        uart0.reclen = 0;
    }
}


