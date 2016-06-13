#include "WorldServer_PCH.h"
#include "UserManager.h"

#include "SharedIncludes.h"
#include "SceneInfoManger.h"
#include "Utility.h"


WorldUser::WorldUser(ClientSession* pClientSession,const StUserDataForWs* fromDpData):m_pCSession(pClientSession),m_sData(fromDpData)
{
	m_nCharID = m_sData.nCharID;
}

WorldUser::~WorldUser()
{

}

void WorldUser::SendToFep(NetMsgHead* pMsg, int32 nSize)
{
	if (m_pCSession)
	{
		m_pCSession->SendMsgToFep(pMsg,nSize);
	}
}

//////////////////////////////////////////////////////////////////////////


ObjPool<WorldUser> UserManager::g_cUserFactory;

UserManager::UserManager(void)
{
}

UserManager::~UserManager(void)
{
}

WorldUser* UserManager::AddWorldUser(ClientSession* pClientSession, const StUserDataForWs* fromDpData)
{
	int64 nCharID = fromDpData->nCharID;
	if(WorldUser* pNewUserData = g_cUserFactory.CreateObj(pClientSession,fromDpData))
	{
		UserMapType::iterator it = m_mapUser.find(nCharID);
		if(it != m_mapUser.end())
		{
			FLOG_WARRING("UserData Had!Why?");
			m_mapUser.erase(it);
		}
		AddSessionID(pClientSession->GetSessionID(),nCharID);
		m_mapUser.insert(std::make_pair(nCharID,pNewUserData));
		FLOG_INFO("[INFO]:Add WorldUser ID:%lld\n", nCharID);
		return pNewUserData;
	}else
	{
		FLOG_ERROR("Create UserData Cache Fail!");
		return NULL;
	}
}


WorldUser* UserManager::GetUserByCharID(int64 nCharID)
{
	UserMapType::iterator it = m_mapUser.find(nCharID);
	if(it != m_mapUser.end())
	{
		return it->second;
	}
	return NULL;
}


void UserManager::RemoveWorldUser(int64 nCharID)
{
	UserMapType::iterator itUser = m_mapUser.find(nCharID);
	if(itUser == m_mapUser.end())
	{
		FLOG_INFO("[ERROR]:Not Found WorldUser ID:%lld\n", nCharID);
		return ;
	}
	else
	{
		if (WorldUser* pWUser = itUser->second)
		{
			RemoveSessionID(pWUser->GetSessionID());
		}
		m_mapUser.erase(itUser);
	}
	FLOG_INFO("[INFO]:Remove WorldUser ID:%lld\n", nCharID);
	
}

void UserManager::AddSessionID(int32 nSessionID, int64 nCharID)
{
	SessionIDUserIDMapType::iterator it = m_mapSessionIDUserID.find(nSessionID);
	if (it != m_mapSessionIDUserID.end())
	{
		it->second = nCharID;
	}
	else
	{
		m_mapSessionIDUserID.insert(std::make_pair(nSessionID,nCharID));
	}
}

void UserManager::RemoveSessionID(int32 nSessionID)
{
	SessionIDUserIDMapType::iterator it = m_mapSessionIDUserID.find(nSessionID);
	if (it != m_mapSessionIDUserID.end())
	{
		m_mapSessionIDUserID.erase(it);
	}
}

int64 UserManager::GetUserIDBySessionID(int32 nSessionID)
{
	SessionIDUserIDMapType::iterator it = m_mapSessionIDUserID.find(nSessionID);
	if (it != m_mapSessionIDUserID.end())
	{
		return it->second;
	}
	return 0;
}

