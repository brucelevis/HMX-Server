#ifndef __COMMON_BAGS_H_
#define __COMMON_BAGS_H_

#include "BaseDefine.h"

/*-------------------------------------------------------------------
 * @Brief : ����
 * 
 * @Author:hzd 2015:11:24
 *------------------------------------------------------------------*/

#pragma pack(push,1)



/*-------------------------------------------------------------------
 * @Brief : ��Ʒ�ڴ����ݣ�DP�У�
 * 
 * @Author:hzd 2015:11:2
 *------------------------------------------------------------------*/
struct StObjectsDataTable
{
	int64 nCharID;
	StObjectsDataTable()
	{
		memset(this,0,sizeof(*this));
	}
	inline int32 GetLength()
	{
		return sizeof(*this);
	}
};


#pragma pack(pop)





#endif