#include "ProcClientHandler.h"


#include "CommonC2S.h"
#include "CommonS2S.h"
#include "ClientSession.h"

#include "NamesManager.h"



ProcClientHandler::ProcClientHandler(void)
{
}


ProcClientHandler::~ProcClientHandler(void)
{
}

void ProcClientHandler::ReqAccountLogin(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

	const C2LAccountLogin* pPacket = static_cast<const C2LAccountLogin*>(pMsg);

	// ����ַ����Ƿ�Ϸ� TODO

	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacket->nClientSessionID);
	ASSERT(pClientSession);

	L2DAccountLogin sMsg;
	strcpy(sMsg.arrAccount,pPacket->arrAccount);
	strcpy(sMsg.arrPassword,pPacket->arrPassword);

	pClientSession->SendMsgToDp(&sMsg,sMsg.GetPackLength());
	
}

void ProcClientHandler::ReqRoleCreate(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

	// ���͵�db���󴴽� 
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pMsg->nClientSessionID);
	ASSERT(pClientSession);

	const C2LRoleCreate* pPacket = static_cast<const C2LRoleCreate*>(pMsg);

	// ������� 
	if(pPacket->nType <= E_ROLE_TYPE_NULL || pPacket->nType >= E_ROLE_TYPE_MAX)
	{
		return ;
	}

	// ������� 
	if(pPacket->arrName[MAX_NICK_LENGTH - 1] != '\0')
	{
		return ;
	}

	L2DRoleCreate sMsg;
	sMsg.nType = pPacket->nType;
	sMsg.nAccountID = pPacket->nAccountID;
	memcpy(sMsg.arrName,pPacket->arrName,MAX_NICK_LENGTH);

	pClientSession->SendMsgToDp(&sMsg,sMsg.GetPackLength());

}

void ProcClientHandler::ReqRandNames(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	// Login�б������Ѿ����������е��ǳƣ��������ѡ������Դ�û��ʹ�ù����ǳ� 

	const C2LRandNames* packet = static_cast<const C2LRandNames*>(pMsg);
	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(packet->nClientSessionID);
	ASSERT(pClientSession);

	L2CNamesList sMsg;
	sMsg.nCount = 0;
	for (int32 i = 0 ; i < 10; ++i)
	{
		static char arrName[32];
		memset(arrName,0,sizeof(arrName));
		if(NamesManager::Instance()->GetRandTtems(arrName))
		{
			memcpy(sMsg.arrNames[i],arrName,sizeof(arrName));
			sMsg.nCount++;
		}else
		{
			break;
		}
	}
	
	pClientSession->SendMsg(&sMsg,sMsg.GetPackLength());

}
