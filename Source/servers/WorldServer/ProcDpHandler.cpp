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
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pRev->nSessionID);
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
		if(pUser == NULL)// 创建内存失败 
		{
			FLOG_ERROR(__FUNCTION__,__LINE__,"LoadUserData Fail!");
			return;
		}
	}

	// 判断是否为相同帐号 
	if (pUser && pUser->GetSessionID() == pRev->nSessionID)
	{
		pUser->EnterScene(pUser->GetCurSceneID());// 进入场景 
		return;
	}

    // 踢掉老玩家,采用回调再进行处理 
	FLOG_WARRING(__FUNCTION__, __LINE__, "Same Character Login!,uid:%lld", nCharID);

	// 发出老用客户端退出消息 
	SSNofityClientExit sMsgExit;
	sMsgExit.stEvent = make_streble(EVENT_REMOTE_SEND_AFTER_ONLY_MSE, sMsgExit.nType, pUser->GetSessionID(), pRev->nSessionID, nCharID);
	sMsgExit.nReason = SSNofityClientExit::E_REASON_REPEAT_CHARACTER;
	pUser->SendToFep(&sMsgExit, sMsgExit.GetPackLength());

}

