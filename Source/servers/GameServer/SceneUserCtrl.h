#ifndef __USER_CTRL_H_
#define __USER_CTRL_H_

#include "SharedIncludes.h"

class SceneUser;

/*-------------------------------------------------------------------
 * @Brief : ��ɫ������,��ɫCtrl��ֻ�ܱ������������ݣ���Ҫ���ݻ��Ƿ���
 *			Character���У����������Ctrl������������ݷ���Ctrl��
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

