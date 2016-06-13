#include "WorldServer_PCH.h"
#include "ProcDpHandler.h"
#include "SharedIncludes.h"
#include "NetIncludes.h"

#include "ServerInfoMgr.h"
#include "UserManager.h"
#include "SceneInfoManger.h"

ProcDpHandler::ProcDpHandler()
{
}


ProcDpHandler::~ProcDpHandler()
{
}

void ProcDpHandler::ReqGlobalInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

}

void ProcDpHandler::RepSelectRoleResult(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const D2WSelectRoleResult* packet = static_cast<const D2WSelectRoleResult*>(pMsg);
	
	ClientSession* pCSession = ClientSessionMgr::Instance()->GetSession(packet->nSessionID);
	if (pCSession == NULL)
	{
		FLOG_ERROR("Not Found pClientSession!");
		return;
	}

	int64 nCharID = packet->sUserData.nCharID;

	ASSERT(nCharID);

	// ����Ƿ�����ͬ�ʺŵ�¼ 
	WorldUser* pUser = UserManager::Instance()->GetUserByCharID(nCharID);
	if(pUser == NULL)
	{
		pUser = UserManager::Instance()->AddWorldUser(pCSession,&packet->sUserData);
		if(pUser == NULL)// �����ڴ�ʧ�� 
		{
			FLOG_ERROR("LoadUserData Fail!");
			return;
		}
	}

	// �ж��Ƿ�Ϊ��ͬ�ʺ� 
	if (pUser && pUser->GetSessionID() == packet->nSessionID)
	{
		// ����ҵ�����ʵ�Ȩ�� �����Ϊ������� ��ǰ����ֵ = ��serverѡ������ + ��scene��ʹ������ 
		SceneInfo* pSceneInfo = SceneManager::Instance()->GetLoadLestServerID(pUser->GetCurSceneID());
		if (pSceneInfo == NULL)
		{
			ASSERT(pSceneInfo);
			return; // ֪ͨǰ��ѡ�����ĳ��� 
		}

		// ѡ�����ó��� 
		ServerSession* pSSession = ServerSessionMgr::Instance()->GetSession(pSceneInfo->nServerID);
		if (pSSession == NULL)
		{
			ASSERT(pSSession);
			return;
		}

		pCSession->SetForLocalWs2(pSSession, pUser->GetCurSceneID());

		W2SReqEnterScene sMsg;
		sMsg.nSessionID = pCSession->GetSessionID();
		sMsg.nCharacterID = nCharID;
		sMsg.nSceneID = pUser->GetCurSceneID();
		sMsg.nDpServerID = pCSession->GetDpServerID();
		sMsg.nFepServerID = pCSession->GetFepServerID();
		sMsg.nEnterType = 0;
		pSSession->SendMsg(&sMsg, sMsg.GetPackLength());

		FLOG_INFO("Send to dist scene of requst enter this %d scene", pUser->GetCurSceneID());
		return;
	}

    // �ߵ������,���ûص��ٽ��д��� 
	FLOG_WARRING("Same Character Login!,uid:%lld", nCharID);

	// �������ÿͻ����˳���Ϣ 
	SSNofityClientExit sMsgExit;
	sMsgExit.stEvent = make_streble(EVENT_REMOTE_SEND_AFTER_ONLY_MSE, sMsgExit.nType, pUser->GetSessionID(), packet->nSessionID, nCharID);
	sMsgExit.nReason = SSNofityClientExit::E_REASON_REPEAT_CHARACTER;
	pUser->SendToFep(&sMsgExit, sMsgExit.GetPackLength());

}

