@echo off


start "�����" WorldServer.exe
ping 127.0.0.1 -n 1 >nul
start "���ݿ��" DBServer.exe
ping 127.0.0.1 -n 1 >nul
start "������" GameServer.exe
ping 127.0.0.1 -n 1 >nul
start "��¼��" LoginServer.exe
ping 127.0.0.1 -n 1 >nul
start "���ط�" GatewayServer.exe
ping 127.0.0.1 -n 1 >nul


