#ifndef __NET_CLIENT_H_
#define __NET_CLIENT_H_

#include "NetSocket.h"

/*
 *
 *	Detail: Socket通信Client类
 *   
 *  Created by hzd 2012-8-4
 *
 */

class NetClient : public io_service
{
public:

	friend class NetSocket;

	NetClient(int32 nMaxRecivedSize = 10 * 1024,int32 nMaxSendoutSize = 10 * 1024,int32 nMaxRecivedCacheSize = 64 * 1024,int32 nMaxSendoutCacheSize = 64 * 1024);
	virtual ~NetClient(void);
	void SetAddress( const char* pIp , uint16 nPort );
	void SetHandler( PNetServerEnterHandler pEnter ,  PNetServerOnMsgHandler pMsg , PNetServerExistHandler pExit );
	void Start();
	void OnConnected();
	void Run();
	void OnUpdateRecived();
	NetSocket& GetSocket() {return m_rGameSocket;}
	bool IsConnnected() { return m_bConnected; }

	void SetHandlerDefault();

	/*
	*	事件定义
	*/
	virtual void EventLocalPreMsg(NetSocket& pSocket, const SocketEvent& sEvent) {}
	virtual void EventLocalAfterMsg(NetSocket& pSocket, const SocketEvent& sEvent) {}
	virtual void EventLocalPreOnlyMsg(NetSocket& pSocket, const SocketEvent& sEvent) {}
	virtual void EventLocalAfterOnlyMsg(NetSocket& pSocket, const SocketEvent& sEvent) {}

	virtual void EventRemoteColse(NetSocket& pSocket, const SocketEvent& sEvent) {}
	virtual void EventRemotePreMsg(NetSocket& pSocket, const SocketEvent& sEvent) {}
	virtual void EventRemoteAfterMsg(NetSocket& pSocket, const SocketEvent& sEvent) {}
	virtual void EventRemotePreOnlyMsg(NetSocket& pSocket, const SocketEvent& sEvent) {}
	virtual void EventRemoteAfterOnlyMsg(NetSocket& pSocket, const SocketEvent& sEvent) {}

protected:

	void HandleStart();
	void HandleConnect(const boost::system::error_code& error,NetSocket* rSocket);
	void SetConnect(NetSocket& rSocket);

public:

	void SetEventLocalPreMsg(PNetEventLocalPreMsg _localPreMsg)
	{
		localPreMsg = _localPreMsg;
	}

	void SetEventLocalAfterMsg(PNetEventLocalAfterMsg _localAfterMsg)
	{
		localAfterMsg = _localAfterMsg;
	}

	void SetEventLcalAfterOnlyMsg(PNetEventLocalPreOnlyMsg _localPreOnlyMsg)
	{
		localPreOnlyMsg = _localPreOnlyMsg;
	}

	void SetEventLocalAfterOnlyMsg(PNetEventLocalAfterOnlyMsg _localAfterOnlyMsg)
	{
		localAfterOnlyMsg = _localAfterOnlyMsg;
	}

	void SetEventRemoteClose(PNetEventRemoteClose _remoteClose)
	{
		remoteClose = _remoteClose;
	}

	void SetEventRemotePreMsg(PNetEventRemotePreMsg _remotePreMsg)
	{
		remotePreMsg = _remotePreMsg;
	}

	void SetEventRemoteAfterMsg(PNetEventRemoteAfterMsg _remoteAfterMsg)
	{
		remoteAfterMsg = _remoteAfterMsg;
	}

	void SetEventRemotePreOnlyMsg(PNetEventRemotePreOnlyMsg _remotePreOnlyMsg)
	{
		remotePreOnlyMsg = _remotePreOnlyMsg;
	}

	void SetEventRemoteAfterOnlyMsg(PNetEventRemoteAfterOnlyMsg _remoteAfterOnlyMsg)
	{
		remoteAfterOnlyMsg = _remoteAfterOnlyMsg;
	}

private:
		
	bool				m_bConnected;		// 是否连接成功  
	thread				m_cServiceThread;
	NetSocket			m_rGameSocket;
	tcp::endpoint		m_cServerAddr;
	PNetServerEnterHandler		m_pOnMsgConnected;
	PNetServerOnMsgHandler		m_pOnMsgRecevied;
	PNetServerExistHandler		m_pOnMsgDisconnect;

	// 本地协议，需要read前后时直接作处理 
	PNetEventLocalPreMsg			localPreMsg;
	PNetEventLocalAfterMsg			localAfterMsg;
	PNetEventLocalPreOnlyMsg		localPreOnlyMsg;
	PNetEventLocalAfterOnlyMsg		localAfterOnlyMsg;

	// 远程协议，为Server.update中的获得处理,对于某用户的协议，需要判断其sessionID  
	PNetEventRemoteClose			remoteClose;
	PNetEventRemotePreMsg			remotePreMsg;
	PNetEventRemoteAfterMsg			remoteAfterMsg;
	PNetEventRemotePreOnlyMsg		remotePreOnlyMsg;
	PNetEventRemoteAfterOnlyMsg		remoteAfterOnlyMsg;
	

};

#endif

