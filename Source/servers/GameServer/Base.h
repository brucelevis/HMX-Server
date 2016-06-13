#ifndef __BASE_H_
#define __BASE_H_

#include "BaseDefine.h"
#include "CommonDefine.h"


// 功能类基本定义 

enum EPkSceneType
{
	E_PK_SCENE_TYPE_NULL = 0,		// 无  
	E_PK_SCENE_TYPE_GENERAL,	// 普通 
	E_PK_SCENE_TYPE_INSTANCE,	// 副本 
};

class SceneUser;

// 场景基类 
class BaseScene
{
public:
	BaseScene(){};

	// 检查是否可以进入该场景 
	// 预前检查/轻度(玩家内存还未加载)
	virtual bool CheckEnter(const stEnterSceneParam& param) = 0;

	// 进入该场景并检查(深度检查，已经加载玩家内存) 
	virtual bool Enter(SceneUser* pUser) = 0;

	// 退出场景 
	virtual void Leave(SceneUser* pUser) = 0;

protected:


};



// 功能基类 

template<typename TTable>
class BaseCtrl
{
public:
	BaseCtrl(){}
	virtual void LoadData(const TTable& data){};
	virtual void GetData(TTable& data){};
	virtual void SendMainData(){};
};


#endif 



