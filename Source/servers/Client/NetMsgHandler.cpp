#include "Client_PCH.h"
#include "NetMsgHandler.h"
#include "ClientNetHandler.h"
#include "CommonS2C.h"

static NetMsgHandlerMap	 gMsgHeandlerMap;
extern bool bExist;


/*--------------------------------------------
*  @brief    :	本文件 编码设置为GB2312 936,因为
*  @input	  : 在cmd中要显示中文，其它编码还是
*  @return   :	UTF-8 无BOM
*-------------------------------------------*/

bool	CheckMsg(const NetMsgHead* pMsg , uint32 nSize )
{
	return true;
}


void	InitNetMsg()
{

	// 收到的消息处理 
	static NetMsgHandler handler[] =
	{
		// 协议=>处理函数 
		// 登录类协议 
		{ PRO_F2C_ENCRYPT_INFO,		ClientNetHandler::RecvEncryptInfo,		" 收到密钥 "},
		{ PRO_F2C_LOGIN_READY_FINISH,ClientNetHandler::RecvLoginReadyFinish ," 收到登录准备完成 "},
		{ PRO_F2C_ENTER_SCENE_LOADED,ClientNetHandler::RecvEnterScene,		" 收到进入场景完成 "},

		//
		{ PRO_L2C_ACCOUNT_LOGIN,		ClientNetHandler::RecvLoginAccountResult," 收到登录结果 "},
		{ PRO_S2C_CHAR_WORLD,			ClientNetHandler::RecvChatWorld,		 " 收到世界聊天 "},
		{ PRO_S2C_SCENE_INIT_RESULT,	ClientNetHandler::RecvSceneInitResult,	 " 收到场景初始化结果 "},
		{ PRO_L2C_CHARACTER_LIST,		ClientNetHandler::RecvCharacterList,	 " 收到角色列表 "},
		{ PRO_L2C_ROLE_CREATE_RESULT,	ClientNetHandler::RecvRoleCreateResult,	 " 创建角色结果 "},

		{
			PRO_S2C_CHAR_MAIN_DATA,		ClientNetHandler::RecvCharacterMainData," 收到角色主数据 "
		},
		{
			PRO_S2C_QUEST_MAIN_DATA,	ClientNetHandler::RecvQuestMainData," 收到任务主数据 "
		},
		{
			PRO_S2C_SEND_DATA_FINISH,	ClientNetHandler::RecvSendDataFinish, " 收到数据发送完毕 " 
		},
		{ PRO_S2C_SCENE_ENTER_INFO ,ClientNetHandler::RecvEnterSceneInfo, " 收到进入场景的信息 " },
		{
			PRO_L2C_NAMES_LIST,			ClientNetHandler::RecvRandNames,	 " 收到随机名字 " 
		}


		// end 


	};

	int32 i = 0;
	while ( handler[i].nType )
	{
		gMsgHeandlerMap[ handler[ i ].nType ] = handler[ i ];
		i++;
	}
}


void OnNetMsgEnter( NetSocket& rSocket )
{
	//
	ClientPlayer* pClientPlayer = ClientPlayerMgr::Instance()->GetClientPlayer(rSocket);
	ClientNetHandler::SendEncrypt(pClientPlayer);
}


void OnNetMsg( NetSocket& rSocket,NetMsgHead* pMsg ,int32 nSize )
{

	ClientPlayer* gPlayer = ClientPlayerMgr::Instance()->GetClientPlayer(rSocket);

	// 解密处理 
	static char msgBuffer[65536];
	memcpy(&msgBuffer[0], pMsg,nSize);

	Encrypt::XorCode(nSize,gPlayer->GetEncryptKey(),msgBuffer,nSize);
	pMsg = ( NetMsgHead*)&msgBuffer[0];

	NetMsgHandlerMapIter iter = gMsgHeandlerMap.find( pMsg->nType );
	if ( iter == gMsgHeandlerMap.end() )
	{
		printf("Not find Pro:%d\n", pMsg->nType);
		ASSERT(0);
		return ;
	}

	NetMsgHandler pHandler = iter->second;
	printf("OnNetMsg %d   %s \n", pMsg->nType,pHandler.strBrief.c_str());
	ClientPlayer* pClientPlayer = ClientPlayerMgr::Instance()->GetClientPlayer(rSocket);
	(pHandler.pFun)(pClientPlayer,pMsg ,nSize);
}



void OnNetMsgExit( NetSocket& socket )
{
	//	FLOG_INFO("=========Had Exist===============","");
	bExist = true;
}


