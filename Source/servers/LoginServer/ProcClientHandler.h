#ifndef __PROC_CLIENT_HANDLER_H_
#define __PROC_CLIENT_HANDLER_H_

#include "BaseDefine.h"
#include "BaseSingle.h"

class BaseSession;
struct NetMsgHead;

class ProcClientHandler : public BaseSingle<ProcClientHandler>
{
public:
	ProcClientHandler(void);
	~ProcClientHandler(void);

	void ReqAccountLogin(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqRoleCreate(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqRandNames(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);
};

#endif

