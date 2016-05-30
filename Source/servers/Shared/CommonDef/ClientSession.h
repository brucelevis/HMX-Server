#ifndef _GAMEPLAYER_H_
#define _GAMEPLAYER_H_

#include "BaseSingle.h"
#include "CommonDefine.h"
#include "BaseSession.h"
#include "ObjPool.h"
#include "ServerSession.h"

class NetSocket;
struct NetMsgHead;
class ServerSession;

/*------------------------------------------------------------------
 *
 * @Brief : ͨ������Server�Ŀͻ���Session��ֻ�����ڼ�¼ͨ�õ���Ϣ
 *          ���಻���޸�������Ϸ���ܣ�ֻ������Ϸ����޸�
 * @Author: hzd 
 * @File  : ClientSession.h
 * @Date  : 2015/10/18 22:33
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
// ���״̬ 
enum EClientStatus
{
	/*-------------------------------------------------------------------
	 * @Brief : ��ʼ��ʱ����Client���ӳɹ�ʱ����ʼ����Sessionʱ
	 *------------------------------------------------------------------*/
	E_CLIENT_STATUS_INITED		=0 ,	// 

	/*-------------------------------------------------------------------
	 * @Brief : ��ʼ����ɺ�����Ϊ�Ѿ����ӳɹ�״̬��
	 *          ��ʱ����������Կ�ӿ�
	 *------------------------------------------------------------------*/
	E_CLIENT_STATUS_CONNECTED	 , 

	/*-------------------------------------------------------------------
	 * @Brief : �Ѿ���������Կ�ɹ�����ʱ��Client�ǲ��������κνӿڵģ�
	 *          ��ʱ���������������为����͵�dp,ls������ID
	 *------------------------------------------------------------------*/
	E_CLIENT_STATUS_ENCRYPTED	 ,	// �����Կ�ɹ���� 

	/*-------------------------------------------------------------------
	 * @Brief : ����ls,dp������id�ɹ�����ʱclient��������ls�����ʺŵ�¼
	 * 
	 *------------------------------------------------------------------*/
	E_CLIENT_STATUS_NOTIFY_INITED ,  

	/*-------------------------------------------------------------------
	 * @Brief : 
	 * 
	 *------------------------------------------------------------------*/
	E_CLIENT_STATUS_LOGINED	,	// ��¼������ 
	E_CLIENT_STATUS_SELECTED,	// ѡ���ɫ��� 
	E_CLIENT_STATUS_IN_WORLD,	// ���Խ������ݽ������  
	E_CLIENT_STATUS_IN_SCENE,	// ����Ϸ������ 
};


/*
 *
 *	Detail: �����
 *   
 *  Created by hzd 2015/01/26  
 *
 */
class ClientSession : public BaseSession
{

public:

	ClientSession(int32 nClientSessionID);

	virtual ~ClientSession();

	// fep��forclientʹ�� 
	void SetForRemoteClient(NetSocket& rSocket);

	// all s ��for ws 
	void SetForRemoteWs(ServerSession* pWsSession);

	// fep,dp��ʹ�� 
	void SetForLocalFepDp(ServerSession* pLsSession,ServerSession* pSsSession); // ͬ������ 

	// ls,ss��ʹ�� 
	void SetForLocalLsSs(ServerSession* pWsSession,ServerSession* pFepSession,ServerSession* pDpSession); // ͬ������ 

	// ws����˵
	void SetForLocalWs1(ServerSession* pFepSession,ServerSession* pLsSession,ServerSession* pDpSession); // ͬ������ 
	void SetForLocalWs2(ServerSession* pSsSession,int32 nSceneID); // �����ͼ���� 

	/*
	 *
	 *	Detail: ���ID
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	int32 GetSessionID();

	int32 GetLsServerID()
	{
		if (m_pLs)
			return m_pLs->ServerID();
		return 0;
	}

	int32 GetSsServerID()
	{
		if (m_pSs)
			return m_pSs->ServerID();
		return 0;
	}

	int32 GetFepServerID()
	{
		if (m_pFep)
			return m_pFep->ServerID();
		return 0;
	}

	int32 GetDpServerID()
	{
		if (m_pDp)
			return m_pDp->ServerID();
		return 0;
	}

	/*
	 *
	 *	Detail: ������Ϣ��client 
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	virtual void SendMsg( NetMsgHead* pMsg,int32 nSize);

	void SendMsgToWs( NetMsgHead* pMsg,int32 nSize);

	void SendMsgToSs( NetMsgHead* pMsg,int32 nSize);

	void SendMsgToLs( NetMsgHead* pMsg,int32 nSize);

	void SendMsgToDp( NetMsgHead* pMsg,int32 nSize);

	void SendMsgToFep( NetMsgHead* pMsg,int32 nSize);

	/*
	 *	��Ҫ�Ͽ������ӣ�����øýӿ� 
	 *	Detail: �������/���������ô��˳�
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
	 *	Detail: ����ͨ����Կ
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	void SetEncryptKey(char arrNewKey[MAX_ENCRYPT_LENTH]);

	/*
	 *
	 *	Detail: ���ͨ����Կ
	 *   
	 *  Created by hzd 2015/01/21  
	 *
	 */
	void GetEncryptKey(char o_arrEncryptKey[MAX_ACCOUNT_LENG]);

	/*
	 *
	 *	Detail: ���÷���������
	 *   
	 *  Created by hzd 2015-5-28
	 *
	 */
	void SetOnServerType(int32 nServerType);

	/*
	 *
	 *	Detail: ��÷���������
	 *   
	 *  Created by hzd 2015-5-28
	 *
	 */
	int32 OnServerType();

	ServerSession* GetWsSession();

	ServerSession* GetSsSession();

	ServerSession* GetFepSession();

	ServerSession* GetLsSession();

	ServerSession* GetDpSession();

	void SetSceneID(int32 nSceneID);

	int32 ScenenID();


protected:

	void SendMsg(NetSocket* pSocket,NetMsgHead* pMsg,int32 nSize);

private:

private:

	NetSocket*			m_pSocket;							// Fep�и�clientר�� 

	int32				m_nSessionID;						// ����fep��client������SocketID 
	int32				m_SendPackCount;					// ����������У���˳�򡢶��������
	char				m_arrEncryptKey[MAX_ENCRYPT_LENTH];	// ͨ����Կ 
	int32				m_nSceneID;							// ���ڵĳ���ID 
	
	ServerSession*		m_pWs;								// ���е�ws 
	ServerSession*      m_pFep;								// ���ڵ�fep�����Ӻ�ɻ�� 
	ServerSession*		m_pLs;								// ���ڵ�ls,Ԥ������	 
	ServerSession*		m_pDp;								// ���ڵ�dp,Ԥ������ 
	ServerSession*		m_pSs;								// ���ڵ�ss,��¼���� 
	
	int32				m_nOnServerType;					// ���������������� 

};


/*
 *
 *	Detail: ��ҹ�����
 *   
 *  Created by hzd 2015/01/26  
 *
 */
class ClientSessionMgr : public BaseSingle<ClientSessionMgr>
{
	typedef map< int32 , ClientSession* >	ClientSessionMapType; // keyΪssessionID(socketid��Ψһ�����session���Զ�Ӧһ��socket) 
public:

	ClientSessionMgr();
	~ClientSessionMgr();

	/*
	 *
	 *	Detail: ����һ��ClientSession
	 *   
	 *  Created by hzd 2015-5-27
	 *
	 */
	ClientSession* AddSession(int32 nClientSessionID);

	/*
	 *
	 *	Detail: �ͷ�Player
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void RemoveSession(int32 nFepSessionID);

	/*
	 *
	 *	Detail: ���Player
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	ClientSession*	GetSession(int32 nFepSessionID);
		  
	/*
	 *
	 *	Detail: �㲥��Ϣ
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void SendToAll( NetMsgHead* pMsg,int32 nSize,EServerType eToServerType);

	/*
	 *
	 *	Detail: ����
	 *   
	 *  Created by hzd 2015/01/26  
	 *
	 */
	void Update(uint32 nSrvTime);

	/*
	 *
	 *	Detail: no
	 *   
	 * Copyright (c) Created by hzd 2015-4-28.All rights reserved
	 *
	 */
	void SendMsgToLs(NetMsgHead* pMsg,int32 nSize);

	/*
	 *
	 *	Detail: no
	 *   
	 * Copyright (c) Created by hzd 2015-4-28.All rights reserved
	 *
	 */
	void SendMsgToWs(NetMsgHead* pMsg,int32 nSize);

	/*
	 *
	 *	Detail: no
	 *   
	 * Copyright (c) Created by hzd 2015-4-28.All rights reserved
	 *
	 */
	void SendMsgToSs(NetMsgHead* pMsg,int32 nSize);

	/*
	 *
	 *	Detail: ͳ��������ws�������
	 *   
	 *  Created by hzd 2015-5-29
	 *
	 */
	int32 ConnectedSessions();

private:

	ClientSessionMapType			m_mapClientSession;			// ������һ���(������δ����)					
	static ObjPool<ClientSession>	s_cClientSessionFactory;	// ClientSession���� 

};

#endif

