#ifndef _FEP_SERVER_PROC_HANDLER_H_
#define _FEP_SERVER_PROC_HANDLER_H_

#include "BaseSingle.h"
#include "BaseProcHandler.h"

class BaseSession;
struct NetMsgHead;

class ProcFepHandler : public BaseProcHandler, public BaseSingle<ProcFepHandler>
{
public:
	ProcFepHandler();
	~ProcFepHandler();


};



#endif

