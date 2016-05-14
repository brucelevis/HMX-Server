#include "Client_PCH.h"
// clientCtl.cpp : 定义控制台应用程序的入口点。
//

#include "ClientCommand.h"
#include "NetMsgHandler.h"
#include "ClientPlayer.h"

#include "DeallineAsync.h"


DeallineAsync* gDealCommon = NULL;	// 定时器 

bool bExist = false;

GameCommand arrGameCommand[MAX_CLIENT_COUNT];

void ClientInit();
void ClientRun();
void RunClientExit();
void InteravalIoUpdate(int32 nSrvTime);

int main(int argc, const char * argv[])
{
	ClientInit();
	ClientRun();
	RunClientExit();
	return 0;
}

void InteravalIoUpdate(int32 nSrvTime)
{
	ClientPlayerMgr::Instance()->CheckConnected();
	ClientPlayerMgr::Instance()->UpdateIO();
	ClientPlayerMgr::Instance()->Update(nSrvTime);
}

void ClientInit()
{
	 InitNetMsg();

	 using namespace boost::property_tree;
	 ptree cPtreeDoc;
	 ptree cPtreeDocItems;
	 ptree cRemoteInfo;

	 read_xml("c_cfg.xml", cPtreeDoc, boost::property_tree::xml_parser::trim_whitespace);
	 cPtreeDocItems = cPtreeDoc.get_child("items");

#define XMLATTR(KEY) "<xmlattr>."#KEY 

	 cRemoteInfo = cPtreeDocItems.get_child("remote");
	 uint32 nID = cRemoteInfo.get<uint32>(XMLATTR(id));
	 string strHost = cRemoteInfo.get<string>(XMLATTR(host));
	 uint32 nPort = cRemoteInfo.get<uint32>(XMLATTR(port));
	 uint32 nType = cRemoteInfo.get<uint32>(XMLATTR(type));

#undef XMLATTR

	 for (int32 i = 0; i < MAX_CLIENT_COUNT; ++i)
	 {
		 NetClient* gNetClient = new NetClient();
		 gNetClient->SetAddress(strHost.c_str(), nPort);
		 gNetClient->SetHandler(OnNetMsgEnter, OnNetMsg, OnNetMsgExit);
		 gNetClient->Start();

		 ClientPlayer* gPlayer = new ClientPlayer();
		 gPlayer->SetSocket(gNetClient->GetSocket());

         ClientPlayerMgr::Instance()->AddSocket(gNetClient,gPlayer);

		 // 命令调试  
		 arrGameCommand[i].Init(gNetClient->GetSocket(),gPlayer);

		 SSleep(10);
	 }

	 // 启动定时器 MAIN_INTEVAL_TIME
	 gDealCommon = new DeallineAsync(50, 0, InteravalIoUpdate);
	 gDealCommon->Start();

	 for (int32 i = 0 ; i < MAX_CLIENT_COUNT; ++i)
	 {
		 arrGameCommand[i].CommandHelp();
	 }

}

void ClientRun()
{
	int32 nRunTimes = 0;
	while (true)
	{
		if(bExist)
			break;

		nRunTimes++;
		if(nRunTimes == 100)
		{
			SSleep(1);
			nRunTimes = 0;
		}

		for (int32 i = 0 ; i < MAX_CLIENT_COUNT; ++i)
		{
			arrGameCommand[i].Command(0.0);
		}	
		
	}

}

void RunClientExit()
{
	// 保存 & 释放资源   

}