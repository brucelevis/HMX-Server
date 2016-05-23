#ifndef __Net_SERVER_H_
#define __Net_SERVER_H_

#include "NetSocket.h"

/*-------------------------------------------------------------------
* @Brief : Socket服务器封装类
*
* @Author:hzd 2012/04/03
*------------------------------------------------------------------*/
class NetServer : public io_service
{
public:

	/*
	 *
	 *	Detail: 创建新服务器
	 *  @param 最大连接数
	 *  @param 最大收到单包大小
	 *  @param 最大一次发送大小(1+包)
	 *  @param 最大接收缓存大小
	 *  @param 最大发送缓存大小
	 *  Created by hzd 2012/04/03
	 *
	 */
	NetServer(int32 nMaxConnected,int32 nMaxRecivedSize = 10 * 1024 /* 最大收到单包大小 */ ,int32 nMaxSendoutSize = 10 * 1024/* 最大一次发送大小(1+包) */,int32 nMaxRecivedCacheSize = 64 * 1024/* 最大接收缓存大小 */,int32 nMaxSendoutCacheSize = 64 * 1024/* 最大发送缓存大小 */ );
	virtual ~NetServer();

	/*
	 *
	 *	Detail: 获得标识ID
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	int32 SID();

	/*
	 *
	 *	Detail: 设置启动嗠地址与监听端口
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void SetAddress(const char* pIp, uint16 pPort);

	/*
	 *
	 *	Detail: 设置默认回调
	 *   
	 * Copyright (c) Created by hzd 2013-4-25.All rights reserved
	 *
	 */
	void SetHandlerDefault();

	/*
	 *
	 *	Detail: 超时断开时间(秒)
	 *   
	 *  Created by hzd 2013-6-2
	 *
	 */
	void SetTimeout(int32 nTimeout);

	/*
	 *
	 *	Detail: 设置Socket事件回调(如果没有指定，则会调用默认回调，可以使用继承获得回调)
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void SetHandler(PNetServerEnterHandler pEnter, PNetServerOnMsgHandler pMsg, PNetServerExistHandler pExit);


	/*
	 *
	 *	Detail: 启动服务
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void Start();

	/*
	 *
	 *	Detail: 停止服务
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void Stop();

	/*
	 *
	 *	Detail: 将Socket连接重新挂起
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void SetAccept(NetSocket& pSocket);

	/*
	 *
	 *	Detail: 消息处理
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void Update();

	/*
	 *
	 *	Detail: 获得Socket
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	NetSocket& GetSocket(int32 nIndex);

	/*
	 *
	 *	Detail: 已经连接中Socket数量
	 *
	 *  Created by hzd 2013-4-16
	 *
	 */
	int32 ConnectedSockets();

	/*
	 *
	 *	Detail: 在进行接受的Socket数量
	 *
	 *  Created by hzd 2013-4-16
	 *
	 */
	int32 AcceptingSockets();

protected:

	/*
	 *
	 *	Detail: 有新连接
	 *   
	 * Copyright (c) Created by hzd 2013-4-25.All rights reserved
	 *
	 */
	virtual void MsgConnected(NetSocket& pSocket);

	/*
	 *
	 *	Detail: 有消息 
	 *   
	 * Copyright (c) Created by hzd 2013-4-25.All rights reserved
	 *
	 */
	virtual void MsgRecived(NetSocket& pSocket, NetMsgHead* pMsg,int32 nSize);

	/*
	 *
	 *	Detail: 连接断开
	 *   
	 * Copyright (c) Created by hzd 2013-4-25.All rights reserved
	 *
	 */
	virtual void MsgDisconnect(NetSocket& pSocket);

private:
	
	/*
	 *
	 *	Detail: IO循环处理
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void Run();

	/*
	 *
	 *	Detail: 对所有客户端连接事件绑定启动
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void   HandleStart();

	/*
	 *
	 *	Detail: 连接回调
	 *
	 *  Created by hzd 2013/01/23
	 *
	 */
	void   HandleAccept(const boost::system::error_code& error, NetSocket* socket);

	/*
	 *
	 *	Detail: 处理连理进入
	 *   
	 * Copyright (c) Created by hzd 2013-4-25.All rights reserved
	 *
	 */
	void OnUpdateAccept();

	/*
	 *
	 *	Detail: 处理有消息Socket
	 *   
	 * Copyright (c) Created by hzd 2013-4-25.All rights reserved
	 *
	 */
	void OnUpdateRecived();


private:

	int32			m_nMaxConnected;			// 最大Socket连接数 < 65536 
	int32			m_nServerID;				// 服务器标识唯一ID  
	SocketVector    m_vecUsedSocket;			// 使用中的Socket  
	SocketVector    m_vecAcceptSocket;			// 进入中的Socket  

	NetSocket*		m_arrSocket[65536];			// 所有的Scoket 

	tcp::endpoint   m_cServerAddr;				// 服务地址端 
	tcp::acceptor   m_cAcceptor;				// 连接管理器 

	PNetServerEnterHandler		m_pOnEnter;		// 连接回调 
	PNetServerOnMsgHandler		m_pOnMsg;		// 消息回调 
	PNetServerExistHandler		m_pOnExit;		// 断开回调 

	thread  m_cServiceThread;

	mutex   m_cClientListMutex;

	friend class NetServerMgr;

};


#endif





