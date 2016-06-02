#include "ProcServerHandler.h"

#include "Config.h"
#include "RsyncTools.h"
#include "DataRsyncStruct.h"
#include "MemoryManger.h"

#include "../DyNetMysql/DbIncludes.h"
#include "../DyNetSocket/NetIncludes.h"
#include "../Shared/CommonDef/SharedIncludes.h"

ProcServerHandler::ProcServerHandler(void)
{
}


ProcServerHandler::~ProcServerHandler(void)
{
}

void ProcServerHandler::ReqNamesList(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	ASSERT(pDB);

	struct MyCallBack : public MyDBCallBack
	{
		BaseSession* pBaseSession;
		virtual void QueryResult(IDbRecordSet* pSet, int32 nCount)
		{
			ServerSession* pSSessoin = static_cast<ServerSession*>(pBaseSession);
			const DbRecordSet* pRecordSet = static_cast<const DbRecordSet*>(pSet);
			if (pRecordSet->Rows() > 0)
			{
				D2LNamesList sMsg;
				sMsg.nCount = 0;
				for (int32 i = 0; i < pRecordSet->Rows(); ++i)
				{
					memcpy(sMsg.arrNameList[i].arrName, pRecordSet->GetRecordData(i), 32);
					sMsg.nCount++;
				}
				pSSessoin->SendMsg(&sMsg, sMsg.GetPackLength());
			}
		}

		MyCallBack(BaseSession* _pBaseSession) :pBaseSession(_pBaseSession)
		{
		}
	};

	MyCallBack* myCallBack = new MyCallBack(pBaseSession);

	memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
	SPRINTF(SQL_BUFFER,"SELECT `name` FROM `names_info` LIMIT 1000;");
	SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
	pDB->ExecAsyncSQL(SQL_BUFFER, myCallBack);

}

void ProcServerHandler::ReqAccountLogin(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	const L2DAccountLogin* pPacket = static_cast<const L2DAccountLogin*>(pMsg);

	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacket->nSessionID);
	ASSERT(pClientSession);

	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	ASSERT(pDB);

	struct MyCallBack : public MyDBCallBack
	{
		ClientSession* pClientSession;
		virtual void QueryResult(IDbRecordSet* pSet, int32 nCount)
		{
			const DbRecordSet* pRecordSet = static_cast<const DbRecordSet*>(pSet);
			if (pRecordSet->Rows() > 0)
			{
				if (int64 nAccountId = *static_cast<int64*>(pRecordSet->GetRecordData(0)))
				{
					// 返回登录结果成功 
					D2LAccountLogin sMsg;
					sMsg.nAccountId = nAccountId;
					sMsg.nResult = D2LAccountLogin::E_RESULT_SUCCESS;
					pClientSession->SendMsgToLs(&sMsg, sMsg.GetPackLength());
					// 继续查询角色
					QueryCharacterList(pClientSession, nAccountId, 0);
					return;
				}
			}

			// 将查询到的记录反回到login
			D2LAccountLogin sMsg;
			sMsg.nAccountId = 0;
			sMsg.nResult = D2LAccountLogin::E_RESULT_FAIL;
			pClientSession->SendMsg(&sMsg, sMsg.GetPackLength());
		}

		MyCallBack(ClientSession* _pClientSession) :pClientSession(_pClientSession)
		{
		}
	};

	MyCallBack* myCallBack = new MyCallBack(pClientSession);
	
	memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
	SPRINTF(SQL_BUFFER,"SELECT `id` FROM `account_info` WHERE `username`='%s' AND `password`='%s';",pPacket->arrAccount,pPacket->arrPassword);
	SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
	pDB->ExecAsyncSQL(SQL_BUFFER, myCallBack);
	
}

void ProcServerHandler::ReqRoleCreate(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	const L2DRoleCreate* rev = static_cast<const L2DRoleCreate*>(pMsg);
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(rev->nSessionID);
	ASSERT(pClientSession);

	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	ASSERT(pDB);

	struct MyCallBack : public MyDBCallBack
	{
		ClientSession* pClientSession;
		virtual void QueryResult(IDbRecordSet* pSet, int32 nCount)
		{
			DbRoleCreate(pClientSession, pSet);
		}

		MyCallBack(ClientSession* _pClientSession) :pClientSession(_pClientSession)
		{
		}
	};

	MyCallBack* myCallBack = new MyCallBack(pClientSession);

	int32 nServerID = 1;		// 服务器ID  
	int32 nLandMapID = 10001;	// 默认地图 
	int32 nLandX = 10;			// 默认地图位置X 
	int32 nLandY = 20;			// 默认地图位置Y 
	int32 nRed = 100;			// 血上限 
	int32 nBlue = 100;			// 蓝上限 
	int32 nNow = Utility::NowTime();
	//rev->arrName;
	memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
	SPRINTF(SQL_BUFFER,"CALL ZP_CREATE_USER(%d,%lld,'%s',%d,%d,%d,%d,%d,%d,%d,%d);",\
		nServerID,rev->nAccountID,rev->arrName,rev->nType,nLandMapID,nLandX,nLandY,nRed,nBlue,MAX_ROLE_TYPE_COUNT,nNow);

	SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
	pDB->ExecAsyncSQL(SQL_BUFFER, myCallBack);

}

void ProcServerHandler::ReqLoadCharacter(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	const S2DLoadCharacter* packet = static_cast<const S2DLoadCharacter*>(pMsg);
	StUserDataForDp* pUserMem = MemoryManager::Instance()->GetUser(packet->nCharID);
	if(pUserMem == NULL)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Not Found UserMem!");
		return;
	}

	// 发送到场景中去 
	D2SLoadCharacter sMsg;
	sMsg.sUserData.LoadCharacterData(pUserMem->sCharacterTable);
	sMsg.sUserData.LoadQuestData(pUserMem->sQuestTable);

	// 这种方式发送更直接，如果传过来又需要回去的，这更直接 
	sMsg.nSessionID = packet->nSessionID;
	pBaseSession->SendMsg(&sMsg,sMsg.GetPackLength());

	// 不太允许采用下面这种方式，很大可能会出错，比如加载数据不是自己，而是别人的，则就会发到别人的场景中去 
	// 
	// 这种方式也可，如果在更改场景ID，由WS通知不及时或出问题，则会发到上一次场景（这是错误的） 
	//ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(packet->nClientSessionID);
	//if(pClientSession == NULL)
	//{
	//	ASSERT(pClientSession);
	//	return ;
	//}
	//pClientSession->SendMsgToSs(&sMsg,sMsg.GetPackLength());

}

void ProcServerHandler::ReqSaveCharacter(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	const S2DSaveUserAllData* packet = static_cast<const S2DSaveUserAllData* >(pMsg);

	StUserDataForDp* pUserMem = MemoryManager::Instance()->GetUser(packet->nCharID);
	if(pUserMem == NULL)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Not Found UserMem!");
		return;
	}

	const StUserDataForSs& sUserData = packet->sUserData;

	// 如何做到保存部分表 
	memcpy(&pUserMem->sCharacterTable,&sUserData.sCharacterTable,sizeof(sUserData.sCharacterTable));
	memcpy(&pUserMem->sQuestTable,&sUserData.sQuestTable,sizeof(sUserData.sQuestTable));

	MemoryManager::Instance()->Modifyed(packet->nCharID);

}

void ProcServerHandler::ReqSaveMixItemNumber(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	const S2DSaveMixItemNumber* packet = static_cast<const S2DSaveMixItemNumber*>(pMsg);
	// 做简单检查 
	if(packet->nType <= E_MIXITEM_NUMBER_TYPE_NULL || packet->nType >= E_MIXITEM_NUMBER_TYPE_MAX )
	{
		FLOG_ERROR(__FUNCTION__,__LINE__,"ReqSaveMixItemNumber Args Error!");
		return ;
	}

	RsyncTools::Instance()->SaveMixItemNumber(packet->nType,packet->nValue);
}

//------------------------------db回调----------------------------------------

void ProcServerHandler::QueryCharacterList(ClientSession* pClientSession,int64 nAccountID/* = 0 */, int64 nCharacterID/* = 0*/)
{
	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	ASSERT(pDB);

	struct MyCallBack : public MyDBCallBack
	{
		ClientSession* pClientSession;
		virtual void QueryResult(IDbRecordSet* pSet, int32 nCount)
		{
			const DbRecordSet* pRecordSet = static_cast<const DbRecordSet*>(pSet);
			D2LCharacterList sMsg;
			sMsg.nCount = 0;
			for (int32 i = 0; i < pRecordSet->Rows(); ++i)
			{
				if (i < MAX_ROLE_TYPE_COUNT)
				{
					memcpy(&sMsg.arrInfo[i], pRecordSet->GetRecordData(i), sizeof(D2LCharacterList::StCharacterInfo));
					sMsg.nCount++;
				}
				else
				{
					FLOG_ERROR(__FUNCDNAME__, __LINE__, "Role number too more!");
				}
			}
			pClientSession->SendMsgToLs(&sMsg, sMsg.GetPackLength());
		}

		MyCallBack(ClientSession* _pClientSession) :pClientSession(_pClientSession)
		{
		}
	};

	MyCallBack* myCallBack = new MyCallBack(pClientSession);

	memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
	if(nAccountID > 0 && nCharacterID > 0)
		SPRINTF(SQL_BUFFER,"SELECT `char_id`,`server_id`,`account_id`,`name`,`type`,`level`,`land_mapid`,`land_x`,`land_y`,`red`,`blue` FROM `user_info` WHERE `account_id`=%lld AND `char_id`=%lld;",nAccountID,nCharacterID);
	if(nAccountID > 0)
		SPRINTF(SQL_BUFFER,"SELECT `char_id`,`server_id`,`account_id`,`name`,`type`,`level`,`land_mapid`,`land_x`,`land_y`,`red`,`blue` FROM `user_info` WHERE `account_id`=%lld;",nAccountID);
	else if(nCharacterID > 0)
		SPRINTF(SQL_BUFFER,"SELECT `char_id`,`server_id`,`account_id`,`name`,`type`,`level`,`land_mapid`,`land_x`,`land_y`,`red`,`blue` FROM `user_info` WHERE `char_id`=%lld;",nCharacterID);
	else
		return;
	SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
	pDB->ExecAsyncSQL(SQL_BUFFER, myCallBack);
}

void ProcServerHandler::DbRoleCreate(void* pSession,const void* pData)
{

#pragma pack(push,1)
	struct StCreateResult
	{
		int64 nNewCharID;
		int32 nAccountID;
	};
#pragma pack(pop)

	ClientSession* pClientSession = static_cast<ClientSession*>(pSession);
	const DbRecordSet* pRecordSet = static_cast<const DbRecordSet*>(pData);
	const StCreateResult* pCreateResult = static_cast<const StCreateResult*>(pRecordSet->GetRecordData(0));
	int64  nNewID = pCreateResult->nNewCharID;
	int32  nAccountID = pCreateResult->nAccountID;

	D2LRoleCreateResult sMsg;
	if(nNewID == 0)
	{
		sMsg.nResult = D2LRoleCreateResult::E_FAIL_SYNC;
	}else if(nNewID == 1)
	{
		sMsg.nResult = D2LRoleCreateResult::E_FAIL_ROLE_MAX;
	}
	else if (nNewID == 2)
	{
		sMsg.nResult = D2LRoleCreateResult::E_FAIL_NAME_EXIST;
	}
	else if (nNewID == 3)
	{
		sMsg.nResult = D2LRoleCreateResult::E_FAIL_INSERT_FAIL;
	}else
	{
		sMsg.nNewCharID = nNewID;
		sMsg.nResult = D2LRoleCreateResult::E_SUCCESS;
		QueryCharacterList(pClientSession,nAccountID,0);
	}
	pClientSession->SendMsgToLs(&sMsg,sMsg.GetPackLength());

}