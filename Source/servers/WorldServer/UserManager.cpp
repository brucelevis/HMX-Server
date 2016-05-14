#include "WorldServer_PCH.h"
#include "UserManager.h"

#include "SharedIncludes.h"
#include "SceneInfoManger.h"



User::User(int32 _nCSID, ClientSession* pClientSession,const StUserDataForWs* fromDpData):m_nCSID(_nCSID),m_pClientSession(pClientSession),m_sData(fromDpData)
{
	m_nCharID = m_sData.nCharID;
}

User::~User()
{

}

void User::EnterScene(int32 nSceneID,int32 nPram0, int32 nPram1, int32 nPram2)
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
	sMsg.nClientSessionID = m_nCSID;
	sMsg.nCharacterID = m_nCharID;
	sMsg.nSceneID = nSceneID;
	sMsg.nDpServerID = m_pClientSession->GetDpServerID();
	sMsg.nFepServerID = m_pClientSession->GetFepServerID();
	sMsg.nPram0 = nPram0;
	sMsg.nPram1 = nPram1;
	sMsg.nPram2 = nPram2;
	pSceneSession->SendMsg(&sMsg,sMsg.GetPackLength());

}

int32 User::GetCurSceneID()
{
	return m_sData.nLandMapid;
}


//////////////////////////////////////////////////////////////////////////


ObjPool<User> UserManager::g_cUserFactory;

UserManager::UserManager(void)
{
}

UserManager::~UserManager(void)
{
}

User* UserManager::LoginUser(int32 nCSID,ClientSession* pClientSession, const StUserDataForWs* fromDpData)
{

	int64 nCharID = fromDpData->nCharID;
	if(User* pNewUserData = g_cUserFactory.CreateObj(nCSID,pClientSession,fromDpData))
	{
		UserMapType::iterator it = m_mapUser.find(nCharID);
		if(it != m_mapUser.end())
		{
			FLOG_WARRING(__FUNCTION__,__LINE__,"UserData Had!Why?");
			m_mapUser.erase(it);
		}
		m_mapUser.insert(std::make_pair(nCharID,pNewUserData));

		CSIDCharIDMapType::iterator it2 = m_mapCSIDCharID.find(nCSID);
		if(it2 != m_mapCSIDCharID.end())
		{
			FLOG_WARRING(__FUNCTION__,__LINE__,"CSIDCharID Had!Why?");
			m_mapCSIDCharID.erase(it2);
		}
		m_mapCSIDCharID.insert(std::make_pair(nCSID,nCharID));

		return pNewUserData;
	}else
	{
		FLOG_ERROR(__FUNCTION__,__LINE__,"Create UserData Cache Fail!");
		return NULL;
	}

}


User* UserManager::GetUserByCharID(int64 nCharID)
{
	UserMapType::iterator it = m_mapUser.find(nCharID);
	if(it != m_mapUser.end())
	{
		return it->second;
	}
	return NULL;
}

User* UserManager::GetUserByCSID(int32 nCSID)
{
	if(int64 nCharID = GetCharIDByCSID(nCSID))
	{
		return GetUserByCharID(nCharID);
	}
	return NULL;
}

int64 UserManager::GetCharIDByCSID(int32 nCSID)
{
	CSIDCharIDMapType::iterator it = m_mapCSIDCharID.find(nCSID);
	if(it != m_mapCSIDCharID.end())
	{
		return it->second;
	}
	return 0;
}

void UserManager::LogoutUser(int32 nCSID)
{
	CSIDCharIDMapType::iterator itCSID = m_mapCSIDCharID.find(nCSID);
	if(itCSID == m_mapCSIDCharID.end())
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Not Found csid %d",nCSID);
		return ;
	}

	int64 nCharID = itCSID->second;
	if(nCharID < 1)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"CharID < 1");
		return;
	}

	m_mapCSIDCharID.erase(itCSID);

	UserMapType::iterator itUser = m_mapUser.find(nCharID);
	if(itUser == m_mapUser.end())
	{
		return ;
	}
	m_mapUser.erase(itUser);
	
}

