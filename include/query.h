#ifndef __QUERY_H
#define __QUERY_H

//���۲�ѯ��ʾ
void DispSum(void);

//����������ʾ������ݱ���¼���ݶ�ȡ��ѯ
void DispShiftSum(void);

//��ˮ��¼��ѯ��ʾ
void DispRecord(void);

//ϵͳ��Ϣ��ʾ1
void DispSysParas1(void);

//ϵͳ��Ϣ��ʾ2
void DispSysParas2(void);

//ϵͳ��Ϣ��ʾ3
void DispSysParas3(void);

//ϵͳ��Ϣ��ʾ4
void DispSysParas4(void);

//ϵͳ��Ϣ��ʾ5
void DispSysParas5(void);

//ϵͳ��Ϣ��ʾ6
void DispSysParas6(void);

//ϵͳ��Ϣ��ʾ7
void DispSysParas7(void);

//ϵͳ��Ϣ��ʾ8
void DispSysParas8(void);


//��ѯ�˵���ʾ1
void DispQuery1(void);

//��ѯ�˵���ʾ2
void DispQuery2(void);

//���������ˮ��
void DispInputShifTransNum(void);

//����ˮ��¼��ѯ��ʾ
void DispshifRecord(void);

//����ˮ���¼���ѯ
void RecordShifUp(void);

//����ˮ���ϼ���ѯ
void RecordShifDown(void);

void DispHeiBai(void);

void DispHei(void);

void DispHeiAdd(void);

void DispHeiDel(void);

void DispBai(void);

//�������޸���ˮ��
void DispInputXGNum(void);
//�޸���ˮ��¼��ѯ��ʾ
void DispXGRecord(void);
//�޸���ˮ���¼���ѯ
void RecordXGUp(void);
//�޸���ˮ���ϼ���ѯ
void RecordXGDown(void);

void Printerid(void);

void Printershifid(void);

void DebugHeiBai(uint32_t addr, uint32_t num, uint8_t *ptr);

void DispInputTransNum(void);
void DispSystemVer();

#endif

