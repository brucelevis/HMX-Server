#include "ProcDpHandler.h"
#include "CommonW2S.h"
#include "CommonS2S.h"
#include "LogManager.h"
#include "ServerSession.h"
#include "SceneUser.h"
#include "SceneUserManager.h"
#include "TemporaryScene.h"

#include "SceneMap.h"

#include "google/protobuf/io/coded_stream.h"
#include "character.pb.h"

ProcDpHandler::ProcDpHandler()
{
}


ProcDpHandler::~ProcDpHandler()
{
}

void ProcDpHandler::RevLoadUserData(BaseSession* pSession, const NetMsgHead* pMsg,int32 nSize)
{

	const D2SLoadCharacter* packet = static_cast<const D2SLoadCharacter*>(pMsg);

	::protobuf::Character character;
	character.ParseFromArray((::google::protobuf::uint8*)packet->arrByte, packet->nByteSize);

	ClientSession* pCSession = ClientSessionMgr::Instance()->GetSession(packet->nSessionID);
	if (pCSession == NULL)
	{
		pCSession = ClientSessionMgr::Instance()->AddSession(packet->nSessionID);
	}
	else
	{
		// ������Ҫע���ʼ����״̬֮��� 
	}

	// ��ʼ���뵽�ڴ��� 
	SceneUser* pUser = SceneUserManager::Instance()->AddUser(pCSession,character);
	if (pUser == NULL)
	{
		ASSERT(pUser);
		return ;
	}

	ServerSession* dpSession = ServerSessionMgr::Instance()->GetSession(packet->nDpServerID);
	if (dpSession == NULL)
	{
		FLOG_ERROR("Not Found Dp Server");
		return ;
	}

	ServerSession* fepSession = ServerSessionMgr::Instance()->GetSession(packet->nFepServerID);
	if (fepSession == NULL)
	{
		FLOG_ERROR("Not Found Fep Server");
		return ;
	}

	ServerSession* wsSession = ServerSessionMgr::Instance()->GetWsSession();
	if (wsSession == NULL)
	{
		FLOG_ERROR("Not Found Ws Server");
		return;
	}

	pUser->GetClientSession()->SetForLocalWs1(fepSession, NULL, dpSession);
	pUser->GetClientSession()->SetForRemoteWs(wsSession);
	pUser->GetClientSession()->SetOnServerType(ESERVER_TYPE_SS);
	
	// ����ǰ�˷�æ���������ݶ����ܷ���ǰ�� 
	pUser->SetClientStatus(1);

	int32 nSceneID = pUser->GetCurrSceneID();

	// ��ӵ������������� 
	SceneMapManager::Instance()->AddSceneEnity(nSceneID,*pUser);

	// ֪ͨWS�Ѿ����뵽������ 
	S2WEnterSceneResult sMsg;
	sMsg.nResult = S2WEnterSceneResult::E_ENTER_SUCCESS;
	sMsg.nSceneID = nSceneID;
	sMsg.nEnterType = packet->nEnterType;
	pUser->GetClientSession()->SendMsgToWs(&sMsg,sMsg.GetPackLength());

	ServerInfoOpt& infoOpt = NetServerOpt::Instance()->GetLocalServerInfo();

	SSSessionNofitySInfo sNofityInfo;
	sNofityInfo.nSessionID = packet->nSessionID;
	sNofityInfo.nSsServerID = infoOpt.nID; // fepSession->ServerID();
	sNofityInfo.nDpServerID = dpSession->ServerID();
	sNofityInfo.nFepServerID = fepSession->ServerID();

	pUser->GetClientSession()->SendMsgToDp(&sNofityInfo,sNofityInfo.GetPackLength());
	pUser->GetClientSession()->SendMsgToFep(&sNofityInfo,sNofityInfo.GetPackLength());

	// ֪ͨClient������Ϣ 
	S2CSceneEnterInfo sSceneInfo;
	sSceneInfo.nSceneID = nSceneID;
	pUser->GetClientSession()->SendMsgToFep(&sSceneInfo,sSceneInfo.GetPackLength());

	printf("[INFO]:Get Character Is Ok....\n");

}


