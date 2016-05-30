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
	if (SceneUser* pUser = SceneUserManager::Instance()->GetUserByCSID(packet->nSessionID))
	{
		int32 nEnterSceneID = packet->nSceneID;
		SceneMap* pOldSceneMap = pUser->GetSceneMap();
		SceneMap* pEnterScene = SceneMapManager::Instance()->GetSceneMap(nEnterSceneID);
		if (pEnterScene)
		{
			// 检查玩家能进入该Scene的资格，能不能进入该场景，则由其他配置判断 
			SceneMapManager::Instance()->DelSceneEnity(*pUser);
			TemporaryScene::Instance()->EnterUserLocal(packet->nSessionID, pUser->GetUid(), nEnterSceneID);
			return;
		}
		else// 需要请求到ws中去验证
		{
			struct MyUserSaveCallBack : public StUserSaveCallBack
			{
				int64 nCharID; 
				int32 nSceneID;
				int32 nPram0;
				int32 nPram1;
				int32 nPram2;
				virtual void SaveCallBack() // 发送到ws，由ws转发到指定场景服去检验是否可以进入那个场景 
				{
					FLOG_INFO("Had Save Call Back!");
					S2WChangeScene sMsgChange;
					sMsgChange.nCharID = nCharID;
					sMsgChange.nSceneID = nSceneID;
					sMsgChange.nPram0 = nPram0;
					sMsgChange.nPram1 = nPram1;
					sMsgChange.nPram2 = nPram2;
					// 异步接口，不能将动态对象指针保存 
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
			if (!bResult) // 请求保存失败，等待下次请求来完成
			{
				S_SAFE_DELETE(myCallback); 

				// TODO 返回告诉前端，请求重试，上次保存进行中，请再次重试 

			}

			return;
		}
	}

}


