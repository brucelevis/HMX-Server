// ��Ȩ���д�������qq296464231���У���������ڸ��ƣ��޸ģ���ҵĿ�ĵȣ�������ȥ����Ȩ���б�ʶ�������ļ�ͷ����ע������ 

����������Ҳ����Ҫx64
boost 1.60-x64 http://pan.baidu.com/s/1pL6DheN
mysql 5.56-x64 http://pan.baidu.com/s/1nu5KZsH


��װ���ۣ�
��Github���غ󣬽���Ŀ����E:/NEW_SERVER/Ŀ¼�£��Ժ��Ż���ȡ��������ƣ�ԭ������˵����
�Ӱٶ����������ұ���õ�Boost����Mysql�⣬������Ҫע����ǣ�����Ŀ��x64�ģ���������win7-64���ϻ������ԣ�mysqlҪ��װ5.6-x64����
����Source/sql/swa_data.sql �ļ� ��������Ϊʲô��swa_*** ��ԭ��������swa-server��Ŀ��ԭ���Ǹ���Ŀ�Ѿ���ά���ˣ�

����Ŀ�ļ���Ҫ����·�����޸�Ĭ��·��
CommonDefine.h
#define CFG_DIR		"../../Config/"
#define RES_DIR		"E:/NEW_SERVER/Resource/"
#define ZONE_DIR	"E:/NEW_SERVER/Resource/Zone/"
#define SCENE_DIR	"E:/NEW_SERVER/Resource/Scene/"

Ŀ¼�ṹ��
E:/NEW_SERVER/BIN/
E:/NEW_SERVER/BIN/DEBUG
E:/NEW_SERVER/Resource
E:/NEW_SERVER/Resource/Scene
E:/NEW_SERVER/Resource/Zone
E:/NEW_SERVER/Source
E:/NEW_SERVER/Source/dependence/boost  ����boost��ѹ�������Ŀ¼
E:/NEW_SERVER/Source/dependence/mysql  ����mysql��ѹ�������Ŀ¼
E:/NEW_SERVER/Source/doc			   ��Ŀ��һЩ��ʱ����˼���ĵ�
E:/NEW_SERVER/Source/sql			   ���뵽���ݿ�
E:/NEW_SERVER/Source/servers		   ���̷���������


������Ŀ�ļ�.sln��ѡ�ã�
�����ͨ����Ŀ�ļ����д�����Ŀ��Ҳ����ͨ��Cmake��������Ŀ����������Ҫ�յ��ֹ��ѣ�͵������������Ҫ9.9�飬�����뿴:http://www.hmx-server.org��

���ɺú���Bin/Debug/Ŀ¼��������˫��RunServer.bat��������������̣���ʱҪ����������֮ǰ�ı������ӡ����[ERROR]:xxxx
����������Client.exe 
��������ԵĹ����л�ż������assert��ʾ�����̻�ҵ������Խ׶����������ҵ��������֪�����������Ҫ�Ż���
����Խ�ASSERT����궨��ֵ assert ��Ϊ printf("assert\n");�Ͳ���Ӱ���㿪��������ˣ������Ҽ������ƣ���ЩASSERT������ȥ��

cmake ����Ϊx64 vs
cmake 3.4 ����

vs����Ҳ��Ҫ����Ϊx64
vs2012 ����



