#ifndef __COMMON_D2L_H_
#define __COMMON_D2L_H_


#include "CommonDefine.h"
#include "CommonCharacter.h"
#include "CommonQuest.h"
#include "NetConfig.h"

#pragma pack(push,1)

enum EProD2L
{

	PRO_D2L_ACCOUNT_LOGIN			= PRO_D2L_BASE + 0, // 返回帐号登录结果 
	PRO_D2L_CHARACTER_LIST			= PRO_D2L_BASE + 1, // 返回角色列表信息 
	PRO_D2L_ROLE_CREATE_RESULT		= PRO_D2L_BASE + 2,	// 角色创建结果 
	PRO_D2L_NAMES_LIST				= PRO_D2L_BASE + 3, 


};


/*------------------------------------------------------------------
 *
 * @Brief : 校验帐户密码返回结果
 *
 * @Author: hzd 
 * @File  : CommonD2L.h
 * @Date  : 2015/10/18 20:52
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
struct D2LAccountLogin : public NetMsgHead
{

	enum 
	{
		E_RESULT_SUCCESS = 0,
		E_RESULT_FAIL,
	};

	int32 nResult;
	int64 nAccountId;

	D2LAccountLogin():NetMsgHead(PRO_D2L_ACCOUNT_LOGIN)
	{
		nResult = 0;
		nAccountId = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

/*-------------------------------------------------------------------
 * @Brief : 角色列表信息，当帐号密码正确时，会自动返回该信息到LS
 * 
 * @Author:hzd 2015:10:24
 *------------------------------------------------------------------*/
struct D2LCharacterList : public NetMsgHead
{
	int32 nCount;
	struct StCharacterInfo
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
		int32 nRed;
		int32 nBlue;
		StCharacterInfo()
		{
			nCharID = nAccountID =  0;
			nServerID = nType = nLevel = nLandMapid = nLandX = nLandY = nRed = nBlue = 0;
			memset(arrName,0,sizeof(arrName));
		}
	};

	StCharacterInfo arrInfo[MAX_ROLE_TYPE_COUNT];

	D2LCharacterList():NetMsgHead(PRO_D2L_CHARACTER_LIST)
	{
		nCount = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct D2LRoleCreateResult : public NetMsgHead
{

	enum
	{
		E_SUCCESS = 0,
		E_FAIL_SYNC,
		E_FAIL_ROLE_MAX,
		E_FAIL_NAME_EXIST,
		E_FAIL_INSERT_FAIL,
	};

	int32 nResult;
	int64 nNewCharID;

	D2LRoleCreateResult():NetMsgHead(PRO_D2L_ROLE_CREATE_RESULT)
	{
		nResult = 0;
		nNewCharID = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct D2LNamesList : public NetMsgHead
{
	int32 nCount;
	struct StNameInfo
	{
		char arrName[32];
	};

	StNameInfo arrNameList[1000];

	D2LNamesList():NetMsgHead(PRO_D2L_NAMES_LIST)
	{
		nCount = 0;
		memset(arrNameList,0,sizeof(arrNameList));
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////


//-------------------dp2ws--------------------- 600-699

enum EProDP2WS
{

};

/////////////////////////////////////l2d/////////////////////////////////////

enum EProL2D
{

	PRO_L2D_NAMES_LIST			= PRO_L2D_BASE + 0, // 请求加载昵称列表 
	PRO_L2D_ACCOUNT_LOGIN		= PRO_L2D_BASE + 1,	// 查询用户名与密码是否正确 
	PRO_L2D_ROLE_CREATE			= PRO_L2D_BASE + 2, // 创建角色

};

struct L2DNamesList : public NetMsgHead
{	

	L2DNamesList():NetMsgHead(PRO_L2D_NAMES_LIST)
	{

	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct L2DAccountLogin : public NetMsgHead
{
	L2DAccountLogin():NetMsgHead(PRO_L2D_ACCOUNT_LOGIN)
	{

	}
	char arrAccount[32];
	char arrPassword[32];

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct L2DRoleCreate : public NetMsgHead
{
	int32 nType;
	int64 nAccountID;
	char arrName[MAX_NICK_LENGTH];
	L2DRoleCreate():NetMsgHead(PRO_L2D_ROLE_CREATE)
	{
		nType = 0;
		nAccountID = 0;
		memset(arrName,0,sizeof(arrName));
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

/////////////////////////////////s2dp/////////////////////////////////////////

enum EProS2D
{

	PRO_S2D_LOAD_CHARACTER			= PRO_S2D_BASE + 0,	// 加载角色数据 
	PRO_S2D_CLIENT_EXIT_SCENE		= PRO_S2D_BASE + 1,	// 玩家从场景退出
	PRO_S2D_SAVE_CHARACTER			= PRO_S2D_BASE + 2, // 保存数据 

	PRO_S2D_SAVE_MIXITEMNUMBER		= PRO_S2D_BASE + 3, // 
	PRO_S2D_SAVE_MXIITEMBINARY		= PRO_S2D_BASE + 4,

};


struct S2DLoadCharacter : public NetMsgHead
{

	int64 nCharID;

	S2DLoadCharacter():NetMsgHead(PRO_S2D_LOAD_CHARACTER)
	{
		nCharID = 0;
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct S2DClientExitScene : public NetMsgHead
{

	S2DClientExitScene():NetMsgHead(PRO_S2D_CLIENT_EXIT_SCENE)
	{

	}

	int32 nReason;

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}

};

struct S2DSaveUserAllData : public NetMsgHead
{
	int64 nCharID;
	int32 nReceiptID;

	StUserDataForSs sUserData;

	S2DSaveUserAllData():NetMsgHead(PRO_S2D_SAVE_CHARACTER)
	{
		nCharID = nReceiptID = 0;
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct S2DSaveMixItemNumber : public NetMsgHead
{
	int32 nType;
	int64 nValue;
	S2DSaveMixItemNumber():NetMsgHead(PRO_S2D_SAVE_MIXITEMNUMBER)
	{
		nType = 0;
		nValue = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

////////////////////////////////d2s//////////////////////////////////////////

enum EProD2S
{
	PRO_D2S_LOAD_CHARACTER	= PRO_D2L_BASE + 0, // 加载角色数据 
	PRO_D2S_SAVE_CALLBACK	= PRO_D2L_BASE + 1, // 保存成功回调 
};

struct D2SLoadCharacter : public NetMsgHead
{
	
	StUserDataForSs sUserData;

	D2SLoadCharacter():NetMsgHead(PRO_D2S_LOAD_CHARACTER)
	{

	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct D2SSaveCallBack : public NetMsgHead
{

	int64 nCharID;
	int32 nReceiptID;

	D2SSaveCallBack() :NetMsgHead(PRO_D2S_SAVE_CALLBACK)
	{
		nCharID = nReceiptID = 0;
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}

};


#pragma pack(pop)














#endif