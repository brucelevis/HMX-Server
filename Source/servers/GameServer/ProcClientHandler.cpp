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
		pCharacter->SetClientStatus(0);

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
		return;
	}

	// 如果副本，则请求ws，检验可进入后，才能保存去跳转

	// 如果是普通场景，则直接设置目的地场景，发送保存，删除本地用户所有信息，回调发送到ws进行跳转 
	pUser->SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_LAND_ID, nEnterSceneID);
	
	// 离开地图(因为已经设置地图ID) 
	SceneMapManager::Instance()->DelSceneEnity(*pUser);

	pUser->Serialize(SOCKET_EVENT_CODE_SAVE_CHANGE_MAP);

	return;

}


