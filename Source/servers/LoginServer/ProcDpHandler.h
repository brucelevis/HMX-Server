#ifndef __PROC_DP_HANDLER_H_
#define __PROC_DP_HANDLER_H_

#include "BaseDefine.h"
#include "BaseSingle.h"

class BaseSession;
struct NetMsgHead;


class ProcDpHandler : public BaseSingle<ProcDpHandler>
{
public:
	ProcDpHandler(void);
	~ProcDpHandler(void);


	void RepNamesList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 帐号登录的返回
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void AccountLogin(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 角色数据返回，只有当帐号密码正确
	 *  @input	: 才会有返回，这个判断在dp中
	 *  @return :	
	 *-------------------------------------------*/
	void CharacterList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 创建角色结果返回
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void RoleCreateResult(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);
};

#endif



