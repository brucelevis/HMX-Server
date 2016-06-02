
#include "ProcWorldHandler.h"
#include "CommonW2S.h"
#include "LogManager.h"
#include "ServerSession.h"

#include "TemporaryScene.h"
#include "SceneUserManager.h"


ProcWorldHandler::ProcWorldHandler()
{
}


ProcWorldHandler::~ProcWorldHandler()
{
}

void ProcWorldHandler::ReqEnterScene(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const W2SReqEnterScene* packet = static_cast<const W2SReqEnterScene*>(pMsg);
	bool bCanEnter = TemporaryScene::Instance()->PreEnterUser(packet->nSceneID, packet->nPram0, packet->nPram1, packet->nPram2);
	if (!bCanEnter) 
	{
		// 不可以进入
		FLOG_INFO("Cann't enter this scene,because condition is notgought!");
		return;
	}
	TemporaryScene::Instance()->EnterUser(packet->nSessionID,packet->nCharacterID,packet->nSceneID,packet->nDpServerID,packet->nFepServerID);
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
