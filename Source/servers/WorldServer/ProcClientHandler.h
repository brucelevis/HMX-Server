#ifndef __WS_CLIENT_PROC_HANDLER_H_
#define __WS_CLIENT_PROC_HANDLER_H_

#include "BaseSingle.h"
#include "BaseProcHandler.h"

class BaseSession;
struct NetMsgHead;


/*-------------------------------------------------------------------
 * @Brief : ���������ֱ�ӷ���WS��������(C2W����)
 *          �ܶ�����ǲ�������C2W���󣬴���ʱ��Ҫע��
 *			�ǲ���Ӧ��������,�������Ѿ��ڳ����У�Ӧ���ɳ�������
 *			����ͳһ�����ط���
 * @Author:hzd 2015:10:21
 *------------------------------------------------------------------*/
class ProcClientHandler : public BaseProcHandler, public BaseSingle<ProcClientHandler>
{
public:
	ProcClientHandler(void);
	~ProcClientHandler(void);

	/*--------------------------------------------
	 *  @brief  : �л���������
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void ReqSwitchScene(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : ѡ���ɫ����ǰѡ���ɫ�ɹ���DB
	 *			  �Żᱣ������Ӧ�ü��ص��ڴ�
	 *  @input	: ��Ҳ�������һ���뵽������ȥ
	 *  @return :	
	 *-------------------------------------------*/
	void ReqSelectRole(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

};


#endif




