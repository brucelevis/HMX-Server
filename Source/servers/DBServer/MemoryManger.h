#ifndef __USER_MEMORY_MGR_H_
#define __USER_MEMORY_MGR_H_

#include "SharedIncludes.h"
#include "ObjPool.h"

class ClientSession;

#pragma pack(push,1)

struct StReadStep
{

	enum 
	{
		E_STEP_NOT_NULL = 0,
		E_STEP_SQL_CHARACTER,
		E_STEP_REV_CHARACTER,
		E_STEP_SQL_QUEST,
		E_STEP_REV_QUEST,
		E_STEP_SQL_OBJECTS,
		E_STEP_REV_OBJECTS,
	};
	int32	nStep;
	int64	nCharID;
	 
	StReadStep(int64 _nCharID)
	{
		nStep= E_STEP_SQL_CHARACTER;
		nCharID = _nCharID;
	}

	StReadStep(const StReadStep& _step)
	{
		memcpy(this,&_step,sizeof(*this));
	}
};

/*

 查询回调

*/
struct StCallBackInfo
{
	virtual void QueryFinish(StUserDataForDp* pUserData)
	{

	}
};

/*------------------------------------------------------------------
 *
 * @Brief : no
 *
 * @Author: hzd 
 * @File  : UserMemoryMgr.h
 * @Date  : 2015/11/04 01:21
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
struct StUserDataMemory
{
	int32   		nInit;					// 初始化情况，0 未初始化完成，1初始化完成 
	int32			nQueryTime;				// 开始查询的时间 
	int32 			nStatus;				// 数据状态，0正常，1修改，2删除 	
	int32 			nLifeTime;				// 生命周期，以上次修改时间开始计算
	int32 			nLastAsk;				// 上次访问时间 
	StCallBackInfo*	pCallBack;				// 查询回调函数
	StUserDataForDp sUserData;				// 用户主数据（所有数据）

	StUserDataMemory(int64 _nCharID) // 构建可传数据库转为用户的数据对象 
	{
		nInit = nQueryTime =  nStatus = nLifeTime = nLastAsk = 0;
	}

	void Relase()
	{
		nInit = nQueryTime = nStatus = nLifeTime = nLastAsk = 0;
	}

};

#pragma pack(pop)

/*-------------------------------------------------------------------
 * @Brief : 玩家内存管理器，缓存在玩家数据，定时回写，
 *			自动从数据库加载数据到内存
 * 
 * @Author:hzd 2015:11:3
 *------------------------------------------------------------------*/
class MemoryManager : public BaseSingle<MemoryManager> 
{
public:
	MemoryManager(void);
	~MemoryManager(void);

	StUserDataForDp* GetUser(int64 nCharID);

	StUserDataForDp* GetUserDb(int64 nCharID,StCallBackInfo* pCbHandler);

	// 设置为有更新，会定时保存，如果没有设置这个，则会等生命周期才会保存到数据库 
	void Modifyed(int64 nCharID);

	void Update();

	void SaveNowByUID(int64 nUID);

	void RemoveByUID(int64 nUID);

public:

	// 取到角色数据回调处理 ,nSrcType
	static void DataCallbackToFrom(StUserDataMemory* pUserDataMemory);

	void DeleteByUID(int64 nUID);  // 回收内存 

private:
	typedef	std::map<int64,StUserDataMemory*>	UserMainMemoryMapType;
	
	mutex   m_cSaveMutex;
	UserMainMemoryMapType				m_mapUserMainMemory;	// User内存对象管理map
	static ObjPool<StUserDataMemory>	g_cUserMainDbFactory;	// User内存构建工厂 

};



#endif

