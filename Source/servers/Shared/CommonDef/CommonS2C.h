#ifndef __COMMONS2C_H_
#define __COMMONS2C_H_

#include "CommonDefine.h"
#include "NetConfig.h"

enum EProcS2CFunc
{
	PRO_S2C_SCENE_BASE		= PRO_S2C_BASE + 0,		// ������� 
	PRO_S2C_CHARACTER_BASE	= PRO_S2C_BASE + 100,	// �������  
	PRO_S2C_QUEST_BASE		= PRO_S2C_BASE + 200,	// ������� 
	PRO_S2C_BAG_BASE		= PRO_S2C_BASE + 300,	// ������� 
};


// s2cЭ�� 
enum EProS2C
{
	
	//////////////////////////////����ģ��////////////////////////////////////////////
	PRO_S2C_SCENE_INIT_RESULT	= PRO_S2C_SCENE_BASE + 0, // ������ʼ�� 
	PRO_S2C_CHAR_WORLD			= PRO_S2C_SCENE_BASE + 1, // ss�ϵ��������� 
	PRO_S2C_SCENE_LOADING		= PRO_S2C_SCENE_BASE + 2, // ���������� 
	PRO_S2C_SEND_DATA_FINISH	= PRO_S2C_SCENE_BASE + 3, // ����������� 
	PRO_S2C_SCENE_ENTER_INFO	= PRO_S2C_SCENE_BASE + 4, // ����ĳ�����Ϣ 

	//////////////////////////////��ɫģ��////////////////////////////////////////////
	PRO_S2C_CHAR_MAIN_DATA		= PRO_S2C_CHARACTER_BASE + 0, // ��ɫ������ 



	//////////////////////////////����ģ��///////////////////////////////////////////////
	PRO_S2C_QUEST_MAIN_DATA		= PRO_S2C_QUEST_BASE + 0, // ���������� 


	//////////////////////////////����ģ��///////////////////////////////////////////////
	PRO_S2C_BAG_MAIN_DATA		= PRO_S2C_BAG_BASE + 0, // ���������� 
	



};


#pragma pack(push, 1)


//////////////////////////////����ģ��////////////////////////////////////////////
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

// ���볡���ĳ�ʼ����� 
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
 * @Brief : �����ݷ������
 * 
 * @Author:hzd 2015:11:24
 *------------------------------------------------------------------*/
struct S2CSendDataFinish : public NetMsgHead
{

	// ��������һЩ���ֵ ������ʱ���  

	S2CSendDataFinish():NetMsgHead(PRO_S2C_SEND_DATA_FINISH)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////��ɫģ��////////////////////////////////////////////

// ���������� 
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


//////////////////////////////����ģ��///////////////////////////////////////////////

// ����������
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

//////////////////////////////����ģ��///////////////////////////////////////////////

// ����������  
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

	PRO_F2C_ENCRYPT_INFO			= PRO_F2C_BASE + 0,	// ������Կ��Ϣ 
	PRO_F2C_LOGIN_READY_FINISH		= PRO_F2C_BASE + 1, // �������Լ�����¼����׼����� 
	PRO_F2C_ENTER_SCENE_LOADED		= PRO_F2C_BASE + 2, // �����������  

};


// ������Կ 
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

// ������� 
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
	PRO_L2C_ACCOUNT_LOGIN		= PRO_L2C_BASE + 0,	// ���ص�¼��Ϣ 
	PRO_L2C_CHARACTER_LIST		= PRO_L2C_BASE + 1, // ��ɫ��Ϣ����(�ʺŵ�¼�ɹ��᷵��)
	PRO_L2C_ROLE_CREATE_RESULT  = PRO_L2C_BASE + 2, // ��ɫ������� 
	PRO_L2C_NAMES_LIST			= PRO_L2C_BASE + 3, // �����б� 
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

