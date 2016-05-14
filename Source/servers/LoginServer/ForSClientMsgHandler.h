#ifndef _SERVER_HANDLER_H_
#define _SERVER_HANDLER_H_

#include "Config.h"
#include "BaseSingle.h"
#include "BaseMsgHandler.h"

/*
 *
 *	Detail: 处理与ws，ss,ls通信消息
 *   
 * Copyright (c) Created by hzd 2015-4-27.All rights reserved
 *
 */

class NetSocket;
struct NetMsgHead;

class ForSClientMsgHandler : public BaseMsgHandler, public BaseSingle<ForSClientMsgHandler>
{

public:

	ForSClientMsgHandler();
	~ForSClientMsgHandler();

	/*--------------------------------------------
	 *  @brief    :	有客户端连接回调 
	 *  @input	  : Socket
	 *  @return   :	
	 *-------------------------------------------*/
	virtual void OnNetMsgEnter(NetSocket& rSocket);

	/*--------------------------------------------
	 *  @brief    :	有客户端消息收到 
	 *  @input	  : 
	 *  @return   :	
	 *-------------------------------------------*/
	virtual void OnNetMsg(NetSocket& rSocket, NetMsgHead* pHead,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 有客户端断开回调
	 *  @input	: NetSocket 该方法调用后socket数据会被清除
	 *  @return :	
	 *-------------------------------------------*/
	virtual void OnNetMsgExit(NetSocket& rSocket);

	/*--------------------------------------------
	 *  @brief  : 请求登录WS的返回结果 
	 *			  在server完成ws连接会，都会主动请求登录ws，
				  登录时会附带自己的ServerID
	 *			  以便ws识别自己的身份，之后会返回要去连接别的server
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void RepLogined(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 
	/*--------------------------------------------
	 *  @brief  : server登录ws成功会返回的服务器列表
	 *			  该服务器列表是代表自己要去连接他们，同时也要登录他们
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/  
	void RepServerInfoList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 同步client连接的server信息 
	void NofityClientSessionInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 收到ping 返回  
	void RepPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 客户端退出通知
	void NofityClientExit(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize);
	
	/*--------------------------------------------
	 *  @brief    :	以下不能添加游戏功能代码，只能
	 *  @input	  : 添加与Ws通信框架代码，需要添加
	 *  @return   :	到proc中
	 *-------------------------------------------*/

private:

	bool m_bFirstMsg;

};

#endif


