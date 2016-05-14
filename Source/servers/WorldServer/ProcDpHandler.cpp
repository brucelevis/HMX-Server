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

	// 发送到Client，加载场景中
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pRev->nClientSessionID);
	if (pClientSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "Not Found pClientSession!");
		return;
	}

	// 创建玩家缓存内存
	
	User* pUser = UserManager::Instance()->GetUserByCharID(nCharID);
	if(pUser == NULL)
	{
		pUser = UserManager::Instance()->LoginUser(pRev->nClientSessionID,pClientSession,&pRev->sUserData);
		if(pUser == NULL)
		{
			// 创建内存失败 
			FLOG_ERROR(__FUNCTION__,__LINE__,"LoadUserData Fail!");
			return;
		}

		// 进入场景 
		pUser->EnterScene(pUser->GetCurSceneID());

	}

	// 
	// 根据根据场景ID，分配最优的Scene，同步到Fep中去，通知Client,场景数据准备中...

}

