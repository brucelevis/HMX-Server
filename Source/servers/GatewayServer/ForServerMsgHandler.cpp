#include "ForServerMsgHandler.h"
#include "../DyNetSocket/NetSocket.h"
#include "../DyNetSocket/NetClient.h"

#include "LogManager.h"
#include "BaseDefine.h"
#include "ServerSession.h"
#include "ClientSession.h"
#include "ServerSession.h"
#include "SharedIncludes.h"
#include "ServerClientCommon.h"

ForServerMsgHandler::ForServerMsgHandler()
{

	// 内部协议 
#define REGISTER_INTERNAL_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForServerMsgHandler::handler, this, _1, _2, _3)); \
}

	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_LOGIN_S, SSReqLoginS, ReqLogined);	// 请求登录 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_PING_S,SSReqPingToS,ReqPingToS);
	REGISTER_INTERNAL_MESSAGE(PRO_SS_NOFTTY_SERVERID,SSSessionNofitySInfo,NofityClientSessionInfo); // 同步分配服务器ID 

#undef REGISTER_INTERNAL_MESSAGE

#define REGISTER_S2C_MESSAGE(msg_idx,cls)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForServerMsgHandler::ServerToClient, this, _1, _2, _3)); \
	}

	// from ss
	REGISTER_S2C_MESSAGE(PRO_S2C_CHAR_WORLD,		S2CRepCharWorld);	// 
	REGISTER_S2C_MESSAGE(PRO_S2C_SCENE_INIT_RESULT,	S2CSceneInitResult);
	REGISTER_S2C_MESSAGE(PRO_S2C_CHAR_MAIN_DATA,	S2CCharacterMainData);
	REGISTER_S2C_MESSAGE(PRO_S2C_SEND_DATA_FINISH,	S2CSendDataFinish);
	REGISTER_S2C_MESSAGE(PRO_S2C_QUEST_MAIN_DATA,	S2CQuestMainData);
	REGISTER_S2C_MESSAGE(PRO_S2C_SCENE_ENTER_INFO,	S2CSceneEnterInfo);

	// from ls
	REGISTER_S2C_MESSAGE(PRO_L2C_ACCOUNT_LOGIN,		L2CAccountLogin);
	REGISTER_S2C_MESSAGE(PRO_L2C_CHARACTER_LIST,	L2CCharacterList);
	REGISTER_S2C_MESSAGE(PRO_L2C_ROLE_CREATE_RESULT,L2CRoleCreateResult);
	REGISTER_S2C_MESSAGE(PRO_L2C_NAMES_LIST,		L2CNamesList);

#undef REGISTER_S2C_MESSAGE

}


ForServerMsgHandler::~ForServerMsgHandler()
{
}

// 有客户端连接回调 
void ForServerMsgHandler::OnNetMsgEnter(NetSocket& rSocket)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerOnMsgEnter(rSocket,ESERVER_TYPE_FEP);
	//---------------------------------服务组代码end-------------------------------
}

// 有客户端消息收到 
void ForServerMsgHandler::OnNetMsg(NetSocket& rSocket, NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerOnMsg(*this,rSocket, pMsg, nSize);
	//---------------------------------服务组代码end-------------------------------
}

// 有客户端断开回调(此时socket已经回收) 
void ForServerMsgHandler::OnNetMsgExit(NetSocket& rSocket)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerOnMsgExit(rSocket);
	//---------------------------------服务组代码end-------------------------------
}

void ForServerMsgHandler::ReqLogined(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerOnLogin(pSession, pMsg);
	//---------------------------------服务组代码end-------------------------------
}


// 收到ping请求信息 
void ForServerMsgHandler::ReqPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
	SSRepPingS sMsg;
	pServerSession->SendMsg(&sMsg,sMsg.GetPackLength());
}

void ForServerMsgHandler::ServerToClient(BaseSession* pSessioin,const NetMsgHead* pHead,int32 nSize)
{

	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pHead->nClientSessionID);
	if(pClientSession == NULL)
	{
		ASSERT(0);
		FLOG_ERROR(__FUNCTION__,__LINE__,"Not Found ClientSession id: %d",pHead->nClientSessionID);
		return;
	}
	pClientSession->SendMsg(const_cast<NetMsgHead*>(pHead),nSize);

}

void ForServerMsgHandler::NofityClientSessionInfo(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerUpdataClientUpConnectInfo(pSession,pMsg,ESERVER_TYPE_FEP);
	//---------------------------------服务组代码end-------------------------------
	int32 nClientSessionID = pMsg->nClientSessionID;
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nClientSessionID);
	if (pClientSession == NULL)
	{
		printf("Not Found ClientSession\n");
		return;
	}
	int32 nClientStatus = pClientSession->Status();
	if(nClientStatus == E_CLIENT_STATUS_IN_SCENE)
	{
		F2CEnterSceneLoaded sMsg;
		sMsg.nResult = F2CEnterSceneLoaded::SUCCESS;
		pClientSession->SendMsg(&sMsg,sMsg.GetPackLength());
	}
}
