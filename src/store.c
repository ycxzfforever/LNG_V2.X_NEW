#include "kernel.h"

/************************************************************************
**	�������ƣ�ReadLossCard(uint32_t cid)
**	��������: ���ݿ��ţ���ѯ�˿��Ƿ�Ϊ��ʧ��			  
**	�������: cid�� ��Ҫ��ѯ��IC �����ţ�HEX��
**	����ֵ  	: true:��������false:��ʧ�����򿨺ų�����Χ��ADD BY LY

**	�� �� ��:	HMM
**	��������:	2016-03-28
*************************************************************************/
uint8_t ReadLossCard(uint32_t cid)
{
    uint8_t index  = cid / 10000000; //ȡ����ֽڵ�ֵ ADD BY LY
#if 0
    uint16_t  offset = cid % 10000000;
#endif
    //8�ֽ�ASCII��IC�����ŵ����
    //���1�ֽ��Ǳ�ʾ������
    //�θ��ֽ���վ���
    //ʣ��6�ֽ�Ϊ����
    uint32_t  offset = cid % 1000000; //�º�̨ ȡ��6λ  //��7λ��ʾվ���
    /*if(index == 2)//(ɽ��)
    {
      if(offset>2000)  offset -= 2000;
      if(offset<40)    offset  = 160 - offset;
      else             return false;
    }*/
    uint32_t  offx   = offset / 8;
    uint32_t  offy   = offset % 8;
    uint32_t  addr;
    uint8_t eepuc;
    //���λΪ 1 ����ʾ���û���
    //���λΪ 2 ����ʾ��Ա����
    //���λΪ 3 ����ʾ�Ǽ��˿�
    //���λΪ 4 ����ʾ��ά�޿�
    //���λΪ 5 ����ʾ���ڲ���
    //���λΪ 6 ����ʾ�Ǽ��ֿ�
    //���λΪ 7 ����ʾ��VIP��

    switch(index)
    {
        case 1: //�û���
            if(offx >= CARD_USRER_LEN) return false;

            addr = CARD_USRER_ADDR + offx;
            break;

        case 2: //Ա����
            if(offx >= CARD_WORKER_LEN) return false;

            addr = CARD_WORKER_ADDR + offx;
            break;

        case 3:  //20130117 ���˿�
            if(offx >= CARD_MEM_LEN) return false;

            addr = CARD_MEM_ADDR + offx;
            break;

        case 4://ά�޿�
            if(offx >= CARD_SERV_LEN) return false;

            addr = CARD_SERV_ADDR + offx;
            break;

        case 5://�ڲ���
            if(offx >= CARD_OWN_LEN) return false;

            addr = CARD_OWN_ADDR + offx;
            break;

        case 6: //���ֿ�
            if(offx >= CARD_CAL_LEN) return false;

            addr = CARD_CAL_ADDR + offx;
            break;

        case 7: //VIP��201610
            if(offx >= CARD_VIP_LEN) return false;

            addr = CARD_VIP_ADDR + offx;
            break;

        default:
            return false;
    }

    if(I2cReadBytesForEeprom(SLA_W_24CXXX, (uint16_t)addr, 1, &eepuc) == -1)
    {

        return false;
    }

    eepuc >>= offy;
    eepuc &= 0x01;

    if(eepuc) return false;

    return true;
}

/************************************************************************
**	�������ƣ�SetLossCard(uint32_t cid, bool bset)
**	��������: ���ÿ���Ϊcid��IC�����ڴ��еĹ�ʧ��־	  
**	�������: cid�� ��Ҫ��ѯ��IC �����ţ�HEX��
			  bset����ʧ��־��1����ʧ��0�����
**	����ֵ  	: true:д�ɹ� �� false:���ų�����Χ ADD BY LY

**	�� �� ��:	HMM
**	��������:	2016-03-28
*************************************************************************/
bool SetLossCard(uint32_t cid, bool bset)
{
    uint32_t 	addr;
    uint8_t  	eepuc;

    uint8_t 	index  = cid / 10000000; //�������λ������������ 7��VIP����6�����ֿ���5���ڲ�����4��ά�޿���3�����˿���2��Ա������1���û��� ADD BY LY
    uint32_t  offset = cid % 1000000; //�º�̨ ȡ��6λ
    uint32_t  offx   = offset / 8; //���ݿ��ź�6λ�����жϣ��Ƿ��Ѿ������洢�ķ�Χ�����ĸ��ֽ� ADD BY LY
    uint32_t  offy   = offset % 8; //���ݿ��ź�6λ�жϿ��Ź�ʧ����Ӧ�ı��λ ADD BY LY

    switch(index)
    {
        case 1: //�û���
            if(offx >= CARD_USRER_LEN) return false;

            addr = CARD_USRER_ADDR + offx;//������ÿ��Ź�ʧ����Ӧ��ǵĵ�ַ ADD BY LY
            break;

        case 2: //Ա����
            if(offx >= CARD_WORKER_LEN) return false;

            addr = CARD_WORKER_ADDR + offx;
            break;

        case 3: //20130117 ���˿�
            if(offx >= CARD_MEM_LEN) return false;

            addr = CARD_MEM_ADDR + offx;
            break;

        case 4://ά�޿�
            if(offx >= CARD_SERV_LEN) return false;

            addr = CARD_SERV_ADDR + offx;
            break;

        case 5://�ڲ���
            if(offx >= CARD_OWN_LEN) return false;

            addr = CARD_OWN_ADDR + offx;
            break;

        case 6: //���ֿ�
            if(offx >= CARD_CAL_LEN) return false;

            addr = CARD_CAL_ADDR + offx;
            break;

        case 7: //VIP��201610
            if(offx >= CARD_VIP_LEN) return false;

            addr = CARD_VIP_ADDR + offx;
            break;

        default:
            return false;
    }

    if(I2cReadBytesForEeprom(SLA_W_24CXXX, (uint16_t)addr, 1, &eepuc) == -1)
    {
        return false;
    }

    if(bset)
        eepuc |= (1 << offy);   //ȡ���ÿ���ʧ��־����λ�õ����ݣ�Ȼ��offy��Ӧ��λ��1��ʾ�ǹ�ʧ //ADD BY LY
    else
        eepuc &= ~(1 << offy);   //ȡ���ÿ���ʧ��־����λ�õ����ݣ�Ȼ��offy��Ӧ��λ��0��ʾȡ����ʧ //ADD BY LY

    if(I2cWriteBytesForEeprom(SLA_W_24CXXX, (uint16_t)addr, 1, &eepuc) == -1)
    {
        return false;
    }

    return true;
}

/************************************************************************
**	�������ƣ�ClearLossCard(void)
**	��������: ������й�ʧ����� ADD BY LY 
**	�������: ��
**	����ֵ  	: ��

**	�� �� ��:	HMM
**	��������:	2016-03-28
*************************************************************************/
void ClearLossCard(void)
{
    uint32_t 	addr;
    uint8_t  	eepuc;

    eepuc = 0;

    FYD12864ClrScreen();

    FYD12864DispPrintfFlash(2, 1, "�����ʧ����");
    FYD12864DispPrintfFlash(3, 1, "���Ժ�..");

    for(addr = CARD_USRER_ADDR; addr < CARD_VIP_ADDR + 100; addr++)
    {
        I2cWriteBytesForEeprom(SLA_W_24CXXX, (uint16_t)addr, 1, &eepuc);
    }

    FYD12864ClrScreen();
    FYD12864DispPrintfFlash(2, 1, "����ɹ�");
    DispUI = DispSetICInfo2;
}

/************************************************************************
**	�������ƣ�WriteLossCardFlag(void)
**	��������: ���ݿ��ţ���ѯϵͳ�иÿ��Ƿ�Ϊ��ʧ��������ǹ�ʧ����д���Ĺ�ʧ��� ADD BY LY
**	�������: ��
**	����ֵ  	: true:�ù�ʧ��־�ɹ� �� false:�ù�ʧ��־ʧ��

**	�� �� ��:	HMM
**	��������:	2016-03-28
*************************************************************************/
uint8_t WriteLossCardFlag(void)
{
    uint8_t buf[16];
    uint8_t bufbak[16];

    if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, buf))     //����2
    {
        return false;
    }

    //�ù�ʧ����־
    buf[8] = 0x30;
    buf[9] = 0x32;

    if(!Mt318WriteM1Card(KEYB, &m1card.keyB[0], 2, buf))     //д��ʧ��־
    {
        return false;
    }

    _delay_ms(10);

    if(!Mt318ReadM1Card(KEYB, &m1card.keyB[0], 2, bufbak))     //�����ո�д�������
    {
        return false;
    }

    if(memcmp(buf, bufbak, 16) != 0)	//�ȶ�д���Ƿ���ȷ ADD BY LY
    {
        return false;
    }

    return true;
}


