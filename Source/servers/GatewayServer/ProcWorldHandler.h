#ifndef _PROC_WORLD_HANDLER_H_
#define _PROC_WORLD_HANDLER_H_

#include "BaseDefine.h"
#include "BaseSingle.h"

class BaseSession;
struct NetMsgHead;

/*-------------------------------------------------------------------
 * @Brief :FEP��Ϊ�ͻ��ˣ�������WS�߼�����Handler 
 * 
 * @Author:hzd 2013/04/03
 *------------------------------------------------------------------*/
class ProcWorldHandler : public BaseSingle<ProcWorldHandler>
{
public:
	ProcWorldHandler();
	~ProcWorldHandler();

	void ReqLogin(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize);
};


#endif

