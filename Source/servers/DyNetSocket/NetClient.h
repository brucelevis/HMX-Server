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
	void Update();
	NetSocket& GetSocket() {return m_rGameSocket;}
	bool IsConnnected() { return m_bConnected; }

protected:

	void HandleStart();
	void HandleConnect(const boost::system::error_code& error,NetSocket* rSocket);
	void SetConnect(NetSocket& rSocket);

private:
		
	bool				m_bConnected;		// 是否连接成功  
	thread				m_cServiceThread;
	NetSocket			m_rGameSocket;
	tcp::endpoint		m_cServerAddr;
	PNetServerEnterHandler		m_pOnMsgConnected;
	PNetServerOnMsgHandler		m_pOnMsgRecevied;
	PNetServerExistHandler		m_pOnMsgDisconnect;


	

};

#endif

