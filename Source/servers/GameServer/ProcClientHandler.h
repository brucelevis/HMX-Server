#ifndef _PROC_CLIENT_HANDLER_H_
#define _PROC_CLIENT_HANDLER_H_

#include "BaseDefine.h"
#include "BaseSingle.h"

class BaseSession;
struct NetMsgHead;


class ProcClientHandler : public BaseSingle<ProcClientHandler>
{
public:
	ProcClientHandler(void);
	~ProcClientHandler(void);

	void ReqChatWorld(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqSceneData(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	void ReqChangeScene(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize);


	// 
	void ReqQuestAccept(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);
	

};


#endif




