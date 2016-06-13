#ifndef __GENERAL_SCENE_H_
#define __GENERAL_SCENE_H_

#include "Base.h"


/*-------------------------------------------------------------------
 * @Brief : ��ͨ�ĵ�ͼ�������糣�õ���Ϸ�е���ͼ��ͼ������ʵ����ͼ
 *          ��ͼ�����г��ù���
 * @Author:hzd 2015:11:9
 *------------------------------------------------------------------*/

class SceneMap;

class GeneralScene : public BaseScene
{
public:
	
	/*-------------------------------------------------------------------
	 * @Brief : ��ͨ�����������������ҹ��еģ�һ������ֻ��һ�������ĳ���
	 *			SceneMap����Ϸ����ʱ���Ѿ�������
	 * @Author:hzd 2015:11:9
	 *------------------------------------------------------------------*/
	GeneralScene(SceneMap& rSceneMap);
	~GeneralScene(void);

	virtual bool CheckEnter(const stEnterSceneParam& param);

	virtual bool Enter(SceneUser* pUser);

	virtual void Leave(SceneUser* pUser);

public:

	// �����ĳ�����ͼ 
	SceneMap& m_rSceneMap;
	
};

#endif


