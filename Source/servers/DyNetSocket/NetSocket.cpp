#include "NetSocket.h"
#include "NetServer.h"

/*
   默认SocketDefine

*/
SocketDefine SocketDefine::g_sDefine;

/*-------------------------------------------------------------------
 * @Brief : Scoket数据处理类
 * 
 * @Author:hzd 2012/04/03
 *------------------------------------------------------------------*/
NetSocket::NetSocket(io_service& rIo_service,int32 nMaxRecivedSize,int32 nMaxSendoutSize,int32 nMaxRecivedCacheSize,int32 nMaxSendoutCacheSize):tcp::socket(rIo_service)
	, m_cHeadBuffer(buffer(m_arrRecvBuffer, PACKAGE_HEADER_SIZE))
	, m_cBodyBuffer(buffer(m_arrRecvBuffer + PACKAGE_HEADER_SIZE, nMaxRecivedSize - PACKAGE_HEADER_SIZE))
	, m_cSendBuffer(buffer(m_arrSendBuffer, nMaxSendoutSize))
	, m_nBodyLen(0)
	, m_eRecvStage(REVC_FSRS_NULL)
	, m_bSending(false)
	, m_cTimer(rIo_service)
	, m_cCloseTimer(rIo_service)
	, m_nMaxSendoutSize(nMaxSendoutSize)
	, m_nMaxRecivedSize(nMaxRecivedSize)
	, m_nTimeout(0)
	, m_errorCode(-1)
{
	static int32 nSocketIncreseID = 1;
	m_nID = nSocketIncreseID++;
	m_cIBuffer.InitBuffer(nMaxRecivedCacheSize);
	m_cOBuffer.InitBuffer(nMaxSendoutCacheSize);
}

NetSocket::~NetSocket()
{

}

void NetSocket::ReadHead()
{
	async_read(*this, m_cHeadBuffer,
		transfer_exactly(PACKAGE_HEADER_SIZE), 
		boost::bind(&NetSocket::RecvMsg, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	TimeoutStart();
}


void NetSocket::ReadBody()
{
	async_read(*this, m_cBodyBuffer,
		transfer_exactly(m_nBodyLen),
		boost::bind(&NetSocket::RecvMsg, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	TimeoutStart();
}

void NetSocket::Disconnect()
{
	m_cCloseTimer.cancel();
	m_cCloseTimer.expires_from_now(boost::posix_time::seconds(0));
	m_cCloseTimer.async_wait(boost::bind(&NetSocket::HandleClose, this, boost::asio::placeholders::error));
}

void NetSocket::HandleClose(const boost::system::error_code& error)
{

#ifdef WIN32

#else
	try
	{
		boost::system::error_code ec;
		shutdown(socket_base::shutdown_both, ec);
	}
	catch (...)
	{
	}
#endif // WIN32


	try
	{
		boost::asio::socket_base::linger option(true, 0);
		boost::system::error_code ec1;
		set_option(option, ec1);
		boost::system::error_code ec2;
		tcp::socket::close(ec2); // 这个不会再收到消息回调 end 
	} catch(...)
	{

	}

}

EMsgRead NetSocket::ReadMsg(NetMsgHead** pMsg,int32& nSize)
{

	if(m_cIBuffer.GetLen() < PACKAGE_HEADER_SIZE) 
		return MSG_READ_WAITTING;

	void* buf = m_cIBuffer.GetStart();
	memcpy(&nSize,buf,PACKAGE_HEADER_SIZE);

	if(m_cIBuffer.GetLen() < nSize + PACKAGE_HEADER_SIZE) 
		return MSG_READ_REVCING;

	*pMsg = (NetMsgHead*)((char*)buf + PACKAGE_HEADER_SIZE);
	return MSG_READ_OK;
	
}

void NetSocket::RemoveMsg(uint32 nLen)
{
	m_cIBuffer.RemoveBuffer(nLen);
}

void NetSocket::RecvMsg(const boost::system::error_code& ec, size_t bytes_transferred)
{
	TimeoutCancel();
	if(ec)
	{
		printf("[ERROR]:Recv error msg,%s\n",ec.message().c_str());
		AddEventLocalClose();
		m_errorCode = ESOCKET_EXIST_REMOTE;
		return;
	}

	switch(m_eRecvStage)
	{
		case REVC_FSRS_HEAD:
		{
			ASSERT(bytes_transferred == PACKAGE_HEADER_SIZE);
			memcpy(&m_nBodyLen,m_arrRecvBuffer,PACKAGE_HEADER_SIZE);
			if(m_nBodyLen < sizeof(NetMsgHead) || m_nBodyLen > m_nMaxRecivedSize)
			{
			   printf("[ERROR]:Recv data length,bodylen:%d, maxLimitLength:%d\n",m_nBodyLen,m_nMaxRecivedSize);
			   AddEventLocalClose();
			   m_errorCode = ESOCKET_EXIST_PACK_LENGTH_ERROR;
			   m_nBodyLen = 0;
			   return;
			}
			bool bResult = m_cIBuffer.Write(m_arrRecvBuffer, PACKAGE_HEADER_SIZE);
			if(!bResult)
			{
				printf("[ERROR]:Write too much data to buffer\n");
				AddEventLocalClose();
				m_errorCode = ESOCKET_EXIST_WRITE_TOO_DATA;
				return;
			}
			m_eRecvStage = REVC_FSRS_BODY;
			ReadBody();
		}
			break;
		case REVC_FSRS_BODY:
		{
			bool bResult = m_cIBuffer.Write(m_arrRecvBuffer + PACKAGE_HEADER_SIZE, m_nBodyLen);
			if(!bResult)
			{
				printf("[ERROR]:Write too much data to buffer\n");
				AddEventLocalClose();
				m_errorCode = ESOCKET_EXIST_WRITE_TOO_DATA;
				return;
			}
			m_eRecvStage = REVC_FSRS_HEAD;
			ReadHead();
		}
			break;
		default:
		{
			ASSERT(0);
		}
			break;
	}
}

void NetSocket::Clear()
{
	m_bSending = false;
	m_nBodyLen = 0;
	m_eRecvStage = REVC_FSRS_NULL;
	m_cIBuffer.ClearBuffer();
	m_cOBuffer.ClearBuffer();
}

void NetSocket::ParkMsg(NetMsgHead* pMsg,int32 nLength)
{
	ASSERT(nLength < 65336);

	if (!m_vecEvents.empty())
	{
		std::vector<SocketEvent> vecEvents;
		GetEvents(vecEvents);
		for (std::vector<SocketEvent>::iterator it = vecEvents.begin(); it != vecEvents.end(); ++it)
		{
			SocketEvent& stEvent = *it;
			switch (stEvent.first)
			{
			case EVENT_REMOTE_SEND_PRE_ONLY_MSG:
			{
				if (stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
				{
					stRemoteMsg sRmMsg;
					sRmMsg.stEvent = stEvent;
					int32 nRmLen = sRmMsg.GetPackLength();
					char arrLen[PACKAGE_HEADER_SIZE];
					memcpy(arrLen, &nRmLen, PACKAGE_HEADER_SIZE);
					m_cOBuffer.Write(arrLen, PACKAGE_HEADER_SIZE);
					m_cOBuffer.Write((char*)&sRmMsg, nRmLen);
					RemoveEvents(stEvent);
				}
			}
			break;
			default:
				printf("[INFO]:Not Found event:%d\n", stEvent.first);
				break;
			}
		}
	}

	{
		char arrLen[PACKAGE_HEADER_SIZE];
		memcpy(arrLen, &nLength, PACKAGE_HEADER_SIZE);
		m_cOBuffer.Write(arrLen, PACKAGE_HEADER_SIZE);
		m_cOBuffer.Write((char*)pMsg, nLength);
	}

	if (!m_vecEvents.empty())
	{
		std::vector<SocketEvent> vecEvents;
		GetEvents(vecEvents);
		for (std::vector<SocketEvent>::iterator it = vecEvents.begin(); it != vecEvents.end(); ++it)
		{
			SocketEvent& stEvent = *it;
			switch (stEvent.first)
			{
			case EVENT_REMOTE_SEND_AFTER_ONLY_MSE:
			{
				if (stEvent.second == pMsg->nType && stEvent.third == pMsg->nSessionID)
				{
					stRemoteMsg sRmMsg;
					sRmMsg.stEvent = stEvent;
					int32 nRmLen = sRmMsg.GetPackLength();
					char arrLen[PACKAGE_HEADER_SIZE];
					memcpy(arrLen, &nRmLen, PACKAGE_HEADER_SIZE);
					m_cOBuffer.Write(arrLen, PACKAGE_HEADER_SIZE);
					m_cOBuffer.Write((char*)&sRmMsg, nRmLen);
					RemoveEvents(stEvent);
				}
			}
			break;
			default:
				printf("[INFO]:Not Found event:%d\n", stEvent.first);
				break;
			}
		}
	}

}

void NetSocket::SendMsg()
{
	if(m_bSending)
		return;

	if(int nLen = m_cOBuffer.ReadRemove(&m_arrSendBuffer,m_nMaxSendoutSize))
	{
		m_bSending = true;
		async_write(*this, m_cSendBuffer, transfer_exactly(nLen), boost::bind(&NetSocket::SendMsg, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void NetSocket::SendMsg(const boost::system::error_code& ec, size_t bytes_transferred)
{
	if(ec)
	{
		printf("[ERROR]:Send msg date error\n");
		AddEventLocalClose();
		m_errorCode = ESCOKET_EXIST_SEND_CONNECT_INVAILD;
		return;
	}

	if(int nLen = m_cOBuffer.ReadRemove(&m_arrSendBuffer,m_nMaxSendoutSize))
	{
		async_write(*this, m_cSendBuffer, transfer_exactly(nLen), boost::bind(&NetSocket::SendMsg, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	} else
	{
		m_bSending = false;
	}
}

void NetSocket::HandleWait(const boost::system::error_code& error)
{
	if(error)
	{
		return;
	}
	printf("[INFO]:That is timeout!\n");
	AddEventLocalClose();
	m_errorCode = ESOCKET_EXIST_TIMEOUT;
}

void NetSocket::Run()
{
	m_eRecvStage = REVC_FSRS_HEAD;
	ReadHead();
}

const string NetSocket::GetIp()
{
	return remote_endpoint().address().to_string();
}

void NetSocket::OnEventColse()
{
	printf("[WARRING]:OnEventColse\n");
	AddEventLocalClose();
	m_errorCode = ESOCKET_EXIST_LOCAL;
}

void NetSocket::SetTimeout(int32 nTimeout)
{
	ASSERT(nTimeout > -1);
	m_nTimeout = nTimeout;
}

void NetSocket::TimeoutStart()
{
	if(m_nTimeout)
	{
		m_cTimer.cancel();
		m_cTimer.expires_from_now(boost::posix_time::seconds(m_nTimeout));
		m_cTimer.async_wait(boost::bind(&NetSocket::HandleWait, this, boost::asio::placeholders::error));
	}
}

void NetSocket::TimeoutCancel()
{
	if(m_nTimeout)
	{
		m_cTimer.cancel();
	}

}

int32 NetSocket::ErrorCode(std::string& error)
{
	switch (m_errorCode)
	{
	case ESOCKET_EXIST_NULL:
		error = "Not thing";
		break;
	case ESOCKET_EXIST_LOCAL:
		error = "Local active close";
		break;
	case ESOCKET_EXIST_REMOTE:
		error = "Remote active close";
		break;
	case ESOCKET_EXIST_TIMEOUT:
		error = "Timeout";
		break;
	case ESOCKET_EXIST_PACK_LENGTH_ERROR:
		error = "Data lenth error";
		break;
	case ESOCKET_EXIST_WRITE_TOO_DATA:
		error = "Write too data";
		break;
	case ESCOKET_EXIST_SEND_CONNECT_INVAILD:
		error = "Send connected invaild";
		break;
	default:
		error = "Unkown";
		break;
	}
	return m_errorCode;
}

bool NetSocket::GetEvents(std::vector<SocketEvent>& o_vecEvents)
{
	if (!m_vecEvents.empty())
	{
		o_vecEvents.resize(m_vecEvents.size());
		std::copy(m_vecEvents.begin(), m_vecEvents.end(),o_vecEvents.begin());
	}
	return !o_vecEvents.empty();
}

void NetSocket::RemoveEvents(const SocketEvent& stEvent)
{
	std::vector<SocketEvent>::iterator it = std::find(m_vecEvents.begin(), m_vecEvents.end(), stEvent);
	if ( it != m_vecEvents.end())
	{
		m_vecEvents.erase(it);
	}
}

void NetSocket::AddEvent(const SocketEvent& nEvent)
{
	m_vecEvents.push_back(nEvent);
}

void NetSocket::AddEventLocalClose()
{
	m_vecEvents.push_back(make_streble(EVENT_LOCAL_REVC_CLOSE, 0, 0, 0, 0));
}

void NetSocket::AddEventLocalPreMsg()
{

}

void NetSocket::AddEventLocalAfterMsg()
{

}

void NetSocket::AddEventLocalPreOnlyMsg(int32 nProtocol)
{

}

void NetSocket::AddEventLocalAfterOnlyMsg(int32 nProtocol)
{

}

void NetSocket::AddEventRemotePreClose()
{

}

void NetSocket::AddEventRemotePreMsg()
{

}

void NetSocket::AddEventRemoteAfterMsg()
{

}

void NetSocket::AddEventRemotePreOnlyMsg(int32 nProtocol)
{

}

void NetSocket::AddEventRemoteAfterOnlyMsg(int32 nProtocol)
{

}




