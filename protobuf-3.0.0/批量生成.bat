@echo off


echo ��ʼ����
CALL protoc.exe  people.proto --cpp_out=.
CALL protoc.exe  character.proto --cpp_out=.
echo �����ļ����
pause
