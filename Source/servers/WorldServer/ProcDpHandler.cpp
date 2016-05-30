#include "WorldServer_PCH.h"
#include "ProcDpHandler.h"
#include "SharedIncludes.h"
#include "NetIncludes.h"

#include "ServerInfoMgr.h"
#include "UserManager.h"

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
	const D2WSelectRoleResult* pRev = static_cast<const D2WSelectRoleResult*>(pMsg);
	
	// ���͵�Client�����س�����
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pRev->nSessionID);
	if (pClientSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "Not Found pClientSession!");
		return;
	}

	int64 nCharID = pRev->sUserData.nCharID;

	ASSERT(nCharID);

	// ������һ����ڴ�
	// ����Ƿ�����ͬ�ʺŵ�¼ 
	WorldUser* pUser = UserManager::Instance()->GetUserByCharID(nCharID);
	if(pUser == NULL)
	{
		pUser = UserManager::Instance()->AddWorldUser(pClientSession,&pRev->sUserData);
		if(pUser == NULL)// �����ڴ�ʧ�� 
		{
			FLOG_ERROR(__FUNCTION__,__LINE__,"LoadUserData Fail!");
			return;
		}
	}

	// �ж��Ƿ�Ϊ��ͬ�ʺ� 
	if (pUser && pUser->GetSessionID() == pRev->nSessionID)
	{
		pUser->EnterScene(pUser->GetCurSceneID());// ���볡�� 
		return;
	}

    // �ߵ������,���ûص��ٽ��д��� 
	FLOG_WARRING(__FUNCTION__, __LINE__, "Same Character Login!,uid:%lld", nCharID);

	// �������ÿͻ����˳���Ϣ 
	SSNofityClientExit sMsgExit;
	sMsgExit.stEvent = make_streble(EVENT_REMOTE_SEND_AFTER_ONLY_MSE, sMsgExit.nType, pUser->GetSessionID(), pRev->nSessionID, nCharID);
	sMsgExit.nReason = SSNofityClientExit::E_REASON_REPEAT_CHARACTER;
	pUser->SendToFep(&sMsgExit, sMsgExit.GetPackLength());

}

