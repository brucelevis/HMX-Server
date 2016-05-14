#ifndef CLIENT_MSG_HANDLER_H_
#define CLIENT_MSG_HANDLER_H_

#include "BaseDefine.h"
#include "BaseSingle.h"
#include "BaseMsgHandler.h"

class ClientSession;
class NetSocket;
struct NetMsgHead;

/*-------------------------------------------------------------------
 * @Brief : 接收处理客户端（用户）连接
 *			固定的客户端数据处理，其它协议在Proc中
 * 
 * @Author:hzd 2013/04/03
 *------------------------------------------------------------------*/
class ForClientMsgHandler : public BaseMsgHandler, public BaseSingle<ForClientMsgHandler>
{
public:
	ForClientMsgHandler();
	~ForClientMsgHandler();

	// 有客户端连接回调 
	virtual void OnNetMsgEnter(NetSocket& rSocket);

	// 有客户端消息收到 
	virtual void OnNetMsg(NetSocket& rSocket, NetMsgHead* pHead,int32 nSize);

	// 有客户端断开回调(此时socket已经回收)  
	virtual void OnNetMsgExit(NetSocket& rSocket);

	// 发送到 ls
	void ReqSendToLoginServer(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize);

	// 发送到 ss
	void ReqSendToSceneServer(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize);

	// 发送到 ws
	void ReqSendToWorldServer(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize);

	//-----------------------------本地处理协议---------------------------------
	// fep
	void ReqFepEncyptHandler(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize);
	void ReqFepCloseHandler(BaseSession* pSession,const NetMsgHead* pMsg,int32 nSize);
	// 预备消息(用于处理fep新clientsession获得分配ls,ls新clientsession获得dp) 

private:
	

};

/*-------------------------------------------------------------------
 * @Brief : 将客户端发来的消息转成SS通用消息（消息头插入了clientSessionID）
 * 
 * @Author:hzd 2015:10:24
 *------------------------------------------------------------------*/
#define MSG_CLIENT_COMMON_CLIENTSESSIONID(_nClientSessinID,pMsg,nSize) static vector<char> vecMsgBuffer;\
{\
	vecMsgBuffer.resize(nSize); \
	memcpy(&vecMsgBuffer[0], pMsg, nSize); \
	reinterpret_cast<NetMsgHead*>(&vecMsgBuffer[0])->nClientSessionID = _nClientSessinID; \
}\


#endif



