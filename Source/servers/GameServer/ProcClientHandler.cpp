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
		pCharacter->SetClientHadReady();

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
	if (SceneUser* pUser = SceneUserManager::Instance()->GetUserByCSID(packet->nSessionID))
	{
		int32 nEnterSceneID = packet->nSceneID;
		SceneMap* pOldSceneMap = pUser->GetSceneMap();
		SceneMap* pEnterScene = SceneMapManager::Instance()->GetSceneMap(nEnterSceneID);
		if (pEnterScene)
		{
			// �������ܽ����Scene���ʸ��ܲ��ܽ���ó������������������ж� 
			SceneMapManager::Instance()->DelSceneEnity(*pUser);
			TemporaryScene::Instance()->EnterUserLocal(packet->nSessionID, pUser->GetUid(), nEnterSceneID);
			return;
		}
		else// ��Ҫ����ws��ȥ��֤
		{
			struct MyUserSaveCallBack : public StUserSaveCallBack
			{
				int64 nCharID; 
				int32 nSceneID;
				int32 nPram0;
				int32 nPram1;
				int32 nPram2;
				virtual void SaveCallBack() // ���͵�ws����wsת����ָ��������ȥ�����Ƿ���Խ����Ǹ����� 
				{
					FLOG_INFO("Had Save Call Back!");
					S2WChangeScene sMsgChange;
					sMsgChange.nCharID = nCharID;
					sMsgChange.nSceneID = nSceneID;
					sMsgChange.nPram0 = nPram0;
					sMsgChange.nPram1 = nPram1;
					sMsgChange.nPram2 = nPram2;
					// �첽�ӿڣ����ܽ���̬����ָ�뱣�� 
					SceneUser* pUser = SceneUserManager::Instance()->GetUserByUID(nCharID);

					if (NULL == pUser) pUser->SendToWs(&sMsgChange, sizeof(sMsgChange));

				}

				MyUserSaveCallBack(int64 _nCharID,int32 _nSceneID,int32 _nPram0 = 0,int32 _nPram1 = 0,int32 _nPram2 = 0)
				{
					nCharID = _nCharID;
					nSceneID = _nSceneID;
					nPram0 = _nPram0;
					nPram1 = _nPram1;
					nPram2 = _nPram2;
				}
			};

			MyUserSaveCallBack* myCallback = new MyUserSaveCallBack(pUser->GetUid(),nEnterSceneID);
			bool bResult = pUser->SaveData(myCallback);
			if (!bResult) // ���󱣴�ʧ�ܣ��ȴ��´����������
			{
				S_SAFE_DELETE(myCallback); 

				// TODO ���ظ���ǰ�ˣ��������ԣ��ϴα�������У����ٴ����� 

			}

			return;
		}
	}

}


