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
	 *  @brief  : �ʺŵ�¼�ķ���
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void AccountLogin(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : ��ɫ���ݷ��أ�ֻ�е��ʺ�������ȷ
	 *  @input	: �Ż��з��أ�����ж���dp��
	 *  @return :	
	 *-------------------------------------------*/
	void CharacterList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : ������ɫ�������
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void RoleCreateResult(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);
};

#endif



