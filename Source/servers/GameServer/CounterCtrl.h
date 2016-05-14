#ifndef __COUNTER_CTRL_H_
#define __COUNTER_CTRL_H_

class SceneUser;


/*------------------------------------------------------------------
 *
 * @Brief : 记录数功能
 *
 * @Author: hzd 
 * @File  : CounterCtrl.h
 * @Date  : 2016/01/08 00:46
 * @Copyright (c) 2016,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
class CounterCtrl
{
public:
	CounterCtrl(SceneUser* _pUser);
	~CounterCtrl(void);

	void InData();

	void OutData();



private:

	SceneUser* mUser;

};

#endif


