#ifndef __STATISTIC_H_
#define __STATISTIC_H_

#include "BaseDefine.h"
#include "BaseSingle.h"

/*
 *
 *	Detail: 收集信息
 *   
 *  Created by hzd 2015/06/19  
 *
 */
class Statistic : public BaseSingle<Statistic>
{
public:
	Statistic(void);
	~Statistic(void);

	void Update(int32 nSrvTime);

};


#endif



