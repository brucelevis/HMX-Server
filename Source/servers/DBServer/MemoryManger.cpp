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
	// �����������ڴ棬�����޸ģ�����5����֮����ᱣ�浽���ݿ���ȥ���������ϴη���ʱ��  
	// ���δ�У��������������Ƿ�ﵽ������ﵽ����ᱣ�浽���ݿ���ȥ��ɾ���ڴ� 
	// ����ClientSession ��ʱɾ���� 
	// ����Ҫ�����������߳� 

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
			if(userMem->nStatus == 0) // ����������������� 
			{

			}
			else if(userMem->nStatus == 1) // ���޸���Ҫ���� 
			{
				vecSaveList.push_back(it->first);
			}
			else if(userMem->nStatus == 2) // ɾ���ڴ� 
			{
				vecDeleteList.push_back(it->first);
			}
			else if (userMem->nInit == 0)
			{
				int32 nowTime = Utility::NowTime();
				if (nowTime > userMem->nQueryTime + 60) // ��ѯ��ʱ����ɾ��
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

		// ��ɫ���� 
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

		// �����Ʋ��ô洢���� 

		memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
		SPRINTF(SQL_BUFFER,"%s" ,ss.str().c_str());
		SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
		pDB->ExecAsyncSQL(SQL_BUFFER,NULL);

	}


	{

		StQuestDataTable& sQuest = pUserData->sQuestTable;

		// ���񱣴� 
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
			// ���ݳ�ʼ��δ��ɣ����绹�ڼ������ݿ������У�������;�����ж�
			// ���������ݣ���update�У���Ҫ�ж�Ȼ��ɾ��������һ�β��������¼��� 
			// �����ڴ˵ط�ɾ�� 
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

			// ��ѯ�������� 
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
						pUserDataMemory->sUserData.LoadCharacterDataForDp(*pCharDb);// �����ݼ��ص��ڴ� 
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

			// ��ѯ���� 
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

						// �������ݵ��ڴ� 
						pUserDataMemory->sUserData.LoadQuestDataForDp(*pQuestDb);
						pUserDataMemory->nInit = 1;

						// ��ѯ��ϣ��������ݸ�ǰ�� 
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

����������Ҫ�������һ��sql�Ĳ�ѯ

*/
void MemoryManager::DataCallbackToFrom(StUserDataMemory* pUserDataMem)
{
	if (NULL == pUserDataMem)
	{
		ASSERT(pUserDataMem);
		return;
	}

	(pUserDataMem->pCallBack->QueryFinish)(&pUserDataMem->sUserData);

	S_SAFE_DELETE(pUserDataMem->pCallBack); // ����Ҫɾ����ָ�� 

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




