#include "Client_PCH.h"
#include "ClientNetHandler.h"
#include "NetMsgHandler.h"
#include "ClientPlayer.h"
#include "CommonC2S.h"
#include "CommonS2C.h"
#include "Utility.h"

//extern	ClientPlayer* gPlayer;


ClientNetHandler::ClientNetHandler(void)
{
}


ClientNetHandler::~ClientNetHandler(void)
{
}

void ClientNetHandler::SendRequestClose(ClientPlayer* gPlayer)
{
	gPlayer->GetSocket()->OnEventColse();
}

void ClientNetHandler::SendEncrypt(ClientPlayer* gPlayer)
{
	C2FepEncryptInfo sMsg;
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
}

void ClientNetHandler::RecvEncryptInfo(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{
	const F2CEncryptInfo& rPacket = *static_cast<const F2CEncryptInfo*>(pHead);
	gPlayer->SetEncryptKey(rPacket.arrEncryptKey);
}

void ClientNetHandler::RecvLoginReadyFinish(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{
	//SendLogin(gPlayer);
}

void ClientNetHandler::SendLogin(ClientPlayer* gPlayer)
{

	// ʹ��Ĭ���˺������� 
	C2LAccountLogin sMsg;
	strcpy(sMsg.arrAccount,"hzd");
	strcpy(sMsg.arrPassword,"123");
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
}


void ClientNetHandler::SendLogin2(ClientPlayer* gPlayer)
{
	// ʹ��Ĭ���˺������� 
	C2LAccountLogin sMsg;
	strcpy(sMsg.arrAccount,"kfc");
	strcpy(sMsg.arrPassword,"123");
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
}

void ClientNetHandler::SendRandName(ClientPlayer* gPlayer)
{
	C2LRandNames sMsg;
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
}

void ClientNetHandler::RecvEnterScene(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{
	
	SendReqSceneData(gPlayer);

}

void ClientNetHandler::RecvChatWorld(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{
	int32 a = 1;
	const S2CRepCharWorld* pPacket = static_cast<const S2CRepCharWorld*>(pHead);
	printf("Recveid Client ID: %s ,Msg:%s\n",pPacket->arrFromNick,pPacket->arrMsg);
}

void ClientNetHandler::RecvSceneInitResult(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{
	int32 a = 1;
}

void ClientNetHandler::RecvRandNames(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{
	const L2CNamesList* packet = static_cast<const L2CNamesList*>(pHead);
	for (int32 i = 0; i < packet->nCount; ++i )
	{
		printf("==== %s === \n",Utf8ToGBK(packet->arrNames[i]));
	}
}

void ClientNetHandler::SendCharacterList(ClientPlayer* gPlayer)
{

}

void ClientNetHandler::SendChar2World(ClientPlayer* gPlayer)
{

}

void ClientNetHandler::SendRoleCreate(ClientPlayer* gPlayer)
{
	std::string arrXing[10];
	arrXing[0] = "��";
	arrXing[1] = "��";
	arrXing[2] = "��";
	arrXing[3] = "��";
	arrXing[4] = "��";
	arrXing[5] = "��";
	arrXing[6] = "��";
	arrXing[7] = "ŷ��";
	arrXing[8] = "�Ϲ�";
	arrXing[9] = "����";

	std::string arrMing[20];
	arrMing[0] = "����";
	arrMing[1] = "����";
	arrMing[2] = "��";
	arrMing[3] = "���";
	arrMing[4] = "��Զ";
	arrMing[5] = "�Ĳ�";
	arrMing[6] = "Ӣ��";
	arrMing[7] = "��Զ";
	arrMing[8] = "����";
	arrMing[9] = "���";
	arrMing[10] = "�ž�";
	arrMing[11] = "����";
	arrMing[12] = "���";
	arrMing[13] = "����";
	arrMing[14] = "����";
	arrMing[15] = "ٻѩ";
	arrMing[16] = "��ܿ";
	arrMing[17] = "��ѩ";
	arrMing[18] = "����";
	arrMing[19] = "����";

	
	int32 nXingIndex = RangedRand(0,9);
	int32 nMingIndex = RangedRand(0,19);

	std::stringstream strName;
	strName << arrXing[nXingIndex] << arrMing[nMingIndex];

	// ������ɫ 
	C2LRoleCreate sMsg;
	sMsg.nType = 1;
	sMsg.nAccountID = gPlayer->AccountID();

	char arrUtf8[32];
	memset(arrUtf8,0,32);
	GBKToUTF8(strName.str().c_str(),arrUtf8,32);
	arrUtf8[31] = '\0';
	strcpy(sMsg.arrName,arrUtf8);
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
	printf(" ���󴴽���ɫ \n");
}

void ClientNetHandler::SendSelectRole(ClientPlayer* gPlayer)
{
	// ѡ���ɫ
	C2WSelectRole sMsg;
	sMsg.nCharID = gPlayer->FirstCharID();
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
	printf(" ѡ���ɫ \n");


}

void ClientNetHandler::SendEnterChangeScene(ClientPlayer* gPlayer)
{	
	C2SChanageScene sMsg;
	sMsg.nSceneID = 10001;
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
}

void ClientNetHandler::SendEnterChangeScene2(ClientPlayer* gPlayer)
{
	C2SChanageScene sMsg;
	sMsg.nSceneID = 10004;
	gPlayer->SendMsg(&sMsg, sMsg.GetPackLength());
}

void ClientNetHandler::SendLoadResed(ClientPlayer* gPlayer)
{

}

void ClientNetHandler::SendReqSceneData(ClientPlayer* gPlayer)
{
	printf(" ��ǰ�˳�ʼ���ó�����Դ��ɣ����󳡾�����Ҫ������\n");
	C2SSceneData sMsg;
	sMsg.nLoadDataFlags = C2SSceneData::E_DATA_TYPE_ALL;
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
}


////////////////////////////����//////////////////////////////////////////


void ClientNetHandler::RecvLoginAccountResult(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{

	const L2CAccountLogin* pMsg = static_cast<const L2CAccountLogin*>(pHead);
	switch(pMsg->nResult)
	{
	case L2CAccountLogin::E_RESULT_SUCCESS:
		{
			gPlayer->SetAccountID(pMsg->nAccountId);
			printf("Login success!\n");
		}
		break;
	case L2CAccountLogin::E_RESULT_FAIL:
		{
			printf("Login fail!\n");
		}
		break;
	}
}

void ClientNetHandler::RecvCharacterList(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{

	const L2CCharacterList* pList = static_cast<const L2CCharacterList*>(pHead);
	int32 nCount = pList->nCount;
	std::vector<int64> arrCharID;
	printf("�յ��Ľ�ɫ���� %d \n",nCount);
	for (int32 i = 0 ; i < pList->nCount; ++i)
	{
		const L2CCharacterList::StCharacterInfo& rInfo = pList->arrInfo[i];
		printf("CharID:%lld Name:%s Level:%d \n",rInfo.nCharID,Utf8ToGBK(rInfo.arrName),rInfo.nLevel);
		arrCharID.push_back(rInfo.nCharID);
	}

	if(nCount)
	{
		gPlayer->SetCharList(arrCharID);
	}

}

void ClientNetHandler::RecvRoleCreateResult(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{
	const L2CRoleCreateResult* packet = static_cast<const L2CRoleCreateResult*>(pHead);
	switch(packet->nResult)
	{
	case 0:
		printf("�����½�ɫID:%lld\n",packet->nNewCharID);
		break;
	case 1:
		printf("����ʧ�ܣ����ݻ���\n");
		break;
	case 2:
		printf("����ʧ�ܣ���������\n");
		break;
	case 3:
		printf("����ʧ�ܣ��ǳ�ͬ��\n");
		break;
	case 4:
		printf("����ʧ�ܣ�����ʧ��\n");
		return;
	}
}

void ClientNetHandler::RecvCharacterMainData(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{

}

void ClientNetHandler::RecvQuestMainData(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{

}

void ClientNetHandler::RecvSendDataFinish(ClientPlayer* gPlayer,NetMsgHead* pHead,int32 nSize)
{

}

void ClientNetHandler::RecvEnterSceneInfo(ClientPlayer* gPlayer, NetMsgHead* pHead, int32 nSize)
{

}


