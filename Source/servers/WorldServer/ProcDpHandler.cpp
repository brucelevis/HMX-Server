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
	int64 nCharID = pRev->sUserData.nCharID;

	// ���͵�Client�����س�����
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pRev->nClientSessionID);
	if (pClientSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "Not Found pClientSession!");
		return;
	}

	// ������һ����ڴ�
	
	User* pUser = UserManager::Instance()->GetUserByCharID(nCharID);
	if(pUser == NULL)
	{
		pUser = UserManager::Instance()->LoginUser(pRev->nClientSessionID,pClientSession,&pRev->sUserData);
		if(pUser == NULL)
		{
			// �����ڴ�ʧ�� 
			FLOG_ERROR(__FUNCTION__,__LINE__,"LoadUserData Fail!");
			return;
		}

		// ���볡�� 
		pUser->EnterScene(pUser->GetCurSceneID());

	}

	// 
	// ���ݸ��ݳ���ID���������ŵ�Scene��ͬ����Fep��ȥ��֪ͨClient,��������׼����...

}

