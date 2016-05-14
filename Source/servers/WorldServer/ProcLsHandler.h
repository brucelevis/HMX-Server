#ifndef _LOGIN_SERVER_PROC_HANDLER_H_
#define _LOGIN_SERVER_PROC_HANDLER_H_

#include "BaseSingle.h"
#include "BaseProcHandler.h"

class BaseSession;
struct NetMsgHead;

class ProcLsHandler : public BaseProcHandler, public BaseSingle<ProcLsHandler>
{
public:
	ProcLsHandler();
	~ProcLsHandler();

};




#endif

