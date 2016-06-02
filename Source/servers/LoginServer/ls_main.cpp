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
*	Detail: 程序主入口 
*
*  Created by hzd 2014-11-28
*
*/

DeallineAsync* gDealCommon;		// 定时器  

bool gIsRun = true;

bool Init(const std::string& strCfgXml);
void ServerStart();
void RunServerExit();
void InteravalIoUpdate(int32 nSrvTime);

int main(int argc, const char * argv[])
{
	// 初始化服务器所都要准备好的数据 
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

	// 启动服务 
	ServerStart();

	// 执行退出的相关操作 
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
	// 与ws,fep,dp之前socket定时处理IO消息，包括消息协议的处理 
	ServerConnectMgr::Instance()->Update(nSrvTime); 

	// 与ws,fep,dp之前的session定时处理,如(ping) 
	ServerSessionMgr::Instance()->Update(nSrvTime);

	// 与玩家(用户)的定时处理事件(如ping，非业务逻辑) 
	ClientSessionMgr::Instance()->Update(nSrvTime);

	if(__INTERAVAL_ONE_SECOND__)
	{
		
	}
	
	if(__INTERAVAL_FIVE_SECOND__)
	{
		//ServerStatistic::Instance()->Update();
		//int32 nServerNum = ServerSessionMgr::Instance()->CountConnected();
		//int32 nClientNum = ClientSessionMgr::Instance()->ConnectedSessions();
		//printf("Server Info:Server Connected Num:%d , Client Connected Num:%d\n", nServerNum,nClientNum);// 标识主线程还在做事中
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

	// 连接WS服务 
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

	// 启动定时器 
	gDealCommon = new DeallineAsync(INTEVAL_COMMON_TIME, 0, InteravalIoUpdate);
	gDealCommon->Start();

	printf("Server Start Success !\n");

	return true;

}

