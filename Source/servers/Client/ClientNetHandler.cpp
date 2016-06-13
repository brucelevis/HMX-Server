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

	// 使用默认账号与密码 
	C2LAccountLogin sMsg;
	strcpy(sMsg.arrAccount,"hzd");
	strcpy(sMsg.arrPassword,"123");
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
}


void ClientNetHandler::SendLogin2(ClientPlayer* gPlayer)
{
	// 使用默认账号与密码 
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
	arrXing[0] = "李";
	arrXing[1] = "王";
	arrXing[2] = "张";
	arrXing[3] = "刘";
	arrXing[4] = "陈";
	arrXing[5] = "杨";
	arrXing[6] = "赵";
	arrXing[7] = "欧阳";
	arrXing[8] = "上官";
	arrXing[9] = "西门";

	std::string arrMing[20];
	arrMing[0] = "辰逸";
	arrMing[1] = "浩宇";
	arrMing[2] = "瑾瑜";
	arrMing[3] = "皓轩";
	arrMing[4] = "致远";
	arrMing[5] = "文博";
	arrMing[6] = "英杰";
	arrMing[7] = "致远";
	arrMing[8] = "雨泽";
	arrMing[9] = "文昊";
	arrMing[10] = "雅静";
	arrMing[11] = "惠茜";
	arrMing[12] = "梦璐";
	arrMing[13] = "漫妮";
	arrMing[14] = "语嫣";
	arrMing[15] = "倩雪";
	arrMing[16] = "灵芸";
	arrMing[17] = "茹雪";
	arrMing[18] = "美琳";
	arrMing[19] = "莉姿";

	
	int32 nXingIndex = RangedRand(0,9);
	int32 nMingIndex = RangedRand(0,19);

	std::stringstream strName;
	strName << arrXing[nXingIndex] << arrMing[nMingIndex];

	// 创建角色 
	C2LRoleCreate sMsg;
	sMsg.nType = 1;
	sMsg.nAccountID = gPlayer->AccountID();

	char arrUtf8[32];
	memset(arrUtf8,0,32);
	GBKToUTF8(strName.str().c_str(),arrUtf8,32);
	arrUtf8[31] = '\0';
	strcpy(sMsg.arrName,arrUtf8);
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
	printf(" 请求创建角色 \n");
}

void ClientNetHandler::SendSelectRole(ClientPlayer* gPlayer)
{
	// 选择角色
	C2WSelectRole sMsg;
	sMsg.nCharID = gPlayer->FirstCharID();
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
	printf(" 选择角色 \n");


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
	printf(" 我前端初始化该场景资源完成，请求场景所需要的数据\n");
	C2SSceneData sMsg;
	sMsg.nLoadDataFlags = C2SSceneData::E_DATA_TYPE_ALL;
	gPlayer->SendMsg(&sMsg,sMsg.GetPackLength());
}


////////////////////////////接收//////////////////////////////////////////


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
	printf("收到的角色数量 %d \n",nCount);
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
		printf("创建新角色ID:%lld\n",packet->nNewCharID);
		break;
	case 1:
		printf("创建失败，数据回退\n");
		break;
	case 2:
		printf("创建失败，数量限制\n");
		break;
	case 3:
		printf("创建失败，昵称同名\n");
		break;
	case 4:
		printf("创建失败，插入失败\n");
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


