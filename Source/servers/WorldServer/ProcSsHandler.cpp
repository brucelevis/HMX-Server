#include "WorldServer_PCH.h"
#include "ProcSsHandler.h"

#include "SharedIncludes.h"
#include "SceneInfoManger.h"
#include "UserManager.h"

ProcSsHandler::ProcSsHandler()
{
}


ProcSsHandler::~ProcSsHandler()
{
}

void ProcSsHandler::ReqSceneRegister(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ServerSession* pSSession = static_cast<ServerSession*>(pSession);
	const S2WRegisterScene* packet = static_cast<const S2WRegisterScene*>(pMsg);
	SceneRegisterManager::Instance()->RegisterScene(packet->nServerID,packet->nSceneID, packet->nSceneType);
}

void ProcSsHandler::ReqSceneCancel(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ServerSession* pSSession = static_cast<ServerSession*>(pSession);
	const S2WReqCancelScene* packet = static_cast<const S2WReqCancelScene*>(pMsg);
	ASSERT( packet->nSceneNum <= MAX_SCENE_NUM);

	std::vector<int32> vecSceneID;
	for (int32 i = 0; i < packet->nSceneNum; ++i)
	{
		int32 nSceneID = packet->arrSceneID[i];
		ASSERT(nSceneID > 0);
		vecSceneID.push_back(nSceneID);
	}
	SceneRegisterManager::Instance()->RemoveScene(vecSceneID,pSSession->ServerID());

}

void ProcSsHandler::RepEnterSceneResult(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ServerSession* pSSession = static_cast<ServerSession*>(pSession);
	const S2WEnterSceneResult* packet = static_cast<const S2WEnterSceneResult*>(pMsg);
	int32 nCSID = packet->nSessionID;
	int32 nSceneID = packet->nSceneID;
	switch(packet->nResult)
	{
	case S2WEnterSceneResult::E_ENTER_SUCCESS:
		{
			ClientSession* pCSession = ClientSessionMgr::Instance()->GetSession(nCSID);
			ASSERT(pCSession);
			ServerSession* oldSsSession = pCSession->GetSsSession();
			if (oldSsSession != pSSession)
			{
				FLOG_INFO("Send Leave Old Scene"); // todo 
			}
			pCSession->SetForLocalWs2(pSSession,nSceneID);
			pCSession->SetStatus(E_CLIENT_STATUS_IN_SCENE);
		}
		break;
	case S2WEnterSceneResult::E_ENTER_FAIL:
		{
			FLOG_INFO("Enter Scene Fail!");
			// 通知原场景  
			if (packet->nCross) // 跨服
			{
				ClientSession* pCSession = ClientSessionMgr::Instance()->GetSession(nCSID);
				ASSERT(pCSession);
				W2SRepEnterResult sMsgResult;
				sMsgResult.nResult = W2SRepEnterResult::E_ENTER_FAIL;
				pCSession->SendMsgToSs(&sMsgResult,sizeof(sMsgResult));
			}
			else // 通常登录上线默认进入上一服失败 
			{


				// 需要进入主场景  10001 

			}
		}
		break;
	}
}

void ProcSsHandler::RepChangeScene(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize)
{
	const S2WChangeScene* packet = static_cast<const S2WChangeScene*>(pMsg);

	FLOG_INFO("Request change new scene id:%d",packet->nSceneID);

	WorldUser* pUser = UserManager::Instance()->GetUserByCharID(packet->nCharacterID);
	if (pUser == NULL)
	{
		FLOG_ERROR( "Not Found User!");
		return;
	}

	// 发送到Client，加载场景中
	ClientSession* pCSession = ClientSessionMgr::Instance()->GetSession(packet->nSessionID);
	if (pCSession == NULL)
	{
		FLOG_ERROR("Not Found pClientSession id:%d!", packet->nSessionID);
		return;
	}

	ServerSession* dpSession = pCSession->GetDpSession();
	ServerSession* fepSession = pCSession->GetFepSession();

	if (dpSession == NULL || fepSession == NULL)
	{
		FLOG_ERROR("Not Found dpSession or fepSession!");
		return;
	}

	const SceneBaseInfo* sceneBase = SceneRegisterManager::Instance()->GetSceneBaseInfo(0,packet->nSceneID);
	if (sceneBase == NULL)
	{
		FLOG_ERROR("Not Found register sceneid :%d", packet->nSceneID);
		return;
	}

	if (sceneBase->nSceneType == 1) // 普通地图
	{
		// 普通地图默认可以进入 
		pUser->SetCurSceneID(packet->nSceneID);

		// 如何找到最合适的权重 最合适为负载最低 当前负载值 = 本server选择数量 + 本scene中使用数量 
		SceneInfo* pSceneInfo = SceneManager::Instance()->GetLoadLestServerID(pUser->GetCurSceneID());
		if (pSceneInfo == NULL)
		{
			ASSERT(pSceneInfo);
			return; // 通知前端选择错误的场景 
		}

		// 选择进入该场景 
		ServerSession* pSSession = ServerSessionMgr::Instance()->GetSession(pSceneInfo->nServerID);
		if (pSSession == NULL)
		{
			ASSERT(pSSession);
			return;
		}

		pCSession->SetForLocalWs2(pSSession, pUser->GetCurSceneID());

		W2SReqEnterScene sMsg;
		sMsg.nSessionID = pCSession->GetSessionID();
		sMsg.nCharacterID = pUser->GetUserID();
		sMsg.nSceneID = pUser->GetCurSceneID();
		sMsg.nDpServerID = pCSession->GetDpServerID();
		sMsg.nFepServerID = pCSession->GetFepServerID();
		sMsg.nEnterType = 1;
		pSSession->SendMsg(&sMsg, sMsg.GetPackLength());

		FLOG_INFO("Send to dist scene of requst enter this %d scene", pUser->GetCurSceneID());
		return;
	}
	else if (sceneBase->nSceneType == 2) // 副本地图 
	{

	}
	else
	{
		ASSERT(0);
	}
}





