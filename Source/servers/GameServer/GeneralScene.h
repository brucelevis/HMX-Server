#ifndef __GENERAL_SCENE_H_
#define __GENERAL_SCENE_H_

#include "Base.h"


/*-------------------------------------------------------------------
 * @Brief : 普通的地图场景，如常用的游戏中的跑图地图，该类实现跑图
 *          地图的所有常用功能
 * @Author:hzd 2015:11:9
 *------------------------------------------------------------------*/

class SceneMap;

class GeneralScene : public BaseScene
{
public:
	
	/*-------------------------------------------------------------------
	 * @Brief : 普通场景，这个是所有玩家共有的，一个服务只有一个这样的场景
	 *			SceneMap在游戏启动时就已经创建好
	 * @Author:hzd 2015:11:9
	 *------------------------------------------------------------------*/
	GeneralScene(SceneMap& rSceneMap);
	~GeneralScene(void);

	virtual bool CheckEnter(const stEnterSceneParam& param);

	virtual bool Enter(SceneUser* pUser);

	virtual void Leave(SceneUser* pUser);

public:

	// 关连的场景地图 
	SceneMap& m_rSceneMap;
	
};

#endif


