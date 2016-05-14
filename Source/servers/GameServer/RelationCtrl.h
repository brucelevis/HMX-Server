#ifndef __RELATION_H_
#define __RELATION_H_

/*-------------------------------------------------------------------
 * @Brief :  关注/被关注--其实与好友功能差不多 
 * 
 * @Author:hzd 2016:2:19
 *------------------------------------------------------------------*/

class SceneUser;

class RelationCtrl
{
public:
	RelationCtrl(SceneUser*	pUser);
	~RelationCtrl(void);

private:

	SceneUser*	m_pUser;

	// 我关注的
	
	// 被关注的


};

#endif

