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
	ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
	const S2WRegisterScene* pPacket = static_cast<const S2WRegisterScene*>(pMsg);
	ASSERT( pPacket->nSceneNum <= MAX_SCENE_NUM);
	std::vector<int32> vecSceneID;
	for (int32 i = 0; i < pPacket->nSceneNum; ++i)
	{
		vecSceneID.push_back(pPacket->arrSceneID[i]);
	}
	SceneInfoManager::Instance()->RegisterScene(pServerSession->ServerID(),vecSceneID);
}

void ProcSsHandler::ReqSceneCancel(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
	const S2WReqCancelScene* pPacket = static_cast<const S2WReqCancelScene*>(pMsg);
	ASSERT( pPacket->nSceneNum <= MAX_SCENE_NUM);

	std::vector<int32> vecSceneID;
	for (int32 i = 0; i < pPacket->nSceneNum; ++i)
	{
		int32 nSceneID = pPacket->arrSceneID[i];
		ASSERT(nSceneID > 0);
		vecSceneID.push_back(nSceneID);
	}
	SceneInfoManager::Instance()->RemoveScene(vecSceneID,pServerSession->ServerID());

}

void ProcSsHandler::RepEnterSceneResult(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	ServerSession* pSceneSession = static_cast<ServerSession*>(pSession);
	const S2WEnterSceneResult* packet = static_cast<const S2WEnterSceneResult*>(pMsg);
	int32 nCSID = packet->nSessionID;
	int32 nSceneID = packet->nSceneID;
	switch(packet->nResult)
	{
	case S2WEnterSceneResult::E_ENTER_SUCCESS:
		{
			ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nCSID);
			ASSERT(pClientSession);
			pClientSession->SetForLocalWs2(pSceneSession,nSceneID);
			pClientSession->SetStatus(E_CLIENT_STATUS_IN_SCENE);
		}
		break;
	case S2WEnterSceneResult::E_ENTER_FAIL:
		{
			FLOG_INFO("Enter Scene Fail!");
			// ֪ͨԭ����  
			if (packet->nCross) // ���
			{
				ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nCSID);
				ASSERT(pClientSession);
				W2SRepEnterResult sMsgResult;
				sMsgResult.nResult = W2SRepEnterResult::E_ENTER_FAIL;
				pClientSession->SendMsgToSs(&sMsgResult,sizeof(sMsgResult));
			}
			else // ͨ����¼����Ĭ�Ͻ�����һ��ʧ�� 
			{


				// ��Ҫ����������  10001 

			}
		}
		break;
	}
}

void ProcSsHandler::RepChangeScene(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize)
{
	const S2WChangeScene* packet = static_cast<const S2WChangeScene*>(pMsg);

	FLOG_INFO("Request change new scene id:%d",packet->nSceneID);

	WorldUser* pUser = UserManager::Instance()->GetUserByCharID(packet->nCharID);
	if (pUser == NULL)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "Not Found User!");
		return;
	}

	// ���͵�Client�����س�����
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(packet->nSessionID);
	if (pClientSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "Not Found pClientSession!");
		return;
	}

	ServerSession* dpSession = pClientSession->GetDpSession();
	ServerSession* fepSession = pClientSession->GetFepSession();

	if (dpSession == NULL || fepSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "Not Found dpSession or fepSession!");
		return;
	}

	// ���볡�� 
	pUser->EnterScene(pUser->GetCurSceneID(), packet->nPram0, packet->nPram1, packet->nPram2);

}





