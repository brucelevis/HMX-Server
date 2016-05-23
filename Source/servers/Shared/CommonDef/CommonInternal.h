#ifndef __COMMON_INTERNAL_H_
#define __COMMON_INTERNAL_H_

/*
 *
 *	Detail: 内部通信协议 
 *   
 *  Created by hzd 2015-5-21
 *
 */

#include "CommonDefine.h"
#include "NetConfig.h"

enum EProReqInternal
{
	PRO_SS_REQ_LOGIN_WS		= PRO_INTERNAL_BASE + 0,	// 登录处理，包括,所有服务器登录到ws 
	PRO_SS_REQ_LOGIN_S		= PRO_INTERNAL_BASE + 1,	// 登录到dp,fep 
	PRO_SS_REQ_LOAD_LEST	= PRO_INTERNAL_BASE + 2,	// 分配服务器信息  
	PRO_SS_REQ_PING_S		= PRO_INTERNAL_BASE + 3,	// PING协议客户端ping服务端，ping后，就socket->setTimeout,收到数据，则取消 
	PRO_SS_REQ_CLIENT_EXIT	= PRO_INTERNAL_BASE + 4,    // 客户端退出 
	PRO_SS_REQ_CONNECT_INFO = PRO_INTERNAL_BASE + 5,	// 连接信息 
};

enum EProRepInternal
{
	PRO_SS_REP_LOGINED			= PRO_INTERNAL_BASE + 50, // 登录结果  
	PRO_SS_REP_SERVERINFO_LIST	= PRO_INTERNAL_BASE + 51, // 服务器信息 
	PRO_SS_NOFTTY_SERVERID		= PRO_INTERNAL_BASE + 52, // 更新服务器信息 
	PRO_SS_REP_PING_S			= PRO_INTERNAL_BASE + 53, // 服务器返回PING的结果，将socket取消定时器  
};


#pragma pack(push,1)


//------------------------req------------------------------

struct SSReqLoginWs : public NetMsgHead
{
	SSReqLoginWs():NetMsgHead(PRO_SS_REQ_LOGIN_WS)
	{
	}
	char arrServerAddr[32]; // 服务器地址 
	int32 nServerPort;		// 端口 
	int32 nServerID;		// 服务器标识ID 
	int32 nServerType;		// 服务器类型 
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct SSReqLoginS : public NetMsgHead 
{
	SSReqLoginS():NetMsgHead(PRO_SS_REQ_LOGIN_S)
	{

	}
	int32 nServerID;		// 服务器标识ID 
	int32 nServerType;		// 服务器类型 
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct SSReqLoadLest : public NetMsgHead
{
	SSReqLoadLest():NetMsgHead(PRO_SS_REQ_LOAD_LEST)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};


struct SSReqPingToS : public NetMsgHead
{
	SSReqPingToS():NetMsgHead(PRO_SS_REQ_PING_S)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

/*-------------------------------------------------------------------
 * @Brief : 退出协议 
 * 
 * @Author:hzd 2015:11:19
 *------------------------------------------------------------------*/
struct SSNofityClientExit : public NetMsgHead
{
	enum 
	{	E_REASON_UNKOWN	= 0,			// 未知 
		E_REASON_SERVER_TICKED,			// 服务器踢出
		E_REASON_CLIENT_EXIT,			// 主动退出 
		E_REASON_TIMEOUT,				// 超时断开 
		E_REASON_PACKAGE_ERROR,			// 非法协议 
		E_REASON_SEND_ERROR,			// 向Client发送时出错 
		E_REASON_SWITCH_SCENE,			// 切换场景（这个仅对Scene，dp）
		E_REASON_REPEAT_CHARACTER = 99,		// 重复登录
	};

	int32 nReason;
	
	enum
	{
		E_STATE_IN_UNKOWN = 0,	// 未知 
		E_STATE_IN_LOGIN,		// 在登录时退出 
		E_STATE_IN_SELECT,		// 在选择角色时退出 
		E_STATE_IN_WORLD,		// 在WORLD中 
		E_STATE_IN_SCENE,		// 在场景中退出 
	};

	int32 nPostion;				// 玩家所在位置 

	SSNofityClientExit(): NetMsgHead(PRO_SS_REQ_CLIENT_EXIT)
	{
		nReason = 0;
		nPostion = 0;
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct SSNotifyConnectInfo : public NetMsgHead
{
	SSNotifyConnectInfo():NetMsgHead(PRO_SS_REQ_CONNECT_INFO)
	{

	}
	int32 nServerLoad;
	int32 nClientLoad;
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};


//--------------------------rep------------------------------------

struct SSRepLogined : public NetMsgHead
{
	enum 
	{
		SUCCESS,
		FAIL,
	};

	int32 nResult;

	SSRepLogined():NetMsgHead(PRO_SS_REP_LOGINED)
	{
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

// 服务器列表信息 
struct SSRepServerInfoList : public NetMsgHead
{

	int32 nCount;
	ServerInfoData sServerInfoData[MAX_SERVERINFO_LIST];

	SSRepServerInfoList() :NetMsgHead(PRO_SS_REP_SERVERINFO_LIST)
	{
		nCount = 0;
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this) - sizeof(sServerInfoData) + nCount * sizeof(ServerInfoData);
	}
};

/*-------------------------------------------------------------------
 * @Brief : 同步玩家的服务器分配信息
 * 
 * @Author:hzd 2015:10:24
 *------------------------------------------------------------------*/
struct SSSessionNofitySInfo : public NetMsgHead
{
	int32 nFepServerID;
	int32 nLsServerID;
	int32 nDpServerID;
	int32 nSsServerID;

	SSSessionNofitySInfo() :NetMsgHead(PRO_SS_NOFTTY_SERVERID)
	{
		nFepServerID = nLsServerID = nDpServerID = nSsServerID = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct SSRepPingS : public NetMsgHead
{
	SSRepPingS():NetMsgHead(PRO_SS_REP_PING_S)
	{

	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

#pragma pack(pop)





#endif


