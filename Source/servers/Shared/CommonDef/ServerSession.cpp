#include "ServerSession.h"
#include "../DyNetSocket/NetServer.h"
#include "Encrypt.h"
#include "CommonInternal.h"
#include "CommonServerOpt.h"

ServerSession::ServerSession(NetSocket* pNetSocket):m_pSocket(pNetSocket)
{
	m_nServerID = 0;
	m_nSendPackCount = 0;

	m_eSessionType = ESESSION_TYPE_NULL;
	m_eLocalServerType = ESERVER_TYPE_NULL;
	m_eRemoteServerType = ESERVER_TYPE_NULL;
}

ServerSession::~ServerSession()
{
}

void ServerSession::SendMsg(NetMsgHead* pMsg,int32 nSize, SocketCallbackBase* pCallback)
{
	m_pSocket->ParkMsg(pMsg, nSize, pCallback);
	m_pSocket->SendMsg();
}

void ServerSession::Exist()
{
	printf("[INFO]:ServerSession Exist\n");
	m_pSocket->OnEventColse();
}

void ServerSession::SetStatus(int32 nStatus)
{
	m_nStatus = nStatus;
}

int32 ServerSession::Status()
{
	return m_nStatus;
}

void ServerSession::Update(int32 nSrvTime)
{

	if (__INTERAVAL_ONE_MINUTE__) // 1分 
	{
		if (GetSessionType() == ESESSION_TYPE_CLINET)
		{
			SSReqPingToS sMsg;
			SendMsg(&sMsg, sMsg.GetPackLength());
		}
	}
}



NetSocket* ServerSession::GetNetSocket()
{
	return m_pSocket;
}


void ServerSession::SetRemoteServerType(EServerType eServerType)
{
	m_eRemoteServerType = eServerType;
}

EServerType ServerSession::LocalServerType()
{
	return m_eLocalServerType;
}

void ServerSession::SetLocalServerType(EServerType eServerType)
{
	m_eLocalServerType = eServerType;
}

EServerType ServerSession::RemoteServerType()
{
	return m_eRemoteServerType;
}

void ServerSession::SetServerID(int32 nServerID)
{
	m_nServerID = nServerID;
}

int32 ServerSession::ServerID()
{
	return m_nServerID;
}


void ServerSession::LoginToWs()
{
	if(RemoteServerType() == ESERVER_TYPE_WS)
	{
		ServerInfoOpt& rLocalInfo = NetServerOpt::Instance()->GetLocalServerInfo();
		SSReqLoginWs sMsg;
		strcpy(sMsg.arrServerAddr,rLocalInfo.strHost.c_str());
		sMsg.nServerPort = rLocalInfo.nPort;
		sMsg.nServerID = rLocalInfo.nID;
		sMsg.nServerType = rLocalInfo.nType;
		SendMsg(&sMsg,sMsg.GetPackLength());
	}
}

void ServerSession::LoginToS()
{
	if(GetSessionType() == ESESSION_TYPE_CLINET)
	{
		ServerInfoOpt& rLocalInfo = NetServerOpt::Instance()->GetLocalServerInfo(); 
		SSReqLoginS sMsg;
		sMsg.nServerID = rLocalInfo.nID; // 告诉对方的自己的信息，而本session的serverID与serverType则代表对方的  
		sMsg.nServerType = rLocalInfo.nType;
		SendMsg(&sMsg,sMsg.GetPackLength());
		printf("[NOTE]:Login to S sendMsg SID:%d,socketID:%d\n",ServerID(),GetNetSocket()->SID());
	}else
	{
		printf("[ERROR]:Login to S error\n");
	}
}

void ServerSession::LoginResult(bool bIsSuccess)
{
	SSRepLogined sMsg;
	sMsg.nResult = bIsSuccess ? SSRepLogined::SUCCESS:SSRepLogined::FAIL;
	SendMsg(&sMsg,sMsg.GetPackLength());
}

void ServerSession::SetSessionType(ESessionType eSessionType)
{
	m_eSessionType = eSessionType;
}

ESessionType ServerSession::GetSessionType()
{
	return m_eSessionType;
}

void ServerSession::SetSessionFinished()
{
	// Session类型,本地ID,远程ID
	printf("[INFO]Session Info: Session_type = %s ,Local_server_id=%d,Remote_server_id=%d\n"\
		, this->GetSessionType() == ESESSION_TYPE_CLINET ? "ForClient" : this->GetSessionType() == ESESSION_TYPE_SERVER ? "ForServer" : "UnKnown" \
		, this->LocalServerType() \
		, this->RemoteServerType());
}


//////////////////////////////////////////////////////////////////////////
ObjPool<ServerSession>	ServerSessionMgr::s_cServerSessionFactory;

ServerSessionMgr::ServerSessionMgr()
{

}

ServerSessionMgr::~ServerSessionMgr()
{

}

ServerSession* ServerSessionMgr::CreateSession(NetSocket& rSocket)
{
	SocketIDSessionMapType::iterator it = m_mapSocketIDSession.find(rSocket.SID());
	if (it != m_mapSocketIDSession.end())
	{
		printf("[WARRING]:Had Session!\n");
		return it->second;
	}

	ServerSession* pSession = s_cServerSessionFactory.CreateObj(&rSocket);
	if(pSession == NULL)
	{
		printf("Cache isnot engough!\n");
		return NULL;
	}
	if(!m_mapSocketIDSession.insert(std::make_pair(rSocket.SID(),pSession)).second)
	{
		s_cServerSessionFactory.DestroyObj(pSession);
		return NULL;
	}

	// Sessioin 
	printf("[NOTE]:Create Session Success,SocketID:%d\n", rSocket.SID());

	return pSession;
}

bool ServerSessionMgr::InitSession(int32 nServerID,ServerSession* pSession)
{
	if(!m_mapServerIdSession.insert(std::make_pair(nServerID,pSession)).second)
	{
		printf("[ERROR]:Insert fail serverid:%d\n",nServerID);
		return false;
	}
	else
	{
		printf("[NOTE]:Insert serverid:%d\n", nServerID);
	}
	
	pSession->SetServerID(nServerID);

	return true;
}


void ServerSessionMgr::RemoveSession(uint32 nServerID)
{
	ServerSession* pSession = NULL;
	ServerIdSessionMapType::iterator it = m_mapServerIdSession.find(nServerID);
	if(it != m_mapServerIdSession.end())
	{
		pSession = it->second;
		m_mapServerIdSession.erase(it);
	}else
	{
		ASSERT(0);
	}
	ASSERT(pSession);
	SocketIDSessionMapType::iterator it2 = m_mapSocketIDSession.find(pSession->GetNetSocket()->SID());
	if(it2 != m_mapSocketIDSession.end())
	{
		m_mapSocketIDSession.erase(it2);
	}
	else
	{
		ASSERT(0);
	}
	s_cServerSessionFactory.DestroyObj(pSession);
}

void ServerSessionMgr::RemoveSession(NetSocket& rSokcet)
{
	ServerSession* pSession = NULL;
	SocketIDSessionMapType::iterator it = m_mapSocketIDSession.find(rSokcet.SID());
	if(it != m_mapSocketIDSession.end())
	{
		pSession = it->second;
		m_mapSocketIDSession.erase(it);
	}
	else
	{
		ASSERT(0);
	}
	ASSERT(pSession);
	ServerIdSessionMapType::iterator it2 = m_mapServerIdSession.find(pSession->ServerID());
	if(it2 != m_mapServerIdSession.end())
	{
		m_mapServerIdSession.erase(it2);
	}else
	{
		ASSERT(0);
	}
	s_cServerSessionFactory.DestroyObj(pSession);
}


ServerSession* ServerSessionMgr::GetSession(uint32 nServerID)
{
	ServerIdSessionMapType::iterator iter = m_mapServerIdSession.find(nServerID);
	if(iter == m_mapServerIdSession.end())
	{
		return NULL;
	}
	return iter->second;
}

ServerSession* ServerSessionMgr::GetSession(NetSocket& rSokcet)
{
	SocketIDSessionMapType::iterator it = m_mapSocketIDSession.find(rSokcet.SID());
	if(it == m_mapSocketIDSession.end())
	{
		return NULL;
	}
	return it->second;
}

void ServerSessionMgr::SendToAll(NetMsgHead* pMsg,int32 nSize)
{
	ServerIdSessionMapType::iterator iter = m_mapServerIdSession.begin();
	ServerIdSessionMapType::iterator iterend = m_mapServerIdSession.end();
	for(; iter != iterend; ++iter)
	{
		iter->second->SendMsg(pMsg,nSize);
	}
}

void ServerSessionMgr::Update(int32 nSrvTime)
{
	ServerIdSessionMapType::iterator it = m_mapServerIdSession.begin();
	ServerIdSessionMapType::iterator itEnd = m_mapServerIdSession.end();
	ServerSession* pSession = NULL;
	for(; it != itEnd; ++it)
	{
		pSession = it->second;
		pSession->Update(nSrvTime);
	}
}

int32 ServerSessionMgr::CountConnected()
{
	return m_mapServerIdSession.size();
}

ServerSession* ServerSessionMgr::GetMainDbSession()
{
	ServerIdSessionMapType::iterator it = m_mapServerIdSession.begin();
	ServerIdSessionMapType::iterator itEnd = m_mapServerIdSession.end();
	ServerSession* pSession = NULL;
	for(; it != itEnd; ++it)
	{
		pSession = it->second;
		if(pSession->RemoteServerType() == ESERVER_TYPE_DP)
		{
			return pSession;
		}
	}
	return NULL;
}

ServerSession* ServerSessionMgr::GetWsSession()
{
	ServerIdSessionMapType::iterator it = m_mapServerIdSession.begin();
	ServerIdSessionMapType::iterator itEnd = m_mapServerIdSession.end();
	ServerSession* pSession = NULL;
	for(; it != itEnd; ++it)
	{
		pSession = it->second;
		if(pSession->RemoteServerType() == ESERVER_TYPE_WS)
		{
			return pSession;
		}
	}
	return NULL;
}

ServerSession* ServerSessionMgr::GetLogSession()
{
	ServerIdSessionMapType::iterator it = m_mapServerIdSession.begin();
	ServerIdSessionMapType::iterator itEnd = m_mapServerIdSession.end();
	ServerSession* pSession = NULL;
	for(; it != itEnd; ++it)
	{
		pSession = it->second;
		if(pSession->RemoteServerType() == ESERVER_TYPE_LOG)
		{
			return pSession;
		}
	}
	return NULL;
}



