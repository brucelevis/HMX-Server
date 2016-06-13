#include "GameServer_PCH.h"
#include "SceneUserManager.h"
#include "SceneUser.h"
#include "SceneMap.h"
#include "ResourceMgr.h"
#include "def_entity.h"


using namespace std;

ObjPool<SceneUser> SceneUserManager::s_cUserFactory;


// 添加角色 
SceneUser* SceneUserManager::AddUser(ClientSession* pClientSession, const ::protobuf::Character& charactor)
{
	if (!pClientSession)
	{
		ASSERT(0);
		return NULL; 
	}

	int64 nUserID = charactor.char_id();
	int32 nCSID = pClientSession->GetSessionID();
	if(GetUserByUID(nUserID) || GetUserByCSID(nCSID))
	{
		ASSERT(0);
		return NULL;
	}

	SceneUser* pUser = s_cUserFactory.CreateObj(nUserID,pClientSession);
	if(pUser == NULL)
	{
		ASSERT(0);
		return NULL;
	}

	pUser->UnSerialize(charactor);

	m_setPlayerExs.insert(pUser);
	m_umapUsers.insert(make_pair(nUserID,pUser));
	m_umapCSIDUser.insert(make_pair(nCSID,pUser));
	m_umapEntityUser.insert(make_pair(pUser->ID(),pUser));

	return pUser;
}

// 删除角色 
void SceneUserManager::RemoveUser(int64 nUserID)
{

	UNORDERED_MAP<int64,SceneUser*>::iterator it = m_umapUsers.find(nUserID);
	if (it == m_umapUsers.end() )
	{
		ASSERT(0);
		return;
	}

	SceneUser* pUser = it->second;
	
	int64 nEntryID = pUser->ID();

	UNORDERED_MAP<int64,SceneUser*>::iterator it2 = m_umapEntityUser.find(nEntryID);
	if(it2 == m_umapEntityUser.end())
	{
		ASSERT(0);
		return;
	}

	m_umapEntityUser.erase(it2);

	int32 nCSID = pUser->GetClientSession()->GetSessionID();

	UNORDERED_MAP<int32,SceneUser*>::iterator it3 = m_umapCSIDUser.find(nCSID);
	if(it3 == m_umapCSIDUser.end())
	{
		ASSERT(0);
		return;
	}
	m_umapCSIDUser.erase(it3);

	m_umapUsers.erase(it);
	
	m_setPlayerExs.erase(pUser);


	SceneMapManager::Instance()->DelSceneEnity(*pUser);
	
	//从工厂中回收
	s_cUserFactory.DestroyObj(pUser);

}


void SceneUserManager::Update(int32 nServerTimes)
{
	UNORDERED_SET<SceneUser*>::iterator it = m_setPlayerExs.begin();
	for (; it != m_setPlayerExs.end(); ++it)
	{
		(*it)->Update(nServerTimes);
	}
}

SceneUser* SceneUserManager::GetUserByUID(int64 nUID)
{
	UNORDERED_MAP<int64,SceneUser*>::iterator it = m_umapUsers.find(nUID);
	if(it != m_umapUsers.end())
		return it->second;
	return NULL;
}

SceneUser* SceneUserManager::GetUserByEntityID(int64 nEntityID)
{
	UNORDERED_MAP<int64,SceneUser*>::iterator it = m_umapEntityUser.find(nEntityID);
	if(it != m_umapEntityUser.end())
		return it->second;
	return NULL;
}

SceneUser* SceneUserManager::GetUserByCSID(int32 nCSID)
{
	UNORDERED_MAP<int32,SceneUser*>::iterator it = m_umapCSIDUser.find(nCSID);
	if( it != m_umapCSIDUser.end())
		return it->second;
	return NULL;
}



