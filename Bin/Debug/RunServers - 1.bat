@echo off


start "���ݷ�2" DBServer.exe -c dp_cfg_1.xml
ping 127.0.0.1 -n 1 >nul



start "��¼��2" LoginServer.exe -c ls_cfg_1.xml
ping 127.0.0.1 -n 1 >nul



start "������2" GameServer.exe -c ss_cfg_1.xml
ping 127.0.0.1 -n 1 >nul




start "���ط�2" GatewayServer.exe -c fep_cfg_1.xml

ping 127.0.0.1 -n 1 >nul



