#include "GameServer_PCH.h"
#include "ForSClientMsgHandler.h"
#include "SharedIncludes.h"
#include "ServerClientCommon.h"

#include "ProcDpHandler.h"
#include "ProcWorldHandler.h"
#include "ProcClientHandler.h"

#include "SceneUser.h"
#include "SceneUserManager.h"



ForSClientMsgHandler::ForSClientMsgHandler()
{

	// 内部协议
#define REGISTER_INTERNAL_MESSAGE(msg_idx,cls,handler)\
	{\
		RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForSClientMsgHandler::handler, this, _1, _2, _3)); \
	}

	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_LOGINED,SSRepLogined,RepLogined);	// 登录结果 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_SERVERINFO_LIST,SSRepServerInfoList,RepServerInfoList);	// 服务器管理信息协议 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_NOFTTY_SERVERID,SSSessionNofitySInfo,NofityClientSessionInfo); // 同步分配服务器ID 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_PING_S,SSRepPingS,RepPingToS); // ping 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_CLIENT_EXIT,SSNofityClientExit,NofityClientExit);

#undef REGISTER_INTERNAL_MESSAGE


// 从WS过来的业务协议  
#define REGISTER_W2S_MESSAGE(msg_idx,cls,handler) \
	{\
		RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcWorldHandler::handler, ProcWorldHandler::Instance(), _1, _2, _3)); \
	}

	REGISTER_W2S_MESSAGE(PRO_W2S_ReqEnterScene,W2SReqEnterScene,ReqEnterScene); // 请求进入场景 
	REGISTER_W2S_MESSAGE(PRO_W2S_RepEnterResult,W2SRepEnterResult,ReqEnterResult); // 请求进入场景 
	
#undef REGISTER_W2S_MESSAGE

	// 从DP过来的协议 
#define REGISTER_D2S_MESSAGE(msg_idx,cls,handler) \
	{\
		RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcDpHandler::handler, ProcDpHandler::Instance(), _1, _2, _3)); \
	}
	
	REGISTER_D2S_MESSAGE(PRO_D2S_LOAD_CHARACTER,D2SLoadCharacter,RevLoadUserData);
	REGISTER_D2S_MESSAGE(PRO_D2S_SAVE_CALLBACK, D2SSaveCallBack, RtSaveCallBack);
		
#undef REGISTER_D2S_MESSAGE


	// 从玩家过来的协议 
#define REGISTER_C2S_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcClientHandler::handler, ProcClientHandler::Instance(), _1, _2, _3)); \
	}

	REGISTER_C2S_MESSAGE(PRO_C2S_CHAT_WORLD,C2SCharWorld,ReqChatWorld);	// 世界聊天  
	REGISTER_C2S_MESSAGE(PRO_C2S_SCENE_DATA,C2SSceneData,ReqSceneData); // 请求场景数据 
	REGISTER_C2S_MESSAGE(PRO_C2S_CHANGE_SCENE, C2SSceneData, ReqChangeScene); // 切换场景 

#undef REGISTER_C2S_MESSAGE

}


ForSClientMsgHandler::~ForSClientMsgHandler()
{
}

// 有客户端连接回调
void ForSClientMsgHandler::OnNetMsgEnter(NetSocket& rSocket)
{
	if(m_bIsWsMsg)
	{
		ClientCommonHandler::ForWsOnMsgEnter(rSocket, ESERVER_TYPE_SS);
		m_bIsWsMsg = false;
	}else
	{
		ClientCommonHandler::ForClientToServerOnMsgEnter(rSocket,ESERVER_TYPE_SS);
	}
}

// 有客户端消息收到
void ForSClientMsgHandler::OnNetMsg(NetSocket& rSocket, NetMsgHead* pHead,int32 nSize)
{
	ClientCommonHandler::ForWsOnMsg(*this,rSocket,pHead,nSize);
}

// 有客户端断开回调(此时socket已经回收) 
void ForSClientMsgHandler::OnNetMsgExit(NetSocket& rSocket)
{
	//---------------------------------服务组代码begin-------------------------------
	ClientCommonHandler::ForWsOnExit(rSocket);
	//---------------------------------服务组代码end---------------------------------
}


void ForSClientMsgHandler::RepLogined(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ClientCommonHandler::ForServerLogined(pBaseSession,pMsg);
	//---------------------------------服务组代码end---------------------------------


	ServerSession* pServerSession = static_cast<ServerSession*>(pBaseSession);
	
	switch(pServerSession->RemoteServerType())
	{
	case ESERVER_TYPE_WS:
		{
			SceneInfoOpt& rSceneInfoOpt = NetServerOpt::Instance()->GetSceneInfo();
			if(rSceneInfoOpt.nSceneNum < 1)
			{
				ASSERT(0);
				return ;
			}

			ASSERT(rSceneInfoOpt.nSceneNum <= MAX_SCENE_NUM);

			S2WRegisterScene sMsg;
			sMsg.nSceneNum = rSceneInfoOpt.nSceneNum;
			for (int32 i = 0; i < rSceneInfoOpt.nSceneNum; ++i)
			{
				sMsg.arrSceneID[i] = rSceneInfoOpt.arrSceneID[i];
			}
			pBaseSession->SendMsg(&sMsg,sMsg.GetPackLength());
		}
		break;
	case ESERVER_TYPE_DP:
	case ESERVER_TYPE_FEP:
		{

		}
		break;
	case ESERVER_TYPE_LOG:
		{
			LogManagerMgr::Instance()->Init(pServerSession,pServerSession->LocalServerType(),0);
		}
		break;
		
	}
}

void  ForSClientMsgHandler::RepServerInfoList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ClientCommonHandler::ForWsReponseServerList(this,pSession,pMsg);
	//---------------------------------服务组代码end---------------------------------
}

void ForSClientMsgHandler::NofityClientSessionInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ClientCommonHandler::ForClientUpdataClientUpConnectInfo(pSession,pMsg,ESERVER_TYPE_SS);
	//---------------------------------服务组代码end---------------------------------

}

void ForSClientMsgHandler::RepPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ServerCommonHandler::ForServerTimeOutCancel(pSession);	// 取消超时 
	//---------------------------------服务组代码end---------------------------------
}

void ForSClientMsgHandler::NofityClientExit(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	const SSNofityClientExit* packet = static_cast<const SSNofityClientExit*>(pMsg);

	// 同步数据到dp上 
	SceneUser* pCharacter = SceneUserManager::Instance()->GetUserByCSID(packet->nClientSessionID);
	ASSERT(pCharacter);

	//pCharacter->SaveData();

	SSNofityClientExit sMsgExit;
	sMsgExit.nReason = packet->nReason;
	sMsgExit.nPostion = packet->nPostion;
	
	pCharacter->GetClientSession()->SendMsgToDp(&sMsgExit,sMsgExit.GetPackLength());

	// 删除 
	ClientSessionMgr::Instance()->RemoveSession(packet->nClientSessionID);

	SceneUserManager::Instance()->RemoveUser(pCharacter->GetUid());
	//---------------------------------服务组代码end---------------------------------
}