#include "WorldServer_PCH.h"
#include "SortsManager.h"
#include "Utility.h"

SortsManager::SortsManager(void)
{
}

SortsManager::~SortsManager(void)
{
}

void SortsManager::AddFriend(const FriendSortKey& key,const FriendSortValue& value)
{
	sortFriend.AddSort(key,value);
}


void SortsManager::TestData()
{

	for (int32 i = 0; i < 10; ++i )
	{
		FriendSortKey key;
		FriendSortValue val;
		key.nID = i + 1;
		key.nLevel = RangedRand(1,10);
		val.nID = i + 1;
		val.nLevel = key.nLevel;
		AddFriend(key,val);
	}

	MySortTemplate<FriendSortKey,FriendSortValue>& friendSort = GetFriendSort();

	std::vector<FriendSortValue> topList;
	friendSort.GetTopList(topList);



}
