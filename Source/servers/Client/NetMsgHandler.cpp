#include "Client_PCH.h"
#include "NetMsgHandler.h"
#include "ClientNetHandler.h"
#include "CommonS2C.h"

static NetMsgHandlerMap	 gMsgHeandlerMap;
extern bool bExist;


/*--------------------------------------------
*  @brief    :	���ļ� ��������ΪGB2312 936,��Ϊ
*  @input	  : ��cmd��Ҫ��ʾ���ģ��������뻹��
*  @return   :	UTF-8 ��BOM
*-------------------------------------------*/

bool	CheckMsg(const NetMsgHead* pMsg , uint32 nSize )
{
	return true;
}


void	InitNetMsg()
{

	// �յ�����Ϣ���� 
	static NetMsgHandler handler[] =
	{
		// Э��=>������ 
		// ��¼��Э�� 
		{ PRO_F2C_ENCRYPT_INFO,		ClientNetHandler::RecvEncryptInfo,		" �յ���Կ "},
		{ PRO_F2C_LOGIN_READY_FINISH,ClientNetHandler::RecvLoginReadyFinish ," �յ���¼׼����� "},
		{ PRO_F2C_ENTER_SCENE_LOADED,ClientNetHandler::RecvEnterScene,		" �յ����볡����� "},

		//
		{ PRO_L2C_ACCOUNT_LOGIN,		ClientNetHandler::RecvLoginAccountResult," �յ���¼��� "},
		{ PRO_S2C_CHAR_WORLD,			ClientNetHandler::RecvChatWorld,		 " �յ��������� "},
		{ PRO_S2C_SCENE_INIT_RESULT,	ClientNetHandler::RecvSceneInitResult,	 " �յ�������ʼ����� "},
		{ PRO_L2C_CHARACTER_LIST,		ClientNetHandler::RecvCharacterList,	 " �յ���ɫ�б� "},
		{ PRO_L2C_ROLE_CREATE_RESULT,	ClientNetHandler::RecvRoleCreateResult,	 " ������ɫ��� "},

		{
			PRO_S2C_CHAR_MAIN_DATA,		ClientNetHandler::RecvCharacterMainData," �յ���ɫ������ "
		},
		{
			PRO_S2C_QUEST_MAIN_DATA,	ClientNetHandler::RecvQuestMainData," �յ����������� "
		},
		{
			PRO_S2C_SEND_DATA_FINISH,	ClientNetHandler::RecvSendDataFinish, " �յ����ݷ������ " 
		},
		{ PRO_S2C_SCENE_ENTER_INFO ,ClientNetHandler::RecvEnterSceneInfo, " �յ����볡������Ϣ " },
		{
			PRO_L2C_NAMES_LIST,			ClientNetHandler::RecvRandNames,	 " �յ�������� " 
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

	// ���ܴ��� 
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


