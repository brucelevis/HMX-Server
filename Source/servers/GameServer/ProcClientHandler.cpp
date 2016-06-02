#include "ProcClientHandler.h"

#include "SharedIncludes.h"
#include "ClientSession.h"

#include "SceneUser.h"
#include "SceneUserCtrl.h"
#include "SceneMap.h"
#include "SceneUserManager.h"
#include "TemporaryScene.h"


ProcClientHandler::ProcClientHandler(void)
{
}


ProcClientHandler::~ProcClientHandler(void)
{
}

void ProcClientHandler::ReqChatWorld(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const C2SCharWorld* pPacket = static_cast<const C2SCharWorld*>(pMsg);
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacket->nSessionID);
	ASSERT(pClientSession);
	int32 nID = pClientSession->GetSessionID();
	stringstream ss;
	ss << nID;
	S2CRepCharWorld sMsg;
	strcpy(sMsg.arrFromNick,ss.str().c_str());
	strcpy(sMsg.arrMsg,pPacket->arrMsg);
	sMsg.nLength = pPacket->nLength;
	ClientSessionMgr::Instance()->SendToAll(&sMsg,sMsg.GetPackLength(),ESERVER_TYPE_FEP);
}

void ProcClientHandler::ReqSceneData(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

	// 发送场景数据 
	const C2SSceneData* packet = static_cast<const C2SSceneData*>(pMsg);
	if(SceneUser* pCharacter = SceneUserManager::Instance()->GetUserByCSID(packet->nSessionID))
	{
		// 前端已经准备好了 
		pCharacter->SetClientHadReady();

		// 发送主数据 
		pCharacter->SendMainData();
	
		// 发送数据完毕消息 
		S2CSendDataFinish sMsg;
		pCharacter->GetClientSession()->SendMsgToFep(&sMsg,sMsg.GetPackLength());

	}
}

void ProcClientHandler::ReqQuestAccept(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const C2SSceneData* packet = static_cast<const C2SSceneData*>(pMsg);
	if(SceneUser* pCharacter = SceneUserManager::Instance()->GetUserByCSID(packet->nSessionID))
	{


	}
}

void ProcClientHandler::ReqChangeScene(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize)
{
	
	const C2SChanageScene* packet = static_cast<const C2SChanageScene*>(pMsg);
	SceneUser* pUser = SceneUserManager::Instance()->GetUserByCSID(packet->nSessionID);
	if (pUser == NULL)
	{
		ASSERT(pUser);
		return;
	}

	// 检查所进入的场景是否为本地场景 
	int32 nEnterSceneID = packet->nSceneID;
	if (nEnterSceneID == pUser->GetMapID())
	{
		FLOG_INFO("You hava in this scene");
		return;
	}

	SceneMap* pEnterScene = SceneMapManager::Instance()->GetSceneMap(nEnterSceneID);
	if (pEnterScene)
	{
		SceneMapManager::Instance()->DelSceneEnity(*pUser);
		TemporaryScene::Instance()->EnterUserLocal(packet->nSessionID, pUser->GetUid(), nEnterSceneID);
		return;
	}

	bool bResult = pUser->SaveData();
	if (!bResult) // 请求保存失败，等待下次请求来完成
	{
		return;
		// TODO 返回告诉前端，请求重试，上次保存进行中，请再次重试 
	}

	FLOG_INFO("Had Save Call Back!");
	FLOG_INFO("Request enter new scene , id=%d", nEnterSceneID);
	S2WChangeScene sMsgChange;
	sMsgChange.nCharID = pUser->GetUid();
	sMsgChange.nSceneID = nEnterSceneID;
	sMsgChange.nPram0 = 0;
	sMsgChange.nPram1 = 0;
	sMsgChange.nPram2 = 0;

	pUser->SendToWs(&sMsgChange, sizeof(sMsgChange));

	return;

}


