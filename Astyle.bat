::-f �����в���صĴ���֮��������
::-p �ڲ��������߲���ո���=��+��-��
::-P ���������߲���ո���-dֻ�������������ո�-Dֻ���������
::-U �Ƴ��������߲���Ҫ�Ŀո�
::-N ��namespace������block��һ��tab������
::-n ��ʽ���󲻶�Դ�ļ�������
::-C ����public,pretected,private�ؼ��֣�һ��tab������
::-S switch��case�ؼ��֣�һ��tab������
::-K switch��case�ؼ��֣�������
::-w ��ʽ�����еĺ궨��
::-l ������ͺ����еĴ�����
::-a �����ű�������һ��
::-x ɾ��������У�3.x�汾��û�У�
@echo off
echo ��ǰ������ȫ·����%~f0
echo �밴�������������ʽ����·�������д���
pause
::for /R ./ %%f in (*.c;*.h) do C:\AStyle\bin\AStyle.exe --style=ansi -f -p -D -U -N -S -n "%%f" 
for /R ./ %%f in (*.c;*.h) do C:\AStyle\bin\AStyle.exe --style=ansi -f -p -U -N -S -n --indent=spaces=4 "%%f" 
pause