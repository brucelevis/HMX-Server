#include "ForSClientMsgHandler.h"
#include "../DyNetSocket/NetSocket.h"
#include "../DyNetSocket/NetClient.h"

#include "LogManager.h"

#include "ProcWorldHandler.h"
#include "ServerSession.h"
#include "ClientSession.h"
#include "ServerConnectMgr.h"
#include "ForServerMsgHandler.h"
#include "ServerClientCommon.h"

#include "LogManager.h"
#include "MemoryManger.h"

/************************************************************************/
/* 本类是将自己作为客户端去处理                                                                     */
/************************************************************************/

ForSClientMsgHandler::ForSClientMsgHandler():m_bFirstMsg(true)
{

#define REGISTER_INTERNAL_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForSClientMsgHandler::handler, this, _1, _2, _3)); \
}

	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_LOGINED,SSRepLogined,RepLogined);	// 登录结果 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_SERVERINFO_LIST,SSRepServerInfoList,RepServerInfoList);	// 服务器管理信息协议 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_NOFTTY_SERVERID,SSSessionNofitySInfo,NofityClientSessionInfo); // 同步分配服务器 ID 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_CLIENT_EXIT,SSNofityClientExit,NofityClientExit);
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_PING_S,SSRepPingS,RepPingToS); // ping 

#undef REGISTER_INTERNAL_MESSAGE

#define REGISTER_W2D_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcWorldHandler::handler, ProcWorldHandler::Instance(), _1, _2, _3)); \
	}

	REGISTER_W2D_MESSAGE(PRO_W2D_SERVER_INFO,W2DNotifyServerInfo,ReqNotifyServerInfo);	// 服务器信息 
	REGISTER_W2D_MESSAGE(PRO_W2D_SCENE_INFO,W2DNotifySceneInfo,ReqNotifySceneInfo);	// 场景信息 
	REGISTER_W2D_MESSAGE(PRO_W2D_EVENT_INFO,W2DReqEventInfo,ReqQueryEventInfo); // 查询事件信息 
	REGISTER_W2D_MESSAGE(PRO_W2D_SELECT_ROLE,W2DSelectRole, ReqSelectRole);	// 选择角色 

#undef REGISTER_W2D_MESSAGE

}


ForSClientMsgHandler::~ForSClientMsgHandler()
{
}

// 有客户端连接回调 
void ForSClientMsgHandler::OnNetMsgEnter(NetSocket& rSocket)
{
	if(m_bFirstMsg)
	{
		ClientCommonHandler::ForWsOnMsgEnter(rSocket,ESERVER_TYPE_DP);
		m_bFirstMsg = false;
	}else
	{
		ClientCommonHandler::ForClientToServerOnMsgEnter(rSocket,ESERVER_TYPE_DP);
	}
}

// 有客户端消息收到 
void ForSClientMsgHandler::OnNetMsg(NetSocket& rSocket, NetMsgHead* pHead,int32 nSize)
{
	ClientCommonHandler::ForWsOnMsg(*this, rSocket, pHead, nSize);
}

// 有客户端断开回调(此时socket已经回收) 
void ForSClientMsgHandler::OnNetMsgExit(NetSocket& rSocket)
{
	ClientCommonHandler::ForWsOnExit(rSocket);
}


void ForSClientMsgHandler::RepLogined(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	ClientCommonHandler::ForServerLogined(pBaseSession, pMsg);

	// 如果是log，则需要去注册 
	ServerSession* pServerSession = static_cast<ServerSession*>(pBaseSession);
	if(pServerSession->RemoteServerType() == ESERVER_TYPE_LOG)
	{
		ServerInfoOpt& infoOpt = NetServerOpt::Instance()->GetLocalServerInfo();
		LogManagerMgr::Instance()->Init(pServerSession,infoOpt.nID,infoOpt.nType);
	}

}

void ForSClientMsgHandler::RepServerInfoList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ClientCommonHandler::ForWsReponseServerList(this,pSession,pMsg);
}

void ForSClientMsgHandler::NofityClientSessionInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerUpdataClientUpConnectInfo(pSession,pMsg,ESERVER_TYPE_DP);
	//---------------------------------服务组代码end-------------------------------
}

void ForSClientMsgHandler::NofityClientExit(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize)
{
	const SSNofityClientExit* pPacket = static_cast<const SSNofityClientExit*>(pMsg);
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacket->nSessionID);
	if(pClientSession == NULL)
	{
		ASSERT(pClientSession);
		return;
	}

	int64 nCharID = MemoryManager::Instance()->GetUserIDBySessionID(pPacket->nSessionID);

	if (nCharID)
	{
		MemoryManager::Instance()->SaveNowByUID(nCharID);
		MemoryManager::Instance()->RemoveByUID(nCharID);
	}

	/*-------------------------------------------------------------------
	 * @Brief : 玩家退出的消息要么从WS过来，要么从SS中过来，二者其一 
	 *			WS过来，则玩家是下线，SS过来可能是切场景或下线，可查看
	 *			退出reason参数 
	 * @Author:hzd 2015:11:20
	 *------------------------------------------------------------------*/
	ClientSessionMgr::Instance()->RemoveSession(pPacket->nSessionID);

}

void ForSClientMsgHandler::RepPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ServerCommonHandler::ForServerTimeOutCancel(pSession);	// 取消超时 
}


