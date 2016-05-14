#include "NetServer.h"
#include "NetSocket.h"

NetServer::NetServer(int32 nMaxConnected,int32 nMaxRecivedSize,int32 nMaxSendoutSize,int32 nMaxRecivedCacheSize,int32 nMaxSendoutCacheSize):m_cAcceptor(*this),m_nMaxConnected(nMaxConnected)
{
	int32 _nPreCacehTotal = 1024 * 1024 * 1024; // 1G
	int32 _nMaxConnectNum = _nPreCacehTotal / (nMaxRecivedCacheSize + nMaxSendoutCacheSize);
	int32 _nMaxRecivedCacheSize = (_nPreCacehTotal - nMaxConnected * nMaxSendoutCacheSize) / nMaxConnected;
	int32 _nMaxSendoutCacheSize = (_nPreCacehTotal - nMaxConnected * nMaxRecivedCacheSize) / nMaxConnected;
	if(!( 0 < nMaxConnected && nMaxConnected < _nPreCacehTotal))
	{
		printf("[ERROR]:nMaxConnectd need < 65536 & > 0\n");
		ASSERT(0);
		return;
	}

	if(nMaxRecivedSize  < 1 || nMaxRecivedSize > 65536  ||  nMaxSendoutSize < 1  || nMaxSendoutSize > 65536 || 
				nMaxRecivedCacheSize < 1  || nMaxRecivedCacheSize > _nMaxRecivedCacheSize || nMaxSendoutCacheSize < 1  || nMaxSendoutCacheSize > _nMaxSendoutCacheSize)
	{
		printf("[ERROR]:nMaxRecivedSize | nMaxSendoutSize | nMaxRecivedCacheSize | nMaxSendoutCacheSize Need < ? & > 0\n");
		ASSERT(0);
	}

	static int32 s_nIncreaseNetServerID = 0;
	m_nServerID = ++s_nIncreaseNetServerID;

	for(int32 i = 0; i < nMaxConnected; i++)
	{
		m_arrSocket[i] = new NetSocket(*this,nMaxRecivedSize, nMaxSendoutSize, nMaxRecivedCacheSize, nMaxSendoutCacheSize);
	}
}


NetServer::~NetServer()
{
	for(int32 i = 0; i < m_nMaxConnected; i++)
	{
		NetSocket* pSocket = m_arrSocket[i];
		if(pSocket)
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
	for(int32 i = 0; i < m_nMaxConnected; i++)
	{
		m_arrSocket[i]->SetTimeout(nTimeout);
	}
}

void NetServer::SetAccept(NetSocket& rSocket)
{
	m_cAcceptor.async_accept(rSocket,boost::bind(&NetServer::HandleAccept, this, boost::asio::placeholders::error,&rSocket));
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
	m_pOnMsg = boost::bind(&NetServer::MsgRecived, this, _1, _2,_3);
	m_pOnExit = boost::bind(&NetServer::MsgDisconnect, this, _1);
}

void NetServer::SetHandler(PNetServerEnterHandler pEnter, PNetServerOnMsgHandler pOnMsg, PNetServerExistHandler pExit)
{
	m_pOnEnter = pEnter;
	m_pOnExit = pExit;
	m_pOnMsg = pOnMsg;
}

void NetServer::Run()
{
	while(true)
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
		} catch(std::exception& e)
		{
			printf("[ERROR]:NetServer Error Msg:%s\n",e.what());
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
	for(int i = 0; i < m_nMaxConnected; ++i)
	{
		SetAccept(*m_arrSocket[i]);
	}
	thread_group tg;
	for(int i = 0; i < MAX_THREAD; ++i)
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
	for(size_t i = 0; i < nSize; ++i)
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

	nSocketSize = m_vecUsedSocket.size();

	for(size_t nIndex = 0; nIndex < nSocketSize; ++nIndex)
	{
		NetSocket* pSocket = m_vecUsedSocket[nIndex];
		if(!pSocket)
			continue;
		
		
		switch(pSocket->ReadMsg(&pMsg,nMsgBodyLen))
		{
			case MSG_READ_INVALID: // 断开socket，再设置为重新等待 
			{
				printf("[WARRING]:MSG_READ_INVALID \n");
				(m_pOnExit)(*pSocket);
				m_vecUsedSocket.erase(m_vecUsedSocket.begin() + nIndex);
				pSocket->Disconnect();
				pSocket->Clear();
				SetAccept(*pSocket);
				nIndex--;
				nSocketSize--;
			}
				break;
			case MSG_READ_OK: // 收到正常的数据请求 
			{
				(m_pOnMsg)(*pSocket, pMsg,nMsgBodyLen);
				pSocket->RemoveMsg(PACKAGE_HEADER_SIZE + nMsgBodyLen);
			}
				break;
			case MSG_READ_WAITTING:
			case MSG_READ_REVCING:
				break;
		}
	}
}


void NetServer::Update()
{
	if(m_vecAcceptSocket.size())
	{
		OnUpdateAccept();
	}

	if(m_vecUsedSocket.size())
	{
		OnUpdateRecived();
	}
}

void NetServer::HandleAccept(const boost::system::error_code& rError, NetSocket* pSocket)
{
	if(rError)
	{
		SetAccept(*pSocket);
		return;
	}

	mutex::scoped_lock lock(m_cClientListMutex);
	m_vecAcceptSocket.push_back(pSocket);
	pSocket->Run();
	lock.unlock();
}

NetSocket& NetServer::GetSocket(int32 nIndex)
{
	return *m_arrSocket[nIndex];
}

int32 NetServer::ConnectedSockets()
{
	return m_vecUsedSocket.size();
}

int32 NetServer::AcceptingSockets()
{
	return m_vecAcceptSocket.size();
}

void NetServer::MsgConnected(NetSocket& pSocket)
{
	
}

void NetServer::MsgRecived(NetSocket& pSocket, NetMsgHead* pMsg,int32 nSize)
{
	
}

void NetServer::MsgDisconnect(NetSocket& pSocket)
{
	
}













