#ifndef __COMMON_CHARACTER_H_
#define __COMMON_CHARACTER_H_

#include "BaseDefine.h"

#include "CommonInternal.h"
#include "CommonBags.h"
#include "CommonQuest.h"
#include "DbConfig.h"


/*-------------------------------------------------------------------
 * @Brief : 角色类共用结构
 * 
 * @Author:hzd 2015:11:24
 *------------------------------------------------------------------*/

#pragma pack(push,1)


/*-------------------------------------------------------------------
 * @Brief : 角色内存数据(DP中) 
 *			与数据库中SP_GET_CHARACTER中Select 对应就可，其他地方不用改 
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

	// 进制数据，会经常变动大小，数据库设置为binary类型，大小为 4096 
	// 大小固定后不能再进行修改，增加数据，需要用预留数据 
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

	// 字段数组 
	static const dbCol fields[];

};





/*-------------------------------------------------------------------
 * @Brief : 数据库中存储的在线玩家主数据（所有数据）
 *			Character表，物品表，其他表等等
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
 * @Brief : 用户数据ForSs,所有数据一次性加载完成（但须小于64k）
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
 * @Brief : 保存在WS上的数据，WS上保存的数据比较少，也减少S2W数据同步
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
