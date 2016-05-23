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
 * ������״̬ 
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
 *	Detail: �������������֮���session
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
	 *	Detail: ΨһID
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	int32 ServerID();

	/*
	 *
	 *	Detail: ������Ϣ
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	virtual void SendMsg( NetMsgHead* pMsg,int32 nSize, SocketCallbackBase* pCallback = NULL);

	/*
	 *
	 *	Detail: �˳�
	 *   
	 *  Created by hzd 2015-1-21
	 *
	 */
	virtual void Exist();

	/*
	 *
	 *	Detail: ����״̬
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	virtual void SetStatus(int32 nStatus);

	/*
	 *
	 *	Detail: ���״̬
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	virtual int32 Status();

	/*
	 *
	 *	Detail: ��ʱѭ��������Ϊ��ʱ����
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	void Update(int32 nSrvTime);

	/*
	 *
	 *	Detail: ��÷���������
	 *   
	 * Copyright (c) Created by hzd 2015-4-29.All rights reserved
	 *
	 */
	EServerType RemoteServerType();

	/*
	 *
	 *	Detail: ���÷���������
	 *   
	 * Copyright (c) Created by hzd 2015-4-29.All rights reserved
	 *
	 */
	void SetRemoteServerType(EServerType eServerType);

	/*
	 *
	 *	Detail: ����session����
	 *   
	 *  Created by hzd 2015-5-25
	 *
	 */
	EServerType LocalServerType();
	
	/*
	 *
	 *	Detail: ���ñ���session����
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
	 *	Detail: ��¼��WS
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
	 *	Detail: ��¼��ĳ������
	 *   
	 *  Created by hzd 2015-5-22
	 *
	 */
	void LoginToS();

	/*
	 *
	 *	Detail: ���NetSocket
	 *   
	 * Copyright (c) Created by hzd 2015-5-27.All rights reserved
	 *
	 */
	NetSocket* GetNetSocket();

	/*
	 *
	 *	Detail: ����Session����
	 *   
	 *  Created by hzd 2015/06/06  
	 *
	 */
	void SetSessionType(ESessionType eSessionType);

	/*
	 *
	 *	Detail: ���Session����
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

	NetSocket*			m_pSocket;					// ��socket�� 

	int32				m_nServerID;				// �����ķ�����ID 
	int32				m_nSendPackCount;			// ����������У���˳�򡢶�������� 

	ESessionType		m_eSessionType;				// ��Session���� 

	EServerType			m_eLocalServerType;			// ���ط��������� 
	EServerType			m_eRemoteServerType;		// Զ�̷��������� 
};


/*
 *
 *	Detail: ss������ 
 *   
 *  Created by hzd 2015/01/26  
 *
 */
class ServerSessionMgr : public BaseSingle<ServerSessionMgr>
{
	typedef map<int32,ServerSession*>	SocketIDSessionMapType;	// ����������Socket��ʱ��¼() 
	typedef map<int32,ServerSession*>	ServerIdSessionMapType;	// �Ѿ���¼������ServerSession 
public:

	ServerSessionMgr();
	~ServerSessionMgr();

	/*
	 *
	 *	Detail: ����һ��ServerSession
	 *   
	 * Copyright (c) Created by hzd 2015-5-27.All rights reserved
	 *
	 */
	ServerSession* CreateSession(NetSocket& rSocket);

	/*
	 *
	 *	Detail: ��ʼ��ServerSession 
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	bool InitSession(int32 nServerID,ServerSession* pSession);

	/*
	 *
	 *	Detail: �ͷ�Session(��ѡһ)
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void RemoveSession(uint32 nServerID);

	/*
	 *
	 *	Detail: �ͷ�Session(��ѡһ)
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	void RemoveSession(NetSocket& rSokcet);

	/*
	 *
	 *	Detail: ���Session
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	ServerSession*	GetSession(uint32 nServerID);

	/*
	 *
	 *	Detail: �ú���Ҫ�������ǲ���ͬһ��socket��ȡ���� 
	 *   
	 *  Created by hzd 2015-5-25
	 *
	 */
	ServerSession* GetSession(NetSocket& rSokcet);
		  
	/*
	 *
	 *	Detail: �㲥��Ϣ
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void SendToAll(NetMsgHead* pMsg,int32 nSize);

	/*
	 *
	 *	Detail: ����
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void Update(int32 nSrvTime);

	/*
	 *
	 *	Detail: ͳ�Ʊ�������������
	 *   
	 *  Created by hzd 2015-5-22
	 *
	 */
	int32 CountConnected();

	/*
	 *
	 *	Detail: �����DB
	 *   
	 *  Created by hzd 2015/06/17  
	 *
	 */
	ServerSession* GetMainDbSession();

	/*
	 *
	 *	Detail: ���Ψһ��WS
	 *   
	 *  Created by hzd 2015/06/20  
	 *
	 */
	ServerSession* GetWsSession();

	/*
	 *
	 *	Detail: ���Ψһ��LOG(��ʱΨһ)
	 *   
	 *  Created by hzd 2015/10/22  
	 *
	 */
	ServerSession* GetLogSession();

private:

private:

	SocketIDSessionMapType				m_mapSocketIDSession;		// ��¼������������Socket 
	ServerIdSessionMapType				m_mapServerIdSession;		// ��¼�Ѿ���¼ServerSession��serverID��Session 
	static ObjPool<ServerSession>		s_cServerSessionFactory;	// ServerSession����  

};

#endif

