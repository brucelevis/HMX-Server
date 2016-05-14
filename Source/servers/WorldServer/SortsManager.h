#ifndef __SORTS_MANAGER_H_
#define __SORTS_MANAGER_H_

#include "SharedIncludes.h"
#include "MySortTemplate.h"


struct FriendSortKey : public SortKeyBase
{
	int32 nID;
	int32 nLevel;
	virtual bool Compare(const SortKeyBase& _key) const
	{
		const FriendSortKey& key = static_cast<const FriendSortKey&>(_key);
		return nLevel > key.nLevel;
	}
};

struct FriendSortValue : public SortValueBase
{
	int32 nID;
	int32 nLevel;
};


/*-------------------------------------------------------------------
 * @Brief : 排序管理器(只对容器中的数据进行排序)
 * 
 * @Author:hzd 2016:1:19
 *------------------------------------------------------------------*/
class SortsManager : public BaseSingle<SortsManager>
{
public:
	SortsManager(void);
	~SortsManager(void);


	void AddFriend(const FriendSortKey& key,const FriendSortValue& value);

	MySortTemplate<FriendSortKey,FriendSortValue>& GetFriendSort(){ return sortFriend; }

	void TestData();

private:

	MySortTemplate<FriendSortKey,FriendSortValue> sortFriend;

};


#endif

