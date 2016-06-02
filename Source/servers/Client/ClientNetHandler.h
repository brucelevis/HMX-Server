#ifndef CLIENT_NET_HANDLER_H_
#define CLIENT_NET_HANDLER_H_

/*
 *
 *	Detail: 客户端Handler事件
 *   
 *  Created by hzd 2015/01/26  
 *
 */
class ClientNetHandler
{
public:

	ClientNetHandler(void);
	~ClientNetHandler(void);

	// 请求由服务器主动断开 
	static void SendRequestClose(ClientPlayer* gPlayer);

	/////////////////////////////发送/////////////////////////////////////////////
	static void SendEncrypt(ClientPlayer* gPlayer);
	static void SendLogin(ClientPlayer* gPlayer);
	static void SendLogin2(ClientPlayer* gPlayer);
	static void SendRandName(ClientPlayer* gPlayer);
	static void SendCharacterList(ClientPlayer* gPlayer);
	static void SendRoleCreate(ClientPlayer* gPlayer);
	static void SendChar2World(ClientPlayer* gPlayer);
	static void SendLoadResed(ClientPlayer* gPlayer);

	static void SendReqSceneData(ClientPlayer* gPlayer); 

	static void SendSelectRole(ClientPlayer* gPlayer);

	static void SendEnterChangeScene(ClientPlayer* gPlayer);
	static void SendEnterChangeScene2(ClientPlayer* gPlayer);

	//C2WSwitchScene sMsg;
	//sMsg.nSceneID = 1;
	//gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());

	/////////////////////////////接收/////////////////////////////////////////////

	static void RecvEncryptInfo(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	static void RecvLoginReadyFinish(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	static void RecvEnterScene(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	static void RecvChatWorld(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	static void RecvSceneInitResult(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);

	static void RecvRandNames(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);

	//////////////////////////////////////////////////////////////////////////
	static void RecvLoginAccountResult(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	static void RecvCharacterList(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	static void RecvRoleCreateResult(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	static void RecvCharacterMainData(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	static void RecvQuestMainData(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);
	
	static void RecvSendDataFinish(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize);

	static void RecvEnterSceneInfo(ClientPlayer* gPlayer, NetMsgHead* pHead, int32 nSize);

};

#endif

