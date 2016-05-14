#include "WorldServer_PCH.h"
#include "SharedIncludes.h"
#include "SceneInfoManger.h"
#include "ServerInfoMgr.h"

ObjPool<SceneInfo> SceneInfoManager::m_sSceneInfoFactory;
SceneInfoManager::SceneInfoManager(void)
{
}


SceneInfoManager::~SceneInfoManager(void)
{
}


// 注册场景ID  
void SceneInfoManager::RegisterScene(int32 nServerID,std::vector<int32>& vecSceneID)
{
	for (std::vector<int32>::iterator it = vecSceneID.begin(); it != vecSceneID.end();++it)
	{
		int32 nSceneID = *it;
		SceneInfo* pSceneInfo = GetSceneInfo(nServerID,nSceneID);
		if(pSceneInfo)
		{
			FLOG_WARRING(__FUNCTION__,__LINE__,"Reapt ServerId:%d,SceneId:%d",nServerID,nSceneID)
			continue;
		}
		pSceneInfo = m_sSceneInfoFactory.CreateObj(nServerID,nSceneID);
		ASSERT(pSceneInfo);
		m_umapSceneInfo.insert(std::make_pair(nServerID,pSceneInfo));
	}
}

// 注销某个服务器的所有场景(事件来源，1主动注销，2ss被断开) 
void SceneInfoManager::RemoveScene(int32 nServerID)
{
	SceneInfoUMapType::iterator itLower = m_umapSceneInfo.lower_bound(nServerID);
	SceneInfoUMapType::iterator itUpper = m_umapSceneInfo.upper_bound(nServerID);

	vector<int32> vecSceneID;

	for (;itLower != itUpper;++itLower)
	{
		vecSceneID.push_back(itLower->second->nSceneID);
	}

	RemoveScene(vecSceneID,nServerID);

}

void SceneInfoManager::RemoveScene(std::vector<int32>& vecSceneID,int32 nServerID/* = 0*/)
{

	if(!vecSceneID.size())
		return;

	SceneInfoUMapType::iterator itBegin;
	SceneInfoUMapType::iterator itEnd;
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
		SceneInfo* pSceneInfo = itBegin->second;
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

SceneInfo* SceneInfoManager::GetLoadLestServerID(int32 nSceneID)
{

	SceneInfoUMapType::iterator itBegin = m_umapSceneInfo.begin();
	SceneInfoUMapType::iterator itEnd = m_umapSceneInfo.end();
	ServerInfoMgr* gServerInfoMgr =	ServerInfoMgr::Instance();

	int32 nMinRank = 0;
	SceneInfo* pMinSceneInfo = NULL;
	for (;itBegin != itEnd;++itBegin)
	{
		int32 nServerID = itBegin->first;
		if(const ServerInfo* pServerInfo = gServerInfoMgr->GetServerInfo(nServerID))
		{
			SceneInfo* pSceneInfoTmp = itBegin->second;
			int32 nCurrRank = pServerInfo->nServerLoad + pSceneInfoTmp->nLoadNum;
			if(pSceneInfoTmp->nSceneID != nSceneID)
			{
				continue;
			}
			if(nMinRank == 0)
			{
				nMinRank = nCurrRank;
				pMinSceneInfo = pSceneInfoTmp;
				continue;
			}
			if(nCurrRank < nMinRank)
			{
				nMinRank = nCurrRank;
				pMinSceneInfo = pSceneInfoTmp;
			}
		}else
		{
			ASSERT(0);
		}
	}
	return pMinSceneInfo;

}

void SceneInfoManager::UpdateSceneInfo(int32 nServerID,int32 nSceneID,int32 nLoadNum)
{
	SceneInfoUMapType::iterator itBegin = m_umapSceneInfo.lower_bound(nServerID);
	for (;itBegin != m_umapSceneInfo.upper_bound(nServerID); ++itBegin)
	{
		if(itBegin->second->nSceneID == nSceneID)
		{
			itBegin->second->nLoadNum = nLoadNum;
			break;
		}
	}
}

void SceneInfoManager::GetSceneInfo(SceneInfoVectorType& o_vecSceneInfo)
{
	o_vecSceneInfo.clear();
	SceneInfoUMapType::iterator itBegin = m_umapSceneInfo.begin();
	for (;itBegin != m_umapSceneInfo.end(); ++itBegin)
	{
		o_vecSceneInfo.push_back(*(itBegin->second));
	}
}

SceneInfo* SceneInfoManager::GetSceneInfo(int32 nServerID,int32 nSceneID)
{
	SceneInfoUMapType::iterator it = m_umapSceneInfo.lower_bound(nServerID);	
	SceneInfoUMapType::iterator itEnd = m_umapSceneInfo.upper_bound(nServerID);
	for (; it != itEnd; ++it)
	{
		if(it->second->nSceneID == nSceneID)
			return it->second;
	}
	return NULL;
}

