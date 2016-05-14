#ifndef _DP_SERVER_PROC_HANDLER_H_
#define _DP_SERVER_PROC_HANDLER_H_

#include "BaseSingle.h"
#include "BaseProcHandler.h"

class ProcDpHandler : public BaseProcHandler, public BaseSingle<ProcDpHandler>
{
public:
	ProcDpHandler();
	~ProcDpHandler();


	/*--------------------------------------------
	 *  @brief  : 
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void ReqGlobalInfo(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);


	/*--------------------------------------------
	 *  @brief  :	
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void RepSelectRoleResult(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

};

#endif



