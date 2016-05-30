#include "ProcWorldHandler.h"

#include "Config.h"
#include "MemoryManger.h"

#include "../Shared/CommonDef/SharedIncludes.h"
#include "../DyNetMysql/DbIncludes.h"

void TestDb(void* pUser,const void* pMsg)
{
	int32 a = 1;
}


ProcWorldHandler::ProcWorldHandler()
{

}

ProcWorldHandler::~ProcWorldHandler()
{

}

void ProcWorldHandler::ReqLogin(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	if(pBaseSession->Status() != E_SERVER_STATUS_CONNETED)
	{
		//FLOG_INFO("pBaseSession->Status() != EWORLD_STATUS_CONNECTED"); 
		return;
	}

	// 判断 
	pBaseSession->SetStatus(E_SERVER_STATUS_LOGINED);

	// ok finish! 
	// 等待其它 ls,ss的连接  

}

void ProcWorldHandler::ReqNotifyServerInfo(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	const W2DNotifyServerInfo* pPacket = static_cast<const W2DNotifyServerInfo*>(pMsg);
	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();

	for (int32 i = 0; i < pPacket->nServerNum; ++i)
	{
		memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
		const NotifyServerInfo& rInfo = pPacket->arrServerInfo[i];
		SPRINTF(SQL_BUFFER,"REPLACE INTO `server_info` (`group_idx`,`server_idx`,`server_type`,`server_host`,`server_port`,`server_load`,`client_load`) VALUES (%d,%d,%d,'%s',%d,%d,%d);",pPacket->nGroupID,rInfo.nServerID,rInfo.nServerType,rInfo.arrHost,rInfo.nPort,rInfo.nServerLoad,rInfo.nClientLoad);
		SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
		pDB->ExecAsyncSQL(SQL_BUFFER,NULL);
	}

}

void ProcWorldHandler::ReqNotifySceneInfo(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	const W2DNotifySceneInfo* pPacket = static_cast<const W2DNotifySceneInfo*>(pMsg);
	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	for (int32 i = 0; i < pPacket->nSceneNum; ++i)
	{
		const NotifySceneInfo& rInfo = pPacket->arrSceneInfo[i];
		memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
		SPRINTF(SQL_BUFFER,"REPLACE INTO `scene_info` (`server_idx`,`scene_idx`,`scene_load`) VALUES (%d,%d,%d);",rInfo.nServerID,rInfo.nSceneID,rInfo.nLoadNum);
		SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
		pDB->ExecAsyncSQL(SQL_BUFFER,NULL);
	}
}

void ProcWorldHandler::ReqQueryEventInfo(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	// 检查event_info表中的事件，取出后，设置为已经执行 
}


void ProcWorldHandler::ReqSelectRole(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	const W2DSelectRole* pPacket = static_cast<const W2DSelectRole*>(pMsg);

	// 检查内存是否存在等 
	int32 nMyCSID = pPacket->nSessionID;

	struct MyUserDataCallBack : public StCallBackInfo
	{
		int32 nCSID;
		virtual void QueryFinish(StUserDataForDp* pUserData) // 当异步查询数据库完成后会调用该接口，当调用完该对象，会安全地删除该指针  
		{
			ProcWorldHandler::Instance()->CallBackSelectRole(nCSID, pUserData);
		}

		MyUserDataCallBack(int32 _nCSID):nCSID(_nCSID)
		{

		}
	};

	// 这里仅能通过new的方式，因为该内容要保存一段时间 
	MyUserDataCallBack* pCallBack = new MyUserDataCallBack(nMyCSID);

	StUserDataForDp* pUserMem = MemoryManager::Instance()->GetUserDb(pPacket->nCharID,nMyCSID,pCallBack);
	if(NULL == pUserMem )
	{
		// 找不到内存，查询数据库后会回调 
		return ;
	}
	else
	{
		CallBackSelectRole(nMyCSID, pUserMem);
		S_SAFE_DELETE(pCallBack); // 删除
	}

}

void ProcWorldHandler::CallBackSelectRole(int32 nCSID, StUserDataForDp* pUserDataMemory)
{
	// 有可能是失效的ClientSession，需要做处理
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nCSID);
	if (NULL == pClientSession)
	{
		ASSERT(pClientSession);
		return;
	}
	
	D2WSelectRoleResult sMsg;
	sMsg.nResult = D2WSelectRoleResult::E_SELECT_SUCCESS;
	sMsg.sUserData.LoadData(pUserDataMemory->sCharacterTable);
	pClientSession->SendMsgToWs(&sMsg, sMsg.GetLength());
	return;
}


