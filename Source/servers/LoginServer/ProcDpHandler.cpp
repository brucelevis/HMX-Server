#include "ProcDpHandler.h"


#include "CommonS2S.h"
#include "CommonS2C.h"
#include "ClientSession.h"
#include "LogManager.h"
#include "ServerSession.h"
#include "LoginAccountMgr.h"
#include "NamesManager.h"

ProcDpHandler::ProcDpHandler(void)
{
}


ProcDpHandler::~ProcDpHandler(void)
{
}

void ProcDpHandler::RepNamesList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const D2LNamesList* packet = static_cast<const D2LNamesList*>(pMsg);
	for (int32 i = 0 ; i < packet->nCount; ++i)
	{
		NamesManager::Instance()->LoadItems(packet->arrNameList[i].arrName);
	}
}

void ProcDpHandler::AccountLogin(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

	const D2LAccountLogin* pPacker = static_cast<const D2LAccountLogin*>(pMsg);

	if(ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacker->nSessionID))
	{

		L2CAccountLogin sMsg;
		switch (pPacker->nResult)
		{
		case D2LAccountLogin::E_RESULT_SUCCESS:
			{
				pClientSession->SetStatus(E_CLIENT_STATUS_LOGINED);

				// ���ص�¼�ɹ���Ϣ 
				sMsg.nAccountId = pPacker->nAccountId;
				sMsg.nResult = L2CAccountLogin::E_RESULT_SUCCESS;
			}
			break;

		case D2LAccountLogin::E_RESULT_FAIL:
			{
				// ���ص�¼ʧ����Ϣ 
				sMsg.nResult = L2CAccountLogin::E_RESULT_FAIL;
			}	
			break;
		default:
			{
				return;
			}
			break;
		}

		pClientSession->SendMsgToFep(&sMsg,sMsg.GetPackLength());

		// �Ƿ���¼��Ƶ����¼��� 
		StAccountInfo* pAccountInfo = LoginAccountMgr::Instance()->GetAccountInfo(pPacker->nSessionID);
		if(!pAccountInfo)
		{
			pAccountInfo = LoginAccountMgr::Instance()->AddAccountInfo(pPacker->nSessionID);
			if(!pAccountInfo)
			{
				return;
			}
		}
		pAccountInfo->AddLoginTimes();
		if(pAccountInfo->CheckLoginIllegal())
		{
			// �������� TODO 

			// ɾ���ü�¼
			LoginAccountMgr::Instance()->RemoveAccountInfo(pPacker->nSessionID);
			return ;
		}
	}	
}

void ProcDpHandler::CharacterList(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

	const D2LCharacterList* pPacket = static_cast<const D2LCharacterList*>(pMsg);
	if(ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacket->nSessionID))
	{

		if(pClientSession->Status() != E_CLIENT_STATUS_LOGINED)
		{

			return ; // �ǵ�¼���״̬�����ܻ�ý�ɫ�б� TODO 
		}

		L2CCharacterList sMsg;
		sMsg.nCount = pPacket->nCount;
		for (int32 i = 0; i < pPacket->nCount; ++i)
		{
			sMsg.arrInfo[i].nCharID = pPacket->arrInfo[i].nCharID;
			sMsg.arrInfo[i].nLevel = pPacket->arrInfo[i].nLevel;
			strcpy(sMsg.arrInfo[i].arrName,pPacket->arrInfo[i].arrName);
		}
		pClientSession->SendMsgToFep(&sMsg,sMsg.GetPackLength());	
	}

}

void ProcDpHandler::RoleCreateResult(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{
	const D2LRoleCreateResult* pPacket = static_cast<const D2LRoleCreateResult*>(pMsg);
	if(ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(pPacket->nSessionID))
	{
		L2CRoleCreateResult sMsg;
		sMsg.nResult = pPacket->nResult;
		sMsg.nNewCharID = pPacket->nNewCharID;
		pClientSession->SendMsgToFep(&sMsg,sMsg.GetPackLength());
	}
}




