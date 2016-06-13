#include "WorldServer_PCH.h"
#include "ProcClientHandler.h"
#include "SharedIncludes.h"

#include "UserManager.h"
#include "ServerInfoMgr.h"
#include "SceneInfoManger.h"


ProcClientHandler::ProcClientHandler(void)
{
}


ProcClientHandler::~ProcClientHandler(void)
{
}


/*

退出旧的随时，但进入新的不一定能进入，则需要先检查新的是否可以进入


*/
void ProcClientHandler::ReqSwitchScene(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

	// 采用，进入新场景，将指令发到原来的场景，立刻保存数据，等待返回成功后，则请求ws,再由ws去请求对应的ss，由ss判断是否可以进入
	// 如果可以进入，则返回到ws，ws再通知原ss

	/*ServerSession* pServerSession = static_cast<ServerSession*>(pSession);
	const C2WSwitchScene* pPacket = static_cast<const C2WSwitchScene*>(pMsg);
	int32 nSceneID = pPacket->nSceneID;
	int32 nCSID = pPacket->nClientSessionID;

	User* pUser = UserManager::Instance()->GetUserByCSID(nCSID);
	if(pUser == NULL)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Switch scene not found user!");
		return ;
	}

	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nCSID);
	if(pClientSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__,__LINE__,"Not Found pClientSession!");
		return;
	}

	ServerSession* dpSession = pClientSession->GetDpSession();
	ServerSession* fepSession = pClientSession->GetFepSession();

	if(dpSession == NULL || fepSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__,__LINE__,"Not Found dpSession or fepSession!");
		return ;
	}

	pUser->EnterScene(nSceneID,dpSession->ServerID(),fepSession->ServerID());*/
	
}

void ProcClientHandler::ReqSelectRole(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

	// 检查状态等
	const C2WSelectRole* pPacket = static_cast<const C2WSelectRole*>(pMsg);
	
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacket->nSessionID);
	ASSERT(pClientSession);

	if(pPacket->nCharID < 1)
	{
		FLOG_WARRING("SelectRole ID INVAIL :%d",pPacket->nCharID);
		return ;
	}

	W2DSelectRole sMsg;
	sMsg.nCharID = pPacket->nCharID;
	pClientSession->SendMsgToDp(&sMsg,sMsg.GetPackLength());

}

