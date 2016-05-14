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

 ��ѯ�ص�

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
	int32   		nInit;					// ��ʼ�������0 δ��ʼ����ɣ�1��ʼ����� 
	int32			nQueryTime;				// ��ʼ��ѯ��ʱ�� 
	int32 			nStatus;				// ����״̬��0������1�޸ģ�2ɾ�� 	
	int32 			nLifeTime;				// �������ڣ����ϴ��޸�ʱ�俪ʼ����
	int32 			nLastAsk;				// �ϴη���ʱ�� 
	StCallBackInfo*	pCallBack;				// ��ѯ�ص�����
	StUserDataForDp sUserData;				// �û������ݣ��������ݣ�

	StUserDataMemory(int64 _nCharID) // �����ɴ����ݿ�תΪ�û������ݶ��� 
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
 * @Brief : ����ڴ��������������������ݣ���ʱ��д��
 *			�Զ������ݿ�������ݵ��ڴ�
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

	// ����Ϊ�и��£��ᶨʱ���棬���û����������������������ڲŻᱣ�浽���ݿ� 
	void Modifyed(int64 nCharID);

	void Update();

	void SaveNowByUID(int64 nUID);

	void RemoveByUID(int64 nUID);

public:

	// ȡ����ɫ���ݻص����� ,nSrcType
	static void DataCallbackToFrom(StUserDataMemory* pUserDataMemory);

	void DeleteByUID(int64 nUID);  // �����ڴ� 

private:
	typedef	std::map<int64,StUserDataMemory*>	UserMainMemoryMapType;
	
	mutex   m_cSaveMutex;
	UserMainMemoryMapType				m_mapUserMainMemory;	// User�ڴ�������map
	static ObjPool<StUserDataMemory>	g_cUserMainDbFactory;	// User�ڴ湹������ 

};



#endif

