#ifndef __COMMON_QUEST_H_
#define __COMMON_QUEST_H_

#include "BaseDefine.h"
#include "CommonDefine.h"

/*-------------------------------------------------------------------
 * @Brief : 任务
 * 
 * @Author:hzd 2015:11:24
 *------------------------------------------------------------------*/

#pragma pack(push,1)



/*-------------------------------------------------------------------
 * @Brief : 任务内存数据 
 * 
 * @Author:hzd 2015:11:2
 *------------------------------------------------------------------*/
struct StQuestDataTable
{
	int64 nCharID;
	int32 nMainLastID;

	struct QuestInfo
	{
		int32 nQuestID;
		int32 nResult0;
		int32 nResult1;
	};

	union BinData
	{
		struct 
		{
			int32 nCount;
			QuestInfo arrHadAccept[MAX_QUEST_LIST_COUNT];
		};
		BinData()
		{
			memset(this,0,MAX_BINARY_SIZE);
		}
		char revser[MAX_BINARY_SIZE];
	};

	BinData binData;

	StQuestDataTable()
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