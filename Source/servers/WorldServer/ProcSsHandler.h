#ifndef _SCENE_SERVER_MSG_HANDLER_H_
#define _SCENE_SERVER_MSG_HANDLER_H_

#include "BaseSingle.h"
#include "BaseMsgHandler.h"

class ProcSsHandler : public BaseSingle<ProcSsHandler>
{
public:
	ProcSsHandler();
	~ProcSsHandler();

	/*--------------------------------------------
	 *  @brief    :	请求场景注册，当ss登录ws完成后，请求注册它所属于的地图ID
	 *  @input	  :
	 *  @return   :	
	 *-------------------------------------------*/
	void ReqSceneRegister(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief    :	请求场景注销，主动取消场景地图注册
	 *  @input	  :
	 *  @return   :	
	 *-------------------------------------------*/
	void ReqSceneCancel(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	/*--------------------------------------------
	 *  @brief  : 进入场景后返回的结果 
	 *  @input	: 
	 *  @return :	
	 *-------------------------------------------*/
	void RepEnterSceneResult(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize);

	void RepChangeScene(BaseSession* pSession, const NetMsgHead* pMsg, int32 nSize);
};

#endif


