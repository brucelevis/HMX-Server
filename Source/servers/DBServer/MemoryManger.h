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
	int64			nCharID;				// ��ɫID�ڴ� 
	int32			nSessionID;				// sessionID;
	int32   		nInit;					// ��ʼ�������0 δ��ʼ����ɣ�1��ʼ����� 
	int32			nQueryTime;				// ��ʼ��ѯ��ʱ�� 
	int32 			nStatus;				// ����״̬��0������1�޸ģ�2ɾ�� 	
	int32 			nLifeTime;				// �������ڣ����ϴ��޸�ʱ�俪ʼ����
	int32 			nLastAsk;				// �ϴη���ʱ�� 
	StCallBackInfo*	pCallBack;				// ��ѯ�ص�����
	StUserDataForDp sUserData;				// �û������ݣ��������ݣ�

	StUserDataMemory(int64 _nCharID,int32 _nSessionID):nCharID(_nCharID),nSessionID(_nSessionID) // �����ɴ����ݿ�תΪ�û������ݶ��� 
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

	StUserDataForDp* GetUser(int64 nCharID,bool bQuery = false);

	StUserDataForDp* GetUserDb(int64 nCharID,int32 nSessionID,StCallBackInfo* pCbHandler);

	StUserDataMemory* GetUserMem(int64 nCharID,bool bQuery = false);

	// ����Ϊ�и��£��ᶨʱ���棬���û����������������������ڲŻᱣ�浽���ݿ� 
	void Modifyed(int64 nCharID);

	void Update();

	void SaveNowByUID(int64 nUID);

	void RemoveByUID(int64 nUID);

	int64 GetUserIDBySessionID(int32 nSessionID);

public:

	// ȡ����ɫ���ݻص����� ,nSrcType
	static void DataCallbackToFrom(StUserDataMemory* pUserDataMemory);

	void RemoveSessionID(int32 nSessionID);

private:
	typedef	std::map<int64,StUserDataMemory*>	UserMainMemoryMapType;
	typedef std::map<int32, int64>				SessionIDUserIDMapType;
	
	mutex   m_cSaveMutex;
	UserMainMemoryMapType				m_mapUserMainMemory;	// User�ڴ�������map
	SessionIDUserIDMapType				m_mapSessionIDUserID;	// 
	static ObjPool<StUserDataMemory>	g_cUserMainDbFactory;	// User�ڴ湹������ 

};



#endif

