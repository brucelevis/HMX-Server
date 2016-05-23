#include "../DyNetSocket/NetServer.h"
#include "../DyNetSocket/NetClient.h"

#include "LogManager.h"
#include "DeallineAsync.h"

#include "ClientSession.h"
#include "ServerSession.h"
#include "CommonServerOpt.h"
#include "ServerConnectMgr.h"
#include "ServerStatistic.h"

#include "ForSClientMsgHandler.h"
#include "ForServerMsgHandler.h"
#include "ForClientMsgHandler.h"

/*
*
*	Detail: 程序主入口 
*   
*  Created by hzd 2014-11-28
*
*/

NetServer*	gNetServerForServer;//for ls,ss and so on
NetServer*	gNetServerForClient;
DeallineAsync* gDealCommon;		// 定时器  

bool gIsRun		= true;

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
		serverCfgFile = "fep_cfg.xml";
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
	while(true)
	{	
		SSleep(10);
	}
}

void RunServerExit()
{
	
}

void InteravalIoUpdate(int32 nSrvTime)
{
	// 处理自己作为其他ss,ls的服务器的IO事件
	gNetServerForServer->Update();

	// 处理自己作为Client（用户）的服务器的IO事件 
	gNetServerForClient->Update();

	// 处理自己去连接其他服务器(ws,log)的IO事件
	ServerConnectMgr::Instance()->Update(nSrvTime);

	// 定时处理有根据server之间的事件，如:ping 
	ServerSessionMgr::Instance()->Update(nSrvTime);

	// 定时处理用户事件，如心跳包 
	ClientSessionMgr::Instance()->Update(nSrvTime);

	if(__INTERAVAL_FIVE_SECOND__)
	{
		//ServerStatistic::Instance()->Update();// 收集信息 
	}

	if(__INTERAVAL_FIVE_SECOND__)
	{
//		int32 nServerNum = ServerSessionMgr::Instance()->CountConnected();
//		int32 nClientNum = gNetServerForClient->ConnectedSockets();
//		FLOG_INFO("Server Info: Server Connected Num:%d ,Client Connected Num:%d", nServerNum, nClientNum);// 标识主线程还在做事中
	}
}

bool Init(const std::string& strCfgXml)
{
	
	bool bResult = NetServerOpt::Instance()->LoadServerConfig(strCfgXml,ESERVER_TYPE_FEP);
	if(!bResult)
	{
		printf("[ERROR]:Load cfg fail\n");
		return false;
	}

	// 连接WS服务 
	ServerInfoOpt& rWsInfo = NetServerOpt::Instance()->GetRemoteServerInfo();
	bResult = ServerConnectMgr::Instance()->ConnectToServer(rWsInfo.nID, rWsInfo.nType, rWsInfo.strHost.c_str(), rWsInfo.nPort,
		boost::bind(&ForSClientMsgHandler::OnNetMsgEnter, ForSClientMsgHandler::Instance(), _1),
		boost::bind(&ForSClientMsgHandler::OnNetMsg, ForSClientMsgHandler::Instance(), _1, _2,_3),
		boost::bind(&ForSClientMsgHandler::OnNetMsgExit, ForSClientMsgHandler::Instance(), _1));
	if(!bResult)
	{
		printf("[ERROR]:Connect Fail!\n" );
		return false;
	}

	// 初始化ForServer服务 
	ServerInfoOpt& rLocalInfo = NetServerOpt::Instance()->GetLocalServerInfo();
	gNetServerForServer = new NetServer(64);
	gNetServerForServer->SetTimeout(SERVER_TIMEOUT);
	gNetServerForServer->SetAddress(rLocalInfo.strHost.c_str(), rLocalInfo.nPort);
	gNetServerForServer->SetHandler(boost::bind(&ForServerMsgHandler::OnNetMsgEnter, ForServerMsgHandler::Instance(), _1),
		boost::bind(&ForServerMsgHandler::OnNetMsg, ForServerMsgHandler::Instance(), _1, _2,_3),
		boost::bind(&ForServerMsgHandler::OnNetMsgExit, ForServerMsgHandler::Instance(), _1));
	gNetServerForServer->Start();

	// 初始化ForClient服务 
	ServerInfoOpt& rClientInfo = NetServerOpt::Instance()->GetClientServerInfo();
	gNetServerForClient = new NetServer(MAX_FEP_SOCKET_CONNECT);
	gNetServerForClient->SetAddress(rClientInfo.strHost.c_str(),rClientInfo.nPort);
	gNetServerForClient->SetHandler(boost::bind(&ForClientMsgHandler::OnNetMsgEnter,ForClientMsgHandler::Instance(),_1),
		boost::bind(&ForClientMsgHandler::OnNetMsg,ForClientMsgHandler::Instance(),_1,_2,_3),
		boost::bind(&ForClientMsgHandler::OnNetMsgExit,ForClientMsgHandler::Instance(),_1));

	// 启动Socket服务 
	gNetServerForClient->Start();

	// 启动定时器 
	gDealCommon = new DeallineAsync(INTEVAL_COMMON_TIME, 0, InteravalIoUpdate);
	gDealCommon->Start();

	printf("[INFO]:Server Start Success !\n");

	return true;

}




