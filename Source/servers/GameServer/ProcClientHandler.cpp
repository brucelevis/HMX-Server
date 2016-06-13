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

	// ���ͳ������� 
	const C2SSceneData* packet = static_cast<const C2SSceneData*>(pMsg);
	if(SceneUser* pCharacter = SceneUserManager::Instance()->GetUserByCSID(packet->nSessionID))
	{
		// ǰ���Ѿ�׼������ 
		pCharacter->SetClientStatus(0);

		// ���������� 
		pCharacter->SendMainData();
	
		// �������������Ϣ 
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

	// ���������ĳ����Ƿ�Ϊ���س��� 
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

	// ���������������ws������ɽ���󣬲��ܱ���ȥ��ת

	// �������ͨ��������ֱ������Ŀ�ĵس��������ͱ��棬ɾ�������û�������Ϣ���ص����͵�ws������ת 
	pUser->SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_LAND_ID, nEnterSceneID);
	
	// �뿪��ͼ(��Ϊ�Ѿ����õ�ͼID) 
	SceneMapManager::Instance()->DelSceneEnity(*pUser);

	pUser->Serialize(SOCKET_EVENT_CODE_SAVE_CHANGE_MAP);

	return;

}


