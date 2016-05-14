#ifndef __FOR_SERVER_MSG_HANDLER_H_
#define __FOR_SERVER_MSG_HANDLER_H_

#include "Config.h"
#include "SharedIncludes.h"
#include "BaseMsgHandler.h"

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

	// 请求登录 
	void ReqLogined(BaseSession* pSessioin, const NetMsgHead* pHead,int32 nSize);

	// 收到ping请求信息 
	void ReqPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);
	
	// ClientSession信息更新 
	void NofityClientSessionInfo(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize);

	// 玩家退出场景(下线或切换场景)
	void ReqClientExitScene(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);
};


#endif // !



