#ifndef __COMMON_CHARACTER_H_
#define __COMMON_CHARACTER_H_

#include "BaseDefine.h"

#include "CommonInternal.h"
#include "CommonBags.h"
#include "CommonQuest.h"
#include "DbConfig.h"


/*-------------------------------------------------------------------
 * @Brief : ��ɫ�๲�ýṹ
 * 
 * @Author:hzd 2015:11:24
 *------------------------------------------------------------------*/

#pragma pack(push,1)


/*-------------------------------------------------------------------
 * @Brief : ��ɫ�ڴ�����(DP��) 
 *			�����ݿ���SP_GET_CHARACTER��Select ��Ӧ�Ϳɣ������ط����ø� 
 * @Author:hzd 2015:11:2
 *------------------------------------------------------------------*/
struct StCharacterTable
{
	int64 nCharID;
	int32 nServerID;
	int64 nAccountID;
	char arrName[MAX_NICK_LENGTH];
	int32 nType;
	int32 nExp;
	int32 nLevel;
	int32 nLandMapId;
	int32 nLandX;
	int32 nLandY;
	int32 nInstanceMapId;
	int32 nInstanceX;
	int32 nInstanceY;
	int32 nRed;
	int32 nBlue;

	// �������ݣ��ᾭ���䶯��С�����ݿ�����Ϊbinary���ͣ���СΪ 4096 
	// ��С�̶������ٽ����޸ģ��������ݣ���Ҫ��Ԥ������ 
	union BinData
	{

		BinData()
		{
			memset(this,0,MAX_BINARY_SIZE);
		}
		char revser[MAX_BINARY_SIZE];
	};

	BinData binData;

	StCharacterTable()
	{
		memset(this,0,sizeof(*this));
	}

	inline int32 GetLength()
	{
		return sizeof(*this);
	}

	// �ֶ����� 
	static const dbCol fields[];

};





/*-------------------------------------------------------------------
 * @Brief : ���ݿ��д洢��������������ݣ��������ݣ�
 *			Character����Ʒ��������ȵ�
 * @Author:hzd 2015:11:2
 *------------------------------------------------------------------*/
struct StUserDataForDp
{
	// Character
	StCharacterTable sCharacterTable;

	// Object
	StObjectsDataTable sObjectsTable;

	// Quest
	StQuestDataTable sQuestTable;

	void LoadCharacterDataForDp(const StCharacterTable& pData)
	{
		memcpy(&sCharacterTable,&pData,sizeof(sCharacterTable));
	}

	void LoadObjectsDataForDp(const StObjectsDataTable& pData)
	{
		memcpy(&sObjectsTable,&pData,sizeof(sObjectsTable));
	}

	void LoadQuestDataForDp(const StQuestDataTable& pData)
	{
		memcpy(&sQuestTable,&pData,sizeof(sQuestTable));
	}

};

/*-------------------------------------------------------------------
 * @Brief : �û�����ForSs,��������һ���Լ�����ɣ�����С��64k��
 * 
 * @Author:hzd 2015:11:11
 *------------------------------------------------------------------*/
struct StUserDataForSs
{
	// Character
	StCharacterTable sCharacterTable;

	// Quest
	StQuestDataTable sQuestTable;

	void LoadCharacterData(const StCharacterTable& dataDp)
	{
		memcpy(&sCharacterTable,&dataDp,sizeof(sCharacterTable));
	}

	void LoadQuestData(const StQuestDataTable& pData)
	{
		memcpy(&sQuestTable,&pData,sizeof(sQuestTable));
	}
};

/*-------------------------------------------------------------------
 * @Brief : ������WS�ϵ����ݣ�WS�ϱ�������ݱȽ��٣�Ҳ����S2W����ͬ��
 * 
 * @Author:hzd 2015:11:6
 *------------------------------------------------------------------*/
struct StUserDataForWs
{
	int64 nCharID;
	int32 nServerID;
	int64 nAccountID;
	char arrName[MAX_NICK_LENGTH];
	int32 nType;
	int32 nLevel;
	int32 nLandMapid;
	int32 nLandX;
	int32 nLandY;
	int32 nInstanceMapId;
	int32 nInstanceX;
	int32 nInstanceY;
	StUserDataForWs()
	{
		memset(this,0,sizeof(*this));
	}

	StUserDataForWs(const StUserDataForWs* _userData)
	{
		memcpy(this,_userData,sizeof(StUserDataForWs));
	}

	inline int32 GetLength()
	{
		return sizeof(*this);
	}

	void LoadData(const StCharacterTable& dataDp)
	{
		nCharID = dataDp.nCharID;
		nServerID = dataDp.nServerID;
		nAccountID = dataDp.nAccountID;
		strcpy(arrName,dataDp.arrName);
		nType = dataDp.nType;
		nLevel = dataDp.nLevel;
		nLandMapid = dataDp.nLandMapId;
		nLandX = dataDp.nLandX;
		nLandY = dataDp.nLandY;
		nInstanceMapId = dataDp.nInstanceMapId;
		nInstanceX = dataDp.nInstanceX;
		nInstanceY = dataDp.nInstanceY;
	}
};

#pragma pack(pop)


#endif
