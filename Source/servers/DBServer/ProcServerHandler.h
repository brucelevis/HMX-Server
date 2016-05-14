#ifndef __PROC_SERVER_HANDLER_H_
#define __PROC_SERVER_HANDLER_H_

#include "SharedIncludes.h"
#include "../DyNetMysql/DbRecordSet.h"

class BaseSession;
class ClientSession;
struct NetMsgHead;

/*
 *	业务逻辑 
 */
class ProcServerHandler : public BaseSingle<ProcServerHandler> 
{
public:
	ProcServerHandler(void);
	~ProcServerHandler(void);

	// 加载所有的昵称 
	void ReqNamesList(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	// client登录查询 
	void ReqAccountLogin(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	// 创建角色 
	void ReqRoleCreate(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	// ss 中加载角色数据 
	void ReqLoadCharacter(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	// ss 中保存数据 
	void ReqSaveCharacter(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqSaveMixItemNumber(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);


	//------------------db回调函数--------------------
public:

	// 创建角色
	static void DbRoleCreate(void* pSession,const void* pData);

	static void QueryCharacterList(ClientSession* pClientSession,int64 nAccountID = 0 , int64 nCharacterID = 0);
	
};


#endif

