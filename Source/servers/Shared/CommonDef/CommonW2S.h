#ifndef __COMMON_W2D_H_
#define __COMMON_W2D_H_


#include "CommonDefine.h"
#include "NetConfig.h"


//-----------------------600~699-----------------

enum EProWS2DP
{
	PRO_W2D_LOGINED		= PRO_W2D_BASE + 0, // 登录结果 
	PRO_W2D_SERVER_INFO = PRO_W2D_BASE + 1,
	PRO_W2D_SCENE_INFO  = PRO_W2D_BASE + 2,
	PRO_W2D_EVENT_INFO	= PRO_W2D_BASE + 3, // 查询event_info是否有后台事件要执行   
	PRO_W2D_SELECT_ROLE = PRO_W2D_BASE + 4, // 选择角色 
};

#pragma pack(push,1)

struct WS2DPLogined : public NetMsgHead
{

	WS2DPLogined() :NetMsgHead(PRO_W2D_LOGINED)
	{

	}
	enum
	{
		SUCCESS,
		FAIL,
	};
	int32 nResult; // 0 成功,其他失败 
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct NotifyServerInfo
{
	NotifyServerInfo()
	{
		int32 nServerLoad = 0;
		int32 nClientLoad = 0;
	}
	char arrHost[32];
	int32 nPort;
	int32 nServerType;
	int32 nServerID;
	int32 nServerLoad; // 该服务器下负载人数 
	int32 nClientLoad; // 客户端选择的人数 
};

struct W2DNotifyServerInfo : public NetMsgHead
{
	W2DNotifyServerInfo():NetMsgHead(PRO_W2D_SERVER_INFO)
	{
		nGroupID = 0;
	}
	int32 nGroupID;
	int32 nServerNum;
	NotifyServerInfo arrServerInfo[MAX_SERVER_NUM];

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct NotifySceneInfo
{
	int32 nServerID;
	int32 nSceneID;
	int32 nLoadNum; // 当前负载量 
};

struct W2DNotifySceneInfo : public NetMsgHead
{
	W2DNotifySceneInfo():NetMsgHead(PRO_W2D_SCENE_INFO)
	{

	}
	int32 nSceneNum;
	NotifySceneInfo arrSceneInfo[MAX_SCENE_NUM];
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct W2DReqEventInfo : public NetMsgHead
{
	W2DReqEventInfo():NetMsgHead(PRO_W2D_EVENT_INFO)
	{

	}
	int32 nTimestamp; // 时间戳  
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct W2DSelectRole : public NetMsgHead
{
	int64 nCharID;
	W2DSelectRole():NetMsgHead(PRO_W2D_SELECT_ROLE)
	{
		nCharID = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////


enum EProW2Fep
{
	PRO_W2FEP_LOGINED = 200 , // 登录结果 

};

// fep登录结果 
struct W2FepLogined : public NetMsgHead
{
	W2FepLogined():NetMsgHead(PRO_W2FEP_LOGINED)
	{

	}
	enum 
	{
		SUCCESS,
		FAIL,
	};
	int32 nResult; // 0 成功,其他失败 
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////

enum EProW2Ls
{

	PRO_W2LS_LOGINED = 300, // 登录结果 
	PRO_W2LS_SERVERINFO_LIST = 301,

};



//////////////////////////////////////////////////////////////////////////

enum EProW2Ss
{
	PRO_W2S_ReqTransfer = PRO_W2S_BASE + 0, // 请求进入场景 
	PRO_W2S_RepEnterResult = PRO_W2S_BASE + 1, // 请求场景结果  
};

struct W2SReqEnterScene : public NetMsgHead
{
	int64 nCharacterID;
	int32 nSceneID;
	int32 nDpServerID;
	int32 nFepServerID;
	int32 nEnterType; // 0 首次进入,1更改场景 
	stEnterSceneParam stParam;
	W2SReqEnterScene():NetMsgHead(PRO_W2S_ReqTransfer)
	{
		nCharacterID = nSceneID = nDpServerID = nFepServerID = nEnterType = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct W2SRepEnterResult : public NetMsgHead
{
	int64 nCharID;
	enum
	{
		E_ENTER_SUCCESS = 0,
		E_ENTER_FAIL,
	};

	int32 nResult;

	W2SRepEnterResult() :NetMsgHead(PRO_W2S_RepEnterResult)
	{
		nCharID = nResult = 0;
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}

};

#pragma pack(pop)


#endif

