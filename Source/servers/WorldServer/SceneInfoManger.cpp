#include "WorldServer_PCH.h"
#include "SharedIncludes.h"
#include "SceneInfoManger.h"
#include "ServerInfoMgr.h"

ObjPool<SceneBaseInfo> SceneRegisterManager::m_sSceneInfoFactory;

ObjPool<SceneInfo> SceneManager::m_sSceneInfoFactory;

SceneRegisterManager::SceneRegisterManager(void)
{
}


SceneRegisterManager::~SceneRegisterManager(void)
{
}


// 注册场景ID  
void SceneRegisterManager::RegisterScene(int32 nServerID, int32 nSceneID, int32 nSceneType)
{
	const SceneBaseInfo* pSceneInfo = GetSceneBaseInfo(nServerID, nSceneID);
	if (pSceneInfo)
	{
		FLOG_WARRING("Reapt ServerId:%d,SceneId:%d", nServerID, nSceneID);
		return;
	}

	FLOG_INFO("Register Scene: scene_id %d,scene_type:%d", nServerID, nSceneID, nSceneType);

	SceneBaseInfo* pNewSceneInfo = m_sSceneInfoFactory.CreateObj();
	ASSERT(pNewSceneInfo);
	pNewSceneInfo->nServerID = nServerID;
	pNewSceneInfo->nSceneID = nSceneID;
	pNewSceneInfo->nSceneType = nSceneType;
	m_umapSceneInfo.insert(std::make_pair(nServerID, pNewSceneInfo));

	if (pNewSceneInfo->nSceneType == 1)
	{
		SceneManager::Instance()->InitStaticScene(*pNewSceneInfo);
	}
}

// 注销某个服务器的所有场景(事件来源，1主动注销，2ss被断开) 
void SceneRegisterManager::RemoveScene(int32 nServerID)
{
	SceneBaseUMapType::iterator itLower = m_umapSceneInfo.lower_bound(nServerID);
	SceneBaseUMapType::iterator itUpper = m_umapSceneInfo.upper_bound(nServerID);

	vector<int32> vecSceneID;

	for (;itLower != itUpper;++itLower)
	{
		vecSceneID.push_back(itLower->second->nSceneID);
	}

	RemoveScene(vecSceneID,nServerID);

}

void SceneRegisterManager::RemoveScene(std::vector<int32>& vecSceneID,int32 nServerID/* = 0*/)
{

	if(!vecSceneID.size())
		return;

	SceneBaseUMapType::iterator itBegin;
	SceneBaseUMapType::iterator itEnd;
	if(nServerID)
	{
		itBegin = m_umapSceneInfo.lower_bound(nServerID);
		itEnd = m_umapSceneInfo.upper_bound(nServerID);
	}else
	{
		itBegin = m_umapSceneInfo.begin();
		itEnd = m_umapSceneInfo.end();
	}

	vector<int32>::iterator itBeginScene = vecSceneID.begin();
	vector<int32>::iterator itEndScene = vecSceneID.end();
	for(; itBegin != itEnd;)
	{
		SceneBaseInfo* pSceneInfo = itBegin->second;
		int32 nSceneIDTmp = pSceneInfo->nSceneID;
		if(itEndScene != std::find(itBeginScene,itEndScene,nSceneIDTmp))
		{
			m_sSceneInfoFactory.DestroyObj(pSceneInfo);
			m_umapSceneInfo.erase(itBegin++);
		}else
		{
			itBegin++;
		}
	}

	// 无需要通知fep，fep会ping时检查到ss断开，会断开client断开，这里ws会保存client上一次的ss所在位置，然后即出client 
	// 设计思想，ws与fep自己处理client信息\ 
	// 找到与该serverID的Client,然后将他们退出 

}

const SceneBaseInfo* SceneRegisterManager::GetSceneBaseInfo(int32 nServerID, int32 nSceneID)
{
	SceneBaseUMapType::const_iterator it;
	SceneBaseUMapType::const_iterator itEnd;
	if (nServerID)
	{
		it = m_umapSceneInfo.lower_bound(nServerID);
		itEnd = m_umapSceneInfo.upper_bound(nServerID);
	}
	else
	{
		it = m_umapSceneInfo.begin();
		itEnd = m_umapSceneInfo.end();
	}
	for (; it != itEnd; ++it)
	{
		if (it->second->nSceneID == nSceneID)
			return it->second;
	}
	return NULL;
}

bool SceneRegisterManager::GetSceneBaseInfoByServerID(int32 nServerID, vector<SceneBaseInfo>& o_vecSceneInfo)
{
	SceneBaseUMapType::const_iterator it = m_umapSceneInfo.lower_bound(nServerID);
	SceneBaseUMapType::const_iterator itEnd = m_umapSceneInfo.upper_bound(nServerID);
	for (; it != itEnd; ++it)
	{
		o_vecSceneInfo.push_back(*it->second);
	}
	return !o_vecSceneInfo.empty();
}

bool SceneRegisterManager::GetSceneBaseInfoBySceneID(int32 nSceneID, vector<SceneBaseInfo>& o_vecSceneInfo)
{

	SceneBaseUMapType::const_iterator it = m_umapSceneInfo.begin();
	for (; it != m_umapSceneInfo.end(); ++it)
	{
		if (it->second->nSceneID == nSceneID)
		{
			o_vecSceneInfo.push_back(*it->second);
		}
	}
	return !o_vecSceneInfo.empty();
}


//---------------------------------------------------------------------

void SceneManager::InitStaticScene(const SceneBaseInfo& baseInfo)
{

	int32 idx = baseInfo.nServerID * 1000 + baseInfo.nSceneID;

	std::map<int32, SceneInfo*>::iterator it = m_mapSceneInfo.find(idx);
	if (it != m_mapSceneInfo.end())
	{
		printf("[ERROR]:repeat server_scene_idx %d\n",idx);
		ASSERT(0);
		return;
	}

	SceneInfo* pSceneInfo = m_sSceneInfoFactory.CreateObj();

	pSceneInfo->idx = idx;
	pSceneInfo->nServerID = baseInfo.nServerID;
	pSceneInfo->nSceneID = baseInfo.nSceneID;
	pSceneInfo->nSceneType = baseInfo.nServerID;
	pSceneInfo->nCreateTime = Utility::NowTime();
	pSceneInfo->nDestroyTime = 0;

	m_mapSceneInfo.insert(std::make_pair(idx, pSceneInfo));
}

void SceneManager::CreateDynamicScene(const SceneBaseInfo& baseInfo)
{

}


void SceneManager::UpdateSceneInfo(int32 idx, int32 nLoadNum)
{

}

SceneInfo* SceneManager::GetLoadLestServerID(int32 nSceneID)
{
	SceneInfoUMapType::iterator it = m_mapSceneInfo.begin();
	SceneInfoUMapType::iterator itEnd = m_mapSceneInfo.end();

	int32 nMinRank = 0;
	SceneInfo* pMinSceneInfo = NULL;
	for (; it != itEnd; ++it)
	{
		SceneInfo* pSceneInfo = it->second;
		if (!pSceneInfo || pSceneInfo->nSceneID != nSceneID)
			continue;
		int32 nServerID = it->second->nServerID;
		const ServerInfo* pServerInfo = ServerInfoMgr::Instance()->GetServerInfo(nServerID);
		if (pServerInfo == NULL)
		{
			ASSERT(0);
			continue;
		}
		int32 nCurrRank = pServerInfo->nServerLoad + pSceneInfo->nLoadNum;
		if (nMinRank == 0)
		{
			nMinRank = nCurrRank;
			pMinSceneInfo = pSceneInfo;
			continue;
		}
		if (nCurrRank < nMinRank)
		{
			nMinRank = nCurrRank;
			pMinSceneInfo = pSceneInfo;
		}
	}
	return pMinSceneInfo;
}

SceneInfo* SceneManager::GetSceneInfo(int32 idx)
{
	std::map<int32, SceneInfo*>::iterator it = m_mapSceneInfo.find(idx);
	if (it != m_mapSceneInfo.end())
	{
		return it->second;
	}
	return NULL;
}

void SceneManager::GetSceneInfo(std::vector<SceneInfo>& o_vecSceneInfo, int32 nSceneID)
{
	std::map<int32, SceneInfo*>::iterator it = m_mapSceneInfo.begin();
	for (; it != m_mapSceneInfo.end(); ++it)
	{
		if (!nSceneID || it->second->nSceneID == nSceneID)
		{
			o_vecSceneInfo.push_back(*it->second);
		}
	}
	
}

