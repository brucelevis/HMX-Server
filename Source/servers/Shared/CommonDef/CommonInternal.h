#ifndef __COMMON_INTERNAL_H_
#define __COMMON_INTERNAL_H_

/*
 *
 *	Detail: �ڲ�ͨ��Э�� 
 *   
 *  Created by hzd 2015-5-21
 *
 */

#include "CommonDefine.h"
#include "NetConfig.h"

enum EProReqInternal
{
	PRO_SS_REQ_LOGIN_WS		= PRO_INTERNAL_BASE + 0,	// ��¼��������,���з�������¼��ws 
	PRO_SS_REQ_LOGIN_S		= PRO_INTERNAL_BASE + 1,	// ��¼��dp,fep 
	PRO_SS_REQ_LOAD_LEST	= PRO_INTERNAL_BASE + 2,	// �����������Ϣ  
	PRO_SS_REQ_PING_S		= PRO_INTERNAL_BASE + 3,	// PINGЭ��ͻ���ping����ˣ�ping�󣬾�socket->setTimeout,�յ����ݣ���ȡ�� 
	PRO_SS_REQ_CLIENT_EXIT	= PRO_INTERNAL_BASE + 4,    // �ͻ����˳� 
	PRO_SS_REQ_CONNECT_INFO = PRO_INTERNAL_BASE + 5,	// ������Ϣ 
};

enum EProRepInternal
{
	PRO_SS_REP_LOGINED			= PRO_INTERNAL_BASE + 50, // ��¼���  
	PRO_SS_REP_SERVERINFO_LIST	= PRO_INTERNAL_BASE + 51, // ��������Ϣ 
	PRO_SS_NOFTTY_SERVERID		= PRO_INTERNAL_BASE + 52, // ���·�������Ϣ 
	PRO_SS_REP_PING_S			= PRO_INTERNAL_BASE + 53, // ����������PING�Ľ������socketȡ����ʱ��  
};


#pragma pack(push,1)


//------------------------req------------------------------

struct SSReqLoginWs : public NetMsgHead
{
	SSReqLoginWs():NetMsgHead(PRO_SS_REQ_LOGIN_WS)
	{
	}
	char arrServerAddr[32]; // ��������ַ 
	int32 nServerPort;		// �˿� 
	int32 nServerID;		// ��������ʶID 
	int32 nServerType;		// ���������� 
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
	int32 nServerID;		// ��������ʶID 
	int32 nServerType;		// ���������� 
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
 * @Brief : �˳�Э�� 
 * 
 * @Author:hzd 2015:11:19
 *------------------------------------------------------------------*/
struct SSNofityClientExit : public NetMsgHead
{
	enum 
	{	E_REASON_UNKOWN	= 0,			// δ֪ 
		E_REASON_SERVER_TICKED,			// �������߳�
		E_REASON_CLIENT_EXIT,			// �����˳� 
		E_REASON_TIMEOUT,				// ��ʱ�Ͽ� 
		E_REASON_PACKAGE_ERROR,			// �Ƿ�Э�� 
		E_REASON_SEND_ERROR,			// ��Client����ʱ���� 
		E_REASON_SWITCH_SCENE,			// �л��������������Scene��dp��
		E_REASON_REPEAT_CHARACTER = 99,		// �ظ���¼
	};

	int32 nReason;
	
	enum
	{
		E_STATE_IN_UNKOWN = 0,	// δ֪ 
		E_STATE_IN_LOGIN,		// �ڵ�¼ʱ�˳� 
		E_STATE_IN_SELECT,		// ��ѡ���ɫʱ�˳� 
		E_STATE_IN_WORLD,		// ��WORLD�� 
		E_STATE_IN_SCENE,		// �ڳ������˳� 
	};

	int32 nPostion;				// �������λ�� 

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

// �������б���Ϣ 
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
 * @Brief : ͬ����ҵķ�����������Ϣ
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


