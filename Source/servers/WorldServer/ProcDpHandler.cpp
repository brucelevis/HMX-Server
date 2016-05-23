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
	
	// 发送到Client，加载场景中
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pRev->nClientSessionID);
	if (pClientSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "Not Found pClientSession!");
		return;
	}

	int64 nCharID = pRev->sUserData.nCharID;

	ASSERT(nCharID);

	// 创建玩家缓存内存
	// 检查是否有相同帐号登录 
	WorldUser* pUser = UserManager::Instance()->GetUserByCharID(nCharID);
	if(pUser == NULL)
	{
		pUser = UserManager::Instance()->AddWorldUser(pClientSession,&pRev->sUserData);
		if(pUser == NULL)
		{
			// 创建内存失败 
			FLOG_ERROR(__FUNCTION__,__LINE__,"LoadUserData Fail!");
			return;
		}

		// 进入场景 
		pUser->EnterScene(pUser->GetCurSceneID());

	}
	else // 踢掉老玩家
	{
		// 采用回调再进行处理 
		FLOG_WARRING(__FUNCTION__, __LINE__, "Same Character Login!,uid:%lld", nCharID);

		{
			struct stRepeatLoginCallBack : public SocketCallbackBase
			{
				stRepeatLoginCallBack(int32 _nCharID,int32 _nSessionID):SocketCallbackBase(0),nCharID(_nCharID),nSessionID(_nSessionID)
				{

				}

				void Finished(int32 nCallbackID)
				{
					// 新登录用户发出到FEP，再出FEP发出登录 todo 
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

			// replogin 内存会在回调完成后，自动释放 
			stRepeatLoginCallBack* replogin = new stRepeatLoginCallBack(nCharID,pRev->nClientSessionID);

			// 发出老用客户端退出消息 
			SSNofityClientExit sMsgExit;
			sMsgExit.nReason = SSNofityClientExit::E_REASON_REPEAT_CHARACTER;
			pUser->SendToFep(&sMsgExit, sMsgExit.GetPackLength(), replogin);
		}
		
	}

	// 
	// 根据根据场景ID，分配最优的Scene，同步到Fep中去，通知Client,场景数据准备中...

}

