#include "ForClientMsgHandler.h"
#include "../DyNetSocket/NetSocket.h"

#include "Encrypt.h"
#include "LogManager.h"
#include "SharedIncludes.h"

#include "ClientSession.h"
#include "ServerSession.h"


// 在 fep 中 ，client的sessionID为socketID

ForClientMsgHandler::ForClientMsgHandler()
{

	// to fep
#define REGISTER_FEP_MESSAGE(msg_idx, cls,handler)\
	{\
		RegisterMessage(msg_idx, sizeof(cls),boost::bind(&ForClientMsgHandler::handler, this, _1, _2,_3)); \
	}

	REGISTER_FEP_MESSAGE(PRO_C2F_ENCRYPT,	C2FepEncryptInfo,	ReqFepEncyptHandler);
	//REGISTER_FEP_MESSAGE(PRO_C2F_CLOSE,		C2FepClose,			ReqFepCloseHandler);

#undef REGISTER_FEP_MESSAGE

	// to ls
#define REGISTER_LS_MESSAGE(msg_idx,cls)\
	{\
	RegisterMessage(msg_idx,sizeof(cls),boost::bind(&ForClientMsgHandler::ReqSendToLoginServer,this, _1, _2,_3));\
	}
	
	REGISTER_LS_MESSAGE(PRO_C2L_ACCOUNT_LOGIN,		C2LAccountLogin); 
	REGISTER_LS_MESSAGE(PRO_C2L_ROLE_CREATE,		C2LRoleCreate);
	REGISTER_LS_MESSAGE(PRO_C2L_RAND_NAMES,			C2LRandNames);

#undef REGISTER_LS_MESSAGE

	// to ws
#define REGISTER_WS_MESSAGE(msg_idx,cls)\
	{\
		RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForClientMsgHandler::ReqSendToWorldServer, this, _1, _2,_3)); \
	}

	REGISTER_WS_MESSAGE(PRO_C2W_SWITCH_SCENE,	C2WSwitchScene);	// 切换新场景 
	REGISTER_WS_MESSAGE(PRO_C2W_SELECT_ROLE,	C2WSelectRole);		// 选择角色

#undef REGISTER_WS_MESSAGE

	// to ss
#define REGISTER_SS_MESSAGE(msg_idx,cls)\
	{\
		RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForClientMsgHandler::ReqSendToSceneServer, this, _1, _2,_3)); \
	}

	REGISTER_SS_MESSAGE(PRO_C2S_CHAT_WORLD,		C2SCharWorld);
	REGISTER_SS_MESSAGE(PRO_C2S_SCENE_DATA,		C2SSceneData);
	REGISTER_SS_MESSAGE(PRO_C2S_CHANGE_SCENE,   C2SChanageScene);

#undef REGISTER_SS_MESSAGE

}


ForClientMsgHandler::~ForClientMsgHandler()
{
}

void ForClientMsgHandler::OnNetMsgEnter(NetSocket& rSocket)
{
	if(ClientSession* pSession = ClientSessionMgr::Instance()->AddSession(rSocket.SID()))
	{
		printf("[INFO]:Client had connected! socketID=%d\n", rSocket.SID());
		ServerInfoOpt& rRemoteInfo = NetServerOpt::Instance()->GetRemoteServerInfo();
		ServerSession* pWsServerSession = ServerSessionMgr::Instance()->GetSession(rRemoteInfo.nID);
		if(pWsServerSession == NULL)
		{
			printf("[ERROR]:WsSession is null, Please try it later!\n");
			pSession->Exist();	// ws 还未连接上或断开了 
			return ;
		}
		pSession->SetForRemoteClient(rSocket);
		pSession->SetForRemoteWs(pWsServerSession);
		pSession->SetStatus(E_CLIENT_STATUS_CONNECTED);
		pSession->SetOnServerType(ESERVER_TYPE_FEP);
	}else
	{
		printf("[ERROR]:Create ClientSession Fail! Had this socketID = %d\n", rSocket.SID());
	}
}


void ForClientMsgHandler::OnNetMsg(NetSocket& rSocket, NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	if(ClientSession* pSession = ClientSessionMgr::Instance()->GetSession(rSocket.SID()))
	{
		// 解密处理 
		static char msgBuffer[65536];
		memcpy(&msgBuffer[0], pMsg,nSize);
		static char arrEncrypt[MAX_ENCRYPT_LENTH];
		pSession->GetEncryptKey(arrEncrypt);
		Encrypt::XorCode(nSize, arrEncrypt, msgBuffer, nSize);
		pMsg = (NetMsgHead*)&msgBuffer[0];

		const MsgHandlerInfo* pMsgHandlerInfo = GetMsgHandler(pMsg->nType);
		if(pMsgHandlerInfo == NULL)
		{
			printf("[ERROR]:Not Find Protocol:%d\n", pMsg->nType);
			rSocket.OnEventColse();
			return;
		}
		else
		{
			printf("[INFO]:OnNetMsg Protocol: %d\n", pMsg->nType);
			(pMsgHandlerInfo->rHandler)((BaseSession*)pSession, pMsg,nSize);
		}
	}else
	{
		printf("[ERROR]:Cannot found clientsession:%d\n",rSocket.SID());
		rSocket.OnEventColse();
	}
	//---------------------------------服务组代码end-------------------------------
}

void ForClientMsgHandler::OnNetMsgExit(NetSocket& rSocket)
{
	//---------------------------------服务组代码begin-------------------------------
	// 服务器主动断开或被客户断开，都是从这里最先开始操作 
	// socket这时不能再给Client发数据,socket可能已经失效 
	int32 nClientSessionID = rSocket.SID();

	// 打印退出原因  
	std::string strExitMsg;
	int32 nErrorCode = rSocket.ErrorCode(strExitMsg);
	printf("[INFO]:Eixst Reason msg:%s\n",strExitMsg.c_str());

	Disconnected(nClientSessionID, nErrorCode);
	//---------------------------------服务组代码end-------------------------------
}

// 发送到 ls 
void ForClientMsgHandler::ReqSendToLoginServer(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ClientSession* pClientSession = static_cast<ClientSession*>(pSession); 
	
	// 
	if(pClientSession->Status() != E_CLIENT_STATUS_NOTIFY_INITED)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Status Not Eq E_CLIENT_STATUS_NOTIFY_INITED");
		return;
	}

	MSG_CLIENT_COMMON_CLIENTSESSIONID(pClientSession->GetSessionID(),pMsg,nSize);
	pClientSession->SendMsgToLs((NetMsgHead*)(&vecMsgBuffer[0]), vecMsgBuffer.size());
	//---------------------------------服务组代码end-------------------------------
}

// 发送到 ss
void ForClientMsgHandler::ReqSendToSceneServer(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------

	ClientSession* pClientSession = static_cast<ClientSession*>(pSession); 

	if(pClientSession->Status() != E_CLIENT_STATUS_IN_SCENE)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Status Not Eq E_CLIENT_STATUS_IN_SCENE");
		return;
	}

	MSG_CLIENT_COMMON_CLIENTSESSIONID(pClientSession->GetSessionID(),pMsg,nSize);
	pClientSession->SendMsgToSs((NetMsgHead*)(&vecMsgBuffer[0]), vecMsgBuffer.size());

	//---------------------------------服务组代码end-------------------------------
}

// 发送到 ws
void ForClientMsgHandler::ReqSendToWorldServer(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	ClientSession* pClientSession = static_cast<ClientSession*>(pSession); 
	
	if(pClientSession->Status() <= E_CLIENT_STATUS_ENCRYPTED)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Status <= E_CLIENT_STATUS_NOTIFY_INITED");
		return;
	}
	
	MSG_CLIENT_COMMON_CLIENTSESSIONID(pClientSession->GetSessionID(),pMsg,nSize) ;
	pClientSession->SendMsgToWs((NetMsgHead*)(&vecMsgBuffer[0]), vecMsgBuffer.size());
	//---------------------------------服务组代码end-------------------------------
}

void ForClientMsgHandler::ReqFepEncyptHandler(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize)
{
	//---------------------------------服务组代码begin-------------------------------
	printf("[INFO]:Client Request Encrypt!\n");
	ClientSession* pClientSession = static_cast<ClientSession*>(pSession);
	if(pClientSession->Status() == E_CLIENT_STATUS_CONNECTED )
	{
		char arrRandKey[MAX_ENCRYPT_LENTH];
		Encrypt::RandKey(arrRandKey);
		
		F2CEncryptInfo sMsgEncrypt;
		memcpy(sMsgEncrypt.arrEncryptKey, arrRandKey, MAX_ENCRYPT_LENTH);
		pClientSession->SendMsg(&sMsgEncrypt,sMsgEncrypt.GetPackLength());

		// 要先发数据后才能改 
		pClientSession->SetEncryptKey(arrRandKey);
		pClientSession->SetStatus(E_CLIENT_STATUS_ENCRYPTED);

		// 请求分配服务器 ID 
		SSReqLoadLest sMsgLoadLest;
		sMsgLoadLest.nClientSessionID = pClientSession->GetSessionID();
		pClientSession->SendMsgToWs(&sMsgLoadLest,sMsgLoadLest.GetPackLength());

	}else
	{
		printf("[ERROR]:ClientSession Status Not Eq EPLAYER_STATUS_CONNECTED\n");
	}
	//---------------------------------服务组代码end-------------------------------
}

// 来自SS,LS,WS请求该该角色下线 
//void ForClientMsgHandler::ReqFepCloseHandler(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize)
//{
//	// 不允许客户端请求服务端来主动断开这种方式，只能由客户端socket.close()方式 
//
//	//Disconnected(nClientSessionID, nErrorCode);
//}



void ForClientMsgHandler::Disconnected(int32 nClientSessionID, int32 nErrorCode)
{
	// 通知Ws,Ws再通知ls,dp，如果玩家在ss中，则会通知 
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nClientSessionID);
	ASSERT(pClientSession);

	// 如果连接未进行验证，可以不发到WS中 
	if (pClientSession->Status() < E_CLIENT_STATUS_ENCRYPTED)
	{
		// 删除ClientSession
		pClientSession->Exist();
		ClientSessionMgr::Instance()->RemoveSession(nClientSessionID);
		return;
	}

	// 发出客户端退出消息 
	SSNofityClientExit sMsgExit;
	switch (nErrorCode)
	{
	case ESOCKET_EXIST_NULL:
		sMsgExit.nReason = SSNofityClientExit::E_REASON_UNKOWN;
		break;
	case ESOCKET_EXIST_LOCAL:
		sMsgExit.nReason = SSNofityClientExit::E_REASON_SERVER_TICKED;
		break;
	case ESOCKET_EXIST_REMOTE:
		sMsgExit.nReason = SSNofityClientExit::E_REASON_CLIENT_EXIT;
		break;
	case ESOCKET_EXIST_TIMEOUT:
		sMsgExit.nReason = SSNofityClientExit::E_REASON_TIMEOUT;
		break;
	case ESOCKET_EXIST_PACK_LENGTH_ERROR:
		sMsgExit.nReason = SSNofityClientExit::E_REASON_PACKAGE_ERROR;
		break;
	case ESCOKET_EXIST_SEND_CONNECT_INVAILD:
		sMsgExit.nReason = SSNofityClientExit::E_REASON_SEND_ERROR;
		break;
	case 99:
		sMsgExit.nReason = SSNofityClientExit::E_REASON_REPEAT_CHARACTER;
		break;
	default:
		sMsgExit.nReason = SSNofityClientExit::E_REASON_UNKOWN;
		break;
	}
	pClientSession->SendMsgToWs(&sMsgExit, sMsgExit.GetPackLength());

	// 删除ClientSession
	pClientSession->Exist();
	ClientSessionMgr::Instance()->RemoveSession(nClientSessionID);
}

