@echo off
echo �˹������XAPI��COM��İ档
echo ���Ҽ����Թ���Ա��ʽ���С�������Ȩ�޲���
@echo on
cd %~dp0
%~d0
RegAsm.exe QuantBox.XAPI.DLL /nologo /codebase /tlb
pause