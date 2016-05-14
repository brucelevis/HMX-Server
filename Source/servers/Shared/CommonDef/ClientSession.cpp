
#include "Encrypt.h"
#include "ClientSession.h"
#include "../DyNetSocket/NetSocket.h"
#include "LogManager.h"
#include "ServerSession.h"

ClientSession::ClientSession(int32 nClientSessionID):m_nSessionID(nClientSessionID)
{
	memcpy(m_arrEncryptKey,EncryptDefualt,MAX_ENCRYPT_LENTH);
	m_SendPackCount = 0;
	m_nSceneID = 0;
	m_pSocket = 0;
	m_pWs = 0;	
	m_pFep = 0;	
	m_pLs = 0;	
	m_pDp = 0;	
	m_pSs = 0;	
	m_nOnServerType = 0;
}

ClientSession::~ClientSession()
{
}

void ClientSession::SetForRemoteClient(NetSocket& rSocket)
{
	m_pSocket = &rSocket;
}

void ClientSession::SetForRemoteWs(ServerSession* pWsSession)
{
	m_pWs = pWsSession;
}

void ClientSession::SetForLocalFepDp(ServerSession* pLsSession,ServerSession* pSsSession)
{
	m_pLs = pLsSession;
	m_pSs = pSsSession;
}

void ClientSession::SetForLocalLsSs(ServerSession* pWsSession,ServerSession* pFepSession,ServerSession* pDpSession)
{
	m_pWs = pWsSession;
	m_pFep = pFepSession;
	m_pDp = pDpSession;
}

void ClientSession::SetForLocalWs1(ServerSession* pFepSession, ServerSession* pLsSession, ServerSession* pDpSession)
{
	m_pFep = pFepSession;
	m_pLs = pLsSession;
	m_pDp = pDpSession;
}
void ClientSession::SetForLocalWs2(ServerSession* pSsSession,int32 nSceneID)
{
	m_pSs = pSsSession;
	m_nSceneID = nSceneID;
}

int32 ClientSession::SessionID()
{
	return m_nSessionID;
}

void ClientSession::SendMsg(NetMsgHead* pMsg,int32 nSize)
{
	if(OnServerType() == ESERVER_TYPE_FEP)
	{
		ASSERT(m_pSocket);
		static char arrBuffer[65536];
		memcpy(&arrBuffer[0], pMsg, nSize);
		Encrypt::XorCode(nSize,m_arrEncryptKey,arrBuffer, nSize);
		pMsg = (NetMsgHead*)&arrBuffer[0];
		SendMsg(m_pSocket, pMsg, nSize);
	}else
	{
		ASSERT(m_pFep);
		SendMsg(m_pFep->GetNetSocket(), pMsg, nSize);
	}
}

void ClientSession::SendMsgToWs(NetMsgHead* pMsg, int32 nSize)
{
	ASSERT(m_pWs);
	SendMsg(m_pWs->GetNetSocket(), pMsg, nSize);
}

void ClientSession::SendMsgToSs(NetMsgHead* pMsg, int32 nSize)
{
	ASSERT(m_pSs); // 由ws判断，如果ss为空，则不会调用该语句 
	SendMsg(m_pSs->GetNetSocket(), pMsg, nSize);
}

void ClientSession::SendMsgToLs(NetMsgHead* pMsg, int32 nSize)
{
	ASSERT(m_pLs);
	SendMsg(m_pLs->GetNetSocket(), pMsg, nSize);
}

void ClientSession::SendMsgToDp(NetMsgHead* pMsg, int32 nSize)
{
	ASSERT(m_pDp);
	SendMsg(m_pDp->GetNetSocket(), pMsg, nSize);
}

void ClientSession::SendMsgToFep(NetMsgHead* pMsg, int32 nSize)
{
	ASSERT(m_pFep);
	SendMsg(m_pFep->GetNetSocket(), pMsg, nSize);
}

void ClientSession::SendMsg(NetSocket* pSocket,NetMsgHead* pMsg,int32 nSize)
{
	ASSERT(pSocket);
	pMsg->nClientSessionID = m_nSessionID;
	pSocket->ParkMsg(pMsg, nSize);
	pSocket->SendMsg();
}

void ClientSession::Exist()
{
	printf("[INFO]:Client Exist");
	m_pSocket->SetWillColse();
}

void ClientSession::SetStatus(int32 nStatus)
{
	m_nStatus = nStatus;
}

int32 ClientSession::Status()
{
	return m_nStatus;
}

void ClientSession::Update(int32 nSrvTime)
{

}

void ClientSession::SetOnServerType(int32 nServerType)
{
	m_nOnServerType = nServerType;
}

int32 ClientSession::OnServerType()
{
	return m_nOnServerType;
}

void ClientSession::SetEncryptKey(char arrNewKey[MAX_ENCRYPT_LENTH])
{
	memcpy(m_arrEncryptKey, arrNewKey, MAX_ENCRYPT_LENTH);
}

void ClientSession::GetEncryptKey(char o_arrEncryptKey[MAX_ACCOUNT_LENG])
{
	memcpy(o_arrEncryptKey, m_arrEncryptKey, MAX_ENCRYPT_LENTH);
}

ServerSession* ClientSession::GetWsSession()
{
	return m_pWs;	
}

ServerSession* ClientSession::GetSsSession()
{
	return m_pSs;
}

ServerSession* ClientSession::GetFepSession()
{
	return m_pFep;
}

ServerSession* ClientSession::GetLsSession()
{
	return m_pLs;
}

ServerSession* ClientSession::GetDpSession()
{
	return m_pDp;
}

void ClientSession::SetSceneID(int32 nSceneID)
{
	m_nSceneID = nSceneID;
}

int32 ClientSession::ScenenID()
{
	return m_nSceneID;
}

//////////////////////////////////////////////////////////////////////////
ObjPool<ClientSession> ClientSessionMgr::s_cClientSessionFactory;

ClientSessionMgr::ClientSessionMgr()
{
}

ClientSessionMgr::~ClientSessionMgr()
{

}

ClientSession* ClientSessionMgr::AddSession(int32 nClientSessionID)
{
	ClientSessionMapType::iterator it = m_mapClientSession.find(nClientSessionID);
	if(it == m_mapClientSession.end())
	{
		ClientSession* pSession = s_cClientSessionFactory.CreateObj(nClientSessionID);
		m_mapClientSession.insert(std::make_pair(nClientSessionID,pSession));
		printf("[INFO]:ClientSession Had Added ClientSessionID:%d\n", nClientSessionID);
		return pSession;
	}else
	{
		return NULL;
	}

}


void ClientSessionMgr::RemoveSession(int32 nFepSessionID)
{
	ClientSessionMapType::iterator it = m_mapClientSession.find(nFepSessionID);
	if(it != m_mapClientSession.end())
	{
		ClientSession* pSession = it->second;
		m_mapClientSession.erase(it);
		s_cClientSessionFactory.DestroyObj(pSession);
		pSession = NULL;
	}
	else
	{
		ASSERT(0);
	}
}


ClientSession*	ClientSessionMgr::GetSession(int32 nFepSessionID)
{
	ClientSessionMapType::iterator it = m_mapClientSession.find(nFepSessionID);
	if(it == m_mapClientSession.end())
	{
		return NULL;
	}
	return it->second;
}


void ClientSessionMgr::SendToAll(NetMsgHead* pMsg,int32 nSize,EServerType eToServerType)
{
	ClientSessionMapType::iterator it = m_mapClientSession.begin();
	ClientSessionMapType::iterator itEnd = m_mapClientSession.end();
	for(; it != itEnd; ++it)
	{
		switch (eToServerType)
		{
		case ESERVER_TYPE_NULL:
			it->second->SendMsg(pMsg,nSize);
			break;
		case ESERVER_TYPE_FEP:
			it->second->SendMsgToFep(pMsg,nSize);
			break;
		case ESERVER_TYPE_LS:
			it->second->SendMsgToLs(pMsg,nSize);
			break;
		case ESERVER_TYPE_WS:
			it->second->SendMsgToWs(pMsg,nSize);
			break;
		case ESERVER_TYPE_SS:
			it->second->SendMsgToSs(pMsg,nSize);
			break;
		case ESERVER_TYPE_DP:
			it->second->SendMsgToDp(pMsg,nSize);
			break;
		default:
			ASSERT(0);
			break;
		}
	}
}

void ClientSessionMgr::Update(uint32 nSrvTime)
{
	ClientSessionMapType::iterator it = m_mapClientSession.begin();
	ClientSessionMapType::iterator itEnd = m_mapClientSession.end();
	ClientSession* pSession = NULL;
	for(; it != itEnd; ++it)
	{
		pSession = it->second;
		pSession->Update(nSrvTime);
	}
}

int32 ClientSessionMgr::ConnectedSessions()
{
	return m_mapClientSession.size();
}

