
#include "ProcWorldHandler.h"
#include "CommonW2S.h"
#include "LogManager.h"
#include "ServerSession.h"

#include "TemporaryScene.h"
#include "SceneUserManager.h"
#include "SceneMap.h"

ProcWorldHandler::ProcWorldHandler()
{
}


ProcWorldHandler::~ProcWorldHandler()
{
}

void ProcWorldHandler::ReqEnterScene(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const W2SReqEnterScene* packet = static_cast<const W2SReqEnterScene*>(pMsg);

	SceneMap* pEnterScene = SceneMapManager::Instance()->GetSceneMap(packet->nSceneID);
	if (pEnterScene == NULL)
	{
		FLOG_INFO("EnterScene Fail,Not Found SceneID:%d", packet->nSceneID);
		return;
	}

	ServerSession* dpSession = ServerSessionMgr::Instance()->GetSession(packet->nDpServerID);
	if (dpSession == NULL)
	{
		ASSERT(dpSession);
		return;
	}

	S2DLoadCharacter sMsgLoad;
	sMsgLoad.nSessionID = packet->nSessionID;
	sMsgLoad.nCharacterID = packet->nCharacterID;
	sMsgLoad.nDpServerID = packet->nDpServerID;
	sMsgLoad.nFepServerID = packet->nFepServerID;
	sMsgLoad.nEnterType = packet->nEnterType;
	dpSession->SendMsg(&sMsgLoad, sMsgLoad.GetPackLength());
}

void ProcWorldHandler::ReqEnterResult(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize)
{
	const W2SRepEnterResult* packet = static_cast<const W2SRepEnterResult*>(pMsg);

	if (packet->nResult == W2SRepEnterResult::E_ENTER_SUCCESS)
	{
		// 成功，删除本地内存即可，无需要其他处理
		SceneUserManager::Instance()->RemoveUser(packet->nCharID);
		ClientSessionMgr::Instance()->RemoveSession(packet->nSessionID);
	}
	else if (packet->nResult == W2SRepEnterResult::E_ENTER_FAIL)
	{
		// 失败，提示前端即可 
	}

}
