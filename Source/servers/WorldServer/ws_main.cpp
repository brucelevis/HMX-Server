#include "WorldServer_PCH.h"
#include "NetIncludes.h"
#include "SharedIncludes.h"

#include "ForServerMsgHandler.h"

#include "SortsManager.h"

/*
*
*  Detail: ws程序主入口
*  (ls|fep|ss)->ws<-dp 
*  Created by hzd 2014-11-28
*  TODO 可以后台调CPU使用方面的调优
*/

NetServer*		gNetServer;
DeallineAsync*	gDealCommon;

bool gIsRun = true;

bool Init();
void ServerStart();
void RunServerExit();
void InteravalIoUpdate(int32 nSrvTime);

int main(int argc, const char * argv[])
{

	SortsManager::Instance()->TestData();

	// 初始化服务器所都要准备好的数据  
	if(!Init())
	{
		return 0;
	}

	// 启动服务 
	ServerStart();

	// 执行退出的相关操作 
	RunServerExit();

	return 0;

}


bool Init()
{
	bool bResult = NetServerOpt::Instance()->LoadServerConfig("ws_cfg.xml",ESERVER_TYPE_WS);
	if(!bResult)
	{
		printf("Load ws_cfg.xml fail!\n");
		return false;
	}

	// 初始化Scoket服务 
	ServerInfoOpt& rLocalInfo = NetServerOpt::Instance()->GetLocalServerInfo();
	gNetServer = new NetServer(MAX_WS_SOCKET_CONNECT);
	gNetServer->SetTimeout(SERVER_TIMEOUT);
	gNetServer->SetAddress(rLocalInfo.strHost.c_str(), rLocalInfo.nPort);
	gNetServer->SetHandler(boost::bind(&ForServerMsgHandler::OnNetMsgEnter, ForServerMsgHandler::Instance(), _1),
		boost::bind(&ForServerMsgHandler::OnNetMsg, ForServerMsgHandler::Instance(), _1, _2, _3), 
		boost::bind(&ForServerMsgHandler::OnNetMsgExit, ForServerMsgHandler::Instance(), _1));

	// 设置事件触发绑定 
	gNetServer->SetEventRemoteClose(boost::bind(&ForServerMsgHandler::OnEventRemoteClose,ForServerMsgHandler::Instance(),_1,_2));
	gNetServer->SetEventRemotePreMsg(boost::bind(&ForServerMsgHandler::OnEventRemotePreMsg,ForServerMsgHandler::Instance(),_1,_2));
	gNetServer->SetEventRemoteAfterMsg(boost::bind(&ForServerMsgHandler::OnEventRemoteAfterMsg,ForServerMsgHandler::Instance(),_1,_2));
	gNetServer->SetEventRemotePreOnlyMsg(boost::bind(&ForServerMsgHandler::OnEventRemotePreOnlyMsg,ForServerMsgHandler::Instance(),_1, _2));
	gNetServer->SetEventRemoteAfterOnlyMsg(boost::bind(&ForServerMsgHandler::OnEventRemoteAfterOnlyMsg,ForServerMsgHandler::Instance(),_1, _2));

	// 启动Socket服务 
	gNetServer->Start();

	// 启动定时器 
	gDealCommon = new DeallineAsync(INTEVAL_COMMON_TIME,0,InteravalIoUpdate);
	gDealCommon->Start();

	printf("Server Start Success !\n");

	return true;

}

void ServerStart()
{
	while(gIsRun)
	{
		SSleep(10);
	}
}

void InteravalIoUpdate(int32 nSrvTime)
{
	// 处理自己作为服务器，处理fep,ls,ss,dp的IO事件 
	gNetServer->Update();

	// 与ws,fep,dp之前的session定时处理,如(ping) 
	ServerSessionMgr::Instance()->Update(nSrvTime);

	ClientSessionMgr::Instance()->Update(nSrvTime);

	if(__INTERAVAL_FIVE_SECOND__)
	{
		//Statistic::Instance()->Update(nSrvTime);
		//int32 nServerNum = ServerSessionMgr::Instance()->CountConnected();
		//int32 nClientNum = ClientSessionMgr::Instance()->ConnectedSessions();
		//printf("Server Info:Server Connected Num:%d , Client Connected Num:%d\n", nServerNum,nClientNum);	// 标识主线程还在做事中
	}

}


void RunServerExit()
{

}




