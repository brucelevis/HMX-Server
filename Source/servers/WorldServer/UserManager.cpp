#include "WorldServer_PCH.h"
#include "UserManager.h"

#include "SharedIncludes.h"
#include "SceneInfoManger.h"
#include "Utility.h"


WorldUser::WorldUser(ClientSession* pClientSession,const StUserDataForWs* fromDpData):m_pClientSession(pClientSession),m_sData(fromDpData)
{
	m_nCharID = m_sData.nCharID;
}

WorldUser::~WorldUser()
{

}

void WorldUser::EnterScene(int32 nSceneID,int32 nPram0, int32 nPram1, int32 nPram2)
{

	// 如何找到最合适的权重 最合适为负载最低 当前负载值 = 本server选择数量 + 本scene中使用数量 
	SceneInfo* pSceneInfo = SceneInfoManager::Instance()->GetLoadLestServerID(nSceneID);
	if(pSceneInfo == NULL)
	{
		ASSERT(pSceneInfo);
		return ; // 通知选择错误的场景 
	}

	// 选择进入该场景 
	ServerSession* pSceneSession  = ServerSessionMgr::Instance()->GetSession(pSceneInfo->nServerID);
	if(pSceneSession == NULL)
	{
		ASSERT(pSceneSession);
		return ;
	}


	if (NULL == m_pClientSession)
	{
		ASSERT(m_pClientSession);
		return;
	}

	m_pClientSession->SetForLocalWs2(pSceneSession,nSceneID);

	// 发送请求进入Scene 
	W2SReqEnterScene sMsg;
	sMsg.nClientSessionID = m_pClientSession->GetSessionID();
	sMsg.nCharacterID = m_nCharID;
	sMsg.nSceneID = nSceneID;
	sMsg.nDpServerID = m_pClientSession->GetDpServerID();
	sMsg.nFepServerID = m_pClientSession->GetFepServerID();
	sMsg.nPram0 = nPram0;
	sMsg.nPram1 = nPram1;
	sMsg.nPram2 = nPram2;
	pSceneSession->SendMsg(&sMsg,sMsg.GetPackLength());

}

int32 WorldUser::GetCurSceneID()
{
	return m_sData.nLandMapid;
}

void WorldUser::SendToFep(NetMsgHead* pMsg, int32 nSize, SocketCallbackBase* pCallback)
{
	if (m_pClientSession)
	{
		m_pClientSession->SendMsgToFep(pMsg,nSize,pCallback);
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
			FLOG_WARRING(__FUNCTION__,__LINE__,"UserData Had!Why?");
			m_mapUser.erase(it);
		}
		AddSessionID(pClientSession->GetSessionID(),nCharID);
		m_mapUser.insert(std::make_pair(nCharID,pNewUserData));
		FLOG_INFO("[INFO]:Add WorldUser ID:%lld\n", nCharID);
		return pNewUserData;
	}else
	{
		FLOG_ERROR(__FUNCTION__,__LINE__,"Create UserData Cache Fail!");
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

