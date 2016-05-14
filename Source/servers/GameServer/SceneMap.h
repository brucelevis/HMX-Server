#ifndef __SCENEMAP_H_
#define __SCENEMAP_H_

#include "SceneRegion.h"

#include <set>
#include <vector>
#include <typedef.h>

#include "UnorderedMap.h"
#include "BaseSingle.h"
#include "Base.h"

/*-

// 地图信息
// 现在的设计为一个地图只有一个场景，以后再扩展

*/
class SceneMap
{
public:

	SceneMap(int32 nMapId,int32 nSceneType,Point2D sTopLeft, int32 nNumX, int32 nNumY, int32 nCellLenX, int32 nCellLenY,
		const std::vector<std::vector<int8> >& vecUseableInfo);

	~SceneMap(void);

	// 对区域管理器进行更新 
	void Update();

	// 获得与rEntity相交的区域的entitys 
	void GetNeighborsEntity(Entity& rEntity ,std::vector<Entity*>& o_setEnities);

	// 获得地图ID 
	int32 MapID(){	return m_nMapId;}
	
	// 获得场景 
	BaseScene* GetBaseScene(){ return m_pBaseScene;}

private:

	// 添加地图实体 
	void AddMapEntity(Entity& rEntity);

	// 删除地图实体 
	void DelMapEntity(Entity& rEntity);

private:

	int32					m_nMapId;			// 地图ID 

	SceneRegionManager		m_oRegionManager;	// 管理对象  
	std::set<Entity*>		m_setEntities;		// 实体对象 

	int32					m_nSceneType;		// 场景类型 
	BaseScene*				m_pBaseScene;		// 普通场景 

	friend class			SceneMapManager;

};

// 角色地图信息 
struct CharacterMapInfo
{
	int32 nLandMapId;		// 主地图ID 
	int32 nInstanceMapId;  // 副本地图ID 
};

// 地图管理类 
class SceneMapManager : public BaseSingle<SceneMapManager>
{
public:

	SceneMapManager();
	~SceneMapManager();

	// 增加地图信息 
	void AddSceneMap(int32 nMapId,int32 nSceneType,Point2D sTopLeftCoordinate, int32 nXAmount, int32 nZAmount, int32 nXCellLength, int32 nZCellLength,const std::vector<std::vector<int8> >& vecUseableInfo);

	// 获得地图信息 
	SceneMap* GetSceneMap(int32 nMapId) ;

	// 获得所在的地图 
	SceneMap* GetEntitySceneMap(Entity& rEntity);

	// 地图中添加一个实体 
	bool AddSceneEnity(int32 nMapId, Entity& rEntity);

	// 删除一个地图的实体 
	bool DelSceneEnity(Entity& rEntity);

	// 地图更新 
	void Update(int32 nSrvTime);

	// 获得所在地图ID  
	int32 GetOnMapId(Entity& entity);

	//-----------------------业务场景接口--------------------------  

private:

	typedef std::map<int32,SceneMap*>			SceneMapMapType;
	typedef UNORDERED_MAP<Entity*,SceneMap*>	EntityUMapType;

	SceneMapMapType								m_mapSceneMaps;			// 地图场景信息 
	UNORDERED_MAP<Entity*,SceneMap*>			m_umapEntities;			// 实体所在地图 
	static ObjPool<SceneMap>					s_cSceneMapFactory;		// 地图工厂 

};

#endif

