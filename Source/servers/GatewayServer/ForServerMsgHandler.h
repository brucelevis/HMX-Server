#ifndef __FOR_SERVER_MSG_HANDLER_H_
#define __FOR_SERVER_MSG_HANDLER_H_

#include "Config.h"
#include "BaseSingle.h"
#include "BaseMsgHandler.h"

/*-------------------------------------------------------------------
 * @Brief : fep作为服务器，处理s客户端(ls,ss)的消息处理 
 * 
 * @Author:hzd 2013/04/03
 *------------------------------------------------------------------*/
class ForServerMsgHandler : public BaseMsgHandler, public BaseSingle<ForServerMsgHandler>
{
public:
	ForServerMsgHandler();
	~ForServerMsgHandler();


	// 有客户端连接回调 
	virtual void OnNetMsgEnter(NetSocket& rSocket);

	// 有客户端消息收到 
	virtual void OnNetMsg(NetSocket& rSocket, NetMsgHead* pHead,int32 nSize);

	// 有客户端断开回调(此时socket已经回收) 
	virtual void OnNetMsgExit(NetSocket& rSocket);

	// 收到ping请求信息 
	void ReqPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 请求登录 
	void ReqLogined(BaseSession* pSessioin,const NetMsgHead* pHead,int32 nSize);

	void ServerToClient(BaseSession* pSessioin,const NetMsgHead* pHead,int32 nSize);

	void NofityClientSessionInfo(BaseSession* pSessioin,const NetMsgHead* pHead,int32 nSize);

};


#endif // !



