#include "ProcServerHandler.h"

#include "Config.h"
#include "RsyncTools.h"
#include "DataRsyncStruct.h"
#include "MemoryManger.h"

#include "../DyNetMysql/DbIncludes.h"
#include "../DyNetSocket/NetIncludes.h"
#include "../Shared/CommonDef/SharedIncludes.h"

#include "character.pb.h"
#include "google/protobuf/io/coded_stream.h"

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

	struct MyCallBack : public DBQueryFunc
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
	static const dbCol namelist_fields[] = { {"name",DB_STR,32},{NULL,0,0} };
	pDB->ExecSelectAsync("names_info", namelist_fields, NULL, NULL, myCallBack);


}

void ProcServerHandler::ReqAccountLogin(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	const L2DAccountLogin* packet = static_cast<const L2DAccountLogin*>(pMsg);

	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(packet->nSessionID);
	ASSERT(pClientSession);

	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	ASSERT(pDB);

	struct MyCallBack : public DBQueryFunc
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
			pClientSession->SendMsgToLs(&sMsg, sMsg.GetPackLength());
		}

		MyCallBack(ClientSession* _pClientSession) :pClientSession(_pClientSession)
		{
		}
	};

	MyCallBack* myCallBack = new MyCallBack(pClientSession);
	memset(SQL_BUFFER, 0, MAX_SQL_BUFFER);
	SPRINTF(SQL_BUFFER, "`username`='%s' AND `password`='%s';", packet->arrAccount, packet->arrPassword);
	SQL_BUFFER[MAX_SQL_BUFFER - 1] = '\0';
	static const dbCol login_fields[] = { { "id",DB_QWORD,64 },{ NULL,0,0 } };
	pDB->ExecSelectAsync("account_info", login_fields, SQL_BUFFER, NULL, myCallBack);
	
}

void ProcServerHandler::ReqRoleCreate(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	const L2DRoleCreate* rev = static_cast<const L2DRoleCreate*>(pMsg);
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(rev->nSessionID);
	ASSERT(pClientSession);

	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	ASSERT(pDB);

	struct MyCallBack : public DBQueryFunc
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
	pDB->ExecSQLAsync(SQL_BUFFER, myCallBack);

}

void ProcServerHandler::ReqLoadCharacter(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	const S2DLoadCharacter* packet = static_cast<const S2DLoadCharacter*>(pMsg);
	StCharacterTable* pUserData = DatabaseManager::GetCharacterMemory().GetData(packet->nCharacterID);
	if (pUserData == NULL)
	{
		FLOG_WARRING("Not Found User Mem!");
		return;
	}

	// 发送到场景中去 

	::protobuf::Character charbuf;
	charbuf.set_db_id(0);
	charbuf.set_char_id(pUserData->nCharID);
	charbuf.set_server_id(pUserData->nServerID);
	charbuf.set_account_id(pUserData->nAccountID);
	charbuf.set_name(pUserData->arrName);
	charbuf.set_type(pUserData->nType);
	charbuf.set_exp(pUserData->nExp);
	charbuf.set_level(pUserData->nLevel);
	charbuf.set_land_mapid(pUserData->nLandMapId);
	charbuf.set_land_x(pUserData->nLandX);
	charbuf.set_land_y(pUserData->nLandY);
	charbuf.set_instance_mapid(pUserData->nInstanceMapId);
	charbuf.set_instance_x(pUserData->nInstanceX);
	charbuf.set_instance_y(pUserData->nInstanceY);
	charbuf.set_red(pUserData->nRed);
	charbuf.set_blue(pUserData->nBlue);

	D2SLoadCharacter charMsg;
	charMsg.nSessionID = packet->nSessionID;
	charMsg.nDpServerID = packet->nDpServerID;
	charMsg.nFepServerID = packet->nFepServerID;
	charMsg.nEnterType = packet->nEnterType;
	charMsg.nByteSize = charbuf.ByteSize();
	charbuf.SerializeToArray((::google::protobuf::uint8*)charMsg.arrByte, charMsg.nByteSize);
	pBaseSession->SendMsg(&charMsg, charMsg.GetPackLength());

}

void ProcServerHandler::ReqSaveCharacter(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{
	const S2DSaveCharacter* packet = static_cast<const S2DSaveCharacter*>(pMsg);

	::protobuf::Character character;
	character.ParseFromArray(packet->arrByte, packet->nByteSize);

	int64 nCharID = character.char_id();

	StCharacterTable* data = DatabaseManager::GetCharacterMemory().GetData(nCharID);

	if(data == NULL)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Not Found StCharacterTable char_id:%lld!", nCharID);
		return;
	}

	strncpy(data->arrName, character.name().c_str(), MAX_NICK_LENGTH);
	data->nType = character.type();
	data->nExp = character.exp();
	data->nLevel = character.level();
	data->nLandMapId = character.land_mapid();
	data->nLandX = character.land_x();
	data->nLandY = character.land_y();
	data->nInstanceMapId = character.instance_mapid();
	data->nInstanceX = character.instance_x();
	data->nInstanceY = character.instance_y();
	data->nRed = character.red();
	data->nBlue = character.blue();

	memset(data->binData.revser, 0, sizeof(MAX_BINARY_SIZE));
	const ::protobuf::BinaryData& binData = character.binary_data();
	binData.SerializeToArray(&data->binData.revser, binData.ByteSize());

	DatabaseManager::GetCharacterMemory().Midify(nCharID);
}

void ProcServerHandler::ReqSaveMixItemNumber(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	const S2DSaveMixItemNumber* packet = static_cast<const S2DSaveMixItemNumber*>(pMsg);
	// 做简单检查 
	if(packet->nType <= E_MIXITEM_NUMBER_TYPE_NULL || packet->nType >= E_MIXITEM_NUMBER_TYPE_MAX )
	{
		FLOG_ERROR("ReqSaveMixItemNumber Args Error!");
		return ;
	}

	RsyncTools::Instance()->SaveMixItemNumber(packet->nType,packet->nValue);
}

//------------------------------db回调----------------------------------------

void ProcServerHandler::QueryCharacterList(ClientSession* pClientSession,int64 nAccountID/* = 0 */, int64 nCharacterID/* = 0*/)
{
	IDbBase* pDB = DbConnManager::Instance()->GetMainDB();
	ASSERT(pDB);

	struct MyCallBack : public DBQueryFunc
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
					FLOG_ERROR("Role number too more!");
				}
			}
			pClientSession->SendMsgToLs(&sMsg, sMsg.GetPackLength());
		}

		MyCallBack(ClientSession* _pClientSession) :pClientSession(_pClientSession)
		{
		}
	};

	MyCallBack* myCallBack = new MyCallBack(pClientSession);

	memset(SQL_BUFFER, 0, MAX_SQL_BUFFER);
	if (nAccountID > 0 && nCharacterID > 0)
		SPRINTF(SQL_BUFFER, "`account_id`=%lld AND `char_id`=%lld;", nAccountID, nCharacterID);
	if (nAccountID > 0)
		SPRINTF(SQL_BUFFER, "`account_id`=%lld;", nAccountID);
	else if (nCharacterID > 0)
		SPRINTF(SQL_BUFFER, "`char_id`=%lld;", nCharacterID);
	else
		return;

	SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';

	static const dbCol login_fields[] = {
		{ "char_id",DB_QWORD,8 },
		{ "server_id",DB_DWORD,4 },
		{ "account_id",DB_QWORD,4 },
		{ "name",DB_STR,32 },
		{ "type",DB_DWORD,4 },
		{ "level",DB_DWORD,4 },
		{ NULL,0,0 }
	};

	pDB->ExecSelectAsync("character_info", login_fields,SQL_BUFFER,NULL, myCallBack);
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

	ClientSession* pCSession = static_cast<ClientSession*>(pSession);
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
		QueryCharacterList(pCSession,nAccountID,0);
	}
	pCSession->SendMsgToLs(&sMsg,sMsg.GetPackLength());

}