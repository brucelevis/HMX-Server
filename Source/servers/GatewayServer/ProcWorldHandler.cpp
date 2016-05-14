#include "ProcWorldHandler.h"
#include "SharedIncludes.h"
#include "LogManager.h"
#include "ServerSession.h"


ProcWorldHandler::ProcWorldHandler()
{
}


ProcWorldHandler::~ProcWorldHandler()
{
}


void ProcWorldHandler::ReqLogin(BaseSession* pBaseSession, const NetMsgHead* pMsg,int32 nSize)
{

	if(pBaseSession->Status() != E_SERVER_STATUS_CONNETED)
	{
//		FLOG_INFO("pBaseSession->Status() != E_SERVER_STATUS_CONNETED");
		return;
	}

	// 判断 
	pBaseSession->SetStatus(E_SERVER_STATUS_LOGINED);

	// ok finish!
	// 等待其它 ls,ss的连接  

}

