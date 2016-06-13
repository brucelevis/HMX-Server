#ifndef __USER_MEMORY_MGR_H_
#define __USER_MEMORY_MGR_H_

#include "SharedIncludes.h"
#include "ObjPool.h"
#include "DbConfig.h"

class ClientSession;

extern char SQL_BUFFER[MAX_SQL_BUFFER];

#pragma pack(push,1)

/*

 查询回调

*/
template<typename TMem>
struct stQueryDB
{
	virtual void QueryFinish(TMem* mem)
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
template<typename TKey,typename TData>
struct StDataMemory
{
	TKey					nCharID;				// 角色ID内存 
	int32					nSessionID;				// sessionID;
	int32   				nInit;					// 初始化情况，0 未初始化完成，1初始化完成 
	int32					nQueryTime;				// 开始查询的时间 
	int32 					nStatus;				// 数据状态，0正常，1修改，2删除 	
	int32 					nLifeTime;				// 生命周期，以上次修改时间开始计算
	int32 					nLastAsk;				// 上次访问时间 
	stQueryDB<TData>*		queryFunc;				// 查询回调函数
	TData					sUserData;				// 用户主数据（所有数据）

	StDataMemory(TKey _nCharID,int32 _nSessionID):nCharID(_nCharID),nSessionID(_nSessionID) // 构建可传数据库转为用户的数据对象 
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
template<typename TKey, typename TData, typename TMemory = StDataMemory<TKey, TData>, typename TQuery = stQueryDB<TData> >
class MemoryManager : public BaseSingle< MemoryManager<TKey, TData > >
{
private:
	typedef typename std::map<TKey, TMemory*>	MemoryMapType;
	typedef typename std::map<int32, TKey>		SessionIDKeyMapType;

	typedef typename TKey UKey;
	typedef typename TData UData;
	typedef typename TMemory UMemory;

	mutex						m_cSaveMutex;			// 锁  
	MemoryMapType				m_mapDataMemory;		// Data内存对象管理map 
	SessionIDKeyMapType			m_mapSessionIDKey;		// SessioinID对应的Key 
	ObjPool<TMemory>			g_sTableMemoryFactory;	// Data内存构建工厂 

public:

	inline TData* GetData(TKey key, bool bQuery = false)
	{
		TMemory* pMemory = GetDataMem(key,bQuery);
		if (pMemory)
		{
			pMemory->nLastAsk = Utility::NowTime();
			return &pMemory->sUserData;
		}
		return NULL;
	}

	inline TData* GetDataDb(TKey key, int32 nSessionID, const char* tableName, const dbCol *column, const char *where = NULL, const char *order = NULL, TQuery* queryFunc = NULL)
	{
		IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
		if (NULL == pDB)
		{
			FLOG_WARRING(__FUNCDNAME__, __LINE__, "IDbBase is null fail!");
			ASSERT(pDB);
			return NULL;
		}
		std::string querySql = pDB->CreateSelectSql(tableName, column,where,order);
		return GetDataDb(key, nSessionID, querySql.c_str(), queryFunc);
	}

	inline TData* GetDataDb(TKey key, int32 nSessionID, const char* querySql, TQuery* queryFunc = NULL)
	{
		TData* pData = GetData(key);
		if (pData)
		{
			return pData;
		}

		TMemory* pMem = g_sTableMemoryFactory.CreateObj(key, nSessionID);
		if (pMem == NULL)
		{
			FLOG_WARRING(__FUNCDNAME__, __LINE__, "Create StUserDataMemory memory fail!");
			ASSERT(pMem);
			return NULL;
		}

		IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
		if (NULL == pDB)
		{
			FLOG_WARRING(__FUNCDNAME__, __LINE__, "IDbBase is null fail!");
			ASSERT(pDB);
			return NULL;
		}

		int32 nowTime = Utility::NowTime();
		pMem->nInit = 0;
		pMem->nQueryTime = nowTime;
		pMem->nLifeTime = 300; // s
		pMem->nLastAsk = nowTime;
		pMem->nStatus = 0;
		pMem->queryFunc = queryFunc;
		m_mapDataMemory.insert(std::make_pair(key, pMem));
		m_mapSessionIDKey.insert(std::make_pair(nSessionID, key));
		FLOG_INFO("[INFO]:Add Mem Key:%lld , sessionID:%d\n", key, nSessionID);

		// 查询主表数据 
		struct MyDbQueryCallBack : public DBQueryFunc
		{
			UKey mKey;
			MemoryManager<UKey, UData>* mMemory;
			MyDbQueryCallBack(UKey _key, MemoryManager<UKey, UData>* _memory) :mKey(_key), mMemory(_memory)
			{
			}
			virtual void QueryResult(IDbRecordSet* pSet, int32 nCount)
			{
				const DbRecordSet* pRecordSet = static_cast<const DbRecordSet*>(pSet);
				if (pRecordSet->Rows() == 0)
				{
					FLOG_WARRING(__FUNCTION__, __LINE__, "NOT Found In Table Key");
					return;
				}
				const UData* pRevData = static_cast<const UData*>(pRecordSet->GetRecordData(0));
				if (pRevData == NULL)
				{
					FLOG_ERROR(__FUNCTION__, __LINE__, "Table is null!");
					ASSERT(0);
					return;
				}
				UData* pGetData = mMemory->GetData(mKey, true);
				if (pGetData == NULL)
				{
					FLOG_ERROR(__FUNCTION__, __LINE__, "Not Found User Mem");
					ASSERT(0);
					return;
				}
				memcpy(pGetData, pRevData, sizeof(UData));
				UMemory* pDataMem = mMemory->GetDataMem(mKey, true);
				if (pDataMem == NULL)
				{
					ASSERT(pDataMem);
					return;
				}
				pDataMem->nInit = 1;
				// 查询完毕，回调请求者给的回调函数 
				(pDataMem->queryFunc->QueryFinish)(&pDataMem->sUserData);
				S_SAFE_DELETE(pDataMem->queryFunc); // 必须要删除该指针 
			}
		};
		MyDbQueryCallBack* myCallBack = new MyDbQueryCallBack(key,this);
		pDB->ExecSQLAsync(querySql, myCallBack);
		return NULL;
	}

	inline TMemory* GetDataMem(TKey key, bool bQuery = false)
	{
		MemoryMapType::iterator it = m_mapDataMemory.find(key);
		if (it != m_mapDataMemory.end())
		{
			if (bQuery || it->second && it->second->nInit == 1)
			{
				return it->second;
			}
		}
		return NULL;
	}

	inline void Midify(TKey key)
	{
		TMemory* pMem = GetDataMem(key);
		if (pMem) pMem->nStatus = 1;
	}

	inline void Update()
	{
		// 遍历检查玩家内存，如有修改，且在5分钟之后，则会保存到数据库中去，并更新上次访问时间  
		// 如果未有，则检查生命周期是否达到，如果达到，则会保存到数据库中去，删除内存 
		// 关于ClientSession 何时删除？ 
		// 不需要加锁，用主线程 
		
		mutex::scoped_lock cLock(m_cSaveMutex);
		
		std::vector<TKey> vecSaveList;
		std::vector<TKey> vecDeleteList;
			
		MemoryMapType::iterator it = m_mapDataMemory.begin();
		MemoryMapType::iterator itEnd = m_mapDataMemory.end();
		for (; it != itEnd; ++it)
		{
			TMemory* pMemory = it->second;
			if(pMemory)
			{
				if(pMemory->nStatus == 0) // 正常，检查生命周期 
				{
		
				}
				else if(pMemory->nStatus == 1) // 有修改需要保存 
				{
					vecSaveList.push_back(it->first);
				}
				else if(pMemory->nStatus == 2) // 删除内存 
				{
					vecDeleteList.push_back(it->first);
				}
				else if (pMemory->nInit == 0)
				{
					int32 nowTime = Utility::NowTime();
					if (nowTime > pMemory->nQueryTime + pMemory->nLifeTime) // 查询超时，则删除
					{
						vecDeleteList.push_back(it->first);
					}
				}
			}
		}
		
		if(vecSaveList.size() > 0)
		{
			for (std::vector<TKey>::iterator it2 = vecSaveList.begin(); it2 != vecSaveList.end(); ++it2)
			{
				SaveNowByUID(*it2);
			}
		}
		
		if(vecDeleteList.size() > 0)
		{
			for (std::vector<TKey>::iterator it3 = vecDeleteList.begin(); it3 != vecDeleteList.end(); ++it3)
			{
				RemoveByUID(*it3);
			}
		}
		
		cLock.unlock();
	}

	inline void SaveNowByUID(TKey key)
	{
		TData* pData = GetData(key);
		if(pData == NULL)
		{
			FLOG_WARRING(__FUNCDNAME__,__LINE__,"SaveNow Not Found UserData!");
			return;
		}
		
		IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
		ASSERT(pDB);

		static const dbCol update_fields[] = {
			{ "char_id",DB_QWORD,8},
			{ "server_id",DB_DWORD,4 },
			{ "account_id",DB_QWORD,8 },
			{ "name",DB_STR,MAX_NICK_LENGTH },
			{ "type",DB_DWORD,4 },
			{ "exp",DB_DWORD,4 },
			{ "level",DB_DWORD,4 },
			{ "land_mapid",DB_DWORD,4 },
			{ "land_x",DB_DWORD,4 },
			{ "land_y",DB_DWORD,4 },
			{ "instance_mapid",DB_DWORD,4 },
			{ "instance_x",DB_DWORD,4 },
			{ "instance_y",DB_DWORD,4 },
			{ "red",DB_DWORD,4 },
			{ "blue",DB_DWORD,4 },
			{ "bin_data",DB_DWORD,MAX_BINARY_SIZE }
		};

		memset(SQL_BUFFER, 0, MAX_SQL_BUFFER);
		SPRINTF(SQL_BUFFER, "char_id =%lld", key);
		SQL_BUFFER[MAX_SQL_BUFFER - 1] = '\0';
		pDB->ExecUpdate("character_info", update_fields, (const char*)pData, SQL_BUFFER);
		
	}

	void MemoryManager::RemoveByUID(TKey key)
	{
		MemoryMapType::iterator it = m_mapDataMemory.find(key);
		if ( it != m_mapDataMemory.end())
		{
			TMemory* pMemory = it->second;
			//pMemory->sUserData.Relase();
			RemoveSessionID(pMemory->nSessionID);
			g_sTableMemoryFactory.DestroyObj(pMemory);
			m_mapDataMemory.erase(it);
			printf("[INFO]:Remove Mem ID:%lld\n", key);
		}
	}

	void RemoveSessionID(int32 nSessionID)
	{
		SessionIDKeyMapType::iterator it = m_mapSessionIDKey.find(nSessionID);
		if (it != m_mapSessionIDKey.end())
		{
			m_mapSessionIDKey.erase(it);
		}
	}

};

/*-------------------------------------------------------------------
 * @Brief : 数据库表管理 
 * 
 * @Author:hzd 2016/06/07
 *------------------------------------------------------------------*/
class DatabaseManager : public BaseSingle<DatabaseManager>
{
public:

	static MemoryManager<int64, StCharacterTable>& GetCharacterMemory()
	{
		return g_sCharacterMemory;
	}

	void Update()
	{
		g_sCharacterMemory.Update();
	}

private:

	static MemoryManager<int64, StCharacterTable> g_sCharacterMemory; // Character表 


};


#endif

