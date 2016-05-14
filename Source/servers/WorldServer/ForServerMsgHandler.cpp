#include "WorldServer_PCH.h"
#include "ForServerMsgHandler.h"

#include "SharedIncludes.h"
#include "ServerClientCommon.h"

#include "BaseProcHandler.h"
#include "ProcClientHandler.h"
#include "ProcDpHandler.h"
#include "ProcFepHandler.h"
#include "ProcSsHandler.h"
#include "ProcLsHandler.h"


#include "SceneInfoManger.h"
#include "UserManager.h"
#include "ServerInfoMgr.h"

ForServerMsgHandler::ForServerMsgHandler()
{

	// common 需要用到内部协议 
	// 内部协议，即不分不同servertype，都去做统一的事情，如登录，返回列表，ping等等操作  
#define REGISTER_INTERNAL_MESSAGE(msg_idx,cls,handler) \
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForServerMsgHandler::handler, this, _1, _2, _3)); \
}

	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_LOGIN_WS,		SSReqLoginWs,		ReqLogin);
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_LOAD_LEST,		SSReqLoadLest,		ReqRoadLest);
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_PING_S,		SSReqPingToS,		ReqPingToS);
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_CLIENT_EXIT,	SSNofityClientExit,	NofityClientExit);
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_CONNECT_INFO,	SSNotifyConnectInfo,NotifyConnectInfo);

#undef REGISTER_INTERNAL_MESSAGE

	// 与dp通信协议注册 
#define REGISTER_DP_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcDpHandler::handler, ProcDpHandler::Instance(), _1, _2, _3)); \
	}

	// 加载统一标识管理
	REGISTER_DP_MESSAGE(PRO_D2W_GLOBAL_INFO,D2WGlobalInfo,ReqGlobalInfo);
	REGISTER_DP_MESSAGE(PRO_D2W_SELECT_ROLE_RESULT,D2WSelectRoleResult,RepSelectRoleResult);

#undef REGISTER_DP_MESSAGE

		// ls
#define REGISTER_LS_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcLsHandler::handler, ProcLsHandler::Instance(), _1, _2, _3)); \
	}

#undef REGISTER_LS_MESSAGE

		// fep
#define REGISTER_FEP_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcFepHandler::handler, ProcFepHandler::Instance(), _1, _2, _3)); \
	}

#undef REGISTER_FEP_MESSAGE

		// ss
#define REGISTER_SS_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcSsHandler::handler, ProcSsHandler::Instance(), _1, _2, _3)); \
	}

	/*------------------------------------------------
	* 由场景服向ws注册自己拥有的场景
	*-----------------------------------------------*/
	REGISTER_SS_MESSAGE(PRO_S2W_REGISTER_SCENE,		S2WRegisterScene,		ReqSceneRegister);
	REGISTER_SS_MESSAGE(PRO_S2W_CANCEL_SCENE,		S2WReqCancelScene,		ReqSceneCancel);
	REGISTER_SS_MESSAGE(PRO_S2W_ENTER_SCENE_RESULT,	S2WEnterSceneResult,	RepEnterSceneResult);
	REGISTER_SS_MESSAGE(PRO_S2W_CHANGE_SCENE,		S2WChangeScene,			RepChangeScene);

#undef REGISTER_SS_MESSAGE

	// client
#define REGISTER_CLIENT_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcClientHandler::handler, ProcClientHandler::Instance(), _1, _2, _3)); \
	}

	//REGISTER_CLIENT_MESSAGE(PRO_C2W_SWITCH_SCENE,C2WSwitchScene,ReqSwitchScene); // 切换场景  
	REGISTER_CLIENT_MESSAGE(PRO_C2W_SELECT_ROLE,C2WSelectRole,  ReqSelectRole);	 // 选择角色  

#undef REGISTER_CLIENT_MESSAGE


}


ForServerMsgHandler::~ForServerMsgHandler()
{ 
}

// 有客户端连接回调 
void ForServerMsgHandler::OnNetMsgEnter(NetSocket& rSocket)
{
	ServerCommonHandler::ForServerOnMsgEnter(rSocket,ESERVER_TYPE_WS);
}

// 有客户端消息收到 
void ForServerMsgHandler::OnNetMsg(NetSocket& rSocket, NetMsgHead* pMsg,int32 nSize)
{
	//FOR_S_ON_MSG(rSocket,pMsg);
	ServerSession* pSession = ServerSessionMgr::Instance()->GetSession(rSocket);
	if(pSession == NULL)
	{
		printf("Can not find session\n");
		rSocket.SetWillColse();
		return ;
	}

	const MsgHandlerInfo* pMsgHandlerInfo = GetMsgHandler(pMsg->nType);
	if(pMsgHandlerInfo == NULL)
	{
		printf("Not found protocol:%d\n",pMsg->nType);
		FLOG_ERROR(__FUNCTION__ , __LINE__ ,"Not found this protocol:%d",pMsg->nType);
		rSocket.SetWillColse();
		return;
	}

	(pMsgHandlerInfo->rHandler)((BaseSession*)(pSession),pMsg,nSize);

}

// 有Server断开回调(此时socket已经回收) 
void ForServerMsgHandler::OnNetMsgExit(NetSocket& rSocket)
{     
	//---------------------------------服务组代码begin-------------------------------
	ServerSession* pSession = ServerSessionMgr::Instance()->GetSession(rSocket);
	int32 nServerID = pSession->ServerID();
	EServerType eServerType = pSession->RemoteServerType();
	ServerCommonHandler::ForServerOnMsgExit(rSocket);
	//---------------------------------服务组代码end-------------------------------
	ServerInfoMgr::Instance()->OffServerLogin(pSession); 
	// 注销场景信息 
	if(eServerType == ESERVER_TYPE_SS)
	{
		SceneInfoManager::Instance()->RemoveScene(nServerID);
	}

}

void ForServerMsgHandler::ReqLogin(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
	if(pServerSession->Status() != E_SERVER_STATUS_CONNETED)
		return;

	const SSReqLoginWs* pPacket = static_cast<const SSReqLoginWs*>(pMsg);

	int32 nServerID = pPacket->nServerID;
	int32 nServerType = pPacket->nServerType;

	// 状态 
	pServerSession->SetServerID(nServerID);
	pServerSession->SetRemoteServerType((EServerType)nServerType);
	pServerSession->SetStatus(E_CLIENT_STATUS_LOGINED);

	// 记录新登入进来的server 
	bool bResult = ServerInfoMgr::Instance()->OnServerLogin(pServerSession,pPacket->arrServerAddr,pPacket->nServerPort,nServerID,nServerType);
	if(!bResult)
	{
		printf("[ERROR]:OnServerLogin fail serverid:%d\n",nServerID);
		FLOG_ERROR(__FUNCTION__,__LINE__,"OnServerLogin fail serverid:%d",nServerID);
		pServerSession->Exist();
		return;
	}

	// 记录serverID与对应的session
	bResult = ServerSessionMgr::Instance()->InitSession(nServerID,pServerSession);
	if(!bResult)
	{
		printf("[ERROR]:InitSession fail serverid:%d\n",nServerID);
		FLOG_ERROR(__FUNCTION__,__LINE__,"InitSession fail serverid:%d",nServerID);
		pServerSession->Exist();
		return;
	}
	else
	{
		pServerSession->SetSessionFinished();
	}

	// 返回登录成功 
	pServerSession->LoginResult(bResult);

	if(nServerType == ESERVER_TYPE_LOG)
	{
		ServerInfoOpt& infoOpt = NetServerOpt::Instance()->GetLocalServerInfo();
		LogManagerMgr::Instance()->Init(pServerSession,infoOpt.nID,infoOpt.nType);
	}

	// fep与dp的服务器信息需要广播给ls与ss 
	if(bResult)
	{
		ServerInfoMgr::Instance()->BoardcastDpAndFepServerInfo2LsAndSs(pPacket->nServerID);
	}
	//---------------------------------服务组代码end-------------------------------

}

// 收到ping请求信息 
void ForServerMsgHandler::ReqPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
	SSRepPingS sMsg;
	pServerSession->SendMsg(&sMsg,sMsg.GetPackLength());
}

void ForServerMsgHandler::ReqRoadLest(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	const SSReqLoadLest* pPacket = static_cast<const SSReqLoadLest*>(pMsg);
	int32 nClientSessionID = pPacket->nClientSessionID;
	ServerSession* pFepSession = static_cast<ServerSession*>(pSession);

	// 找出分配最小的(或上一次分配的，以后再优化) 
	ServerInfoMgr* pServerInfoMgr = ServerInfoMgr::Instance();
	ServerSession* pLsSession = pServerInfoMgr->GetLoadLestSession(ESERVER_TYPE_LS);
	ServerSession* pDpSession = pServerInfoMgr->GetLoadLestSession(ESERVER_TYPE_DP);
	ASSERT(pLsSession && pDpSession);

	// 第一次创建ClientSesson数据 
	ClientSession* pClientSession = ClientSessionMgr::Instance()->AddSession(nClientSessionID);
	ASSERT(pClientSession);
	pClientSession->SetForLocalWs1(pFepSession,pLsSession,pDpSession);
	pClientSession->SetOnServerType(ESERVER_TYPE_WS);

	// 同步到ls,dp,fep 
	SSSessionNofitySInfo sNofityInfo;
	sNofityInfo.nClientSessionID = nClientSessionID;
	sNofityInfo.nLsServerID = pLsSession->ServerID();
	sNofityInfo.nDpServerID = pDpSession->ServerID();
	sNofityInfo.nFepServerID = pFepSession->ServerID();

	// 通知分配给Client的ServerID 
	pClientSession->SendMsgToLs(&sNofityInfo,sNofityInfo.GetPackLength());
	pClientSession->SendMsgToDp(&sNofityInfo,sNofityInfo.GetPackLength());
	pClientSession->SendMsgToFep(&sNofityInfo,sNofityInfo.GetPackLength());
	//---------------------------------服务组代码end-------------------------------

}

void ForServerMsgHandler::NofityClientExit(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const SSNofityClientExit* pPacket = static_cast<const SSNofityClientExit*>(pMsg);
	int32 nClientSessionID = pPacket->nClientSessionID;
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nClientSessionID);
	if(!pClientSession)
	{
		ASSERT(pClientSession);
		return;
	}
	int32 nStatus = pClientSession->Status();
	SSNofityClientExit sMsgExit;
	sMsgExit.nReason = pPacket->nReason;
	switch(nStatus)
	{
	case E_CLIENT_STATUS_LOGINED:
		sMsgExit.nPostion = SSNofityClientExit::E_STATE_IN_LOGIN;
		break;
	case E_CLIENT_STATUS_SELECTED:
		sMsgExit.nPostion = SSNofityClientExit::E_STATE_IN_SELECT;
		break;
	case E_CLIENT_STATUS_IN_WORLD:
		sMsgExit.nPostion = SSNofityClientExit::E_STATE_IN_WORLD;
		break;
	case E_CLIENT_STATUS_IN_SCENE:
		sMsgExit.nPostion = SSNofityClientExit::E_STATE_IN_SCENE;
		break;
	default:
		sMsgExit.nPostion = SSNofityClientExit::E_STATE_IN_UNKOWN;	
		break;
	}
	sMsgExit.nClientSessionID = nClientSessionID;
	pClientSession->SendMsgToLs(&sMsgExit,sMsgExit.GetPackLength());
	if( nStatus == E_CLIENT_STATUS_IN_SCENE)
	{
		pClientSession->SendMsgToSs(&sMsgExit,sMsgExit.GetPackLength());
	}else
	{
		/*-------------------------------------------------------------------
		 * @Brief : ws->dp ，这里 dp 中只保存该玩家在 ws 上的数据，并且该数据是 
		 *					不允许影响场景玩家的数据的，两者的数据分离，通常 
		 *					WS 上的数据由 SS 更新或从 dp 获得，但不更新 dp 中的数据 
		 *			ws->ss->dp , 这里保存在 ss 上的数据 
		 *			ws->ls->dp ,保存在 ls 上的数据 
		 * @Author:hzd 2015:11:19 
		 *------------------------------------------------------------------*/  
		pClientSession->SendMsgToDp(&sMsgExit,sMsgExit.GetPackLength());
	}
	// 删除Session，UserData 
	UserManager::Instance()->LogoutUser(nClientSessionID);
	ClientSessionMgr::Instance()->RemoveSession(nClientSessionID);
}

void ForServerMsgHandler::NotifyConnectInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const SSNotifyConnectInfo* pInfo = static_cast<const SSNotifyConnectInfo*>(pMsg);
	ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
	ServerInfoMgr::Instance()->UpdateServerInfo(pServerSession->ServerID(),pInfo->nServerLoad,pInfo->nClientLoad);
}



