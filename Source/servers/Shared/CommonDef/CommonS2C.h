#ifndef __COMMONS2C_H_
#define __COMMONS2C_H_

#include "CommonDefine.h"
#include "NetConfig.h"

enum EProcS2CFunc
{
	PRO_S2C_SCENE_BASE		= PRO_S2C_BASE + 0,		// 场景相关 
	PRO_S2C_CHARACTER_BASE	= PRO_S2C_BASE + 100,	// 主角相关  
	PRO_S2C_QUEST_BASE		= PRO_S2C_BASE + 200,	// 任务相关 
	PRO_S2C_BAG_BASE		= PRO_S2C_BASE + 300,	// 背包相关 
};


// s2c协议 
enum EProS2C
{
	
	//////////////////////////////场景模块////////////////////////////////////////////
	PRO_S2C_SCENE_INIT_RESULT	= PRO_S2C_SCENE_BASE + 0, // 场景初始化 
	PRO_S2C_CHAR_WORLD			= PRO_S2C_SCENE_BASE + 1, // ss上的世界聊天 
	PRO_S2C_SCENE_LOADING		= PRO_S2C_SCENE_BASE + 2, // 场景加载中 
	PRO_S2C_SEND_DATA_FINISH	= PRO_S2C_SCENE_BASE + 3, // 发送数据完毕 
	PRO_S2C_SCENE_ENTER_INFO	= PRO_S2C_SCENE_BASE + 4, // 进入的场景信息 

	//////////////////////////////角色模块////////////////////////////////////////////
	PRO_S2C_CHAR_MAIN_DATA		= PRO_S2C_CHARACTER_BASE + 0, // 角色主数据 



	//////////////////////////////任务模块///////////////////////////////////////////////
	PRO_S2C_QUEST_MAIN_DATA		= PRO_S2C_QUEST_BASE + 0, // 任务主数据 


	//////////////////////////////背包模块///////////////////////////////////////////////
	PRO_S2C_BAG_MAIN_DATA		= PRO_S2C_BAG_BASE + 0, // 背包主数据 
	



};


#pragma pack(push, 1)


//////////////////////////////场景模块////////////////////////////////////////////
struct S2CRepCharWorld : public NetMsgHead
{

	char arrFromNick[MAX_NICK_LENGTH];
	int32 nLength;
	char arrMsg[MAX_CHAR_WORLD_MSG_LENGTH];

	S2CRepCharWorld():NetMsgHead(PRO_S2C_CHAR_WORLD)
	{
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this) - sizeof(arrMsg) + nLength;
	}
};

// 进入场景的初始化结果 
struct S2CSceneInitResult : public NetMsgHead
{

	enum 
	{
		SUCCESS = 0,
		FAIL
	};
	int32 nResult;

	S2CSceneInitResult():NetMsgHead(PRO_S2C_SCENE_INIT_RESULT)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct S2CSceneEnterInfo : public NetMsgHead
{
	int32 nSceneID;
	S2CSceneEnterInfo():NetMsgHead(PRO_S2C_SCENE_ENTER_INFO)
	{
		nSceneID = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

/*-------------------------------------------------------------------
 * @Brief : 主数据发送完成
 * 
 * @Author:hzd 2015:11:24
 *------------------------------------------------------------------*/
struct S2CSendDataFinish : public NetMsgHead
{

	// 这里做发一些检查值 服务器时间等  

	S2CSendDataFinish():NetMsgHead(PRO_S2C_SEND_DATA_FINISH)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////角色模块////////////////////////////////////////////

// 主角主数据 
struct S2CCharacterMainData : public NetMsgHead
{
	int64 nCharID;
	char  arrName[MAX_NICK_LENGTH];
	int32 nType;
	int32 nLevel;
	int32 nSceneId;
	int32 nSceneX;
	int32 nSceneY;
	int32 nRed;
	int32 nBlue;

	S2CCharacterMainData():NetMsgHead(PRO_S2C_CHAR_MAIN_DATA)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};


//////////////////////////////背包模块///////////////////////////////////////////////

// 背包主数据
struct S2CBagMainData : public NetMsgHead
{




	S2CBagMainData():NetMsgHead(PRO_S2C_BAG_MAIN_DATA)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////任务模块///////////////////////////////////////////////

// 任务主数据  
struct S2CQuestMainData : public NetMsgHead
{

	struct QuestInfo
	{
		int32 nQuestID;
		int32 nResult0;
		int32 nResult1;
	};

	QuestInfo arrHadAccept[MAX_QUEST_LIST_COUNT];
	QuestInfo arrCanAccept[MAX_QUEST_LIST_COUNT];

	S2CQuestMainData():NetMsgHead(PRO_S2C_QUEST_MAIN_DATA)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};



//////////////////////////////////////////////////////////////////////////


enum EProF2C
{

	PRO_F2C_ENCRYPT_INFO			= PRO_F2C_BASE + 0,	// 返回密钥信息 
	PRO_F2C_LOGIN_READY_FINISH		= PRO_F2C_BASE + 1, // 服务器对即将登录请求准备完成 
	PRO_F2C_ENTER_SCENE_LOADED		= PRO_F2C_BASE + 2, // 场景进入完成  

};


// 返回密钥 
struct F2CEncryptInfo : public NetMsgHead
{

	char arrEncryptKey[MAX_ENCRYPT_LENTH];

	F2CEncryptInfo():NetMsgHead(PRO_F2C_ENCRYPT_INFO)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct F2CLoginReadyFinish : public NetMsgHead
{
	F2CLoginReadyFinish():NetMsgHead(PRO_F2C_LOGIN_READY_FINISH)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

// 加载完成 
struct F2CEnterSceneLoaded : public NetMsgHead
{
	enum EEnterSceneResult
	{
		SUCCESS = 0,
		FAIL
	};

	int32 nResult;

	F2CEnterSceneLoaded():NetMsgHead(PRO_F2C_ENTER_SCENE_LOADED)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////

enum EProL2C
{
	PRO_L2C_ACCOUNT_LOGIN		= PRO_L2C_BASE + 0,	// 返回登录信息 
	PRO_L2C_CHARACTER_LIST		= PRO_L2C_BASE + 1, // 角色信息返回(帐号登录成功会返回)
	PRO_L2C_ROLE_CREATE_RESULT  = PRO_L2C_BASE + 2, // 角色创建结果 
	PRO_L2C_NAMES_LIST			= PRO_L2C_BASE + 3, // 名字列表 
};


struct L2CAccountLogin : public NetMsgHead
{
	enum 
	{
		E_RESULT_SUCCESS,
		E_RESULT_FAIL,
	};
	int64 nAccountId;
	int32 nResult;
	L2CAccountLogin():NetMsgHead(PRO_L2C_ACCOUNT_LOGIN)
	{
		nAccountId =0;
		nResult = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct L2CCharacterList : public NetMsgHead
{
	
	int32 nCount;
	struct StCharacterInfo
	{
		int64 nCharID;
		int32 nLevel;
		char arrName[MAX_NICK_LENGTH];
	};

	StCharacterInfo arrInfo[MAX_ROLE_TYPE_COUNT];

	L2CCharacterList():NetMsgHead(PRO_L2C_CHARACTER_LIST)
	{
		
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct L2CRoleCreateResult : public NetMsgHead
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

	L2CRoleCreateResult():NetMsgHead(PRO_L2C_ROLE_CREATE_RESULT)
	{
		nResult = 0;
		nNewCharID = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct L2CNamesList : public NetMsgHead
{
	int32 nCount;	
	char arrNames[10][32];
	L2CNamesList():NetMsgHead(PRO_L2C_NAMES_LIST)
	{
		nCount = 0;
		memset(arrNames,0,10*32);
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};


#pragma pack(pop)




#endif

