#ifndef _SEVER_CLIENT_COMMON_H_
#define _SEVER_CLIENT_COMMON_H_

#include "BaseSingle.h"
#include "CommonDefine.h"
#include "BaseSession.h"
#include "ServerSession.h"
#include "ClientSession.h"
#include "CommonServerOpt.h"
#include "ServerConnectMgr.h"

/************************************************************************/
/* ���������ͻ���ͨ�ô�����                                                    */
/************************************************************************/
class ClientCommonHandler
{
public:
	/************************************************************************/
	/* ����Ws�ɹ��Ļص�����������WS�����¼������WS����˭����fep,ls,ss,dp ����ws                                                                     */
	/************************************************************************/
	static bool ForWsOnMsgEnter(NetSocket& rSocket, EServerType nLocalServerType)
	{
		printf("[INFO]:Connect WS Success.....................ok!\n");
		ServerSession* pSession = ServerSessionMgr::Instance()->CreateSession(rSocket);
		ServerInfoOpt& rRemoteInfo = NetServerOpt::Instance()->GetRemoteServerInfo();
		if (!ServerSessionMgr::Instance()->InitSession(rRemoteInfo.nID, pSession))
		{
			printf("[ERROR]:Inition Ws Connected Fail, Repeat ServerID:%d\n", rRemoteInfo.nID);
			pSession->Exist();
			return false;
		}
		pSession->SetRemoteServerType(ESERVER_TYPE_WS);
		pSession->SetLocalServerType(nLocalServerType);
		pSession->SetStatus(E_SERVER_STATUS_CONNETED);
		pSession->SetSessionType(ESESSION_TYPE_CLINET);
		pSession->SetSessionFinished();
		pSession->LoginToWs();
		
		return true;
	}

	/************************************************************************/
	/* �ú괦��������WS����Ϣ����,ls,ss,fep,dp������ws�Ļص�                   */
	/************************************************************************/
	static bool ForWsOnMsg(BaseMsgHandler& baseMsgHandler, NetSocket& rSocket, NetMsgHead* pMsg, int32 nSize)
	{
		const MsgHandlerInfo* pMsgHandler = baseMsgHandler.GetMsgHandler(pMsg->nType);
		if (NULL == pMsgHandler)
		{
			printf("Not Find Protocol:%d\n", pMsg->nType);
			rSocket.SetWillColse();
			return false;
		}

		ServerSession* pWsSession = (ServerSessionMgr::Instance()->GetSession(rSocket));
		if (NULL == pWsSession)
		{
			printf("Not Find ServerSession Of WS:\n");
			return false;
		}

		(pMsgHandler->rHandler)((BaseSession*)pWsSession, pMsg, nSize);

		return true;
	}

	/************************************************************************/
	/* �ú���WS�Ͽ����ӵĻص�����                                                                     */
	/************************************************************************/
	static bool ForWsOnExit(NetSocket& rSocket)
	{
		if (ServerSession* pSession = ServerSessionMgr::Instance()->GetSession(rSocket))
		{
			ServerSessionMgr::Instance()->RemoveSession(pSession->ServerID());
			printf("OnNetMsgExit:%d\n", pSession->ServerID());
		}
		else
		{
			printf("Can not find session by socket!\n");
			ASSERT(0);
		}
		return true;
	}

	/************************************************************************/
	/* ��괦���¼WS��REP��ĳɹ��ص�����                                                                     */
	/************************************************************************/
	static bool ForServerLogined(BaseSession* pBaseSession, const NetMsgHead* pMsg)
	{
		if (pBaseSession->Status() != E_SERVER_STATUS_CONNETED)
		{
			printf("pBaseSession->Status() != EWORLD_STATUS_CONNECTED\n");
			return false;
		}

		const SSRepLogined* pPacket = static_cast<const SSRepLogined*>(pMsg);
		if (pPacket->nResult != SSRepLogined::SUCCESS)
		{
			printf("Login TO ws Faild!\n");
			return false;
		}

		pBaseSession->SetStatus(E_SERVER_STATUS_LOGINED);

		return true;
	}

	/************************************************************************/
	/* WS���ص�������������������Ϣ                                                                     */
	/************************************************************************/
	static bool ForWsReponseServerList(BaseMsgHandler* baseMsgHandler,BaseSession* pSession, const NetMsgHead* pMsg)
	{
		if (pSession->Status() != E_SERVER_STATUS_LOGINED)
		{
			printf("pSession->Status() != ESERVER_STATUS_LOGINEDn");
			return false;
		}
		const SSRepServerInfoList* pPacket = static_cast<const SSRepServerInfoList*>(pMsg);
		for (int32 i = 0; i < pPacket->nCount; ++i)
		{
			const ServerInfoData& rInfo = pPacket->sServerInfoData[i];
			if (rInfo.nServerType == ESERVER_TYPE_DP || rInfo.nServerType == ESERVER_TYPE_FEP || rInfo.nServerType == ESERVER_TYPE_LOG)
			{
				ServerConnectMgr* pConnMgr = ServerConnectMgr::Instance();
				bool bResult = pConnMgr->ConnectToServer(rInfo.nServerID, rInfo.nServerType, rInfo.arrHost, rInfo.nPort,
					boost::bind(&BaseMsgHandler::OnNetMsgEnter, baseMsgHandler, _1),
					boost::bind(&BaseMsgHandler::OnNetMsg, baseMsgHandler, _1, _2, _3),
					boost::bind(&BaseMsgHandler::OnNetMsgExit, baseMsgHandler, _1));
				if (!bResult)
				{
					printf("Connect Fail!\n");
					return false;
				}
			}
		}
		return true;
	}

	/************************************************************************/
	/* �Լ���Ϊ�ͻ��ˣ����ӱ�ķ�����(����WS)�ɹ��Ļص���������ss����fep��dp�ĳɹ��ص�                                                                     */
	/************************************************************************/
	static bool ForClientToServerOnMsgEnter(NetSocket& rSocket,EServerType nLocalServerType)
	{
		ServerSession* pSession = ServerSessionMgr::Instance()->CreateSession(rSocket); 
		if (NULL == pSession)
		{
			ASSERT(pSession);
			return false;
		}
		
		NetClientInfo* pClientInfo = ServerConnectMgr::Instance()->GetNetClient(rSocket);
		if (NULL == pClientInfo)
		{
			ASSERT(pClientInfo);
			return false;
		}

		uint32 nServerID = pClientInfo->nServerID;
		if (!ServerSessionMgr::Instance()->InitSession(nServerID, pSession))
		{
			printf("[ERROR]:Reap serverID\n"); 
			pSession->Exist(); 
			return false; 
		}
		pSession->SetStatus(E_SERVER_STATUS_CONNETED); 
		pSession->SetRemoteServerType((EServerType)(pClientInfo->nServerType)); 
		pSession->SetLocalServerType((EServerType)nLocalServerType); 
		pSession->SetSessionType(ESESSION_TYPE_CLINET); 
		pSession->SetSessionFinished();
		pSession->LoginToS();
		return true;
	}

	/************************************************************************/
	/* ��Ϊ�ͻ���(ls,ss)����WS��������Client�û�����dp,��fep��������Ϣ                                                                     */
	/************************************************************************/
	static bool ForClientUpdataClientUpConnectInfo(BaseSession* pSession, const NetMsgHead* pMsg, EServerType nOnServerType)
	{
		ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
		const SSSessionNofitySInfo* pPacket = static_cast<const SSSessionNofitySInfo*>(pMsg);
		ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacket->nClientSessionID);
		if (pClientSession == NULL)
		{
			pClientSession = ClientSessionMgr::Instance()->AddSession(pPacket->nClientSessionID);
			ASSERT(pClientSession);
			pClientSession->SetForRemoteWs(pServerSession);
		}
		ServerSession* pFepServerSession = NULL;
		if (pPacket->nFepServerID)
		{
			pFepServerSession = ServerSessionMgr::Instance()->GetSession(pPacket->nFepServerID);
			ASSERT(pFepServerSession);
		}
		ServerSession* pDpServerSession = NULL;
		if (pPacket->nDpServerID)
		{
			pDpServerSession = ServerSessionMgr::Instance()->GetSession(pPacket->nDpServerID);
			ASSERT(pDpServerSession);
		}
		pClientSession->SetForLocalLsSs(pServerSession, pFepServerSession, pDpServerSession);
		pClientSession->SetStatus(E_CLIENT_STATUS_NOTIFY_INITED);
		pClientSession->SetOnServerType(nOnServerType);
		return true;
	}

};

/************************************************************************/
/* ��Ϊ��������ͨ����Ϣ������                                                                     */
/************************************************************************/
class ServerCommonHandler
{
public:

	/************************************************************************/
	/* ��Ϊ������(fep,dp)���������Կͻ���(ls,ss)������                                                                     */
	/************************************************************************/
	static bool ForServerOnMsg(BaseMsgHandler& baseMsgHandler,NetSocket& rSocket, NetMsgHead* pMsg,int32 nSize)
	{
		if(ServerSession* pSession = ServerSessionMgr::Instance()->GetSession(rSocket))
		{
			const MsgHandlerInfo* pMsgHandlerInfo = baseMsgHandler.GetMsgHandler(pMsg->nType);
			if(pMsgHandlerInfo == NULL)
			{
				printf("Can not find protocol:%d\n",pMsg->nType);
				rSocket.SetWillColse();
				return false;
			}
			else
			{
				(pMsgHandlerInfo->rHandler)((BaseSession*)(pSession),pMsg,nSize);
			}
		}
		else
		{
			printf("Can not find session\n");
			ASSERT(0);
			return false;
		}
		return true;
	}

	/************************************************************************/
	/*  �Լ���Ϊ������(ws,fep,dp)���������Կͻ���(fep,dp,ls,ss)��������                                                    */
	/************************************************************************/
	static bool ForServerOnMsgEnter(NetSocket& rSocket, EServerType nServerType)
	{	
		ServerSession* pSession = ServerSessionMgr::Instance()->CreateSession(rSocket);
		pSession->SetStatus(E_SERVER_STATUS_CONNETED);
		pSession->SetLocalServerType(nServerType);
		pSession->SetSessionType(ESESSION_TYPE_SERVER);
		printf("Recive New Socket In, local_type:%d remote_type:%d,!\n",pSession->LocalServerType(),pSession->RemoteServerType());
		return true;
	}

	/************************************************************************/
	/* �Լ���Ϊ������(ws,fep,dp)���������Կͻ���(fep,dp,ls,ss)�˳���Ϣ                                                                     */
	/************************************************************************/
	static bool ForServerOnMsgExit(NetSocket& rSocket)
	{
		if (ServerSession* pSession = ServerSessionMgr::Instance()->GetSession(rSocket))
		{
			ServerSessionMgr::Instance()->RemoveSession(pSession->ServerID());
			printf("OnNetMsgExit:%d\n", pSession->ServerID());
		}
		else
		{
			printf("Can not find session by socket!\n");
			ASSERT(0);
		}
		return true;
	}

	/************************************************************************/
	/* ��Ϊ������(ws,fep,dp)������ͻ���(fep,dp,ls,ss)������                                                                     */
	/************************************************************************/
	static bool ForServerOnLogin(BaseSession* pSession, const NetMsgHead* pMsg)
	{
		ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
		const SSReqLoginS* pPacket = static_cast<const SSReqLoginS*>(pMsg);
		bool bResult = true;
		if(pSession->Status() != E_SERVER_STATUS_CONNETED)
		{
			printf("pSession->Status() != E_SERVER_STATUS_CONNETED\n");
			bResult = false;
		}
		else
		{
			uint32 nServerID = pPacket->nServerID;
			if(!ServerSessionMgr::Instance()->InitSession(nServerID, pServerSession))
			{
				bResult = false;
				printf("Reapt ServerID Serversion:%d status:%d remoteServerType:%d\n",nServerID,pSession->Status(),pServerSession->RemoteServerType());
			}
			else
			{
				pServerSession->SetServerID(pPacket->nServerID);
				pServerSession->SetRemoteServerType((EServerType)(pPacket->nServerType));
				pServerSession->SetSessionFinished();
			}
		}

		pServerSession->LoginResult(bResult);

		if(!bResult) pServerSession->Exist();

		return bResult;

	}

	/************************************************************************/
	/* ��ΪΪ������(ws,fep,dp),ȡ���������Կͻ���(ls,ss)��Ϣ����                                                                     */
	/************************************************************************/
	static bool ForServerTimeOutCancel(BaseSession* pSession)
	{
		ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
		pServerSession->GetNetSocket()->TimeoutCancel();
		pServerSession->GetNetSocket()->SetTimeout(0);
		return true;
	}

	/************************************************************************/
	/* ��Ϊ������(fep,dp),��WS��������Client�û���ls��ss��������Ϣ(ls_id,ss_id)                                                                     */
	/************************************************************************/
	static bool ForServerUpdataClientUpConnectInfo(BaseSession* pSession, const NetMsgHead* pMsg,EServerType nOnServerType)
	{
		ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
		const SSSessionNofitySInfo* pPacket = static_cast<const SSSessionNofitySInfo*>(pMsg);
		int32 nClientSessionID = pPacket->nClientSessionID;
		ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nClientSessionID);
		if(pClientSession == NULL)
		{
			pClientSession = ClientSessionMgr::Instance()->AddSession(nClientSessionID);
			ASSERT(pClientSession);
			pClientSession->SetForRemoteWs(pServerSession);
		}
		ServerSession* pLsServerSession = NULL;
		if(pPacket->nLsServerID)
		{
			pLsServerSession = ServerSessionMgr::Instance()->GetSession(pPacket->nLsServerID);
			ASSERT(pLsServerSession);
			pClientSession->SetStatus(E_CLIENT_STATUS_NOTIFY_INITED);
		}
		ServerSession* pSsServerSession = NULL;
		if(pPacket->nSsServerID)
		{
			pSsServerSession = ServerSessionMgr::Instance()->GetSession(pPacket->nSsServerID);
			ASSERT(pSsServerSession);
			pClientSession->SetStatus(E_CLIENT_STATUS_IN_SCENE);
		}
		pClientSession->SetForLocalFepDp(pLsServerSession,pSsServerSession);
		pClientSession->SetOnServerType(nOnServerType);

		return true;
	}
};


#endif

