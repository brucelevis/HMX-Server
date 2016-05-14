#ifndef __MY_SORT_TEMPLATE_H_
#define __MY_SORT_TEMPLATE_H_

#include "SharedIncludes.h"

/*-------------------------------------------------------------------
 * @Brief : 这里需要打通与DB表相通，当DB启动后，需要加载DB的表的排序榜
 *			数据
 * @Author:hzd 2016:1:28
 *------------------------------------------------------------------*/

struct SortKeyBase
{
	virtual bool Compare(const SortKeyBase& key) const = 0;
};

struct SortValueBase
{
	
};

template<typename TSortKey>
struct SortCompareBase : public std::binary_function<TSortKey,TSortKey,bool>
{
public:
	SortCompareBase(){}
	~SortCompareBase(){}
	bool operator()(const TSortKey lSortVal, const TSortKey rSortVal) const
	{
		return lSortVal.Compare(rSortVal);
	}
};



// 排序类模板类 
template<typename TSortKey,typename TSortValue,typename TSortComparetor = SortCompareBase<TSortKey> >
class MySortTemplate
{
public:
	typedef std::multimap<TSortKey,TSortValue,TSortComparetor>	SortMultiMap;
	typedef typename SortMultiMap::iterator						SortMultiMapIter;
public:
	MySortTemplate(void):mCurSortList(mAllSortList[0])
	{

	};
	~MySortTemplate(void)
	{
		
	};

	enum // 周期分类 
	{
		SORT_CYCLE_HOUR = 0,
		SORT_CYCLE_DAY,
		SORT_CYCLE_WEEK,
		SORT_CYCLE_MONTY,
	};

	enum // 存储分类 
	{
		SORT_LIST_CUR = 0,
		SORT_LIST_LAST,
		SORT_LIST_MAX,
	};

private:

	// 辅助数据 
	int32 mCycle;
	int32 mCountMax;
	std::string mTableName;
	SortMultiMap& mCurSortList;				  // 当前数据容器 
	SortMultiMap mAllSortList[SORT_LIST_MAX]; // 数据容器 


public:

	void AddSort(const TSortKey& key,const TSortValue& value)
	{
		mCurSortList.insert(std::make_pair(key,value));
	}

	void DelSort(const TSortKey& key)
	{
		SortMultiMapIter it = mCurSortList.find(key);
		if(it != mCurSortList.end())
		{
			mCurSortList.erase(it);
		}
	}

	void UpdateSort(const TSortKey& key,const TSortValue& value)
	{
		SortMultiMapIter it = mCurSortList.find(key);
		if(it != mCurSortList.end())
		{
			it->second = value;
		}
	}

	void GetTopList(std::vector<TSortValue>& vecSort,int32 nNum = 10)
	{
		SortMultiMapIter it = mCurSortList.begin();
		SortMultiMapIter itEnd = mCurSortList.end();
		for (; it != itEnd && vecSort.size() < nNum;++it)
		{
			vecSort.push_back(it->second);
		}
	}

};

#endif

