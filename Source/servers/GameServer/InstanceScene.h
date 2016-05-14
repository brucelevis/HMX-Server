#ifndef __INSTANCE_SCENE_H_
#define __INSTANCE_SCENE_H_

#include "Base.h"

class SceneMap;

/*-------------------------------------------------------------------
 * @Brief : 副本场景共用类，所有副本都需要继承该类 
 * 
 * @Author:hzd 2015:11:9
 *------------------------------------------------------------------*/
class InstanceScene : public BaseScene
{
public:
	/*-------------------------------------------------------------------
	 * @Brief : 副本场景是根据条件创建的，一个server会出现N个副本，有不同
	 *			的玩家加入进来(动态副本)
	 * @Author:hzd 2015:11:9
	 *------------------------------------------------------------------*/
	InstanceScene(SceneMap& rSceneMap);
	~InstanceScene(void);

private:

	// 关连的场景地图 
	SceneMap& m_rSceneMap;
};



#endif

