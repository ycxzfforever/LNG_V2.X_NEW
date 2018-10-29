#ifndef __STORE_H
#define __STORE_H

/*****************************************************************************
** ˵����EEPROM �ڴ滮��
**       1���ڲ�EEPROM�洢������4K Bytes    (Atmega1280�Դ�) MAXADDR 0x1000
**       2���ⲿEEPROM�洢������16K Bytes   (AT24C128) 		     MAXADDR 0x4000
**       3���洢���ݣ�a��ϵͳ������������ˮ
**	     4���ڲ����ⲿEEPROM�洢��ַ������ͬ
******************************************************************************/
#define EEPROM_W_TIMES        		90000    //�ڲ�EEPROM���д���� 

#define LOCKVER_ADDR            	0x0010   //B��Կ�洢��ַ

#define SYSPARA_ADDR1          		0x0040   //ϵͳ�������ڲ�EEPROM�е���ʼ�洢��ַ �ӵ�2ҳ��ʼ
/*#define SYSPARA_ADDR1_STR    		0x0040   //ϵͳ�������ڲ�EEPROM�е���ʼ�洢��ַ �ӵ�2ҳ��ʼ
**#define SYSPARA_ADDR1_END	   		0x017F   //��2ҳ �� ��6ҳ       ϵͳ������ſռ�1 ��320�ֽڿռ�*/

#define SYSPARA_ADDR2	       		0x0180   //��7ҳ �� ��11ҳ ϵͳ������ſռ�2 ��320�ֽڿռ�
/*#define SYSPARA_ADDR2_STR	   		0x0180   //��7ҳ �� ��11ҳ ϵͳ������ſռ�2 ��320�ֽڿռ�
**#define SYSPARA_ADDR2_END	   		0X02BF 	*/

#define SYSPARA_ADDR3	       		0x02C0   //��12ҳ �� ��16ҳ ϵͳ������ſռ�3 ��320�ֽڿռ�
/*#define SYSPARA_ADDR3_STR	   		0x02C0   //��12ҳ �� ��16ҳ ϵͳ������ſռ�3 ��320�ֽڿռ�
**#define SYSPARA_ADDR3_END	   		0x03FF   // */

#define SYSPARA_ADDR4	   	   		0x0400   //��17ҳ �� ��21ҳ ϵͳ������ſռ�4 ��320�ֽڿռ�
/*#define SYSPARA_ADDR4_STR	   		0x0400   //��17ҳ �� ��21ҳ ϵͳ������ſռ�4 ��320�ֽڿռ�
**#define SYSPARA_ADDR4_END	   		0x053F   //��ȥǰ��洢4���ֽڵĿռ䣬��0x07FF */

#define REALTIMEDATA_ADDR 	   		0x0540	//�������һ�ʵ�����ˮ ʣ��704 �ֽ�ȫ�����ڱ������һ��ʵʱ��ˮ
/*#define REALTIMEDATA_ADDR_STR 	0x0540	//�������һ�ʵ�����ˮ ʣ��704 �ֽ�ȫ�����ڱ������һ��ʵʱ��ˮ
**#define REALTIMEDATA_ADDR_END     0x07FF    */

#define SYSPARA_MAX_ADDR       	    0x07FF   //�ڲ�EEPROM 0-2K ��Χ���ڴ洢ϵͳ����(ϵͳ����276�ֽ�) ʣ��2KԤ��


#define EEPROM_FUELRECORDINFO_STR       0x0800  //�ⲿEEPROM ��3K��Χ��ʼ���洢40������ļ�����ˮ (����ʹ�� ADD BY LY)
#define EEPROM_TESTADDR					0x3000  //�ⲿEEPROM ���Ե�ַ

/***************************************************************************************************
** ˵������ʧ���洢��ַ �ⲿEERPOM �ӵ�4K �ռ俪ʼ�洢
**       1��CARD_XX_LEN����ʾCARD��ռ�ռ�ĳ��ȣ��������� = CARD_XX_LEN * 8
**       2���÷ֶ�ֻʹ�����ⲿEEPROMΪ16�����
**		 3�����˼�룺��EEPROM�Ĵ洢�ռ��Ϊ���飬ÿһ�����ͬ���͵�CARD��
**		 EEPOMһ���ֽ��е�һbit������һ�ſ��Ĺ�ʧ�����bit=0,�ǹ�ʧ����bit=1 ��ʧ��
**		 ��ͬ���͵Ŀ��Ŀ�������ֽ���ͬ ADD BY LY
****************************************************************************************************/
#define    CARD_USRER_LEN   1024*5 /*�û���  ����1*/
#define    CARD_WORKER_LEN  512    /*Ա����  ����2*/
#define    CARD_MEM_LEN     512    /*���˿�  ����3*/
#define    CARD_SERV_LEN    16     /*ά�޿�  ����4*/
#define    CARD_OWN_LEN     64     /*�ڲ���  ����5*/
#define    CARD_CAL_LEN     2*1024 /*���ֿ�  ����6*/
#define    CARD_VIP_LEN     100    /*����VIP������7 -201610*/

//��ռ�ô�Լ9K�Ĵ洢�ռ�
enum _CARD_PARAS_ADDR
{
    CARD_USRER_ADDR   = 0x1000,
    CARD_WORKER_ADDR  = CARD_USRER_ADDR + CARD_USRER_LEN,    	//  0x2400
    CARD_MEM_ADDR     = CARD_WORKER_ADDR + CARD_WORKER_LEN,    	//	0x2600
    CARD_SERV_ADDR    = CARD_MEM_ADDR + CARD_MEM_LEN,           //	0x2800
    CARD_OWN_ADDR     = CARD_SERV_ADDR + CARD_SERV_LEN,        	//	0x2810
    CARD_CAL_ADDR     = CARD_OWN_ADDR + CARD_OWN_LEN, 			//	0x2850
    CARD_VIP_ADDR     = CARD_CAL_ADDR + CARD_CAL_LEN,  			//	0x3010����VIP��201610
};

#define    LOCK_ADDR        0x3500								//���ܰ����ݴ洢��ַ ADD BY LY
/***********************************************************************************************/


/**********************************************************************************************************************
** ˵�����ⲿFLASH E28F128FLASH �ڴ����
**       1���洢������16M Bytes(128K/block ��128 blocks)
**       2���洢���ݣ�a����ʧ�����ţ�b����ˮ(ǰ��40 block ���ڴ���ˮ);c�����ۼ�¼
**
** ��ע��1��LNG ֻ�к�������û������/ɾ �ڰ�����
**********************************************************************************************************************/
/*****************��ˮ��¼ 20480�� (0-39 block)  ********************/
#define  FlashRecSADDR        0x000000
#define  FlashRecEADDR        0x4FFFFF
#define  FlashRecLen          256
#define  FlashRecMaxID        20000 //128K/256=512(ÿ��block��512����ˮ),512*40(block)=20480 ��Ҫ40��blocks �ɴ�20480����ʵ��20000��

/*****************  E28F128FLASH ��ز���  ************************************************/
#define   MAX_ADDR            0x0FFFFFF   // оƬ�ڲ�����ַ        
#define   MAX_BLOCK_NUM       127         // �����                                     

/*****************�����޸ļ�¼  8192��  (40 - 47 block)    LNG2.7û�в�����¼********************/
#define  FlashModSADDR  	  0x500000
#define  FlashModEAddr  	  0x5FFFFF
#define  FlashModLen    	  128
#define  FlashModMaxID		  8000//128K/128 =1024��1024*8=8192 ��Ҫ8��blocks �ɴ�8192����ʵ��8000��


/*****************���°��¼  3072����48 - 50 block) ********************/
#define  FlashShiftSADDR       0x600000
#define  FlashShiftEADDR       0x66FFFF
#define  FlashShiftLen         128
#define  FlashShiftMaxID       3000//128K/128=1024, 1024*3=3072 ��Ҫ3��blocks���ɴ�3072����ʵ��3000��


/**************������ 32768 ��block 51-52  block) *****************/
#define  FlashBlkSADDR       0x670000
#define  FlashBlkEADDR       0x6AFFFF
#define  FlashBlkLen         8  		//�洢���ǹ�ʧ������ �洢ʱ��Ҫ���������֯�洢�����ҵ�����
#define  FlashBlkMaxID       32768 		//128K/8=16384 16384*2=32768 ��Ҫ2�� blocks
/**********************************************************************/



/***************************����Ϊ2.7ԭ�����ڴ����**********************************/
//���¼��Ϣ 54�ֽ�
typedef struct ShiftRecordInfo
{
    uint32_t  classid;                //���°��¼��ˮ�� ����ˮ
    uint8_t   classnum;               //��ţ�1�ֽڡ�
    uint8_t   classstarttime[6];      //�࿪ʼʱ�䣬6�ֽڣ�BCD�롣
    uint8_t   classendtime[6];        //�����ʱ�䣬6�ֽڣ�BCD�롣
    uint32_t  startgunvolume;		  //�࿪ʼ����ǹ��
    uint32_t  endallgunvolume;		  //���������ǹ��
    uint32_t  startgunmoney;		  //�࿪ʼ����ǹ��
    uint32_t  endagunmoney;		  	  //���������ǹ��
    uint32_t  classcalgasnum;         //��ϼ�����(���������-�࿪ʼ����)
    uint32_t  classgassum;		  	  //�ϼ����� ������ˮ��¼����
    uint32_t  classgasmoney;          //�������� ������ˮ��¼����
    uint32_t  classrealmoney;		  //��ʵ���ֽ�(�ֽ�+��ֵ)
    uint32_t  classtimes;             //�����������4�ֽڡ�
    uint8_t   upflag;             	  //�ϴ��ɹ���־
    uint8_t   measuretype;            //������λ��1�ֽڣ�1-�귽��0-Kg��
    uint8_t   devicetype;             //�豸����0�� LNG ������1�� CNG ������2�� ������3�� ж����
    uint32_t  stationcode;            //����վ���
    uint16_t  crc;
} ShiftRecordInfo;

//ϵͳ������Ϣ 19�ֽ�
typedef struct ErrorRecordInfo
{
    uint32_t  errtransactionid;     //6 BCD ������ˮ��
    uint8_t   errtime[6];              //6 BCD ����ʱ��
    uint8_t   classnum     ;           //1 BCD ��� (ȡֵ1,2,3,4)
    uint8_t   errcode;                 //1 BCD �������
    uint32_t  errusernum;              //4 BCD ������Ŀ���
    uint8_t   upflag;             //1 BCD �ϴ���־ (0:û���ϴ�;1:�Ѿ��ϴ�)
    uint16_t  data;                    //2 BCD Ԥ��
} ErrorRecordInfo;


//������ˮ��¼��Ϣ 137�ֽ�
typedef struct FuelRecordInfo
{
    uint32_t  transnum;               //������ˮ�ţ�4�ֽڡ�
    uint8_t   classnum;               //��ţ�1�ֽڡ�
    uint32_t  classid;                //����ˮ�š�
    uint8_t   starttime[6];           //������ʼʱ�䣬6�ֽڣ�BCD�롣
    uint8_t   endtime[6];             //��������ʱ�䣬6�ֽڣ�BCD�롣
    uint32_t  usernum;			  	  //�û�����
    uint8_t   cardtype;               //������ �� 1�ֽ�
    uint32_t  beforebalance;          //������ǰ��4�ֽڡ�
    uint32_t  volume;                 //��ǰ����������4�ֽڡ�
    uint16_t  price;                  //���μ�������,2�ֽڡ�
    uint32_t  money;                  //��ǰ������4�ֽڡ�
    uint32_t  afterbalance;           //����������4�ֽڡ�
    uint32_t  allrechargetimes;       //���ۼƳ�ֵ������4�ֽڡ�
    uint32_t  allrechargemoney;       //���ۼƳ�ֵ��4�ֽڡ�
    uint32_t  allconsumptiontimes;    //���ۼ����Ѵ�����4�ֽڡ�
    uint32_t  allconsumptionnum;      //���ۼ�����������4�ֽڡ�
    uint32_t  allconsumptionmoney;    //���ۼ����ѽ�4�ֽڡ�
    uint32_t  InitialMoney;			  //���ڳ����
    uint32_t  CalMoney;				  //���������
    uint32_t  carnum_another;		  //���ƺź���λ HEX
    uint16_t  beforepressure;         //����ǰѹ����2�ֽڡ�
    uint16_t  afterpressure;          //������ѹ����2�ֽڡ�
    uint32_t  beforegunsum;		  	  //����ǰ����ǹ��
    uint32_t  aftergunsum;		  	  //����������ǹ��
    uint8_t   greyflag;               //�Ƿ�ҿ� 0����1����
    uint8_t	  printflag;			  //�Ƿ��ӡ     0����1����
    uint8_t   upflag;				  //�ϴ��ɹ���־ 0����1����
    uint8_t   measuretype;            //������λ��1�ֽڣ�1-�귽��0-Kg��
    uint8_t   devicetype;             //�豸����0�� LNG ������1�� CNG ������2�� ������3�� ж����
    uint8_t   stationcode;			  //�����б��
    uint32_t  allgunnum;              //ǹ�����ۼƣ�4�ֽڡ�
    uint32_t  allgunmoney;            //ǹ����ۼ�,4�ֽڡ�
    uint8_t   stopreason;             //ͣ��ԭ��
    uint32_t  backgas;                //������
    uint32_t  density;                //�ܶȣ�4�ֽڡ�
    uint32_t  curnum;                 //������4�ֽڡ�����ϵ����
    uint8_t   cardstate;			  //��״̬ 1�����룻0���γ�
    uint8_t   fuelmode;               //������ʽ �ֶ�

    uint16_t  crc;
} FuelRecordInfo;

//���������� 267�ֽ� + 33�ֽڱ���,��300�ֽ�
typedef struct def_SysParas
{
    uint8_t     head;       		//0 ͷ
    uint32_t    writeE2num;		 	//1дeeprom����
    uint8_t     gunnum;     		//2ǹ��
    uint32_t    transnum;   		//3������ˮ��(ǹ��ˮ��)
    uint32_t    uptransnum; 		//4�Ѵ�������ˮ��
    uint32_t    totvolume;  		//5ǹ������    2λС��
    uint32_t    totmoney;   		//6ǹ�ܽ��    2λС��
    uint16_t    price[20];  		//7-26 20�����ۣ�����0ΪĬ�ϵ��ۣ����ݵ��۵ȼ�ѡ����Ӧ����
    uint8_t     modbustype; 		//27���������ͣ�1-��˹���أ�2-E+H
    uint8_t     unit;       		//28������λ 1-kg,2-�귽
    uint8_t     qcmode;     		//29�ʼ�ģʽ����
    uint32_t    densty;             //30�ܶ�              4λС��//����4������������16λ�޸�Ϊ32λ���ұ���Ҫ�ı�
    uint32_t    coeff;              //31����ϵ��  4λС��
    uint32_t    pzero;              //32ѹ�����
    uint32_t    pcoeff;             //33ѹ��ϵ��  2λС��
    uint32_t    modnum;     		//34�޸ļ�¼��ˮ��
    uint32_t    upmodnum;   		//35�Ѵ��޸ļ�¼��ˮ��

    //������Ϣ
    uint8_t     shiftmaxnum;        //36�����
    uint8_t     shiftnum;           //37���
    uint32_t    shiftransnum;       //38����ˮ��
    uint32_t    shiftuptransnum;    //39�Ѵ�����ˮ��
    uint8_t     shiftstarttime[6];  //40�࿪ʼʱ��   BCD
    uint8_t     shiftendtime[6];    //41�����ʱ��    BCD
    uint32_t    shiftstartvol;      //42�࿪ʼǹ��    2λС��
    uint32_t    shiftendvol;        //43�����ǹ��    2λС��
    uint32_t	shiftstartmon;		//44�࿪ʼ���
    uint32_t	shiftvolsum;		//45��ϼƼ����� = ǹ�۽��� - ǹ�ۿ�ʼ
    uint32_t    shiftotvol;         //46��ϼ�����    2λС����������ˮ���������
    uint32_t    shiftotmon;         //47��ϼƽ��    2λС����������ˮ���������
    uint32_t	shiftrealmoney;		//48ʵ���ֽ�
    uint32_t    shiftfueltimes;     //49���������

    //ͨ����Ϣ
    uint8_t     shengcode[2];       //50ʡ����
    uint8_t     stationcodearray[4];//51վ����
    uint32_t    stationcode;        //52վ����
    uint8_t     citycode[2];        //53���д���
    uint8_t     parentcode[4];      //54�ϼ���λ����

    //����Ϣ
    uint32_t    cardmaxyue;         //55����������    2λС��
    uint32_t    cardminyue;         //56������������    2λС��
    uint32_t	accountcardmaxyue;	//57���˿��������    2λС��
    uint8_t     cardpsamnum;        //58psam������ѡ��
    uint16_t    cardmaxday;         //59��δʹ�����������
    uint8_t     cardcheck;          //60�Ƿ�����ƿ 0���������ƿ��1�������ƿ
    uint8_t     usecard;            //61�Ƿ��ÿ�    	0-�ɲ�ʹ�ÿ���1-�ÿ���2-�޿�

    //����������
    uint32_t    modbusbaud;         //62�����Ʋ�����
    uint32_t    pcbaud;             //63��̨������
    uint8_t     fuelmode;           //64����ģʽ 		 0���ֶ���1����̨��2�����ӱ�ǩ
    uint8_t     printmode;          //65СƱ��ӡ��ʽ 0�����Զ���ӡ��1���Զ���ӡ

    //�汾�������
    uint8_t     pricever;           //66���۰汾��
    //uint8_t     blackver;         //67�������汾��

    /* LNG2.7 ֻ�к����� �����Ķ�û�� ADD BY LY*/
    uint8_t     comminfover;        //68ͨ����Ϣ�汾��
    uint8_t     lockver;            //69��Կ�汾�š�

    //��������
    uint32_t    menupwd;            //70�˵���������
    uint32_t    qcpwd;              //71�ʼ�����
    uint32_t    testpwd;            //72�������룬�簴������ŷ��Ȳ���
    uint32_t    resetpwd;           //73�ָ���������
    uint8_t     pwdboard[6];        //74���ܰ�����
    uint8_t     usepwdboard;        //75�Ƿ�ʹ�ü��ܰ� 0����ʹ�ü��ܰ壬1��ʹ�ü��ܰ�
    uint8_t		offlinemode;		//76�ѻ�ģʽ 1���ѻ� 0������
    uint8_t     lcdtype;            //77��Ļѡ��0������1����
    uint8_t     simulation;         //78����ģʽ    1:ģ�������0������������
    uint8_t     intnumflag;         //79�Ƿ�ȡ��    0:����ʱ����ȡ�� 1:����ȡ��
    uint32_t    maxvolumeonce;      //80������������
    uint32_t    maxmoneyonce;       //81�������������
    uint8_t     devicetype;         //82�豸����0�� LNG ������1�� CNG ������2�� ������3�� ж����
    uint32_t	err_id;				//83�쳣��¼��ˮ��
    uint32_t 	uperr_id;			//84δ�ϴ��쳣��¼��ˮ��
    uint8_t		IsPrintCarnum;		//85�Ƿ��ӡ���ƺ� 1����ӡ 0������ӡ
	uint8_t		backup1[20];		//���ã���ֹ���Ӳ�����EEPROM����
	
    //LNG���Ʋ���
    uint16_t    backgascalc;        //201�Ƿ����������0-��������1-����
    uint16_t    precoolmode;        //202Ԥ�䷽ʽ��0-Сѭ����1-��ѭ��
    uint16_t    precooltemp;        //203Ԥ���ٽ��¶� ������
    uint16_t    precoolgain;        //204Ԥ���ٽ�����
    uint16_t    precooldensty;      //205Ԥ���ٽ��ܶȣ�4λС����0-1.0000
    uint16_t    precooltimeout;     //206Ԥ�䳬ʱʱ��
    uint16_t    delaystoptime;      //207�տ�ʼ��Һʱ,���ж�ͣ����������ʱʱ��
    uint16_t    lowratestop;        //208������ͣ��
    uint16_t    highratestop;       //209������ͣ��
    uint16_t    backdenstystop;     //210�����ܶ�ͣ����4λС����0-1.0000
    uint16_t    backgainstop;       //211��������ͣ��
    uint16_t    pressurestop;       //212ͣ��ѹ��
    uint16_t    stoppumptime;       //213��Һ����ʱͣ��
    uint16_t    precoolfinishedtime;//214����Ԥ����ʱ
    uint8_t		modbusaddr_g;		//215���������Ƶ�ַ
    uint8_t		modbusaddr_l;		//216Һ�������Ƶ�ַ
    uint8_t		modbusRS485;		//217 1����Һ��RS485�ߺϲ�Ϊ1,2����Һ��RS485��
    uint8_t		printtimes;			//218 �������� СƱ��ӡ�������� ADD BY LY
	uint8_t		backup2[12];		//���ã���ֹ���Ӳ�����EEPROM����û����N��������backup2��Ҫ����N�����ݣ��������EEPROM����
	
    uint16_t     crc;               //16λ����У��

} Sysparas;

//ȫ�ֱ��� 82�ֽ�
typedef struct GlobalVar
{
    uint8_t         U8Temp;         //��ʱ�洢u8����ֵ
    uint8_t         U16Temp;         //��ʱ�洢u16����ֵ
    uint32_t        U32Temp;        //��ʱ�洢u32����ֵ

    uint32_t        KeyValue;       //����ֵ,ȫ�ֱ���
    uint32_t        KeyPreValue;    //��ǰ����ֵ

    uint8_t         MsgTime;        //������ʾĳЩ��̵���Ϣ
    uint8_t         MsgFlag;        //����1ʱ��msgδ����
    uint8_t         displayonce;    //����1ʱ������ֻ��ʾһ�λ�ִֻ��һ��

    uint8_t         circulatefue;   //ѭ���������á�1-ѭ����0-ֹͣ��
    uint8_t         fuelstep;       //�������衣
    uint32_t        fixnum;         //��������
    uint32_t        fixmoney;       //����
    uint8_t         fix;            //������־��1-��������2-����

    uint8_t         workstate;      //����������״̬
    uint8_t         BeepCount;      //���������ʱ�� 100msΪ��λ
    uint8_t         eepromflag;     //0-����Ҫ������1-��Ҫ����

    uint16_t        paraindex;      //���������š�
    uint32_t        xgtransnum;     //�޸���ˮ�ű��������ڲ�ѯ��
    uint32_t        uptransnum;     //�ϴ���ˮʱ���Ѵ���ˮ��
    uint8_t         UploadIdCount;  //�ϴ���ˮʧ�ܴ�����
    uint8_t         comreceiveflag; //��̨�������־��
    uint8_t         comfailcount;   //��̨������ͨ���Ƿ�������־���������10sδ�յ���̨��Ϣ�����ʾ�ѵ��ߡ�

    uint8_t         modbus_g_error; //��Һʱ�����������Ƴ������
    uint8_t         modbus_l_error; //��Һʱ��Һ�������Ƴ������
    uint8_t         pressure_error; //ѹ���������������

    uint32_t        beforedex;      //�����޸�ǰ��ֵ��
    uint32_t        afterdex;       //�����޸ĺ��ֵ��
    uint8_t         pointnum;       //����С���������

    uint32_t        timeout;        //��ʱʱ��
    uint16_t        stoptimes;      //ͣ�������������
    uint16_t        stoppumptime;   //��Һ��ͣ����ʱ
    uint8_t         otherstate;     //һ��˫��ʱ���Է���Һ��״̬0-�Է���Һ��1-�Է�����
    uint8_t         devicetest;     //����1ʱ�ڽ��е�ŷ�����
    uint8_t         precoolfinished;//Ԥ���Ѿ���ɣ���Һʱֱ�Ӽ�Һ
    uint8_t         finishedtimeout;//Ԥ����ɺ�ʱʱ��

    float           starttotalizeG; //���������ƿ�ʼ����
    float           starttotalizeL; //Һ�������ƿ�ʼ����
    float           pretotalG;      //������������һ�����ۣ������ж�˲ʱ����
    float           pretotalL;      //Һ����������һ�����ۣ������ж�˲ʱ����
} GlobalVar;


// M1 ���ṹ  172�ֽ�
typedef	struct	M1carddata
{
    uint8_t		serialnumber[4];        //�������к�
    uint32_t    key;                    //�û���Ŀ�����
    uint8_t     keyB[6];
    uint8_t     existflag;              //�Ƿ��п����� 1�������룻0�����γ�
    uint8_t     checkflag;              //��֤����־�� 1����֤ͨ����0����֤ʧ�ܣ�
    uint8_t		checkresult;            //����֤�����0xFF:�ȴ���֤��0����֤ͨ����������������� ADD BY LY
    uint16_t    debittimes;             //����ۿ������

    //��1.
    uint8_t		 backUp0[2];			//1
    uint8_t		 cardnum[8];			//����
    uint8_t		 projectNo[2];			//��Ŀ��˾���
    uint8_t		 backUp1[4];
    //��2.
    uint8_t	 	 card[2];				//01����02������
    uint8_t	 	 cardtype[2];			//������(0-9)
    uint8_t 	 pricelevel[2];			//��������(0-9)
    uint8_t 	 greyFlag[2];			//�ҿ���־(1:��������2���ҿ�)
    uint8_t 	 lossFlag[2];			//��ʧ����־��0����������1����ʧ��
    uint8_t 	 backUp2[6];			//����
    //��4.
    uint8_t 	 balancecrc[2];			//�����Ч��
    uint8_t 	 cardbalance[8];		//�����
    uint8_t		 lasttradetime[6];		//���ʹ��ʱ��
    //��5.
    uint8_t		 lastBalanceCrc[2];		//�ϴο����Ч��
    uint8_t		 lastBalance[8];		//�ϴο����
    uint8_t		 lastUseDate[6];		//�ϴ�ʹ��ʱ��
    //��6.
    uint8_t		 payMoneyCrc[2];		//�������У��
    uint8_t		 payMoney[8];			//���������ֵ���
    uint8_t		 operateType[2];		//����ģʽ 0��û�в�����1��������2����ֵ��3���ۿ4��ת�棻5���һ���6�����
    uint8_t		 backUp3[4];
    //��9.
    uint8_t		 block9[16];			//����
    //��10.
    uint8_t		 block10[16];			//����

    //��13.
    uint8_t		 cylindersdate[6];		//��ƿ��������
    uint8_t		 carnum[4];				//���ƺź���λ������
    uint8_t		 backUp4[6];			//Ԥ��
    //��14.
    uint8_t		 backUp5[6];			//14
    uint8_t		 gunnum;				//���һ�μ���ǹ�� HEX
    uint8_t		 transnum[4];			//���һ�μ�����ˮ�� HEX
    uint8_t		 laststationcode;		//����������վ���
    uint8_t		 backUp6[4];
    //��16.
    uint8_t block16[16];			//16
    //��17.
    uint8_t block17[16];			//17
    //��18.
    uint8_t block18[16];//18

    /******* ���ϸ��鶨�������տ�Э���д����ò��������޸� ADD BY LY ********/

    uint8_t		cardtype_another;		   //������
    uint32_t 	carnum_another;			   //���ƺ�
    uint32_t	cardnum_another;		   //����(�����ڿ����кţ������к��ǿ�����ʱ�����趨��)ADD BY LY
    uint16_t 	pricelevel_another;		   //���۵Ǽ�
    uint32_t	currentprice;			   //�˿���ǰ����
    uint8_t 	createcardtime[7];		   //����ʱ��YYYYMMDDHHMMSS
    uint8_t 	lastoptmode;			   //���һ�β�����ʽ
    uint8_t	    stationcode_another;	   //���һ�μ���վ����
    uint32_t	transnum_another;		   //������ˮ��

    uint32_t	trademoney; 			   //��������� 2λС��
    uint32_t	cardbalance_another;	   //�����  2λС��
    uint32_t	sumtimes;				   //�ۼ����Ѵ���
    uint32_t	summoney;				   //�ۼ����ѽ��  2λС��
    uint32_t	sumvolme;				   //�ۼ���������  2λС��
    uint32_t	sumchargetimes; 		  //�ۼƳ�ֵ����
    uint32_t	sumchargemoney; 		  //�ۼƳ�ֵ���
    uint32_t	cardinitmoney;			  //���ڳ����
    uint32_t	cardcalmoney;			  //���������

    uint8_t 	newkeyb[6];				  //����B��
} M1carddata;

extern M1carddata       m1card;				//���ṹ���ݡ�
extern uint32_t         syserr;				//ϵͳ�쳣
extern uint32_t         stopreason;			//ͣ��ԭ��
extern uint32_t		 	carderror;			//��IC�������йصĴ���
extern GlobalVar        globalvar;			//ȫ�ֱ���
extern ShiftRecordInfo  shiftrecordinfo;	//���°��¼
extern FuelRecordInfo   fuelrecordinfo;    	//��ˮ��¼��Ϣ
extern FuelRecordInfo   fuelrecordinfo_tmp;    	//��ˮ��¼��Ϣ
extern ErrorRecordInfo  errorRecordinfo;	//������ˮ
extern Sysparas         sysparas;			//ϵͳ����
extern Sysparas         tmp_sysparas;		//�û��޸�ϵͳ����ʱ����ʱ�洢��Ҫ�޸ĵĲ���

#endif

bool 	SetLossCard(uint32_t cid, bool bset);
uint8_t ReadLossCard(uint32_t cid);
void 	ClearLossCard(void);
uint8_t WriteLossCardFlag(void);

