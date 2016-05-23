
#include "NetClient.h"
#include "NetSocket.h"

/*-------------------------------------------------------------------
 * @Brief : Scoket客户端封装类
 * 
 * @Author:hzd 2012/04/03
 *------------------------------------------------------------------*/
NetClient::NetClient(int32 nMaxRecivedSize/* = 10 * 1024*/,int32 nMaxSendoutSize /*= 10 * 1024*/,int32 nMaxRecivedCacheSize /*= 64 * 1024*/,int32 nMaxSendoutCacheSize /*= 64 * 1024*/):m_rGameSocket(*this,nMaxRecivedSize,nMaxSendoutSize,nMaxRecivedCacheSize,nMaxSendoutCacheSize)
{
	m_bConnected = false;
}

NetClient::~NetClient(void)
{
}


void NetClient::Start()
{
	thread t(boost::bind(&NetClient::HandleStart,this));
	this_thread::yield();
	t.swap(m_cServiceThread);
}

void NetClient::HandleStart()
{
	// Use thread group can add code to connect more servers 
	// here can SetConnect more socket connect
	SetConnect(m_rGameSocket);

	thread_group tg;
	tg.create_thread(boost::bind( &NetClient::Run , this ) );
	this_thread::yield();
	tg.join_all(); // Waitting threads to complete

}

void NetClient::SetAddress( const char* ip , uint16 port )
{
	boost::system::error_code ec;
	m_cServerAddr = tcp::endpoint( address::from_string( ip , ec ) , port );
	assert(!ec);
}

void NetClient::SetConnect(NetSocket& rSocket)
{
	rSocket.async_connect(m_cServerAddr,boost::bind(&NetClient::HandleConnect,this,boost::asio::placeholders::error,&rSocket));
}


void NetClient::HandleConnect(const boost::system::error_code& error,NetSocket* pSocket)
{
	if(error)
	{ 
		printf("[NOTE]:Connected Fail.................ok\n");
		SetConnect(*pSocket);
		printf("[NOTE]:SetConnect Again.................ok\n");
	}else
	{
		// 设置连接成功 
		m_bConnected = true;
		// 这里改为统一由ServerConnectMgr管理，解决线程安全问题，逻辑部分统一由外主逻辑线程处理  
		//(m_pOnMsgConnected)(*pSocket);
		printf("[NOTE]:Connected Success.................ok\n");
		pSocket->Clear();
		pSocket->Run();// 绑定该线程读头消息，这个操作必须是该线程 
		printf("[NOTE]:Socket io event start.................ok\n");
	}
}

void NetClient::OnConnected()
{
	(m_pOnMsgConnected)(m_rGameSocket);
}

void NetClient::SetHandler( PNetServerEnterHandler enter ,  PNetServerOnMsgHandler msg , PNetServerExistHandler exit )
{
	m_pOnMsgConnected	= enter;
	m_pOnMsgRecevied	= msg;
	m_pOnMsgDisconnect	= exit;
}


void NetClient::Run()
{
	while (true)
	{
		boost::system::error_code ec;
#ifdef _DEBUG
		run(ec);
		break;
#else
		try
		{
			run(ec);
			break;
		}
		catch (std::exception& e)
		{
			printf("[ERROR]:NetClient Error Msg:%s\n", e.what());
		}
#endif
	}
}

void NetClient::Update()
{
	int32 nBodyLen = 0;
	NetMsgHead* pMsg = NULL;
	switch (m_rGameSocket.ReadMsg(&pMsg,nBodyLen))
	{
	case MSG_READ_INVALID:
		{
			(m_pOnMsgDisconnect)(m_rGameSocket);
			m_rGameSocket.Disconnect();
		}
		break;
	case MSG_READ_OK:
		{
			if (pMsg->nType == PRO_CALLBACK)
			{
				const stCallbackMsg* rev = static_cast<const stCallbackMsg*>(pMsg);
				m_rGameSocket.RunCallBack(rev->nRepCallbackID);
				printf("[INFO]:Callback Local ID :%d\n", rev->nRepCallbackID);
			}
			else
			{
				(m_pOnMsgRecevied)(m_rGameSocket, pMsg, nBodyLen);
				if (pMsg->nCallbackID) // 需要回调 
				{
					stCallbackMsg callMsg;
					callMsg.nRepCallbackID = pMsg->nCallbackID;
					m_rGameSocket.ParkMsg(&callMsg, callMsg.GetPackLength());
					m_rGameSocket.SendMsg();
					printf("[INFO]:Callback Remote ID :%d ,From Pro %d\n", pMsg->nCallbackID, pMsg->nType);
				}
				m_rGameSocket.RemoveMsg(PACKAGE_HEADER_SIZE + nBodyLen);
			}
		}
		break;
	case MSG_READ_WAITTING:
	case MSG_READ_REVCING:
		break;
	}

}


