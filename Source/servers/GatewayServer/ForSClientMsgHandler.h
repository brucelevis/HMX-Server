#ifndef _SERVER_HANDLER_H_
#define _SERVER_HANDLER_H_

#include "Config.h"
#include "BaseSingle.h"
#include "BaseMsgHandler.h"

class NetSocket;
struct NetMsgHead;

/*-------------------------------------------------------------------
 * @Brief : Fep作为WS的客户端，处理与WS（服务器）的消息 
 * 
 * @Author:hzd 2013/04/03
 *------------------------------------------------------------------*/

class ForSClientMsgHandler : public BaseMsgHandler, public BaseSingle<ForSClientMsgHandler>
{

public:
	ForSClientMsgHandler();
	~ForSClientMsgHandler();

	// 有客户端连接回调 
	virtual void OnNetMsgEnter(NetSocket& rSocket);

	// 有客户端消息收到 
	virtual void OnNetMsg(NetSocket& rSocket, NetMsgHead* pHead,int32 nSize);

	// 有客户端断开回调(此时socket已经回收) 
	virtual void OnNetMsgExit(NetSocket& rSocket);

	/*--------------------------------------------
	 *  @brief  : server登录ws成功会返回的服务器列表
	 *			  该服务器列表是代表自己要去连接他们，同时也要登录他们
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/  
	void RepServerInfoList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);
	// 登录结果 
	void RepLogined(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 同步client连接的server信息 
	void NofityClientSessionInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 收到ping 返回  
	void RepPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 
	void ServerToClient(BaseSession* pSessioin,const NetMsgHead* pHead,int32 nSize);

private:

	bool m_bIsWsMsg;

};

#endif


