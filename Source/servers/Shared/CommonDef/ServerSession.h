#ifndef _SEVER_SESSION_H_
#define _SEVER_SESSION_H_

#include "BaseSingle.h"
#include "CommonDefine.h"
#include "BaseSession.h"
#include "ObjPool.h"

class NetSocket;
class NetServer;
struct NetMsgHead;

/*------------------------------------------------
 * 服务器状态 
 *-----------------------------------------------*/
enum EServerStatus
{
	E_SERVER_STATUS_INIT	= 0,
	E_SERVER_STATUS_CONNETED,
	E_SERVER_STATUS_LOGINED,
	E_SERVER_STATUS_OK,
};


/*
 *
 *	Detail: 服务器与服务器之间的session
 *   
 *  Created by hzd 2015/01/26  
 *
 */
class ServerSession : public BaseSession
{
public:

	ServerSession(NetSocket* pNetSocket);
	virtual ~ServerSession();

	/*
	 *
	 *	Detail: 唯一ID
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	int32 ServerID();

	/*
	 *
	 *	Detail: 发送消息
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	virtual void SendMsg( NetMsgHead* pMsg,int32 nSize, SocketCallbackBase* pCallback = NULL);

	/*
	 *
	 *	Detail: 退出
	 *   
	 *  Created by hzd 2015-1-21
	 *
	 */
	virtual void Exist();

	/*
	 *
	 *	Detail: 设置状态
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	virtual void SetStatus(int32 nStatus);

	/*
	 *
	 *	Detail: 获得状态
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	virtual int32 Status();

	/*
	 *
	 *	Detail: 定时循环，参数为延时毫秒
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	void Update(int32 nSrvTime);

	/*
	 *
	 *	Detail: 获得服务器类型
	 *   
	 * Copyright (c) Created by hzd 2015-4-29.All rights reserved
	 *
	 */
	EServerType RemoteServerType();

	/*
	 *
	 *	Detail: 设置服务器类型
	 *   
	 * Copyright (c) Created by hzd 2015-4-29.All rights reserved
	 *
	 */
	void SetRemoteServerType(EServerType eServerType);

	/*
	 *
	 *	Detail: 本地session类型
	 *   
	 *  Created by hzd 2015-5-25
	 *
	 */
	EServerType LocalServerType();
	
	/*
	 *
	 *	Detail: 设置本地session类型
	 *   
	 *  Created by hzd 2015-5-25
	 *
	 */
	void SetLocalServerType(EServerType eServerType);

	/*
	 *
	 *	Detail: no
	 *   
	 *  Created by hzd 2015-5-20
	 *
	 */
	void SetServerID(int32 nServerID);

	/*
	 *
	 *	Detail: 登录到WS
	 *   
	 *  Created by hzd 2015-5-22
	 *
	 */
	void LoginToWs();

	/*
	 *
	 *	Detail: no
	 *   
	 *  Created by hzd 2015-5-22
	 *
	 */
	void LoginResult(bool bIsSuccess);

	/*
	 *
	 *	Detail: 登录到某服务器
	 *   
	 *  Created by hzd 2015-5-22
	 *
	 */
	void LoginToS();

	/*
	 *
	 *	Detail: 获得NetSocket
	 *   
	 * Copyright (c) Created by hzd 2015-5-27.All rights reserved
	 *
	 */
	NetSocket* GetNetSocket();

	/*
	 *
	 *	Detail: 设置Session类型
	 *   
	 *  Created by hzd 2015/06/06  
	 *
	 */
	void SetSessionType(ESessionType eSessionType);

	/*
	 *
	 *	Detail: 获得Session类型
	 *   
	 *  Created by hzd 2015/06/06  
	 *
	 */
	ESessionType GetSessionType();

	/*-------------------------------------------------------------------
	 * @Brief : 
	 * 
	 * @Author:hzd 2013/04/03
	 *------------------------------------------------------------------*/
	void SetSessionFinished();

protected:

private:

private:

	NetSocket*			m_pSocket;					// 与socket绑定 

	int32				m_nServerID;				// 所属的服务器ID 
	int32				m_nSendPackCount;			// 发包数量（校验包顺序、丢包情况） 

	ESessionType		m_eSessionType;				// 该Session类型 

	EServerType			m_eLocalServerType;			// 本地服务器类型 
	EServerType			m_eRemoteServerType;		// 远程服务器类型 
};


/*
 *
 *	Detail: ss管理器 
 *   
 *  Created by hzd 2015/01/26  
 *
 */
class ServerSessionMgr : public BaseSingle<ServerSessionMgr>
{
	typedef map<int32,ServerSession*>	SocketIDSessionMapType;	// 连接上来的Socket临时记录() 
	typedef map<int32,ServerSession*>	ServerIdSessionMapType;	// 已经登录创建的ServerSession 
public:

	ServerSessionMgr();
	~ServerSessionMgr();

	/*
	 *
	 *	Detail: 创建一个ServerSession
	 *   
	 * Copyright (c) Created by hzd 2015-5-27.All rights reserved
	 *
	 */
	ServerSession* CreateSession(NetSocket& rSocket);

	/*
	 *
	 *	Detail: 初始化ServerSession 
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	bool InitSession(int32 nServerID,ServerSession* pSession);

	/*
	 *
	 *	Detail: 释放Session(二选一)
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void RemoveSession(uint32 nServerID);

	/*
	 *
	 *	Detail: 释放Session(二选一)
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	void RemoveSession(NetSocket& rSokcet);

	/*
	 *
	 *	Detail: 获得Session
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	ServerSession*	GetSession(uint32 nServerID);

	/*
	 *
	 *	Detail: 该函数要函数到是不是同一个socket来取数据 
	 *   
	 *  Created by hzd 2015-5-25
	 *
	 */
	ServerSession* GetSession(NetSocket& rSokcet);
		  
	/*
	 *
	 *	Detail: 广播消息
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void SendToAll(NetMsgHead* pMsg,int32 nSize);

	/*
	 *
	 *	Detail: 更新
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void Update(int32 nSrvTime);

	/*
	 *
	 *	Detail: 统计被服务器连接数
	 *   
	 *  Created by hzd 2015-5-22
	 *
	 */
	int32 CountConnected();

	/*
	 *
	 *	Detail: 获得主DB
	 *   
	 *  Created by hzd 2015/06/17  
	 *
	 */
	ServerSession* GetMainDbSession();

	/*
	 *
	 *	Detail: 获得唯一的WS
	 *   
	 *  Created by hzd 2015/06/20  
	 *
	 */
	ServerSession* GetWsSession();

	/*
	 *
	 *	Detail: 获得唯一的LOG(暂时唯一)
	 *   
	 *  Created by hzd 2015/10/22  
	 *
	 */
	ServerSession* GetLogSession();

private:

private:

	SocketIDSessionMapType				m_mapSocketIDSession;		// 记录刚连接上来的Socket 
	ServerIdSessionMapType				m_mapServerIdSession;		// 记录已经登录ServerSession，serverID找Session 
	static ObjPool<ServerSession>		s_cServerSessionFactory;	// ServerSession工厂  

};

#endif

