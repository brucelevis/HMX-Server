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
 *  单个场景负载信息
 *-------------------------------------------*/
struct SceneInfo
{
	int32 nServerID;
	int32 nSceneID;
	int32 nLoadNum; // 当前负载量 
	SceneInfo(int32 _nServerID,int32 _nSceneID)
	{
		nServerID = _nServerID;
		nSceneID = _nSceneID;
		nLoadNum = 0;
	}
};

typedef vector<SceneInfo> SceneInfoVectorType;
typedef multimap<int32,SceneInfo*> SceneInfoUMapType;


class SceneInfoManager : public BaseSingle<SceneInfoManager>
{
	
public:
	SceneInfoManager(void);
	~SceneInfoManager(void);

	// 注册场景ID 
	void RegisterScene(int32 nServerID,std::vector<int32>& vecSceneID);

	// 注销某个服务器的所有场景 
	void RemoveScene(int32 nServerID);

	// 注销某个服下的一些场景 
	void RemoveScene(std::vector<int32>& vecSceneID,int32 nServerID = 0);

	// 获得负载最少的场景服务器 
	SceneInfo* GetLoadLestServerID(int32 nSceneID);

	/*--------------------------------------------
	 *  @brief  : 获得某个场景信息
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	SceneInfo* GetSceneInfo(int32 nServerID,int32 nSceneID);

	/*--------------------------------------------
	 *  @brief  : 刷新场景信息
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void UpdateSceneInfo(int32 nServerID,int32 nSceneID,int32 nLoadNum);

	// 获得场景信息  
	void GetSceneInfo(SceneInfoVectorType& o_vecSceneInfo);

private:


private:

	SceneInfoUMapType m_umapSceneInfo;
	static ObjPool<SceneInfo> m_sSceneInfoFactory;
};

#endif

