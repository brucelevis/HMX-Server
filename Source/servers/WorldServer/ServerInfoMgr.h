#ifndef __SERVER_INFO_MGR_H_
#define __SERVER_INFO_MGR_H_

#include "BaseDefine.h"
#include "BaseSingle.h"
#include "BaseSession.h"
#include "CommonDefine.h"

#include "CommonS2W.h"

/*
 *
 *	Detail: 服务器信息管理(新增，转发，退出)，这里仅在ws上用 
 *  这里的信息除ws本身以外，其它server数据由ss,ls,dp,fep等上传到ws
 * Copyright (c) Created by hzd 2015-5-14.All rights reserved
 *
 */


/*-------------------------------------------------------------------
 * @Brief : 本类的使用：全服[服务器]信息的汇总，这里的汇集信息来自于
 *			各服务器的定时推送 
 * @Author:hzd 2015:11:18
 *------------------------------------------------------------------*/

class ServerSession;
class ClientSession;

#pragma pack(push,1)

struct ServerInfo
{
	ServerInfo()
	{
		memset(this, 0, sizeof(*this));
	}
	char arrHost[32];
	int32 nPort;
	int32 nServerType;
	int32 nServerID;
	int32 nServerLoad; // 该服务器连接负载 
	int32 nClientLoad; // 客户端使用该负载 
	ServerSession* pServerSession;
};

#pragma pack(pop)

typedef std::vector<ServerInfo> ServerInfoVecType;
typedef std::map<uint32, ServerInfo> ServerInfoMapType;


class ServerInfoMgr : public BaseSingle<ServerInfoMgr>
{
public:

	ServerInfoMgr();
	~ServerInfoMgr();

	// 获得某服务器信息 
	const ServerInfo* GetServerInfo(uint32 nServerID);

	// 登录一新服务器 
	bool OnServerLogin(ServerSession* pSession,const char arrHost[32],int32 nPort,int32 nServerID,uint32 nServerType);

	// 登出信息
	void OffServerLogin(ServerSession* pSession);

	// 负载更新 
	void UpdateServerInfo(uint32 nServerID,int32 nServerLoad,int32 nClientLoad);

	// 获得服务器列表 
	void GetServerInfoList(int32 nServerType,ServerInfoVecType& o_rServerList);

	// 获得负载最少的服务器 
	ServerSession* GetLoadLestSession(int32 nServerType);

	// 广播服务器信息 
	void BoardcastDpAndFepServerInfo2LsAndSs(uint32 nLoginServerID);

	// 加载全局信息 
	void LoadGlobalInfo(D2WGlobalInfo& rInfo);

private:

	int32				m_nStartTimes;		// 当前是第几次启动
	ServerInfoMapType	m_mapServerInfo;	// 服务器列表  

};



#endif




