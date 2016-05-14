#include "ServerStatistic.h"
#include "ClientSession.h"
#include "ServerSession.h"

#include "CommonInternal.h"

ServerStatistic::ServerStatistic(void)
{
}


ServerStatistic::~ServerStatistic(void)
{
}

void ServerStatistic::Update()
{
	ServerSession* pWs = ServerSessionMgr::Instance()->GetWsSession();
	if(pWs == NULL)
	{
		return;
	}

	SSNotifyConnectInfo sMsg;

	int32 nServerNum = ServerSessionMgr::Instance()->CountConnected();
	int32 nClientNum = ClientSessionMgr::Instance()->ConnectedSessions();

	sMsg.nServerLoad = nServerNum;
	sMsg.nClientLoad = nClientNum;

	pWs->SendMsg(&sMsg,sMsg.GetPackLength());
}


