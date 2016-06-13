#ifndef __SCENE_INFO_MANAGER_H_
#define __SCENE_INFO_MANAGER_H_

#include "BaseDefine.h"
#include "BaseSingle.h"
#include "ObjPool.h"


/*-------------------------------------------------------------------
 * @Brief : 本类记录场景服的信息，记录场景服在WS上注册了哪些场景ID
 *			同时
 * @Author:hzd 2015:11:18
 *------------------------------------------------------------------*/

/*--------------------------------------------
 *  单个场景基本信息
 *-------------------------------------------*/
struct SceneBaseInfo
{
	int32 nServerID;	// serverid
	int32 nSceneID;		// mapid
	int32 nSceneType;	// 1普通,2副本
	SceneBaseInfo()
	{
		nServerID = nSceneID = nSceneType = 0;
	}
};

typedef vector<SceneBaseInfo> SceneBaseVectorType;
typedef multimap<int32,SceneBaseInfo*> SceneBaseUMapType; // serverID=>xxx

/*
	场景动态信息 
*/
struct SceneInfo
{
	int32 idx;
	int32 nServerID;
	int32 nSceneID;
	int32 nSceneType;
	int32 nLoadNum;
	int32 nCreateTime;
	int32 nDestroyTime;
	SceneInfo()
	{
		idx = nServerID = nSceneID = nSceneType = nLoadNum = nCreateTime = nDestroyTime = 0;
	}
};

typedef vector<SceneInfo> SceneInfoVectorType;
typedef multimap<int32, SceneInfo*> SceneInfoUMapType;// idx=>xxx

/*

	场景基本类型消息 

*/
class SceneRegisterManager : public BaseSingle<SceneRegisterManager>
{
	
public:
	SceneRegisterManager(void);
	~SceneRegisterManager(void);

	// 注册场景ID 
	void RegisterScene(int32 nServerID,int32 nSceneID,int32 nSceneType);

	// 注销某个服务器的所有场景 
	void RemoveScene(int32 nServerID);

	// 注销某个服下的一些场景 
	void RemoveScene(std::vector<int32>& vecSceneID,int32 nServerID = 0);

	const SceneBaseInfo* GetSceneBaseInfo(int32 nServerID,int32 nSceneID);

	bool GetSceneBaseInfoByServerID(int32 nServerID, vector<SceneBaseInfo>& o_vecSceneInfo);

	bool GetSceneBaseInfoBySceneID(int32 nSceneID,vector<SceneBaseInfo>& o_vecSceneInfo);

private:


private:

	SceneBaseUMapType m_umapSceneInfo;
	static ObjPool<SceneBaseInfo> m_sSceneInfoFactory;
};

/*--

	动态场景管理器 

*/
class SceneManager : public BaseSingle<SceneManager>
{
public:

	// 静态类 
	void InitStaticScene(const SceneBaseInfo& baseInfo);

	// 创建动态地图（副本类）
	void CreateDynamicScene(const SceneBaseInfo& baseInfo);
	
	/*--------------------------------------------
	*  @brief  : 刷新场景信息
	*-------------------------------------------*/
	void UpdateSceneInfo(int32 idx, int32 nLoadNum);

	// 获得负载最少的场景服务器 
	SceneInfo* GetLoadLestServerID(int32 nSceneID);
	
	SceneInfo* GetSceneInfo(int32 idx);

	void GetSceneInfo(std::vector<SceneInfo>& o_vecSceneInfo, int32 nSceneID = 0);

private:


	static ObjPool<SceneInfo> m_sSceneInfoFactory;

	std::map<int32, SceneInfo*> m_mapSceneInfo;

};


#endif

