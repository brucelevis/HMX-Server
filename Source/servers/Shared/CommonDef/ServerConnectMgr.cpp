#include "ServerConnectMgr.h"


ServerConnectMgr::ServerConnectMgr()
{
}


ServerConnectMgr::~ServerConnectMgr()
{
}

bool ServerConnectMgr::ConnectToServer(int32 nServerID,int32 nServerType,const char arrHost[32],int32 nPort,NetMsgEnter fEnter,NetMsgOn fMsg,NetMsgExit fExit)
{
	NetClientInfoVecterType::iterator it = m_vecNetClientInfoAll.begin();
	NetClientInfoVecterType::iterator itEnd = m_vecNetClientInfoAll.end();
	for (; it != itEnd;++it)
	{
		if((*it)->nServerID == nServerID)
			return false;
	}

	NetClient* pNetClient = new NetClient(64 * 1024,64 * 1024,64 * 1024,64 * 1024);

	NetClientInfo* pConn = new NetClientInfo();
	pConn->pNetClient = pNetClient;
	pConn->nServerID = nServerID;
	pConn->nServerType = nServerType;
	m_vecNetClientInfoAdd.push_back(pConn);

	printf("[INFO]:New Server Size : %lld\n", m_vecNetClientInfoAdd.size());

	pNetClient->SetAddress(arrHost, nPort);
	pNetClient->SetHandler(fEnter, fMsg, fExit);
	pNetClient->Start();

	printf("[NOTE]:Connect to Server severid:%d nServerType:%d\n", nServerID, nServerType);

	return true;

}

void ServerConnectMgr::Update(int32 nSrvTime)
{
	// 检查要断开的NetClient 
	if (m_vecNetClientDisConnect.size() > 0)
	{
		DisconnectServerIDVectorType::iterator itServerID = m_vecNetClientDisConnect.begin();
		DisconnectServerIDVectorType::iterator itEndServerID = m_vecNetClientDisConnect.end();
		for (; itServerID != itEndServerID; ++itServerID)
		{
			uint32 nServerID = *itServerID;
			for (NetClientInfoVecterType::iterator it = m_vecNetClientInfoAll.begin(); it != m_vecNetClientInfoAll.end();)
			{
				if ((*it)->nServerID == nServerID)
				{
					delete (*it)->pNetClient;
					(*it)->pNetClient = NULL;
					it = m_vecNetClientInfoAll.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		m_vecNetClientDisConnect.clear();
	}

	// 新增的Client(这里也必须确认有连接成功返回才加入到All中)
	if (m_vecNetClientInfoAdd.size() > 0)
	{
		NetClientInfoVecterType::iterator itAdd = m_vecNetClientInfoAdd.begin();
		for (; itAdd != m_vecNetClientInfoAdd.end();)
		{
			NetClientInfo* pClientInfo = *itAdd;
			if (pClientInfo && pClientInfo->pNetClient && pClientInfo->pNetClient->IsConnnected())
			{
				m_vecNetClientInfoAll.push_back(pClientInfo);
				pClientInfo->pNetClient->OnConnected();
				itAdd = m_vecNetClientInfoAdd.erase(itAdd);
			}
			else
			{
				++itAdd;
			}
		}
	}
	
	// 执行现有的NetClient消息
	if (m_vecNetClientInfoAll.size() > 0)
	{
		NetClientInfoVecterType::iterator it = m_vecNetClientInfoAll.begin();
		NetClientInfoVecterType::iterator itEnd = m_vecNetClientInfoAll.end();
		for (; it != itEnd; ++it)
		{
			NetClient* pNetClient = (*it)->pNetClient;
			if (pNetClient)
			{
				pNetClient->Update();//1
			}
		}

	}
	


}

// 删除一个Server 
void ServerConnectMgr::RemoveServer(int32 nServerID) // 2 由1中进入2
{
	m_vecNetClientDisConnect.push_back(nServerID);
}

NetClientInfo* ServerConnectMgr::GetNetClient(NetSocket& rSocket)
{
	NetClientInfoVecterType::iterator it = m_vecNetClientInfoAll.begin();
	NetClientInfoVecterType::iterator itEnd = m_vecNetClientInfoAll.end();
	for(; it != itEnd; ++it)
	{
		if((*it)->pNetClient->GetSocket().SID() == rSocket.SID())
		{
			return (*it);
		}
	}
	printf("[INFO]:New Had Server Size:%lld\n", m_vecNetClientInfoAll.size());
	return NULL;
}

int32 ServerConnectMgr::CountConnected()
{
	return m_vecNetClientInfoAll.size();
}


