#ifndef _SERVER_MSG_HANDLER_H_
#define _SERVER_MSG_HANDLER_H_

#include "BaseSingle.h"
#include "BaseMsgHandler.h"
#include "NetConfig.h"

/*
 *
 *	Detail: 处理dp,ss,ls(无),fep请求过来的协议
 *   
 * Copyright (c) Created by hzd 2015-4-28.All rights reserved
 *
 */

class NetSocket;
struct NetMsgHead;

 
/*------------------------------------------------------------------
 *
 * @Brief : 处理所有的服务器列表
 *
 * @Author: hzd 
 * @File  : ForServerMsgHandler.h
 * @Date  : 2015/10/18 19:09
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
class ForServerMsgHandler : public BaseMsgHandler , public BaseSingle<ForServerMsgHandler>
{
public:
	ForServerMsgHandler();
	~ForServerMsgHandler();


	/*--------------------------------------------
	 *  @brief  : ls,dp,fep,ss 请求连接ws回调
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void OnNetMsgEnter(NetSocket& rSocket);

	/*--------------------------------------------
	 *  @brief  : 有ls,dp,fep,ss 发送的信息回调	
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void OnNetMsg(NetSocket& rSocket, NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 有ls,dp,fep,ss 断开的回调 
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void OnNetMsgExit(NetSocket& rSocket);
 
	/*--------------------------------------------
	 *  @brief  : ls,fep,dp,ss 请求登录ws回调	
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void ReqLogin(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 当有玩家请求Fep获得密钥时，Fep再向ws请求分配
	 *			  一个负载低的ls,dp给该用户Session
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void ReqRoadLest(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 收到ls,dp,fep,ss ping的请求，如果长期收不到
	 *  @input	: ls/dp/fep/ss的ping则超时会断开连接
	 *  @return :	
	 *-------------------------------------------*/
	void ReqPingToS(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 客户端退出通知 
	/*--------------------------------------------
	 *  @brief  : 收到玩家退出或断开的消息，该消息
	 *  @input	: 会由ws通知ss,ls,dp，
	 *  @return :	
	 *-------------------------------------------*/
	void NofityClientExit(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	// 
	/*--------------------------------------------
	 *  @brief  : 收到ls/ss/dp中各自的连接信息，状态等
	 *  @input	: 该消息会定时保存到数据中，供后台查看
	 *  @return :	
	 *-------------------------------------------*/
	void NotifyConnectInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	virtual void OnEventRemoteClose(NetSocket& rSocket, const SocketEvent& stEvent);

	virtual void OnEventRemotePreMsg(NetSocket& rSocket, const SocketEvent& stEvent);

	virtual void OnEventRemoteAfterMsg(NetSocket& rSocket, const SocketEvent& stEvent);

	virtual void OnEventRemotePreOnlyMsg(NetSocket& rSocket, const SocketEvent& stEvent);

	virtual void OnEventRemoteAfterOnlyMsg(NetSocket& rSocket, const SocketEvent& stEvent);


};

#endif


