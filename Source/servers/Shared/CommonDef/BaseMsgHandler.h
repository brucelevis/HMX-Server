#ifndef __BASE_MSG_HANDLER_H_
#define __BASE_MSG_HANDLER_H_

#include "BaseDefine.h"
#include "CommonDefine.h"


class NetSocket;
struct NetMsgHead;
class BaseSession;

typedef boost::function<void(BaseSession*, const NetMsgHead*,int32 nSize)> HandlePackType;

// 协议与对应的调用函数 
struct MsgHandlerInfo
{
	MsgHandlerInfo(int32 _nPacketSize, const HandlePackType& _rHandler) :nPacketSize(_nPacketSize), rHandler(_rHandler)
	{
	}
	int32 nPacketSize;   // 数据大小 
	HandlePackType rHandler;	// 处理函数 
};

typedef	map< uint32, MsgHandlerInfo > NetMsgHandlerMap;

// 
class BaseMsgHandler 
{
public:

	virtual void OnNetMsgEnter(NetSocket& rSocket) = 0;

	virtual void OnNetMsg(NetSocket& rSocket, NetMsgHead* pMsg,int32 nSize) = 0;

	virtual void OnNetMsgExit(NetSocket& rSocket) = 0;

	const MsgHandlerInfo* GetMsgHandler(uint32 nProtocol)
	{
		NetMsgHandlerMap::iterator it = m_mapClientMsgHandler.find(nProtocol);
		if(it == m_mapClientMsgHandler.end())
		{
			return NULL;
		}
		return &(it->second);
	}
protected:
	virtual bool RegisterMessage(uint16 nMsgIdx, int32 nPacketSize, const HandlePackType& rHandler)
	{
		if(!m_mapClientMsgHandler.insert(std::make_pair(nMsgIdx, MsgHandlerInfo(nPacketSize, rHandler))).second)
		{
			ASSERT(0);
		}
		return true;
	}

private:
	NetMsgHandlerMap m_mapClientMsgHandler;
};


#endif


