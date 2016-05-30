#include "GameServer_PCH.h"
#include "TemporaryScene.h"

#include "SharedIncludes.h"

#include "Base.h"
#include "SceneMap.h"

#include "SceneUser.h"
#include "SceneUserManager.h"



TemporaryScene::TemporaryScene(void)
{
}

TemporaryScene::~TemporaryScene(void)
{
}

bool TemporaryScene::PreEnterUser(int32 nSceneID, int32 nPram0, int32 nPram1, int32 nPram2)
{


	return true;
}

void TemporaryScene::EnterUser(int32 nCSID,int64 nCharID,int32 nSceneID,int32 nDpServerID,int32 nFepServerID\
	,int32 nStep, bool bCrossSs)
{
	StTempUserInfo* pTempUser = GetTempUserInfo(nCSID);
	if(pTempUser)
	{
		// 可能在查询中...处理快速双击问题 
		int64 nNow = Utility::MicroTime();
		if( nNow - pTempUser->nReqTime < 500)
		{
			FLOG_WARRING(__FUNCTION__,__LINE__,"TempUser Enter Time < 500ms");
			return;
		}
		RemoveTempUser(nCSID);
	}

	if(pTempUser == NULL)
	{
		StTempUserInfo sTempUser(nCSID,nCharID,nSceneID);
		sTempUser.nDpServerID = nDpServerID;
		sTempUser.nFepServerID = nFepServerID;
		sTempUser.nReqTime = Utility::MicroTime();
		sTempUser.nStep = nStep;
		sTempUser.bCrossSs = bCrossSs;
		m_mapUserInfo.insert(std::make_pair(nCSID,sTempUser));
		pTempUser = GetTempUserInfo(nCSID);
	}

	if(!DbLoadData(nCSID,NULL))
	{
		RemoveTempUser(nCSID);
	}
}

void TemporaryScene::EnterUserLocal(int32 nCSID, int64 nCharID, int32 nSceneID)
{

	SceneUser* pUser = SceneUserManager::Instance()->GetUserByCSID(nCSID);
	if (NULL == pUser)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "EnterUserLocal Not Found SceneUser");
		return;
	}

	ClientSession* pCSession = pUser->GetClientSession();
	if (NULL == pCSession)
	{
		FLOG_ERROR(__FUNCTION__, __LINE__, "EnterUserLocal ClientSession Is NULL");
		return;
	}

	int32 nDpServerID = pCSession->GetDpSession()->ServerID();
	int32 nFepServerID = pCSession->GetFepSession()->ServerID();

	EnterUser(nCSID,nCharID,nSceneID, nDpServerID, nFepServerID, StTempUserInfo::E_STEP_ENTER_SCENE);

}

StTempUserInfo* TemporaryScene::GetTempUserInfo(int32 nCSID)
{
	std::map<int32,StTempUserInfo>::iterator it = m_mapUserInfo.find(nCSID);
	if(it != m_mapUserInfo.end())
	{
		return &(it->second);
	}
	return NULL;
}

void TemporaryScene::RemoveTempUser(int32 nCSID)
{
	std::map<int32,StTempUserInfo>::iterator it = m_mapUserInfo.find(nCSID);
	if(it != m_mapUserInfo.end())
	{
		m_mapUserInfo.erase(it);
	}
}

bool TemporaryScene::DbLoadData(int32 nCSID,const void* data)
{

	StTempUserInfo* pTempUser = GetTempUserInfo(nCSID);
	if(pTempUser == NULL)
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"Not Found StTempUserInfo!");
		return false;
	}

	ServerSession* dpSession = ServerSessionMgr::Instance()->GetSession(pTempUser->nDpServerID);
	if(dpSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__,__LINE__,"Not Found Dp Server");
		return false;
	}

	ServerSession* fepSession = ServerSessionMgr::Instance()->GetSession(pTempUser->nFepServerID);
	if(fepSession == NULL)
	{
		FLOG_ERROR(__FUNCTION__,__LINE__,"Not Found Fep Server");
		return false;
	}

	switch (pTempUser->nStep)
	{
	case StTempUserInfo::E_STEP_SQL_CHARACTER:
		{
			pTempUser->nStep = StTempUserInfo::E_STEP_REV_CHARACTER;
			S2DLoadCharacter sMsgLoad;
			sMsgLoad.nSessionID = nCSID;
			sMsgLoad.nCharID = pTempUser->nCharID;
			dpSession->SendMsg(&sMsgLoad,sMsgLoad.GetPackLength());
			return true;
		}
		break;
	case StTempUserInfo::E_STEP_REV_CHARACTER:
		{
			const StUserDataForSs* pUserData = static_cast<const StUserDataForSs*>(data);
			int32 nMapID = pUserData->sCharacterTable.nLandMapId;
			if(nMapID != pTempUser->nSceneID)
			{
				FLOG_ERROR(__FUNCTION__,__LINE__,"mapid != enter sceneid!");
				ASSERT(0);
				return false;
			}

			ClientSession* pClientSession = ClientSessionMgr::Instance()->AddSession(nCSID);
			if(pClientSession == NULL)
			{
				FLOG_ERROR(__FUNCTION__,__LINE__,"Add ClientSession Fail!");
				return false;
			}
			SceneUser* pCharacter = SceneUserManager::Instance()->AddUser(nCSID,pTempUser->nCharID,pClientSession,pUserData);
			if(pCharacter == NULL)
			{
				// 通知创建失败 
				return false;
			}

			// 创建角色内存数据，检查进入场景 
			pTempUser->nStep = StTempUserInfo::E_STEP_ENTER_SCENE;

			return DbLoadData(nCSID,NULL);

		}
		break;
	case StTempUserInfo::E_STEP_ENTER_SCENE:
		{
			SceneUser* pCharacter = SceneUserManager::Instance()->GetUserByUID(pTempUser->nCharID);
			if(pCharacter == NULL)
			{
				FLOG_WARRING(__FUNCTION__,__LINE__,"GetCharacter fail!");
				return false;
			}
			int32 nSceneID = pCharacter->GetCurrSceneID();
			if(nSceneID != pTempUser->nSceneID) 
			{
				// 加载到的Character数据是旧的，或者是原场景保存数据失败，需要取消本次的进入 
				// 原场景必须确保已经将数据保存成功后才能请求场景跳转 
				FLOG_WARRING(__FUNCTION__,__LINE__,"SceneID Not Sync!");
				return false;
			}

			// 再次检查是否可以进入该场景 
			SceneMap* pSceneMap = SceneMapManager::Instance()->GetSceneMap(nSceneID);
			if(pSceneMap == NULL)
			{
				FLOG_ERROR(__FUNCTION__,__LINE__,"Not SceneMap In The Server , SceneID:%d",nSceneID);
				ASSERT(pSceneMap);
				return false;
			}

			BaseScene* pScene = pSceneMap->GetBaseScene();
			if(pScene == NULL)
			{
				FLOG_ERROR(__FUNCTION__,__LINE__,"Dst Scene == NULL");
				return false;
			}


			ServerSession* wsSession = ServerSessionMgr::Instance()->GetWsSession();

			if (pTempUser->bCrossSs)
			{
				pCharacter->GetClientSession()->SetForLocalWs1(fepSession, NULL, dpSession);
				pCharacter->GetClientSession()->SetForRemoteWs(wsSession);
				pCharacter->GetClientSession()->SetOnServerType(ESERVER_TYPE_SS);
			}


			bool bResult = pScene->Enter(pCharacter);
			if (!bResult)
			{
				FLOG_WARRING(__FUNCTION__, __LINE__, "Enter Scene Fail!");
				
				if (pTempUser->bCrossSs) // 跨场景，返回回调 
				{
					pCharacter->SaveData(NULL);
					SceneUserManager::Instance()->RemoveUser(pCharacter->GetUid());
					// 返回通知进入结果 
					S2WEnterSceneResult sMsg;
					sMsg.nResult = S2WEnterSceneResult::E_ENTER_FAIL;
					sMsg.nSceneID = nSceneID;
					sMsg.nCross = 1;
					wsSession->SendMsg(&sMsg, sMsg.GetPackLength());
				}
				else // 本地 
				{
					// 提示进入失败 TODO


				}
				return false;
			}

			// 设置前端繁忙，所有数据都不能发给前端 
			pCharacter->SetClientBusy();
		
			// 添加到场景管理器中
			SceneMapManager::Instance()->AddSceneEnity(nSceneID,*pCharacter);

			// 通知WS已经进入到场景中 
			// WS更新自己的sid  
			// SS更新fep,dp中的sid 
			S2WEnterSceneResult sMsg;
			sMsg.nResult = S2WEnterSceneResult::E_ENTER_SUCCESS;
			sMsg.nSceneID = nSceneID;
			pCharacter->GetClientSession()->SendMsgToWs(&sMsg,sMsg.GetPackLength());

			ServerInfoOpt& infoOpt = NetServerOpt::Instance()->GetLocalServerInfo();

			SSSessionNofitySInfo sNofityInfo;
			sNofityInfo.nSessionID = nCSID;
			sNofityInfo.nSsServerID = infoOpt.nID; // fepSession->ServerID();
			sNofityInfo.nDpServerID = dpSession->ServerID();
			sNofityInfo.nFepServerID = fepSession->ServerID();

			pCharacter->GetClientSession()->SendMsgToDp(&sNofityInfo,sNofityInfo.GetPackLength());
			pCharacter->GetClientSession()->SendMsgToFep(&sNofityInfo,sNofityInfo.GetPackLength());
	
			// 进入场景成功，删除临时数据  
			RemoveTempUser(nCSID);

			// 通知Client场景信息 
			S2CSceneEnterInfo sSceneInfo;
			sSceneInfo.nSceneID = nSceneID;
			pCharacter->GetClientSession()->SendMsgToFep(&sSceneInfo,sSceneInfo.GetPackLength());

			return true;

		}
		break;
	}

	return true;

}


