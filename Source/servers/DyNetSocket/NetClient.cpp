
#include "NetClient.h"
#include "NetSocket.h"

/*-------------------------------------------------------------------
 * @Brief : Scoket客户端封装类
 *
 * @Author:hzd 2012/04/03
 *------------------------------------------------------------------*/
NetClient::NetClient(int32 nMaxRecivedSize/* = 10 * 1024*/, int32 nMaxSendoutSize /*= 10 * 1024*/, int32 nMaxRecivedCacheSize /*= 64 * 1024*/, int32 nMaxSendoutCacheSize /*= 64 * 1024*/) :m_rGameSocket(*this, nMaxRecivedSize, nMaxSendoutSize, nMaxRecivedCacheSize, nMaxSendoutCacheSize)
{
	m_bConnected = false;
	SetHandlerDefault();
}

NetClient::~NetClient(void)
{
}


void NetClient::Start()
{
	thread t(boost::bind(&NetClient::HandleStart, this));
	this_thread::yield();
	t.swap(m_cServiceThread);
}

void NetClient::HandleStart()
{
	// Use thread group can add code to connect more servers 
	// here can SetConnect more socket connect
	SetConnect(m_rGameSocket);

	thread_group tg;
	tg.create_thread(boost::bind(&NetClient::Run, this));
	this_thread::yield();
	tg.join_all(); // Waitting threads to complete

}

void NetClient::SetAddress(const char* ip, uint16 port)
{
	boost::system::error_code ec;
	m_cServerAddr = tcp::endpoint(address::from_string(ip, ec), port);
	assert(!ec);
}

void NetClient::SetConnect(NetSocket& rSocket)
{
	rSocket.async_connect(m_cServerAddr, boost::bind(&NetClient::HandleConnect, this, boost::asio::placeholders::error, &rSocket));
}


void NetClient::HandleConnect(const boost::system::error_code& error, NetSocket* pSocket)
{
	if (error)
	{
		printf("[NOTE]:Connected Fail.................ok\n");
		SetConnect(*pSocket);
		printf("[NOTE]:SetConnect Again.................ok\n");
	}
	else
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

void NetClient::SetHandler(PNetServerEnterHandler enter, PNetServerOnMsgHandler msg, PNetServerExistHandler exit)
{
	m_pOnMsgConnected = enter;
	m_pOnMsgRecevied = msg;
	m_pOnMsgDisconnect = exit;
}

void NetClient::SetHandlerDefault()
{
	// 设置默认的回调接口 

	localPreMsg = boost::bind(&NetClient::EventLocalPreMsg, this, _1, _2);
	localAfterMsg = boost::bind(&NetClient::EventLocalAfterMsg, this, _1, _2);
	localPreOnlyMsg = boost::bind(&NetClient::EventLocalPreOnlyMsg, this, _1, _2);
	localAfterOnlyMsg = boost::bind(&NetClient::EventLocalAfterOnlyMsg, this, _1, _2);

	remoteClose = boost::bind(&NetClient::EventRemoteColse, this, _1, _2);
	remotePreMsg = boost::bind(&NetClient::EventRemotePreMsg, this, _1, _2);
	remoteAfterMsg = boost::bind(&NetClient::EventRemoteAfterMsg, this, _1, _2);
	remotePreOnlyMsg = boost::bind(&NetClient::EventRemotePreOnlyMsg, this, _1, _2);
	remoteAfterOnlyMsg = boost::bind(&NetClient::EventRemoteAfterOnlyMsg, this, _1, _2);

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

void NetClient::OnUpdateRecived()
{

	static bool bNeedReadNextMsg = false;
	static bool bNeedCloseSocket = false;

	static int32 nBodyLen = 0;
	static NetMsgHead* pMsg = NULL;

	int32 nMsgResult = m_rGameSocket.ReadMsg(&pMsg, nBodyLen);

	// Pre处理事件
	std::vector<SocketEvent> vecPreEvents;
	m_rGameSocket.GetEvents(vecPreEvents);
	if (!vecPreEvents.empty())
	{
		for (std::vector<SocketEvent>::const_iterator it = vecPreEvents.begin(); it != vecPreEvents.end(); ++it)
		{
			const SocketEvent& stEvent = *it;
			switch (stEvent.first)
			{
			case EVENT_LOCAL_REVC_CLOSE: // 退出 
			{
				bNeedCloseSocket = true;
				m_rGameSocket.RemoveEvents(stEvent);
			}
			break;
			case EVENT_LOCAL_REVC_PRE_MSG:
			{
				// 调用注册好的协议
				if (localPreMsg)
				{
					(localPreMsg)(m_rGameSocket, stEvent);
					m_rGameSocket.RemoveEvents(stEvent);
				}
			}
			break;
			case EVENT_LOCAL_REVC_PRE_ONLY_MSG:
			{
				// 调用注册好的协议
				if (localPreOnlyMsg && stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
				{
					(localPreOnlyMsg)(m_rGameSocket, stEvent);
					m_rGameSocket.RemoveEvents(stEvent);
				}
			}
			break;
			case EVENT_REMOTE_REVC_CLOSE:
			case EVENT_REMOTE_REVC_PRE_MSG:
			{
				stRemoteMsg sMsg;
				sMsg.stEvent = stEvent;
				m_rGameSocket.ParkMsg(&sMsg, sizeof(stRemoteMsg));
				m_rGameSocket.SendMsg();
				m_rGameSocket.RemoveEvents(stEvent);
			}
			case EVENT_REMOTE_REVC_PRE_ONLY_MSG:
			{
				if (stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
				{
					stRemoteMsg sMsg;
					sMsg.stEvent = stEvent;
					m_rGameSocket.ParkMsg(&sMsg, sizeof(stRemoteMsg));
					m_rGameSocket.SendMsg();
					m_rGameSocket.RemoveEvents(stEvent);
				}
			}
			break;
			}
		}
	}

	// 
	if (bNeedCloseSocket)
	{
		(m_pOnMsgDisconnect)(m_rGameSocket);
		m_rGameSocket.Disconnect();
		bNeedReadNextMsg = true;
	}

	// 跳下一个Socket
	if (bNeedReadNextMsg)
		return;

	switch (nMsgResult)
	{
	case MSG_READ_INVALID:
	{
		m_rGameSocket.AddEventLocalClose();
	}
	break;
	case MSG_READ_OK:
	{
		if (pMsg->nType == PRO_REMOTE)
		{
			// 处理远程的调用，结束，无需设置状态回去 
			const stRemoteMsg* remoteMsg = static_cast<const stRemoteMsg*>(pMsg);
			switch (remoteMsg->stEvent.first)
			{
			case EVENT_REMOTE_REVC_CLOSE:
				if (remoteClose) (remoteClose)(m_rGameSocket, remoteMsg->stEvent);
				break;
			case EVENT_REMOTE_REVC_PRE_MSG:
				if (remotePreMsg) (remotePreMsg)(m_rGameSocket, remoteMsg->stEvent);
				break;
			case EVENT_REMOTE_REVC_AFTER_MSG:
				if (remoteAfterMsg) (remoteAfterMsg)(m_rGameSocket, remoteMsg->stEvent);
				break;
			case EVENT_REMOTE_REVC_PRE_ONLY_MSG:
			case EVENT_REMOTE_SEND_PRE_ONLY_MSG:
				if (remotePreOnlyMsg) (remotePreOnlyMsg)(m_rGameSocket, remoteMsg->stEvent);
				break;
			case EVENT_REMOTE_REVC_AFTER_ONLY_MSG:
			case EVENT_REMOTE_SEND_AFTER_ONLY_MSE:
				if (remoteAfterOnlyMsg) (remoteAfterOnlyMsg)(m_rGameSocket, remoteMsg->stEvent);
				break;
			default:
				printf("[ERROR]:REMOTE EVENT NOT FOUNT : %d\n", remoteMsg->stEvent.first);
				break;
			}
		}
		else
		{
			(m_pOnMsgRecevied)(m_rGameSocket, pMsg, nBodyLen);
			if (pMsg->stEvent.first) // 远程设置事件 
			{
				m_rGameSocket.AddEvent(pMsg->stEvent);
			}
			m_rGameSocket.RemoveMsg(PACKAGE_HEADER_SIZE + nBodyLen);
		}
	}
	break;
	case MSG_READ_WAITTING:
	case MSG_READ_REVCING:
		break;
	}

	// Afer检查定义事件 
	std::vector<SocketEvent> vecAfterEvents;
	m_rGameSocket.GetEvents(vecAfterEvents);
	if (!vecAfterEvents.empty())
	{
		for (std::vector<SocketEvent>::const_iterator it = vecAfterEvents.begin(); it != vecAfterEvents.end(); ++it)
		{
			const SocketEvent& stEvent = *it;
			switch (stEvent.first)
			{
			case EVENT_LOCAL_REVC_AFTER_MSG:
			{
				// 调用注册好的协议
				if (localAfterMsg)
				{
					(localAfterMsg)(m_rGameSocket, stEvent);
					m_rGameSocket.RemoveEvents(stEvent);
				}
			}
			break;
			case EVENT_LOCAL_REVC_AFTER_ONLY_MSG:
			{
				// 调用注册好的协议
				if (localAfterOnlyMsg && stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
				{
					(localPreOnlyMsg)(m_rGameSocket, stEvent);
					m_rGameSocket.RemoveEvents(stEvent);
				}
			}
			break;
			case EVENT_REMOTE_REVC_AFTER_MSG:
			{
				stRemoteMsg sMsg;
				sMsg.stEvent = stEvent;
				m_rGameSocket.ParkMsg(&sMsg, sizeof(stRemoteMsg));
				m_rGameSocket.SendMsg();
				m_rGameSocket.RemoveEvents(stEvent);
			}
			break;
			case EVENT_REMOTE_REVC_AFTER_ONLY_MSG:
			{
				if (stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
				{
					stRemoteMsg sMsg;
					sMsg.stEvent = stEvent;
					m_rGameSocket.ParkMsg(&sMsg, sizeof(stRemoteMsg));
					m_rGameSocket.SendMsg();
					m_rGameSocket.RemoveEvents(stEvent);
				}
			}
			break;
			}
		}
	}

}


