#include "ForSClientMsgHandler.h"
#include "../DyNetSocket/NetSocket.h"
#include "../DyNetSocket/NetClient.h"
#include "SharedIncludes.h"
#include "LogManager.h"
#include "BaseDefine.h"
#include "ProcWorldHandler.h"
#include "ServerSession.h"
#include "ClientSession.h"
#include "ServerConnectMgr.h"
#include "ServerClientCommon.h"
#include "ForClientMsgHandler.h"


ForSClientMsgHandler::ForSClientMsgHandler():m_bIsWsMsg(true)
{

#define REGISTER_INTERNAL_MESSAGE(msg_idx,cls,handler)\
	{\
		RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForSClientMsgHandler::handler, this , _1, _2, _3)); \
	}

	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_LOGINED,SSRepLogined,RepLogined);	// 登录结果 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_SERVERINFO_LIST,SSRepServerInfoList,RepServerInfoList);	// 服务器管理信息协议 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_NOFTTY_SERVERID,SSSessionNofitySInfo,NofityClientSessionInfo); // 同步分配服务器ID 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_PING_S,SSRepPingS,RepPingToS); // ping 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_CLIENT_EXIT, SSNofityClientExit, NofityClientExit);

#undef REGISTER_INTERNAL_MESSAGE

// 这里其实fep只作为ws的客户端，也只有ws到client的消息 
#define REGISTER_S2C_MESSAGE(msg_idx,cls)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForSClientMsgHandler::ServerToClient, this, _1, _2, _3)); \
	}
	// from ws 

#undef REGISTER_S2C_MESSAGE

}


ForSClientMsgHandler::~ForSClientMsgHandler()
{
}

// 有客户端连接回调 
void ForSClientMsgHandler::OnNetMsgEnter(NetSocket& rSocket)
{
	if(m_bIsWsMsg)
	{
		ClientCommonHandler::ForWsOnMsgEnter(rSocket, ESERVER_TYPE_FEP);
		m_bIsWsMsg = false;
	}else
	{
		ClientCommonHandler::ForClientToServerOnMsgEnter(rSocket,ESERVER_TYPE_FEP);
	}
}

// 有客户端消息收到 
void ForSClientMsgHandler::OnNetMsg(NetSocket& rSocket, NetMsgHead* pHead,int32 nSize)
{
	ClientCommonHandler::ForWsOnMsg(*this, rSocket, pHead, nSize);
}

// 有客户端断开回调
void ForSClientMsgHandler::OnNetMsgExit(NetSocket& rSocket)
{
	//---------------------------------服务组代码begin-------------------------------
	ClientCommonHandler::ForWsOnExit(rSocket);
	//---------------------------------服务组代码end-------------------------------
}


void ForSClientMsgHandler::RepLogined(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ClientCommonHandler::ForServerLogined(pBaseSession, pMsg);
	//---------------------------------服务组代码end-------------------------------
	ServerSession* pSession = static_cast<ServerSession*>(pBaseSession);
	if(pSession->RemoteServerType() == ESERVER_TYPE_LOG)
	{
		LogManagerMgr::Instance()->Init(pSession,pSession->LocalServerType(),0);
	}
}

void ForSClientMsgHandler::RepServerInfoList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ClientCommonHandler::ForWsReponseServerList(this,pSession,pMsg);
}

void ForSClientMsgHandler::NofityClientSessionInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerUpdataClientUpConnectInfo(pSession,pMsg,ESERVER_TYPE_FEP);
	//---------------------------------服务组代码end-------------------------------
	int32 nClientSessionID = pMsg->nSessionID;
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nClientSessionID);
	if (pClientSession == NULL)
	{
		printf("[ERROR]:Not Found ClientSession\n");
		return;
	}

	int32 nClientStatus = pClientSession->Status();
	if(nClientStatus == E_CLIENT_STATUS_NOTIFY_INITED)
	{
		F2CLoginReadyFinish sMsg;
		pClientSession->SendMsg(&sMsg,sMsg.GetPackLength());
	}

}

void ForSClientMsgHandler::RepPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerTimeOutCancel(pSession);	// 取消超时 
	//---------------------------------服务组代码end-------------------------------
}

void ForSClientMsgHandler::ServerToClient(BaseSession* pSessioin,const NetMsgHead* pHead,int32 nSize)
{

	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pHead->nSessionID);
	if(pClientSession == NULL)
	{
		return;
	}
	pClientSession->SendMsg(const_cast<NetMsgHead*>(pHead),nSize);

}

void ForSClientMsgHandler::NofityClientExit(BaseSession* pSessioin, const NetMsgHead* pMsg, int32 nSize)
{
	const SSNofityClientExit* packet = static_cast<const SSNofityClientExit*>(pMsg);

	// 通知Ws,Ws再通知ls,dp，如果玩家在ss中，则会通知 
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(packet->nSessionID);
	if (pClientSession == NULL)
	{
		ASSERT(pClientSession);
		return;
	}
	pClientSession->Exist();
	
	ServerSession* pServerSession = static_cast<ServerSession*>(pSessioin);
	//pServerSession->GetNetSocket()->AddEventRemoteAfterOnlyMsg()

}
