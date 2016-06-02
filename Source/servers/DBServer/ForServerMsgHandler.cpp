#include "ForServerMsgHandler.h"
#include "../DyNetSocket/NetSocket.h"
#include "../DyNetSocket/NetClient.h"

#include "LogManager.h"
#include "ServerSession.h"
#include "ProcServerHandler.h"
#include "ClientSession.h"
#include "ServerClientCommon.h"

#include "MemoryManger.h"


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
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_CLIENT_EXIT,SSNofityClientExit, ReqClientExitScene);

#undef REGISTER_INTERNAL_MESSAGE

// 来自server的请求协议 
#define REGISTER_SS_MESSAGE(msg_idx,cls,handler)\
	{\
		RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcServerHandler::handler, ProcServerHandler::Instance(), _1, _2, _3)); \
	}

	//----------------for ls----------------------
	REGISTER_SS_MESSAGE(PRO_L2D_NAMES_LIST,L2DNamesList,ReqNamesList);
	REGISTER_SS_MESSAGE(PRO_L2D_ACCOUNT_LOGIN,	L2DAccountLogin,ReqAccountLogin); 	// 用户登录查询 
	REGISTER_SS_MESSAGE(PRO_L2D_ROLE_CREATE,	L2DRoleCreate,	ReqRoleCreate);

	//----------------for ss----------------------
	REGISTER_SS_MESSAGE(PRO_S2D_LOAD_CHARACTER,	S2DLoadCharacter,ReqLoadCharacter);
	REGISTER_SS_MESSAGE(PRO_S2D_SAVE_CHARACTER, S2DSaveUserAllData, ReqSaveCharacter);
	REGISTER_SS_MESSAGE(PRO_S2D_SAVE_MIXITEMNUMBER,S2DSaveMixItemNumber,ReqSaveMixItemNumber);
	



#undef REGISTER_SS_MESSAGE

}


ForServerMsgHandler::~ForServerMsgHandler()
{
}

// 有客户端连接回调 
void ForServerMsgHandler::OnNetMsgEnter(NetSocket& rSocket)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerOnMsgEnter(rSocket,ESERVER_TYPE_DP);
	//---------------------------------服务组代码end-------------------------------
}

// 有客户端消息收到
void ForServerMsgHandler::OnNetMsg(NetSocket& rSocket, NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerOnMsg(*this,rSocket,pMsg,nSize);
	//---------------------------------服务组代码end-------------------------------
}

// 有客户端断开回调(此时socket已经回收) 
void ForServerMsgHandler::OnNetMsgExit(NetSocket& rSocket)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerOnMsgExit(rSocket);
	//---------------------------------服务组代码end-------------------------------
}

void ForServerMsgHandler::ReqLogined(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
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

void ForServerMsgHandler::NofityClientSessionInfo(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerUpdataClientUpConnectInfo(pSession,pMsg,ESERVER_TYPE_FEP);
	//---------------------------------服务组代码end-------------------------------
}


void ForServerMsgHandler::ReqClientExitScene(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	const SSNofityClientExit* packet = static_cast<const SSNofityClientExit*>(pMsg);
	if(packet->nReason != SSNofityClientExit::E_REASON_SWITCH_SCENE)
	{
		int64 nCharID = MemoryManager::Instance()->GetUserIDBySessionID(packet->nSessionID);
		if (nCharID)
		{
			MemoryManager::Instance()->SaveNowByUID(nCharID);
			MemoryManager::Instance()->RemoveByUID(nCharID);
		}
		ClientSessionMgr::Instance()->RemoveSession(packet->nSessionID);
	}
}

void ForServerMsgHandler::OnEventRemoteClose(NetSocket& rSocket, const SocketEvent& stEvent)
{

}

void ForServerMsgHandler::OnEventRemotePreMsg(NetSocket& rSocket, const SocketEvent& stEvent)
{


}

void ForServerMsgHandler::OnEventRemoteAfterMsg(NetSocket& rSocket, const SocketEvent& stEvent)
{

}

void ForServerMsgHandler::OnEventRemotePreOnlyMsg(NetSocket& rSocket, const SocketEvent& stEvent)
{
	printf("[INFO]:OnEventRemotePreOnlyMsg\n");
}

void ForServerMsgHandler::OnEventRemoteAfterOnlyMsg(NetSocket& rSocket, const SocketEvent& stEvent)
{
	printf("[INFO]:OnEventRemoteAfterOnlyMsg\n");

	/*if (stEvent.first == EVENT_REMOTE_SEND_AFTER_ONLY_MSE)
	{
	int32 nCsessionID = stEvent.fourth;
	int64 repeatLoginID = stEvent.fifth;
	if (!nCsessionID || !repeatLoginID)
	{
	ASSERT(0);
	return;
	}

	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nCsessionID);
	if (pClientSession == NULL)
	{
	ASSERT(pClientSession);
	return;
	}

	W2DSelectRole sMsg;
	sMsg.nCharID = repeatLoginID;
	pClientSession->SendMsgToDp(&sMsg, sMsg.GetPackLength());
	}*/

}

