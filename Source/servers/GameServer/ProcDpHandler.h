#ifndef _PROC_DP_HANDLER_H_
#define _PROC_DP_HANDLER_H_

#include "BaseDefine.h"
#include "BaseSingle.h"

class BaseSession;
struct NetMsgHead;


class ProcDpHandler : public BaseSingle<ProcDpHandler>
{
public:
	ProcDpHandler();
	~ProcDpHandler();

	void RevLoadUserData(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	void RtSaveCallBack(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize);

};


#endif

