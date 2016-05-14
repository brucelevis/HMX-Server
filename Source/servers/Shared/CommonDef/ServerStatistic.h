#ifndef __SERVER_STATISTIC_H_
#define __SERVER_STATISTIC_H_

#include "BaseDefine.h"
#include "BaseSingle.h"

/*------------------------------------------------------------------
 *
 * @Brief : 统计定时管理器，用于收集当前server的相关状态向ws反应
 *
 * @Author: hzd 
 * @File  : ServerStatistic.h
 * @Date  : 2015/09/19 02:02
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
class ServerStatistic : public BaseSingle<ServerStatistic>
{
public:
	ServerStatistic(void);
	~ServerStatistic(void);

	void Update();

};


#endif


