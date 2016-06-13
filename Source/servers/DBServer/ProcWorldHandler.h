#ifndef _PROC_WORLD_HANDLER_H_
#define _PROC_WORLD_HANDLER_H_

#include "SharedIncludes.h"

class BaseSession;
class ClientSession;
struct NetMsgHead;


void TestDb(void* pUser,const void* pMsg);

class ProcWorldHandler : public BaseSingle<ProcWorldHandler>
{
public:
	ProcWorldHandler();
	~ProcWorldHandler();

	void ReqLogin(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqNotifyServerInfo(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqNotifySceneInfo(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqQueryEventInfo(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqSelectRole(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);

	//////////////////////////////////////////////////////////////////////////

	void CallBackSelectRole(int32 nCSID, StCharacterTable* pUserDataMemory);
	

};


#endif

