#include "DBServer_PCH.h"
#include "MemoryManger.h"
#include "Config.h"
#include "../DyNetMysql/DbIncludes.h"
#include "../DyNetSocket/NetIncludes.h"
#include "../Shared/CommonDef/SharedIncludes.h"

ObjPool<StUserDataMemory> MemoryManager::g_cUserMainDbFactory;

MemoryManager::MemoryManager(void)
{
}

MemoryManager::~MemoryManager(void)
{
}

void MemoryManager::Update()
{
	// 遍历检查玩家内存，如有修改，且在5分钟之后，则会保存到数据库中去，并更新上次访问时间  
	// 如果未有，则检查生命周期是否达到，如果达到，则会保存到数据库中去，删除内存 
	// 关于ClientSession 何时删除？ 
	// 不需要加锁，用主线程 

	mutex::scoped_lock cLock(m_cSaveMutex);

	std::vector<int64> vecSaveList;
	std::vector<int64> vecDeleteList;
	
	UserMainMemoryMapType::iterator it = m_mapUserMainMemory.begin();
	UserMainMemoryMapType::iterator itEnd = m_mapUserMainMemory.end();
	for (; it != itEnd; ++it)
	{
		StUserDataMemory* userMem = it->second;
		if(userMem)
		{
			if(userMem->nStatus == 0) // 正常，检查生命周期 
			{

			}
			else if(userMem->nStatus == 1) // 有修改需要保存 
			{
				vecSaveList.push_back(it->first);
			}
			else if(userMem->nStatus == 2) // 删除内存 
			{
				vecDeleteList.push_back(it->first);
			}
			else if (userMem->nInit == 0)
			{
				int32 nowTime = Utility::NowTime();
				if (nowTime > userMem->nQueryTime + 60) // 查询超时，则删除
				{
					vecDeleteList.push_back(it->first);
				}
			}
		}
	}

	if(vecSaveList.size() > 0)
	{
		for (std::vector<int64>::iterator it2 = vecSaveList.begin(); it2 != vecSaveList.end(); ++it2)
		{
			SaveNowByUID(*it2);
		}
	}

	if(vecDeleteList.size() > 0)
	{
		for (std::vector<int64>::iterator it3 = vecDeleteList.begin(); it3 != vecDeleteList.end(); ++it3)
		{
			RemoveByUID(*it3);
		}
	}

	cLock.unlock();

}

StUserDataForDp* MemoryManager::GetUser(int64 nCharID)
{
	UserMainMemoryMapType::iterator it = m_mapUserMainMemory.find(nCharID);
	if(it != m_mapUserMainMemory.end())
	{
		StUserDataMemory* pUserDataMemory = it->second;
		if (pUserDataMemory->nInit != 1) return NULL;
		pUserDataMemory->nLastAsk = Utility::NowTime();
		return &pUserDataMemory->sUserData;
	}
	return NULL;
}


void MemoryManager::SaveNowByUID(int64 nUID)
{
	StUserDataForDp* pUserData = GetUser(nUID);
	if(pUserData == NULL)
	{
		FLOG_WARRING(__FUNCDNAME__,__LINE__,"SaveNow Not Found UserData!");
		return;
	}

	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	ASSERT(pDB);

	static char arrData[MAX_BINARY_SIZE];
	
	{

		// 角色保存 
		StCharacterDataTable& sChar = pUserData->sCharacterTable;

		ostringstream ss;
		ss << "UPDATE `user_info` SET ";
		ss << "`name`='" << sChar.arrName;
		ss << "',`type`=" << sChar.nType;
		ss << ",`exp`=" << sChar.nExp;
		ss << ",`level`=" << sChar.nLevel;
		ss << ",`land_mapid`=" << sChar.nLandMapId;
		ss << ",`land_x`=" << sChar.nLandX;
		ss << ",`land_y`=" << sChar.nLandY;
		ss << ",`instance_mapid`=" << sChar.nInstanceMapId;
		ss << ",`instance_x`=" << sChar.nInstanceX;
		ss << ",`instance_y`=" << sChar.nInstanceY;
		ss << ",`red`=" << sChar.nRed;
		ss << ",`blue`=" << sChar.nBlue;
		ss << ",`created_time`=" << Utility::NowTime();

		memset(arrData,0,sizeof(arrData));
		pDB->BinaryToString((char*)(&sChar.binData),arrData,sizeof(arrData));

		ss << ",`bin_data`='" << arrData << "'";
		ss << "  WHERE `user_info`.char_id =" << sChar.nCharID;

		// 二进制采用存储过程 

		memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
		SPRINTF(SQL_BUFFER,"%s" ,ss.str().c_str());
		SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
		pDB->ExecAsyncSQL(SQL_BUFFER,NULL);

	}


	{

		StQuestDataTable& sQuest = pUserData->sQuestTable;

		// 任务保存 
		ostringstream ss;
		ss << "UPDATE `quest_info` SET ";
		ss << "`main_last_id`=" << sQuest.nMainLastID;

		memset(arrData,0,sizeof(arrData));
		pDB->BinaryToString((char*)(&sQuest.binData),arrData,sizeof(arrData));

		ss << ",`bin_data`='" << arrData << "'";
		ss << "  WHERE `quest_info`.char_id =" << sQuest.nCharID;

		memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
		SPRINTF(SQL_BUFFER,"%s" ,ss.str().c_str());
		SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
		pDB->ExecAsyncSQL(SQL_BUFFER,NULL);

	}

}


void MemoryManager::RemoveByUID(int64 nUID)
{
	UserMainMemoryMapType::iterator it = m_mapUserMainMemory.find(nUID);
	if ( it != m_mapUserMainMemory.end())
	{
		StUserDataMemory* udm = it->second;
		udm->Relase();
		g_cUserMainDbFactory.DestroyObj(udm);
		m_mapUserMainMemory.erase(it);
	}
}


StUserDataForDp* MemoryManager::GetUserDb(int64 nCharID, StCallBackInfo* pCbHandler)
{
	UserMainMemoryMapType::iterator it = m_mapUserMainMemory.find(nCharID);
	if(it != m_mapUserMainMemory.end())
	{
		StUserDataMemory* pUserDataMemory = it->second;
		if (pUserDataMemory->nInit != 1)
		{
			// 数据初始化未完成，比如还在加载数据库数据中，或者中途加载中断
			// 这样的数据，在update中，需要判断然后删除，让下一次查找再重新加载 
			// 不能在此地方删除 
			return NULL;
		}
		pUserDataMemory->nLastAsk = Utility::NowTime();
		return &pUserDataMemory->sUserData;
	}else
	{
		if(StUserDataMemory* pUserDataMemory = g_cUserMainDbFactory.CreateObj(nCharID))
		{
			int32 nowTime = Utility::NowTime();
			pUserDataMemory->nInit = 0;
			pUserDataMemory->nQueryTime = nowTime;
			pUserDataMemory->nLifeTime = 60; // s
			pUserDataMemory->nLastAsk = nowTime;
			pUserDataMemory->pCallBack = pCbHandler;
			m_mapUserMainMemory.insert(std::make_pair(nCharID,pUserDataMemory));

			IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
			if (NULL == pDB)
			{
				ASSERT(pDB);
				return NULL;
			}

			// 查询主角数据 
			{
				struct MyCallBack : public MyDBCallBack
				{
					StUserDataMemory* pUserDataMemory;
					virtual void QueryResult(IDbRecordSet* pSet, int32 nCount)
					{
						const DbRecordSet* pRecordSet = static_cast<const DbRecordSet*>(pSet);
						if (pRecordSet->Rows() == 0)
						{
							FLOG_WARRING(__FUNCTION__, __LINE__, "NOT Found In Data Character");
							return;
						}

						const StCharacterDataTable* pCharDb = static_cast<const StCharacterDataTable*>(pRecordSet->GetRecordData(0));
						if (pCharDb == NULL)
						{
							FLOG_ERROR(__FUNCTION__, __LINE__, "Character is null!");
							ASSERT(0);
							return;
						}
						pUserDataMemory->sUserData.LoadCharacterDataForDp(*pCharDb);// 把数据加载到内存 
					}

					MyCallBack(StUserDataMemory* _pUserDataMemory) :pUserDataMemory(_pUserDataMemory)
					{
					}
				};

				MyCallBack* myCallBack = new MyCallBack(pUserDataMemory);

				memset(SQL_BUFFER, 0, MAX_SQL_BUFFER);
				SPRINTF(SQL_BUFFER, "CALL ZP_GET_USER(%lld);", nCharID);
				SQL_BUFFER[MAX_SQL_BUFFER - 1] = '\0';
				pDB->ExecAsyncSQL(SQL_BUFFER, myCallBack);

			}

			// 查询任务 
			{
				struct MyCallBack : public MyDBCallBack
				{
					StUserDataMemory* pUserDataMemory;
					virtual void QueryResult(IDbRecordSet* pSet, int32 nCount)
					{
						const DbRecordSet* pRecordSet = static_cast<const DbRecordSet*>(pSet);
						if (pRecordSet->Rows() == 0)
						{
							FLOG_WARRING(__FUNCTION__, __LINE__, "NOT Found In Data Quest");
							return;
						}

						const StQuestDataTable* pQuestDb = static_cast<const StQuestDataTable*>(pRecordSet->GetRecordData(0));
						if (pQuestDb == NULL)
						{
							FLOG_ERROR(__FUNCTION__, __LINE__, "Quest is null!");
							ASSERT(0);
							return;
						}

						// 保存数据到内存 
						pUserDataMemory->sUserData.LoadQuestDataForDp(*pQuestDb);
						pUserDataMemory->nInit = 1;

						// 查询完毕，返回数据给前端 
						DataCallbackToFrom(pUserDataMemory);
					}

					MyCallBack(StUserDataMemory* _pUserDataMemory) :pUserDataMemory(_pUserDataMemory)
					{
					}
				};

				MyCallBack* myCallBack = new MyCallBack(pUserDataMemory);
				memset(SQL_BUFFER, 0, MAX_SQL_BUFFER);
				SPRINTF(SQL_BUFFER, "SELECT * FROM `quest_info` WHERE `char_id`=%lld;", nCharID);
				SQL_BUFFER[MAX_SQL_BUFFER - 1] = '\0';
				pDB->ExecAsyncSQL(SQL_BUFFER, myCallBack);

				return NULL;
			}
		}else
		{
			FLOG_WARRING(__FUNCDNAME__,__LINE__,"Create StUserDataMemory memory fail!");
		}
		return NULL;
	}
}

/*

本函数必须要放在最后一个sql的查询

*/
void MemoryManager::DataCallbackToFrom(StUserDataMemory* pUserDataMem)
{
	if (NULL == pUserDataMem)
	{
		ASSERT(pUserDataMem);
		return;
	}

	(pUserDataMem->pCallBack->QueryFinish)(&pUserDataMem->sUserData);

	S_SAFE_DELETE(pUserDataMem->pCallBack); // 必须要删除该指针 

}

void MemoryManager::Modifyed(int64 nCharID)
{
	UserMainMemoryMapType::iterator it = m_mapUserMainMemory.find(nCharID);
	if (it != m_mapUserMainMemory.end())
	{
		StUserDataMemory* pUserDataMemory = it->second;
		if (pUserDataMemory->nInit != 1)
			return ;
		pUserDataMemory->nStatus = 1;
	}
}




