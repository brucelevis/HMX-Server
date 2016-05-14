#ifndef __COMMON_C2F_H_
#define __COMMON_C2F_H_

#include "CommonDefine.h"
#include "NetConfig.h"

#pragma pack(push,1)

//////////////////////////////////////////////////////////////////////////

enum EProC2F
{

	PRO_C2F_ENCRYPT		= PRO_C2F_BASE + 0, // �����Կ 
	PRO_C2F_CLOSE		= PRO_C2F_BASE + 1, // �����˳� 
	
};



// ���ͼ������� 
struct C2FepEncryptInfo : public NetMsgHead
{
	C2FepEncryptInfo():NetMsgHead(PRO_C2F_ENCRYPT)
	{
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct C2FepClose : public NetMsgHead
{
	C2FepClose():NetMsgHead(PRO_C2F_CLOSE)
	{
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};


//////////////////////////////////////////////////////////////////////////


enum EProC2L
{

	PRO_C2L_ACCOUNT_LOGIN	= PRO_C2L_BASE + 0, // �ʺŵ�¼ 
	PRO_C2L_ROLE_CREATE		= PRO_C2L_BASE + 1, // ������ɫ
	PRO_C2L_RAND_NAMES		= PRO_C2L_BASE + 2, // �������� 


};

struct C2LAccountLogin : public NetMsgHead
{
	char arrAccount[32];
	char arrPassword[32];
	C2LAccountLogin():NetMsgHead(PRO_C2L_ACCOUNT_LOGIN)
	{
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct C2LRoleCreate : public NetMsgHead
{
	int32 nType; // ��ɫ���� 
	int64 nAccountID; 
	char  arrName[MAX_NICK_LENGTH];
	C2LRoleCreate():NetMsgHead(PRO_C2L_ROLE_CREATE)
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

struct C2LRandNames : public NetMsgHead
{
	C2LRandNames():NetMsgHead(PRO_C2L_RAND_NAMES)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};


//////////////////////////////////////////////////////////////////////////

enum EProC2S
{

	PRO_C2S_CHAT_WORLD = PRO_C2S_BASE + 0,		// ��������  
	PRO_C2S_SCENE_DATA = PRO_C2S_BASE + 1,		// ���볡��������� 
	PRO_C2S_CHANGE_SCENE = PRO_C2S_BASE + 2,	// �л����� 

};

/*
 *	�������� 
 */
struct C2SCharWorld : public NetMsgHead
{
	C2SCharWorld():NetMsgHead(PRO_C2S_CHAT_WORLD)
	{
		nLength = 0;
	}
	int32 nLength;
	char arrMsg[MAX_CHAR_WORLD_MSG_LENGTH];
	inline int32 GetPackLength()const
	{
		return sizeof(*this) - sizeof(arrMsg) + nLength;
	}
};

/*
 *	������س��������� 
 */
struct C2SSceneData : public NetMsgHead
{
	enum
	{
		E_DATA_TYPE_NULL		= 0x00000000,
		E_DATA_TYPE_CHARMAIN	= 0x00000001,
		E_DATA_TYPE_PACKAGE		= 0x00000002,
		E_DATA_TYPE_RELATION	= 0x00000004,
		E_DATA_TYPE_PET			= 0x00000008,
		E_DATA_TYPE_ALL			= 0xffffffff,
	};
	
	int32 nLoadDataFlags; // ��Ҫ���ص����� 
		
	C2SSceneData():NetMsgHead(PRO_C2S_SCENE_DATA)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

/*
 *	�л����� 
 */
struct C2SChanageScene : public NetMsgHead
{
	int32 nSceneID;
	C2SChanageScene() :NetMsgHead(PRO_C2S_CHANGE_SCENE)
	{
		nSceneID = 0;
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////

enum EProC2W
{

	PRO_C2W_SWITCH_SCENE = PRO_C2W_BASE + 0, // ��������  ѡ���ɫ���ֱ�Ӹ��ݳ���ID������Ϸ 
	PRO_C2W_SELECT_ROLE	 = PRO_C2W_BASE + 1, // ѡ���ɫ 

};

// fep->ws->ss,��ssȷ�� 
struct C2WSwitchScene : public NetMsgHead
{
	C2WSwitchScene():NetMsgHead(PRO_C2W_SWITCH_SCENE)
	{
		nSceneID = 0;
	}
	int32 nSceneID;
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct C2WSelectRole : public NetMsgHead
{
	int64 nCharID;
	C2WSelectRole():NetMsgHead(PRO_C2W_SELECT_ROLE)
	{
		nCharID = 0; 
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

#pragma pack(pop)





#endif

