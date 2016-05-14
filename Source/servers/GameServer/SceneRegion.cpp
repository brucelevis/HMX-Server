#include "SceneRegion.h"

#include <assert.h>

#include "Entity.h"
#include "SceneChannel.h"

ObjPool<SceneRegion> SceneRegionManager::s_cSceneRegionFactory;	

using namespace std;

SceneRegion::SceneRegion(const Rectangle2D& rRange) : m_sScope(rRange)
{

}

// 注册本区域频道 
void SceneRegion::RegistReceiveChannels(IChannelRegist& rEntity)
{
	// 给实现注册对应的频道 
	for (int32 i = 0; i < m_vecChannels.size(); ++i)
	{
		m_vecChannels[i]->RegistReceiver(rEntity);
	}
	std::cout << "[SceneRegion]Register Recvier New Zone :xz(" << m_sScope.sTopLeft.nX << "," << m_sScope.sTopLeft.nY << "),("<< m_sScope.sBottomRight.nX << "," << m_sScope.sBottomRight.nY << ")" << std::endl;
}

// 取消注册收集信息 
void SceneRegion::CancelReceiveChannels(IChannelRegist& rEntity)
{
	// 取消对应的注册 
	for (int32 i = 0; i < m_vecChannels.size(); ++i)
	{
		m_vecChannels[i]->CancelReceiver(rEntity);
	}
	std::cout << "[SceneRegion]Cancal Reciver Zone:xz(" << m_sScope.sTopLeft.nX << "," << m_sScope.sTopLeft.nY << "),("<< m_sScope.sBottomRight.nX << "," << m_sScope.sBottomRight.nY << ")" << std::endl;
}

void SceneRegion::RegistBroadcasterChannels(IChannelRegist& rEntity)
{
	for (int32 i = 0; i < m_vecChannels.size(); ++i)
	{
		m_vecChannels[i]->RegistBroadcaster(rEntity);
	}
	std::cout << "[SceneRegion]Register Broadcaster Zone:xy(" << m_sScope.sTopLeft.nX << "," << m_sScope.sTopLeft.nY << "),("<< m_sScope.sBottomRight.nX << "," << m_sScope.sBottomRight.nY << ")" << std::endl;
}

void SceneRegion::CancelBroadcasterChannels(IChannelRegist& rEntity)
{
	for (int32 i = 0; i < m_vecChannels.size(); ++i)
	{
		m_vecChannels[i]->CancelBroadcaster(rEntity);
	}
	std::cout << "[SceneRegion]Cancel Broadcaster Zone:xy(" << m_sScope.sTopLeft.nX << "," << m_sScope.sTopLeft.nY << "),("<< m_sScope.sBottomRight.nX << "," << m_sScope.sBottomRight.nY << ")" << std::endl;
}

// 增加频道 
void SceneRegion::AddChannel(Channel& rChannel)
{
	m_vecChannels.push_back(&rChannel);
	SceneChannelManager::Instance()->AddSceneChannel(rChannel);
}

// 增加实体 
void SceneRegion::AddEntity(Entity& rEntity)
{
	std::set<Entity*>::iterator it = m_setEntities.find(&rEntity);
	if(it == m_setEntities.end())
	{
		m_setEntities.insert(&rEntity);
	}
	RegistBroadcasterChannels(rEntity);
}

// 移除实体 
void SceneRegion::DelEntity(Entity& rEntity)
{
	m_setEntities.erase(&rEntity);
#ifdef _DEBUG
	FLOG_INFO("[SceneRegion]SceneRegin DelEntity : topLeft(%d,%d),bottomRight(%d,%d)"\
		, m_sScope.sTopLeft.nX, m_sScope.sTopLeft.nY	\
		, m_sScope.sBottomRight.nX, m_sScope.sBottomRight.nY);
#endif
	CancelBroadcasterChannels(rEntity);
}

// 检测是否在区域内 
bool SceneRegion::InScope(const Point2D& rPosition)
{
	return CheckInside(rPosition, m_sScope);
}

const set<Entity*> SceneRegion::GetEntities()
{
	return m_setEntities;
}

SceneRegionManager::SceneRegionManager(Point2D sTopLeftCoordinate, int32 nXAmount, int32 nYAmount, int32 nXCellLength, int32 nYCellLength,
	const vector<vector<int8> >& vecUseableInfo)
	: m_nXAmount(nXAmount), m_nYAmount(nYAmount), m_nXCellLength(nXCellLength), m_nYCellLength(nYCellLength)
{
	assert(vecUseableInfo.size() == nYAmount && vecUseableInfo[0].size() == nXAmount);
	
	// 初始化管理区域 
	/*
	
	原点坐标设置为左上角
	+y
	|
	|
	|
	|
	|
	._____________-> +x

	*/

	{
		m_vecRegions.resize(nYAmount);
		for (int32 iz = 0; iz < nYAmount; ++iz)
		{
			for (int32 ix = 0; ix < nXAmount; ++ix)
			{
				if (vecUseableInfo[iz][ix])
				{
					Rectangle2D sScope(
							sTopLeftCoordinate.nX + ix * nXCellLength,
							sTopLeftCoordinate.nY - iz * nYCellLength,
							sTopLeftCoordinate.nX + (ix + 1) * nXCellLength - 1,
							sTopLeftCoordinate.nY - (iz + 1) * nYCellLength + 1
						);
					m_vecRegions[iz].push_back(s_cSceneRegionFactory.CreateObj(sScope));
				}
				else
				{
					m_vecRegions[iz].push_back(0);
				}
			}
		}



		{
			// 生成所有的频道 
			vector<Channel*> vecChannels;
			for (int32 iy = 0; iy < nYAmount; ++iy)
			{
				for (int32 ix = 0; ix < nXAmount; ++ix)
				{
					vecChannels.clear();
					if(m_vecRegions[iy][ix])
					{
						// 位置信息频道 
						if(Channel* pChannel = g_cPositionChannelFactory.CreateObj(E_CHANNEL_REGIST_TYPE_POS_B,E_CHANNEL_REGIST_TYPE_POS_R))
							m_vecRegions[iy][ix]->AddChannel(*pChannel);

						// 生物体信息 
						if(Channel* pChannel = g_cCreatureChannelFactory.CreateObj(E_CHANNEL_REGIST_TYPE_CREATURE_B,E_CHANNEL_REGIST_TYPE_CREATURE_R))
							m_vecRegions[iy][ix]->AddChannel(*pChannel);

						// 技能信息频道 
						if(Channel* pChannel = g_cSpellChannelFactory.CreateObj(E_CHANNEL_REGIST_TYPE_SPELL_B,E_CHANNEL_REGIST_TYPE_SPELL_R))
							m_vecRegions[iy][ix]->AddChannel(*pChannel);

						// Buff信息 
						if(Channel* pChannel = g_cBuffChannelFactory.CreateObj(E_CHANNEL_REGIST_TYPE_BUFF_B,E_CHANNEL_REGIST_TYPE_BUFF_R))
							m_vecRegions[iy][ix]->AddChannel(*pChannel);
					}
				}
			}
			
		}
	}

	// 大区域范围 
	m_sScope.sTopLeft = sTopLeftCoordinate;
	m_sScope.sBottomRight.nX = m_sScope.sTopLeft.nX + nXAmount * nXCellLength - 1;
	m_sScope.sBottomRight.nY = 1;
}

SceneRegionManager::~SceneRegionManager()
{
	for (int32 iz = 0; iz < m_nYAmount; ++iz)
	{
		for (int32 ix = 0; ix < m_nXAmount; ++ix)
		{
			if (SceneRegion* pSceneRegion = m_vecRegions[iz][ix])
			{
				s_cSceneRegionFactory.DestroyObj(pSceneRegion);
			}
		}
	}
}

// 添加场景实体 
void SceneRegionManager::AddSceneEntity(Entity& rEntity)
{
	if (SceneRegion* pInRegion = GetBelongRegion(rEntity.GetPosition()))
	{
		// 添加到所在区域 
		pInRegion->AddEntity(rEntity);
	
		// 注册相关区域监听 
		// 计算相交区域 
		set<SceneRegion*> collisionRegions;
		GetCollisionRegion(rEntity.InViewRange(), collisionRegions);

		if (rEntity.IsListen())
		{
			set<SceneRegion*>::iterator it = collisionRegions.begin();
			set<SceneRegion*>::iterator itEnd = collisionRegions.end();
			for (; it != itEnd; ++it)
			{
				(*it)->RegistReceiveChannels(rEntity);
			}
		}

		// 添加管理信息 
		m_umapEntityRegions.insert(make_pair(&rEntity, pair<SceneRegion*, set<SceneRegion*> >(pInRegion, collisionRegions)));
	}
}

// 删除场景实体 
void SceneRegionManager::DelSceneEntity(Entity& rEntity)
{
	EntitySceneRegionCollectUMapType::iterator it = m_umapEntityRegions.find(&rEntity);
	if( it == m_umapEntityRegions.end())
		return;

	// 取消相关区域监听 
	if (rEntity.IsListen())
	{
		set<SceneRegion*>::iterator region_it = it->second.second.begin();
		set<SceneRegion*>::iterator region_it_end = it->second.second.end();
		for (; region_it != region_it_end; ++region_it)
		{
			(*region_it)->CancelReceiveChannels(rEntity);
		}
	}

	//删除所在的区域 
	it->second.first->DelEntity(rEntity);

	// 移除管理信息 
	m_umapEntityRegions.erase(it);
}

// 更新频道注册、更新区域中的实体集合 
void SceneRegionManager::Update()
{
	EntitySceneRegionCollectUMapType::iterator it = m_umapEntityRegions.begin();
	EntitySceneRegionCollectUMapType::iterator it_end = m_umapEntityRegions.end();

	// 根据实体位置更新，以及所属的矩形指针 
	for (;it!=it_end;++it)
	{
		Entity* pEntity = it->first;
		SceneRegion* pLastBelongRegion = it->second.first;
		set<SceneRegion*>& rOldCollistionRegions = it->second.second;

		// 检查所属区域指向是否改变，若改变则更新 
		if (!pLastBelongRegion->InScope(pEntity->GetPosition()))
		{
			// 添加到新区域中 
			if (SceneRegion* pNewBelongRegion = GetBelongRegion(pEntity->GetPosition()))
			{
				// 从旧区域中删除 
				pLastBelongRegion->DelEntity(*pEntity);

				std::cout << "[SceneRegion]Entity Leave :" << pEntity->ID() << "xy(" << pLastBelongRegion->m_sScope.sTopLeft.nX << "," << pLastBelongRegion->m_sScope.sTopLeft.nY << "),(" << pLastBelongRegion->m_sScope.sBottomRight.nX << "," << pLastBelongRegion->m_sScope.sBottomRight.nY << ")" << std::endl;
				// 加入新的所属区域 
				pNewBelongRegion->AddEntity(*pEntity);
				std::cout << "[SceneRegion]Entiry Enter:" << pEntity->ID() << "xy(" << pNewBelongRegion->m_sScope.sTopLeft.nX << "," << pNewBelongRegion->m_sScope.sTopLeft.nY << "),(" << pNewBelongRegion->m_sScope.sBottomRight.nX << "," << pNewBelongRegion->m_sScope.sBottomRight.nY << ")" << std::endl;

				// 离开相关区域操作 
				{
					// 计算相交区域 
					set<SceneRegion*> newCollisionRegions;
					GetCollisionRegion(pEntity->OutViewRange(), newCollisionRegions);

					static vector<SceneRegion*> vecLeaveRegions;
					vecLeaveRegions.resize(rOldCollistionRegions.size());
					vector<SceneRegion*>::iterator retEndPosTmp;

					retEndPosTmp = set_difference(rOldCollistionRegions.begin(), rOldCollistionRegions.end(), newCollisionRegions.begin(), newCollisionRegions.end(), vecLeaveRegions.begin());
					
					for (vector<SceneRegion*>::iterator iter = vecLeaveRegions.begin(); iter != retEndPosTmp; ++iter)
					{
						(*iter)->CancelReceiveChannels(*pEntity);
					}
				}

				{// 求新增区域 
					// 计算相交区域 
					set<SceneRegion*> newCollisionRegions;
					GetCollisionRegion(pEntity->OutViewRange(), newCollisionRegions);

					static vector<SceneRegion*> vecNewRegions;
					vecNewRegions.resize(newCollisionRegions.size());
					vector<SceneRegion*>::iterator retEndPosTmp;

					retEndPosTmp = set_difference(newCollisionRegions.begin(), newCollisionRegions.end(), rOldCollistionRegions.begin(), rOldCollistionRegions.end(), vecNewRegions.begin());
					
					for (vector<SceneRegion*>::iterator iter = vecNewRegions.begin(); iter != retEndPosTmp; ++iter)
					{
						(*iter)->RegistReceiveChannels(*pEntity);
						
					}

					// 更新管理信息 
					it->second.first = pNewBelongRegion;
					it->second.second = newCollisionRegions;
				}
				
			}
		}	
	}

}

SceneRegion* SceneRegionManager::GetSceneRegion(int32 nZIndex, int32 nXIndex) const
{
	if (nZIndex < m_nYAmount && nXIndex < m_nXAmount)
	{
		return m_vecRegions[nZIndex][nXIndex];
	}
	return 0;
}

static int32 FixCoordinate(int32 nValXY)
{
	return nValXY < 1 ? 0 : nValXY - 1;
}

void SceneRegionManager::GetCollisionRegion(const Rectangle2D& i_rSrcRange, std::set<SceneRegion*>& o_setCollisionRegions)
{
	o_setCollisionRegions.clear();

	// 相交区域 
	Rectangle2D sIntersectRegion;
	if (GetIntersect(m_sScope, i_rSrcRange, sIntersectRegion))
	{
		// -1 是为了处理边界问题 
		int32 nXStart = FixCoordinate(sIntersectRegion.sTopLeft.nX) / m_nXCellLength;
		int32 nXEnd = FixCoordinate(sIntersectRegion.sBottomRight.nX) / m_nXCellLength;
		int32 nZStart = FixCoordinate(m_sScope.sTopLeft.nY - sIntersectRegion.sTopLeft.nY) / m_nYCellLength;
		int32 nZEnd = FixCoordinate(m_sScope.sTopLeft.nY - sIntersectRegion.sBottomRight.nY) / m_nYCellLength;

		for (int32 nZ = nZStart; nZ < nZEnd; ++nZ)
		{
			for (int32 nX = nXStart; nX < nXEnd; ++nX)
			{
				if (SceneRegion* pRegion = GetSceneRegion(nZ, nX))
				{
					o_setCollisionRegions.insert(pRegion);
				}
			}
		}
	}
}


SceneRegion* SceneRegionManager::GetBelongRegion(const Point2D& rSrcCoordinate)
{
	if (CheckInside(rSrcCoordinate, m_sScope))
	{
		int32 nXIndex = FixCoordinate(rSrcCoordinate.nX) / m_nXCellLength;
		int32 nZIndex = FixCoordinate(m_sScope.sTopLeft.nY - rSrcCoordinate.nY) / m_nYCellLength;
		return GetSceneRegion(nZIndex, nXIndex);
	}
	else
	{
		return NULL;
	}
}

