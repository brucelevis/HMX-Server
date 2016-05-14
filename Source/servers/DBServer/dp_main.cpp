#include "../DyNetSocket/NetIncludes.h"
#include "../DyNetMysql/DbIncludes.h"
#include "SharedIncludes.h"

#include "ServerStatistic.h"
#include "ServerConnectMgr.h"
#include "ForSClientMsgHandler.h"
#include "ForServerMsgHandler.h"

#include "MemoryManger.h"
#include "ClientCommand.h"


/*
*
*	Detail: 程序主入口 
*
*  Created by hzd 2014-11-28
*
*/


char SQL_BUFFER[ MAX_SQL_BUFFER ];

bool gIsRun = true;

DeallineAsync*	gDealCommon = NULL;			// 通用
GameCommand*	gGameCommand = NULL;	// 命令窗口 
DbConnManager*	gDbServerMgr = DbConnManager::Instance(); // db 
NetServer*		gNetServerForServer = NULL; // connected to other sever

bool Init(const std::string& strCfgXml);
bool LoadDataFormDb();
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
		serverCfgFile = "dp_cfg.xml";
	}
	bool bResult = Init(serverCfgFile);
	if(!bResult)
	{
		//FLOG_INFO("Init Fail");
		ASSERT(0);
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
	// TODO，处理停服相关的数据 
}

void InteravalIoUpdate(int32 nSrvTime)
{
	// 数据处理sql请求 
	gDbServerMgr->Update();

	// 处理自己作为其他ss,ls的服务器的IO事件
	gNetServerForServer->Update();

	// 处理自己去连接其他服务器(ws,log)的IO事件
	ServerConnectMgr::Instance()->Update(nSrvTime);

	if(__INTERAVAL_ONE_SECOND__) // 1 秒 
	{
		ClientSessionMgr::Instance()->Update(nSrvTime);
		ServerSessionMgr::Instance()->Update(nSrvTime);
	}

	if(__INTERAVAL_FIVE_SECOND__) // 5 秒 
	{
		ServerStatistic::Instance()->Update();
	}

	if(__INTERAVAL_ONE_MINUTE__) // 1 分 
	{
		MemoryManager::Instance()->Update();
	}

}

bool Init(const std::string& strCfgXml)
{
	bool bResult = NetServerOpt::Instance()->LoadServerConfig(strCfgXml,ESERVER_TYPE_DP);
	if(!bResult)
	{
		printf("Load cfg fail\n");
		return false;
	}

	// 连接数据库  
	DatabaseInfoOpt& rDbInfo = NetServerOpt::Instance()->GetDatabaseInfo();
	IDbBase* pDb = gDbServerMgr->AddDatabase(rDbInfo.strHost.c_str(),rDbInfo.strUser.c_str(),rDbInfo.strPass.c_str(),rDbInfo.strDbname.c_str());
	if(pDb == NULL)
	{
		printf("Connnect to Database fail!\n");
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
		printf("Connect Fail!\n" );
		return false;
	}

	// 加载必备数据库 
	bResult = LoadDataFormDb();
	if(!bResult)
	{
		printf("LoadDataForm Db Fail!\n");
		return false;
	}

	// 初始化ForServer服务   
	ServerInfoOpt& rLocalInfo = NetServerOpt::Instance()->GetLocalServerInfo();
	gNetServerForServer = new NetServer(MAX_SERVER_CONNECTED);
	gNetServerForServer->SetTimeout(SERVER_TIMEOUT);
	gNetServerForServer->SetAddress(rLocalInfo.strHost.c_str(), rLocalInfo.nPort);
	gNetServerForServer->SetHandler(boost::bind(&ForServerMsgHandler::OnNetMsgEnter, ForServerMsgHandler::Instance(), _1),
		boost::bind(&ForServerMsgHandler::OnNetMsg, ForServerMsgHandler::Instance(), _1, _2, _3),
		boost::bind(&ForServerMsgHandler::OnNetMsgExit, ForServerMsgHandler::Instance(), _1));
	gNetServerForServer->Start();

	// 通用   
	gDealCommon = new DeallineAsync(INTEVAL_NETIO_TIME,0,InteravalIoUpdate);
	gDealCommon->Start();

	printf("Server Start Success !\n");

	return true;

}

bool LoadDataFormDb()
{


	return true;
}


