#ifndef __INSTANCE_SCENE_H_
#define __INSTANCE_SCENE_H_

#include "Base.h"

class SceneMap;

/*-------------------------------------------------------------------
 * @Brief : �������������࣬���и�������Ҫ�̳и��� 
 * 
 * @Author:hzd 2015:11:9
 *------------------------------------------------------------------*/
class InstanceScene : public BaseScene
{
public:
	/*-------------------------------------------------------------------
	 * @Brief : ���������Ǹ������������ģ�һ��server�����N���������в�ͬ
	 *			����Ҽ������(��̬����)
	 * @Author:hzd 2015:11:9
	 *------------------------------------------------------------------*/
	InstanceScene(SceneMap& rSceneMap);
	~InstanceScene(void);

private:

	// �����ĳ�����ͼ 
	SceneMap& m_rSceneMap;
};



#endif

