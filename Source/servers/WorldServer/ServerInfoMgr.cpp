#include "WorldServer_PCH.h"
#include "ServerInfoMgr.h"
#include "SharedIncludes.h"

ServerInfoMgr::ServerInfoMgr()
{
}


ServerInfoMgr::~ServerInfoMgr()
{
}

// 获得某服务器信息 
const ServerInfo* ServerInfoMgr::GetServerInfo(uint32 nServerID)
{
	ServerInfoMapType::const_iterator it = m_mapServerInfo.find(nServerID);
	if(it == m_mapServerInfo.end())
		return NULL;
	return &(it->second);
}

// 登录一新服务器 
bool ServerInfoMgr::OnServerLogin(ServerSession* pSession,const char arrHost[32],int32 nPort,int32 nServerID,uint32 nServerType)
{
	ServerInfo sServerInfo;
	sServerInfo.pServerSession = pSession;
	memcpy(sServerInfo.arrHost, arrHost, 32);
	sServerInfo.nPort = nPort;
	sServerInfo.nServerID = nServerID;
	sServerInfo.nServerType = nServerType;
	sServerInfo.nServerLoad = 0;

	ServerInfoMapType::iterator it = m_mapServerInfo.find(nServerID);
	if(it == m_mapServerInfo.end())
	{
		m_mapServerInfo.insert(std::make_pair(nServerID,sServerInfo));
		return true;
	}else
	{
		printf("Repeat server_id login %d",nServerID);
		FLOG_ERROR("OnServerLogin");
		return false;
	}
}

void ServerInfoMgr::OffServerLogin(ServerSession* pSession)
{
	int32 nServerID = pSession->ServerID();
	ServerInfoMapType::iterator it = m_mapServerInfo.find(nServerID);
	if(it != m_mapServerInfo.end())
	{
		m_mapServerInfo.erase(it);
		return ;
	}else
	{
		printf("Not found server_id logout %d",nServerID);
		FLOG_ERROR("OffServerLogin");
		return ;
	}
}

void ServerInfoMgr::UpdateServerInfo(uint32 nServerID,int32 nServerLoad,int32 nClientLoad)
{
	ServerInfoMapType::iterator it = m_mapServerInfo.find(nServerID); // 服务器列表 
	if(it == m_mapServerInfo.end())
	{
		printf("ServerID Not Find %d\n",nServerID);
		FLOG_WARRING("OnNotifyConnect not found serverID:%d",nServerID);
	}else
	{
		ServerInfo& rInfo = it->second;
		rInfo.nServerLoad = nServerLoad;
		rInfo.nClientLoad = nClientLoad;
	}

}


// 获得服务器列表 
void ServerInfoMgr::GetServerInfoList(int32 nServerType,ServerInfoVecType& o_rServerList)
{
	for (ServerInfoMapType::iterator it = m_mapServerInfo.begin(); it != m_mapServerInfo.end();++it)
	{
		if(nServerType == 0 || it->second.nServerType == nServerType)
		{
			o_rServerList.push_back(it->second);
		}
	}
}

ServerSession* ServerInfoMgr::GetLoadLestSession(int32 nServerType)
{
	ServerInfo* pServerInfo = NULL;
	ServerInfoVecType o_vecServerList;
	GetServerInfoList(nServerType, o_vecServerList);
	for(ServerInfoVecType::iterator it = o_vecServerList.begin(); it != o_vecServerList.end(); ++it)
	{
		if(pServerInfo == NULL)
		{
			pServerInfo = &(*it);
			if(pServerInfo->nServerLoad == 0)
				break;
		}
		else
		{
			if((*it).nServerLoad < pServerInfo->nServerLoad)
				pServerInfo = &(*it);
		}
	}
	ASSERT(pServerInfo);
	return pServerInfo->pServerSession;
}

// 广播服务器信息 
void ServerInfoMgr::BoardcastDpAndFepServerInfo2LsAndSs(uint32 nLoginServerID)
{

	ServerInfoMapType::iterator itNew = m_mapServerInfo.find(nLoginServerID);
	if(itNew == m_mapServerInfo.end())
	{
		return;
	}

	// 如果登录的是dp或fep，则向ls与ss发送该服务器信息 
	// 如果登录的是ls或ss，则向它发送dp、fep、log的服务器列表 
	// 如果登录的是log，则向ls,ss,dp,fep发log信息
	// 如果登录除了log，则要向它自己发送log
	// note:给他们发的服务器列表中不能包含他自己 
	ServerInfo& rLoginServerInfo = itNew->second;
	if(rLoginServerInfo.nServerType == ESERVER_TYPE_DP || rLoginServerInfo.nServerType == ESERVER_TYPE_FEP)
	{
		ServerInfoData sServerInfoData;
		memcpy(sServerInfoData.arrHost, rLoginServerInfo.arrHost, MAX_SERVERINFO_LIST);
		sServerInfoData.nPort = rLoginServerInfo.nPort;
		sServerInfoData.nServerID = rLoginServerInfo.nServerID;
		sServerInfoData.nServerType = rLoginServerInfo.nServerType;

		ServerInfoMapType::iterator it = m_mapServerInfo.begin();
		ServerInfoMapType::iterator itEnd = m_mapServerInfo.end();
		for(; it != itEnd; ++it)
		{
			ServerInfo& sInfo = it->second;
			switch(sInfo.nServerType)
			{
			case ESERVER_TYPE_LS:
			case ESERVER_TYPE_SS:
			{
				SSRepServerInfoList sServerList;
				sServerList.nCount = 1;
				memcpy(&sServerList.sServerInfoData[0], &sServerInfoData, sizeof(sServerInfoData));
				sInfo.pServerSession->SendMsg(&sServerList, sServerList.GetPackLength());
			}
			break;
			case ESERVER_TYPE_LOG:
				{
					SSRepServerInfoList sServerList;
					sServerList.nCount = 1;
					strcpy(sServerList.sServerInfoData[0].arrHost,sInfo.arrHost);
					sServerList.sServerInfoData[0].nPort = sInfo.nPort;
					sServerList.sServerInfoData[0].nServerID = sInfo.nServerID;
					sServerList.sServerInfoData[0].nServerType = sInfo.nServerType;
					rLoginServerInfo.pServerSession->SendMsg(&sServerList, sServerList.GetPackLength());
				}
				break;
			default:
			break;
			}
		}
	}else if (rLoginServerInfo.nServerType == ESERVER_TYPE_LS || rLoginServerInfo.nServerType == ESERVER_TYPE_SS)
	{
		int32 nCount = 0;
		ServerInfoData sServerInfoData[MAX_SERVERINFO_LIST];
		ServerInfoMapType::iterator it = m_mapServerInfo.begin();
		ServerInfoMapType::iterator itEnd = m_mapServerInfo.end();
		for(; it != itEnd; ++it)
		{
			ServerInfo& sInfo = it->second;
			switch(sInfo.nServerType)
			{
			case ESERVER_TYPE_FEP:
			case ESERVER_TYPE_DP:
			case ESERVER_TYPE_LOG:
			{
				strcpy(sServerInfoData[nCount].arrHost,sInfo.arrHost);
				sServerInfoData[nCount].nPort = sInfo.nPort;
				sServerInfoData[nCount].nServerID = sInfo.nServerID;
				sServerInfoData[nCount].nServerType = sInfo.nServerType;
				nCount++;
			}
			break;
			default:
			break;
			}
		}

		SSRepServerInfoList sServerList;
		sServerList.nCount = nCount;
		memcpy(sServerList.sServerInfoData, sServerInfoData, sizeof(sServerInfoData));
		rLoginServerInfo.pServerSession->SendMsg(&sServerList, sServerList.GetPackLength());
		
	}else if(rLoginServerInfo.nServerType == ESERVER_TYPE_LOG)
	{
		ServerInfoData sServerInfoData;
		memcpy(sServerInfoData.arrHost, rLoginServerInfo.arrHost, MAX_SERVERINFO_LIST);
		sServerInfoData.nPort = rLoginServerInfo.nPort;
		sServerInfoData.nServerID = rLoginServerInfo.nServerID;
		sServerInfoData.nServerType = rLoginServerInfo.nServerType;

		ServerInfoMapType::iterator it = m_mapServerInfo.begin();
		ServerInfoMapType::iterator itEnd = m_mapServerInfo.end();
		for(; it != itEnd; ++it)
		{
			ServerInfo& sInfo = it->second;
			switch(sInfo.nServerType)
			{
			case ESERVER_TYPE_LS:
			case ESERVER_TYPE_SS:
			case ESERVER_TYPE_DP:
			case ESERVER_TYPE_FEP:
				{
					SSRepServerInfoList sServerList;
					sServerList.nCount = 1;
					memcpy(&sServerList.sServerInfoData[0], &sServerInfoData, sizeof(sServerInfoData));
					sInfo.pServerSession->SendMsg(&sServerList, sServerList.GetPackLength());
				}
				break;
			default:
				break;
			}
		}
	}

}

void ServerInfoMgr::LoadGlobalInfo(D2WGlobalInfo& rInfo)
{
	m_nStartTimes = rInfo.nStartTimes;
}



