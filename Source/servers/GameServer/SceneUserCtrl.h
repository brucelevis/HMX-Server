#ifndef __USER_CTRL_H_
#define __USER_CTRL_H_

#include "SharedIncludes.h"

class SceneUser;

/*-------------------------------------------------------------------
 * @Brief : 角色控制器,角色Ctrl类只能保存少量的数据，主要数据还是放在
 *			Character类中，如果是其他Ctrl，则该所在数据放在Ctrl中
 * @Author:hzd 2015:11:23
 *------------------------------------------------------------------*/
class SceneUserCtrl
{
	friend class SceneUser;
public:

	SceneUserCtrl(SceneUser* pUser);

	~SceneUserCtrl(void);

private:

	bool			m_bModify;
	SceneUser*		m_pUser; 

};



#endif

