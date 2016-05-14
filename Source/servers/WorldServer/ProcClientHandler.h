#ifndef __WS_CLIENT_PROC_HANDLER_H_
#define __WS_CLIENT_PROC_HANDLER_H_

#include "BaseSingle.h"
#include "BaseProcHandler.h"

class BaseSession;
struct NetMsgHead;


/*-------------------------------------------------------------------
 * @Brief : 处理由玩家直接发向WS的请求处理(C2W类型)
 *          很多操作是不允许由C2W请求，处理时需要注意
 *			是不是应该这样做,如果玩家已经在场景中，应该由场景发起。
 *			或者统一由网关发起
 * @Author:hzd 2015:10:21
 *------------------------------------------------------------------*/
class ProcClientHandler : public BaseProcHandler, public BaseSingle<ProcClientHandler>
{
public:
	ProcClientHandler(void);
	~ProcClientHandler(void);

	/*--------------------------------------------
	 *  @brief  : 切换场景请求
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void ReqSwitchScene(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 选择角色，当前选择角色成功后，DB
	 *			  才会保存该玩家应该加载的内存
	 *  @input	: 这也代表该玩家会进入到场景中去
	 *  @return :	
	 *-------------------------------------------*/
	void ReqSelectRole(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

};


#endif




