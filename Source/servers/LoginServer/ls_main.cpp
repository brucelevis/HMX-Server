#include "../DyNetSocket/NetServer.h"
#include "../DyNetSocket/NetClient.h"

#include "LogManager.h"
#include "DeallineAsync.h"

#include "ServerStatistic.h"

#include "ClientSession.h"
#include "ServerSession.h"
#include "CommonServerOpt.h"
#include "ServerConnectMgr.h"

#include "ServerConnectMgr.h"
#include "ForSClientMsgHandler.h"


/*
*
*	Detail: ��������� 
*
*  Created by hzd 2014-11-28
*
*/

DeallineAsync* gDealCommon;		// ��ʱ��  

bool gIsRun = true;

bool Init(const std::string& strCfgXml);
void ServerStart();
void RunServerExit();
void InteravalIoUpdate(int32 nSrvTime);

int main(int argc, const char * argv[])
{
	// ��ʼ������������Ҫ׼���õ����� 
	std::string serverCfgFile;
	if (argc > 2)
	{
		for (int32 i = 1; i < argc;)
		{
			if (strncmp("-c", argv[i], 2) == 0)
			{
				serverCfgFile = argv[i + 1];
			}
			i += 2;
		}
	}
	else
	{
		serverCfgFile = "ls_cfg.xml";
	}

	if (!Init(serverCfgFile))
	{
		FLOG_ERROR(__FUNCDNAME__, __LINE__, "[SERVER START] STATR INITITION FAIL!");
		return 0;
	}

	// �������� 
	ServerStart();

	// ִ���˳�����ز��� 
	RunServerExit();

	return 0;

}

void ServerStart()
{
	while(gIsRun)
	{
		SSleep(10);
	}
}

void RunServerExit()
{

}

void InteravalIoUpdate(int32 nSrvTime)
{
	// ��ws,fep,dp֮ǰsocket��ʱ����IO��Ϣ��������ϢЭ��Ĵ��� 
	ServerConnectMgr::Instance()->Update(nSrvTime); 

	// ��ws,fep,dp֮ǰ��session��ʱ����,��(ping) 
	ServerSessionMgr::Instance()->Update(nSrvTime);

	// �����(�û�)�Ķ�ʱ�����¼�(��ping����ҵ���߼�) 
	ClientSessionMgr::Instance()->Update(nSrvTime);

	if(__INTERAVAL_ONE_SECOND__)
	{
		
	}
	
	if(__INTERAVAL_FIVE_SECOND__)
	{
		//ServerStatistic::Instance()->Update();
		//int32 nServerNum = ServerSessionMgr::Instance()->CountConnected();
		//int32 nClientNum = ClientSessionMgr::Instance()->ConnectedSessions();
		//printf("Server Info:Server Connected Num:%d , Client Connected Num:%d\n", nServerNum,nClientNum);// ��ʶ���̻߳���������
	}
}

bool Init(const std::string& strCfgXml)
{
	bool bResult = NetServerOpt::Instance()->LoadServerConfig(strCfgXml,ESERVER_TYPE_LS);
	if(!bResult)
	{
		printf("Load cfg fail\n");
		return false;
	}

	// ����WS���� 
	ServerInfoOpt& rWsInfo = NetServerOpt::Instance()->GetRemoteServerInfo();
	bResult = ServerConnectMgr::Instance()->ConnectToServer(rWsInfo.nID, rWsInfo.nType, rWsInfo.strHost.c_str(), rWsInfo.nPort,
		boost::bind(&ForSClientMsgHandler::OnNetMsgEnter, ForSClientMsgHandler::Instance(), _1),
		boost::bind(&ForSClientMsgHandler::OnNetMsg, ForSClientMsgHandler::Instance(), _1, _2,_3),
		boost::bind(&ForSClientMsgHandler::OnNetMsgExit, ForSClientMsgHandler::Instance(), _1),
		boost::bind(&ForSClientMsgHandler::OnEventRemoteClose, ForSClientMsgHandler::Instance(), _1, _2),
		boost::bind(&ForSClientMsgHandler::OnEventRemotePreMsg, ForSClientMsgHandler::Instance(), _1, _2),
		boost::bind(&ForSClientMsgHandler::OnEventRemoteAfterMsg, ForSClientMsgHandler::Instance(), _1, _2),
		boost::bind(&ForSClientMsgHandler::OnEventRemotePreOnlyMsg, ForSClientMsgHandler::Instance(), _1, _2),
		boost::bind(&ForSClientMsgHandler::OnEventRemoteAfterOnlyMsg, ForSClientMsgHandler::Instance(), _1, _2)
		);
	if(!bResult)
	{
		printf("Connect Fail!\n" );
		return false;
	}

	// ������ʱ�� 
	gDealCommon = new DeallineAsync(INTEVAL_COMMON_TIME, 0, InteravalIoUpdate);
	gDealCommon->Start();

	printf("Server Start Success !\n");

	return true;

}

