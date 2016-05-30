#include "NetServer.h"
#include "NetSocket.h"

NetServer::NetServer(int32 nMaxConnected, int32 nMaxRecivedSize, int32 nMaxSendoutSize, int32 nMaxRecivedCacheSize, int32 nMaxSendoutCacheSize) :m_cAcceptor(*this), m_nMaxConnected(nMaxConnected)
{
	int32 _nPreCacehTotal = 1024 * 1024 * 1024; // 1G
	int32 _nMaxConnectNum = _nPreCacehTotal / (nMaxRecivedCacheSize + nMaxSendoutCacheSize);
	int32 _nMaxRecivedCacheSize = (_nPreCacehTotal - nMaxConnected * nMaxSendoutCacheSize) / nMaxConnected;
	int32 _nMaxSendoutCacheSize = (_nPreCacehTotal - nMaxConnected * nMaxRecivedCacheSize) / nMaxConnected;
	if (!(0 < nMaxConnected && nMaxConnected < _nPreCacehTotal))
	{
		printf("[ERROR]:nMaxConnectd need < 65536 & > 0\n");
		ASSERT(0);
		return;
	}

	if (nMaxRecivedSize < 1 || nMaxRecivedSize > 65536 || nMaxSendoutSize < 1 || nMaxSendoutSize > 65536 ||
		nMaxRecivedCacheSize < 1 || nMaxRecivedCacheSize > _nMaxRecivedCacheSize || nMaxSendoutCacheSize < 1 || nMaxSendoutCacheSize > _nMaxSendoutCacheSize)
	{
		printf("[ERROR]:nMaxRecivedSize | nMaxSendoutSize | nMaxRecivedCacheSize | nMaxSendoutCacheSize Need < ? & > 0\n");
		ASSERT(0);
	}

	static int32 s_nIncreaseNetServerID = 0;
	m_nServerID = ++s_nIncreaseNetServerID;

	for (int32 i = 0; i < nMaxConnected; i++)
	{
		m_arrSocket[i] = new NetSocket(*this, nMaxRecivedSize, nMaxSendoutSize, nMaxRecivedCacheSize, nMaxSendoutCacheSize);
	}
	SetHandlerDefault();
}


NetServer::~NetServer()
{
	for (int32 i = 0; i < m_nMaxConnected; i++)
	{
		NetSocket* pSocket = m_arrSocket[i];
		if (pSocket)
		{
			delete pSocket;
			pSocket = NULL;
		}
	}
}

int32 NetServer::SID()
{
	return m_nServerID;
}

void NetServer::SetTimeout(int32 nTimeout)
{
	for (int32 i = 0; i < m_nMaxConnected; i++)
	{
		m_arrSocket[i]->SetTimeout(nTimeout);
	}
}

void NetServer::SetAccept(NetSocket& rSocket)
{
	m_cAcceptor.async_accept(rSocket, boost::bind(&NetServer::HandleAccept, this, boost::asio::placeholders::error, &rSocket));
}


void NetServer::SetAddress(const char* pIp, uint16 nPort)
{
	boost::system::error_code ec;
	m_cServerAddr = tcp::endpoint(address::from_string(pIp, ec), nPort);
	assert(!ec);
}

void NetServer::SetHandlerDefault()
{
	// 设置默认的回调接口 
	m_pOnEnter = boost::bind(&NetServer::MsgConnected, this, _1);
	m_pOnMsg = boost::bind(&NetServer::MsgRecived, this, _1, _2, _3);
	m_pOnExit = boost::bind(&NetServer::MsgDisconnect, this, _1);

	localPreMsg = boost::bind(&NetServer::EventLocalPreMsg, this, _1, _2);
	localAfterMsg = boost::bind(&NetServer::EventLocalAfterMsg, this, _1, _2);
	localPreOnlyMsg = boost::bind(&NetServer::EventLocalPreOnlyMsg, this, _1, _2);
	localAfterOnlyMsg = boost::bind(&NetServer::EventLocalAfterOnlyMsg, this, _1, _2);

	remoteClose = boost::bind(&NetServer::EventRemoteColse, this, _1, _2);
	remotePreMsg = boost::bind(&NetServer::EventRemotePreMsg, this, _1, _2);
	remoteAfterMsg = boost::bind(&NetServer::EventRemoteAfterMsg, this, _1, _2);
	remotePreOnlyMsg = boost::bind(&NetServer::EventRemotePreOnlyMsg, this, _1, _2);
	remoteAfterOnlyMsg = boost::bind(&NetServer::EventRemoteAfterOnlyMsg, this, _1, _2);

}

void NetServer::SetHandler(PNetServerEnterHandler pEnter, PNetServerOnMsgHandler pOnMsg, PNetServerExistHandler pExit)
{
	m_pOnEnter = pEnter;
	m_pOnMsg = pOnMsg;
	m_pOnExit = pExit;
}

void NetServer::Run()
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
			printf("[ERROR]:NetServer Error Msg:%s\n", e.what());
		}
#endif
	}
}


void NetServer::Start()
{
	// Start socket use a new thread , that can loop
	thread t(boost::bind(&NetServer::HandleStart, this));
	this_thread::yield();		// Temporarily give up a CPU time , to other threads
	t.swap(m_cServiceThread);	// Swaps the state of the object with that of mServiceThread
}


void NetServer::HandleStart()
{
	boost::system::error_code ec;
	m_cAcceptor.open(m_cServerAddr.protocol(), ec);
	assert(!ec);
	m_cAcceptor.set_option(tcp::acceptor::reuse_address(true), ec);
	assert(!ec);
	m_cAcceptor.bind(m_cServerAddr, ec);
	assert(!ec);
	m_cAcceptor.listen(socket_base::max_connections, ec);
	assert(!ec);
	for (int i = 0; i < m_nMaxConnected; ++i)
	{
		SetAccept(*m_arrSocket[i]);
	}
	thread_group tg;
	for (int i = 0; i < MAX_THREAD; ++i)
	{
		tg.create_thread(boost::bind(&NetServer::Run, this));
	}
	this_thread::yield();
	tg.join_all();
}

void NetServer::Stop()
{

}

void NetServer::OnUpdateAccept()
{
	mutex::scoped_lock cLock(m_cClientListMutex);
	size_t nSize = m_vecAcceptSocket.size();
	for (size_t i = 0; i < nSize; ++i)
	{
		(m_pOnEnter)(*m_vecAcceptSocket[i]);
		m_vecUsedSocket.push_back(m_vecAcceptSocket[i]);
	}
	m_vecAcceptSocket.clear();
	cLock.unlock();
}

void NetServer::OnUpdateRecived()
{
	static size_t nSocketSize = 0;
	static int32 nMsgBodyLen = 0;
	static NetMsgHead* pMsg = NULL;
	static bool bNeedReadNextMsg = false;
	static bool bNeedCloseSocket = false;

	nSocketSize = m_vecUsedSocket.size();

	for (size_t nIndex = 0; nIndex < nSocketSize; ++nIndex)
	{
		NetSocket* pSocket = m_vecUsedSocket[nIndex];
		if (!pSocket)
			continue;

		bNeedReadNextMsg = false;
		bNeedCloseSocket = false;

		int nMsgResult = pSocket->ReadMsg(&pMsg, nMsgBodyLen);

		// Pre处理事件
		std::vector<SocketEvent> vecEvents;
		pSocket->GetEvents(vecEvents);
		if (!vecEvents.empty())
		{
			for (std::vector<SocketEvent >::const_iterator it = vecEvents.begin(); it != vecEvents.end(); ++it)
			{
				const SocketEvent& stEvent = *it;
				switch (stEvent.first)
				{
				case EVENT_LOCAL_REVC_CLOSE: // 退出 
				{
					bNeedCloseSocket = true;
					pSocket->RemoveEvents(stEvent);
				}
				break;
				case EVENT_LOCAL_REVC_PRE_MSG:
				{
					// 调用注册好的协议
					if (localPreMsg)
					{
						(localPreMsg)(*pSocket, stEvent);
					}
					pSocket->RemoveEvents(stEvent);
				}
				break;
				case EVENT_LOCAL_REVC_PRE_ONLY_MSG:
				{
					// 调用注册好的协议
					if (localPreOnlyMsg && stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
					{
						(localPreOnlyMsg)(*pSocket, stEvent);
						pSocket->RemoveEvents(stEvent);
					}
				}
				break;
				case EVENT_REMOTE_REVC_CLOSE:
				case EVENT_REMOTE_REVC_PRE_MSG:
				{
					stRemoteMsg sMsg;
					sMsg.stEvent = stEvent;
					pSocket->ParkMsg(&sMsg, sizeof(stRemoteMsg));
					pSocket->SendMsg();
					pSocket->RemoveEvents(stEvent);
				}
				case EVENT_REMOTE_REVC_PRE_ONLY_MSG:
				{
					if (stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
					{
						stRemoteMsg sMsg;
						sMsg.stEvent = stEvent;
						pSocket->ParkMsg(&sMsg, sizeof(stRemoteMsg));
						pSocket->SendMsg();
						pSocket->RemoveEvents(stEvent);
					}
				}
				break;
				}
			}
		}

		// 
		if (bNeedCloseSocket)
		{
			(m_pOnExit)(*pSocket);
			m_vecUsedSocket.erase(m_vecUsedSocket.begin() + nIndex);
			nIndex--; nSocketSize--;
			pSocket->Disconnect();
			Sleep(10); // for disconnect
			SetAccept(*pSocket);
			bNeedReadNextMsg = true;
		}

		// 跳下一个Socket
		if (bNeedReadNextMsg)
			continue;

		switch (nMsgResult)
		{
		case MSG_READ_INVALID: // 断开socket，再设置为重新等待 
		{
			pSocket->AddEventLocalClose();
		}
		break;
		case MSG_READ_OK: // 收到正常的数据请求 
		{
			if (pMsg->nType == PRO_REMOTE) // 来自于远程协议 
			{
				// 处理远程的调用，结束，无需设置状态回去 
				const stRemoteMsg* remoteMsg = static_cast<const stRemoteMsg*>(pMsg);
				switch (remoteMsg->stEvent.first)
				{
				case EVENT_REMOTE_REVC_CLOSE:
					if (remoteClose) (remoteClose)(*pSocket, remoteMsg->stEvent);
					break;
				case EVENT_REMOTE_REVC_PRE_MSG:
					if (remotePreMsg) (remotePreMsg)(*pSocket, remoteMsg->stEvent);
					break;
				case EVENT_REMOTE_REVC_AFTER_MSG:
					if (remoteAfterMsg) (remoteAfterMsg)(*pSocket, remoteMsg->stEvent);
					break;
				case EVENT_REMOTE_REVC_PRE_ONLY_MSG:
				case EVENT_REMOTE_SEND_PRE_ONLY_MSG:
					if (remotePreOnlyMsg) (remotePreOnlyMsg)(*pSocket, remoteMsg->stEvent);
					break;
				case EVENT_REMOTE_REVC_AFTER_ONLY_MSG:
				case EVENT_REMOTE_SEND_AFTER_ONLY_MSE:
					if (remoteAfterOnlyMsg) (remoteAfterOnlyMsg)(*pSocket, remoteMsg->stEvent);
					break;
				default:
					printf("[ERROR]:REMOTE EVENT NOT FOUNT : %d\n", remoteMsg->stEvent.first);
					break;
				}
			}
			else
			{
				(m_pOnMsg)(*pSocket, pMsg, nMsgBodyLen);
				if (pMsg->stEvent.first) // 远程增加事件 
				{
					pSocket->AddEvent(pMsg->stEvent);
				}
			}
			pSocket->RemoveMsg(PACKAGE_HEADER_SIZE + nMsgBodyLen);
		}
		break;
		case MSG_READ_WAITTING:
		case MSG_READ_REVCING:
			break;
		}

		// Afer检查定义事件 
		if (!vecEvents.empty())
		{
			for (std::vector<SocketEvent >::const_iterator it = vecEvents.begin(); it != vecEvents.end(); ++it)
			{
				const SocketEvent& stEvent = *it;
				switch (stEvent.first)
				{
				case EVENT_LOCAL_REVC_AFTER_MSG:
				{
					// 调用注册好的协议
					if (localAfterMsg)
					{
						(localAfterMsg)(*pSocket, stEvent);
						pSocket->RemoveEvents(stEvent);
					}
				}
				break;
				case EVENT_LOCAL_REVC_AFTER_ONLY_MSG:
				{
					// 调用注册好的协议
					if (localAfterOnlyMsg && stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
					{
						(localPreOnlyMsg)(*pSocket, stEvent);
						pSocket->RemoveEvents(stEvent);
					}
				}
				break;
				case EVENT_REMOTE_REVC_AFTER_MSG:
				{
					stRemoteMsg sMsg;
					sMsg.stEvent = stEvent;
					pSocket->ParkMsg(&sMsg, sizeof(stRemoteMsg));
					pSocket->SendMsg();
					pSocket->RemoveEvents(stEvent);
				}
				break;
				case EVENT_REMOTE_REVC_AFTER_ONLY_MSG:
				{
					if (stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
					{
						stRemoteMsg sMsg;
						sMsg.stEvent = stEvent;
						pSocket->ParkMsg(&sMsg, sizeof(stRemoteMsg));
						pSocket->SendMsg();
						pSocket->RemoveEvents(stEvent);
					}
				}
				break;
				}
			}
		}
	}
}


void NetServer::Update()
{
	if (m_vecAcceptSocket.size())
	{
		OnUpdateAccept();
	}

	if (m_vecUsedSocket.size())
	{
		OnUpdateRecived();
	}
}

void NetServer::HandleAccept(const boost::system::error_code& rError, NetSocket* pSocket)
{
	if (rError)
	{
		SetAccept(*pSocket);
		return;
	}

	mutex::scoped_lock lock(m_cClientListMutex);
	pSocket->Clear();
	m_vecAcceptSocket.push_back(pSocket);
	pSocket->Run();
	lock.unlock();
}

NetSocket& NetServer::GetSocket(int32 nIndex)
{
	return *m_arrSocket[nIndex];
}

size_t NetServer::ConnectedSockets()
{
	return m_vecUsedSocket.size();
}

size_t NetServer::AcceptingSockets()
{
	return m_vecAcceptSocket.size();
}

void NetServer::MsgConnected(NetSocket& pSocket)
{

}

void NetServer::MsgRecived(NetSocket& pSocket, NetMsgHead* pMsg, int32 nSize)
{

}

void NetServer::MsgDisconnect(NetSocket& pSocket)
{

}













