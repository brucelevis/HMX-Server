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
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pRev->nClientSessionID);
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
		if(pUser == NULL)
		{
			// �����ڴ�ʧ�� 
			FLOG_ERROR(__FUNCTION__,__LINE__,"LoadUserData Fail!");
			return;
		}

		// ���볡�� 
		pUser->EnterScene(pUser->GetCurSceneID());

	}
	else // �ߵ������
	{
		// ���ûص��ٽ��д��� 
		FLOG_WARRING(__FUNCTION__, __LINE__, "Same Character Login!,uid:%lld", nCharID);

		{
			struct stRepeatLoginCallBack : public SocketCallbackBase
			{
				stRepeatLoginCallBack(int32 _nCharID,int32 _nSessionID):SocketCallbackBase(0),nCharID(_nCharID),nSessionID(_nSessionID)
				{

				}

				void Finished(int32 nCallbackID)
				{
					// �µ�¼�û�������FEP���ٳ�FEP������¼ todo 
					printf("[INFO]:Prefab User Had Exist\n");

					ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nSessionID);
					ASSERT(pClientSession);

					if (pClientSession == NULL)
						return;

					W2DSelectRole sMsg;
					sMsg.nCharID = nCharID;
					pClientSession->SendMsgToDp(&sMsg, sMsg.GetPackLength());
				}
				int64 nCharID;
				int32 nSessionID;
			};

			// replogin �ڴ���ڻص���ɺ��Զ��ͷ� 
			stRepeatLoginCallBack* replogin = new stRepeatLoginCallBack(nCharID,pRev->nClientSessionID);

			// �������ÿͻ����˳���Ϣ 
			SSNofityClientExit sMsgExit;
			sMsgExit.nReason = SSNofityClientExit::E_REASON_REPEAT_CHARACTER;
			pUser->SendToFep(&sMsgExit, sMsgExit.GetPackLength(), replogin);
		}
		
	}

	// 
	// ���ݸ��ݳ���ID���������ŵ�Scene��ͬ����Fep��ȥ��֪ͨClient,��������׼����...

}

