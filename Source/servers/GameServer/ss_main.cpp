#include "GameServer_PCH.h"
#include "NetIncludes.h"
#include "SharedIncludes.h"

#include "SceneMap.h"
#include "SceneUser.h"
#include "SceneUserManager.h"

#include "ResourceMgr.h"
#include "ForSClientMsgHandler.h"

/*-------------------------------------------------------------------
 * @Brief : 本程序主要设计方向为：以世界地图为设计中心，其他功能都基于
 *			地图(其他框架可能基于主角)，其他物体存放于地图上，以地图向
 *          内外扩展功能，详细看SceneMap类 
 * @Author:hzd 2015:11:10
 *------------------------------------------------------------------*/


/*
*
*	Detail: 程序主入口 
*
*  Created by hzd 2014-11-28 
*
*/

NetClient*	gNetClientForWs;	// connected to ws 
DeallineAsync* gDealCommon;		// 共用定时器 
DeallineAsync* gDealNetIo;	// IO定时器	 
DeallineAsync* gDealSceneMap;	// 场景定时器

bool gIsRun = true;

bool Init(const std::string& strCfgXml);
void ServerStart();
void RunServerExit();
void InteravalIoUpdate(int32 nSrvTime);
void InteravalNetIoUpdate(int32 nSrvTime);
void InteravalSceneMapUpdate(int32 nSrvTime);

//参数 -c cfg.xml -l 日志文件   
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
		serverCfgFile = "ss_cfg.xml";
	}

	if (!Init(serverCfgFile))
	{
		FLOG_ERROR(__FUNCDNAME__,__LINE__,"[SERVER START] STATR INITITION FAIL!");
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
	while (gIsRun)
	{
		SSleep(10);
	}
}

void RunServerExit()
{

}

void InteravalIoUpdate(int32 nSrvTime)
{
	if(__INTERAVAL_ONE_SECOND__)
	{
		
	}
	
	if(__INTERAVAL_FIVE_SECOND__) // 5 seconds 
	{
		//int32 nServerNum = ServerSessionMgr::Instance()->CountConnected();
		//int32 nClientNum = ClientSessionMgr::Instance()->ConnectedSessions();
		//FLOG_INFO("Server Info:Server Connected Num:%d , Client Connected Num:%d", nServerNum,nClientNum);	// 标识主线程还在做事中
	}
}

void InteravalNetIoUpdate(int32 nSrvTime)
{
	// 与ws,fep,dp之前socket定时处理IO消息，包括消息协议的处理 
	ServerConnectMgr::Instance()->Update(nSrvTime);

	// 与ws,fep,dp之前的session定时处理,如(ping) 
	ServerSessionMgr::Instance()->Update(nSrvTime); 

	// 与玩家(用户)的定时处理事件(如ping，非业务逻辑) 
	ClientSessionMgr::Instance()->Update(nSrvTime);

	// 玩家定时处理有需要的时间事件(业务逻辑)
	SceneUserManager::Instance()->Update(nSrvTime);
	
}

void InteravalSceneMapUpdate(int32 nSrvTime)
{
	SceneMapManager::Instance()->Update(nSrvTime);
}

bool Init(const std::string& strCfgXml)
{
	bool bResult = NetServerOpt::Instance()->LoadServerConfig(strCfgXml,ESERVER_TYPE_SS);
	if(!bResult)
	{
		printf("Load ss_cfg.xml fail\n");
		return false;
	}

	// 加载配置 
	bool bResultC = ResourceMgr::Instance()->LoadAllResources();
	if(!bResultC)
	{
		printf("Load LoadAllResources fail\n");
		return false;
	}

	// 连接WS服务 
	ServerInfoOpt& rWsInfo = NetServerOpt::Instance()->GetRemoteServerInfo();
	bResult = ServerConnectMgr::Instance()->ConnectToServer(rWsInfo.nID, rWsInfo.nType, rWsInfo.strHost.c_str(), rWsInfo.nPort,
		boost::bind(&ForSClientMsgHandler::OnNetMsgEnter, ForSClientMsgHandler::Instance(), _1),
		boost::bind(&ForSClientMsgHandler::OnNetMsg, ForSClientMsgHandler::Instance(), _1, _2, _3),
		boost::bind(&ForSClientMsgHandler::OnNetMsgExit, ForSClientMsgHandler::Instance(), _1),
		boost::bind(&ForSClientMsgHandler::OnEventRemoteClose,ForSClientMsgHandler::Instance(),_1,_2),
		boost::bind(&ForSClientMsgHandler::OnEventRemotePreMsg,ForSClientMsgHandler::Instance(),_1,_2),
		boost::bind(&ForSClientMsgHandler::OnEventRemoteAfterMsg,ForSClientMsgHandler::Instance(),_1,_2),
		boost::bind(&ForSClientMsgHandler::OnEventRemotePreOnlyMsg,ForSClientMsgHandler::Instance(),_1,_2),
		boost::bind(&ForSClientMsgHandler::OnEventRemoteAfterOnlyMsg,ForSClientMsgHandler::Instance(),_1,_2)
		);
	if (!bResult)
	{
		printf("Connect Fail!\n");
		return false;
	}

	// 通用 
	gDealCommon = new DeallineAsync(INTEVAL_COMMON_TIME, 0, InteravalIoUpdate);
	gDealCommon->Start();

	// 网络IO
	gDealNetIo = new DeallineAsync(INTEVAL_NETIO_TIME, 0,InteravalNetIoUpdate);
	gDealNetIo->Start();

	// 场景 
	gDealSceneMap = new DeallineAsync(INTEVAL_SCENEMAP_TIME,0,InteravalSceneMapUpdate);
	gDealSceneMap->Start();

	printf("Server Start Success !\n");

	return true;
}

