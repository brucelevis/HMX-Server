#ifndef __WS_RESOURCE_MGR_HEADER__
#define __WS_RESOURCE_MGR_HEADER__

#include <vector>
#include "BaseSingle.h"
#include "typedef.h"
#include "def_resource.h"

#include "SharedIncludes.h"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
using namespace twp::app;

typedef std::map<int32,StMapInfoResCfg>			MapInfoResMapType;
typedef std::map<int32,StMapZoneResCfg>			MapZoneResMapType;
typedef std::map<int32,StMapSceneResCfg>		MapSceneResMapType;
typedef std::map<int32,StCharacterLevelCfg>		CharacterLevelMapType;
typedef std::map<int32,StRoleTypeInitCfg>		RoleTypeInitMapType;
typedef std::map<int32,StQuestInfoResCfg>		QuestResMapType;

class ResourceMgr  : public BaseSingle<ResourceMgr> 
{
public:

	ResourceMgr();
	~ResourceMgr();

	// 加载所有资源文件 
	bool LoadAllResources();


	const StCharacterLevelCfg* GetCharacterLevelBase(int32 nRoleType,int32 nLevel);

	const StRoleTypeInitCfg* GetRoleTypeInitInfoRes(int32 nRoleType);

	const StMapInfoResCfg* GetMapInfoCfg(int32 nMapID);

	const StMapZoneResCfg* GetMapZoneRes(int32 nMapID);

private:

	//------------------------资源加载函数-----------------------------------------

	/*------------------------------------------------
	 * 加载角色等级
	 *-----------------------------------------------*/
	bool LoadCharacterLevelBaseResource(const std::string& strFileName);

	/*------------------------------------------------
	 * 加载地图文件 
	 *-----------------------------------------------*/
	bool LoadMapInfoResource(const std::string& strFileName);

	/*------------------------------------------------------------------
	 * 加载创建角色的资源
	 *-----------------------------------------------------------------*/
	bool LoadRoleTypeInitResource(const std::string& strFileName);

	/*------------------------------------------------
	 * 加载地图区域资源 
	 *-----------------------------------------------*/
	bool LoadMapZoneResource(int32 nMapId,const std::string& strFileName);

	/*------------------------------------------------
	 * 加载地图阻挡资源
	 *-----------------------------------------------*/
	bool LoadMapSceneResource(int32 nMapId,const std::string& strFileName);

	/*-------------------------------------------------------------------
	 * @Brief : 加载任务 
	 * 
	 * @Author:hzd 2015:11:26
	 *------------------------------------------------------------------*/
	bool LoadQuestResource(const std::string& strFileName);

	////怪物基础
	//bool LoadMonsterBaseResource(const std::string& strFileName);
	////怪物出生
	//bool LoadMonsterBornResource(const std::string& strFileName);
	////怪物工厂
	//bool LoadMonsterFactoryResource(const std::string& strFileName);
	
private:


	//------------------------资源数据定义-------------------------------



	RoleTypeInitMapType		m_mapRoleTypeInitInfo;	// 角色类型初始化信息 
	

	// 地图资源，本Server只负责读取到配置文件，由WS通知我后才进行创建地图，等待完成后 
	// 一般在Server登录成功后，WS就会通知它创建地图 
	MapInfoResMapType		m_mapMapInfo;					// 地图信息资源(由ws通知后加载) 
	MapZoneResMapType		m_mapZoneInfo;					// 地图区域资源 
	MapSceneResMapType		m_mapSceneInfo;					// 地图阻挡资源 


	CharacterLevelMapType	m_mapCharacterLevelBase;		// 角色等级升级资源 

	QuestResMapType			m_mapQuestResInfo;				// 任务资源(剧情前端自己控制)  

};

#define g_cResourceMgr ResourceMgr::get_instance()

#endif
