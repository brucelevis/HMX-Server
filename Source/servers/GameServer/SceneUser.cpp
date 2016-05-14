#include "SceneUser.h"
#include "SceneMap.h"
#include "ResourceMgr.h"
#include "def_entity.h"
#include "def_resource.h"

SceneUser::SceneUser(int64 nUserID,ClientSession* pClientSession)
	:Creature(ENTITY_TYPE_PLAYER, 0,true,E_CHANNEL_REGIST_TYPE_POS_B | E_CHANNEL_REGIST_TYPE_CREATURE_B | E_CHANNEL_REGIST_TYPE_SPELL_B | E_CHANNEL_REGIST_TYPE_BUFF_B)
	,m_pClientSession(pClientSession)
	,m_cUserCtrl(this),m_cQuestCtrl(this)
{
	ASSERT(nUserID);
	ASSERT(pClientSession);

	m_bClientReady = 0;
	m_pChangeSceneCallBack = NULL;

	SceneUserInitAttributeOffet();
};

void SceneUser::SceneUserInitAttributeOffet()
{
#define	INIT_ATTRIBUTE_OFFSET(index, name, set, handler)	\
						{\
			m_sAttributeOffset[index].nOffset = &(this->m_sAttribute.name);\
			m_sAttributeOffset[index].nSize = sizeof(this->m_sAttribute.name);\
			m_sAttributeOffset[index].bCanSet = set;\
			m_sAttributeOffset[index].hHandler = handler;\
						}
	// 偏移信息 
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_UID, nUid, false, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_EXP, nExp, true, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_LAND_ID, nLandID, true, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_LAND_X, nLandX, true, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_LAND_Y, nLandY, true, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_INSTANCE_ID, nInstanceID, true, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_INSTANCE_X, nInstanceX, true, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_INSTANCE_Y, nInstanceY, true, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_ROLETYPE, nRoleType, false, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_CLOTHESID, nClothesID, true, NULL);
	INIT_ATTRIBUTE_OFFSET(SCENEUSER_ATTRIBUTE_WEAPONID, nWeaponID, true, NULL);

#undef INIT_ATTRIBUTE_OFFSET

}


bool SceneUser::SetSceneUserAttribute(SceneUserAttributeType eAttributeType, const ValueType& vNewValue)
{
	// 检测是否有偏移值   
	if (m_sAttributeOffset[eAttributeType].nOffset && m_sAttributeOffset[eAttributeType].bCanSet)
	{
		SetSceneUserAttribute(eAttributeType, vNewValue, false);
		return true;
	}
	return false;
}

bool SceneUser::SetSceneUserAttribute(SceneUserAttributeType eAttributeType, const ValueType& vNewValue, bool bEnforce)
{
	// 检测是否有偏移值  
	if (m_sAttributeOffset[eAttributeType].nOffset && (m_sAttributeOffset[eAttributeType].bCanSet || bEnforce))
	{
		// 比较新旧值 
		if (memcmp(m_sAttributeOffset[eAttributeType].nOffset, &vNewValue, m_sAttributeOffset[eAttributeType].nSize))
		{
			// 保存旧值 
			ValueType vOldValue;
			memcpy(&vOldValue, m_sAttributeOffset[eAttributeType].nOffset, m_sAttributeOffset[eAttributeType].nSize);

			// 设置新值 
			memcpy(m_sAttributeOffset[eAttributeType].nOffset, &vNewValue, m_sAttributeOffset[eAttributeType].nSize);

			// 回调 
			if (m_sAttributeOffset[eAttributeType].hHandler)
			{
				m_sAttributeOffset[eAttributeType].hHandler(vOldValue, vNewValue);
			}
		}
		return true;
	}
	return false;
}

bool SceneUser::GetSceneUserAttribute(SceneUserAttributeType i_eAttributeType, ValueType& o_vValue)
{
	// 检测是否有偏移值 
	if (m_sAttributeOffset[i_eAttributeType].nOffset)
	{
		memcpy(&o_vValue, m_sAttributeOffset[i_eAttributeType].nOffset, m_sAttributeOffset[i_eAttributeType].nSize);
		return true;
	}
	return false;
}

int32 SceneUser::GetSceneUserAttributeInt32(SceneUserAttributeType i_eAttributeType)
{
	ValueType o_vValue;
	GetSceneUserAttribute(i_eAttributeType, o_vValue);
	return o_vValue.nInt32;
}

int64 SceneUser::GetSceneUserAttributeInt64(SceneUserAttributeType i_eAttributeType)
{
	ValueType o_vValue;
	GetSceneUserAttribute(i_eAttributeType, o_vValue);
	return o_vValue.nInt64;
}

// 初始化角色基本数据 
void SceneUser::LoadAllData(const StUserDataForSs& rData)
{
	// user数据 
	const StCharacterDataTable& rUserData = rData.sCharacterTable;

	// quest数据
	const StQuestDataTable& rQuestData = rData.sQuestTable;

/*
关于修改数据是用SetXXXAttribute还是用m_sAttribute方式，如果数据可以修改且数据变化会影响其他变化，则用SetXXXAttribute方式
如果数据不可以设置，则用m_sAttribute方式，往往这种是初始化数据，或者特殊情况，
如果又要强行修改，又要影响其他，则用SetXXXAttribute，将第三个参数传入true
*/

/////////////////////////////////基础数据，是不需要保存的/////////////////////////////////////////
	// 加载基础数据 
	if(const StRoleTypeInitCfg* pRoleInfo = ResourceMgr::Instance()->GetRoleTypeInitInfoRes(rUserData.nType))
	{
		SetEntityAttribute(ENTITY_ATTRIBUTE_INVIEWRANGE,pRoleInfo->nInViewRange);
		SetEntityAttribute(ENTITY_ATTRIBUTE_OUTVIEWRANGE,pRoleInfo->nOutViewRange);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_ATTACKTYPE,pRoleInfo->nAttackType);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_SPEED,pRoleInfo->nSpeed);
	}

	if(const StCharacterLevelCfg* pBaseInfo = ResourceMgr::Instance()->GetCharacterLevelBase(rUserData.nType,rUserData.nLevel))
	{
		SetCreatureAttribute(CREATURE_ATTRIBUTE_REDMAX,pBaseInfo->nRedMax);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_BLUEMAX,pBaseInfo->nBlueMax);
		
		SetCreatureAttribute(CREATURE_ATTRIBUTE_REDRECOVER,pBaseInfo->nRedRecover);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_BLUERECOVER,pBaseInfo->nBlueRecover);

		SetCreatureAttribute(CREATURE_ATTRIBUTE_PHYSICATTACK,pBaseInfo->nPhysicAttack);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_PHYSICDEFEND,pBaseInfo->nPhysicDefend);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_SPELLATTACK,pBaseInfo->nSpellAttack);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_SPELLDEFEND,pBaseInfo->nSpellDefend);

		SetCreatureAttribute(CREATURE_ATTRIBUTE_BAOJI,pBaseInfo->nBaoji);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_BAOJILV,pBaseInfo->nBaojilv);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_MINGZHONGLV,pBaseInfo->nMingZhonglv);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_SHANBILV,pBaseInfo->nShanbilv);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_GEDANG,pBaseInfo->nGedang);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_GEDANGLV,pBaseInfo->nGedanglv);

	}

/////////////////////////////////动态数据，需要保存的/////////////////////////////////////////

	// 设置Entry属性 坐标
	if (rUserData.nInstanceMapId > 0)
	{
		Point2D sPoint(rUserData.nInstanceX, rUserData.nInstanceY);
		SetPosition(sPoint);
	}
	else if (rUserData.nLandMapId > 0)
	{
		Point2D sPoint(rUserData.nLandX, rUserData.nLandY);
		SetPosition(sPoint);
	}

	// 设置Creatrue属性 等级，红蓝
	SetCreatureAttribute(CREATURE_ATTRIBUTE_STATUS, 0); // 该状态需要后面的其他功能计算再覆盖 
	SetCreatureAttribute(CREATURE_ATTRIBUTE_LEVEL, rUserData.nLevel);
	SetCreatureAttribute(CREATURE_ATTRIBUTE_RED, rUserData.nRed);
	SetCreatureAttribute(CREATURE_ATTRIBUTE_BLUE, rUserData.nBlue);

	// 设置角色SceneUser属性 
	m_sAttribute.nUid = rUserData.nCharID;
	m_sAttribute.nRoleType = rUserData.nType;

	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_EXP, rUserData.nExp);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_LAND_ID, rUserData.nLandMapId);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_LAND_X, rUserData.nLandX);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_LAND_Y, rUserData.nLandY);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_INSTANCE_ID, rUserData.nInstanceMapId);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_INSTANCE_X, rUserData.nInstanceX);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_INSTANCE_Y, rUserData.nInstanceY);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_CLOTHESID, 0);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_WEAPONID, 0);

	// 加载Buff数据
	

	// 加载其他数据 


	//------------------------------------------ 其它功能控制器数据加载 ------------------------------------------   


}

// 保存数据 
bool SceneUser::SaveData(StUserSaveCallBack* pSaveCallBack)
{
	int32 nReceiptID = 0;
	if (pSaveCallBack)
	{
		m_vecSaveCallBack.push_back(pSaveCallBack);
		nReceiptID = pSaveCallBack->GetReceiptID();
		FLOG_INFO("SAVE CALLBACK NUM:%d",m_vecSaveCallBack.size());
	}	

	S2DSaveUserAllData sUserAllData;

	sUserAllData.nCharID = GetUid();
	sUserAllData.nReceiptID = nReceiptID;

	// 立刻保存到dp

	// creature

	// user数据 
	StCharacterDataTable& rCharacterData = sUserAllData.sUserData.sCharacterTable;

	// status 由于其他功能计算获得 from creature
	rCharacterData.nLevel = GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_LEVEL);
	rCharacterData.nRed = GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_RED);
	rCharacterData.nBlue = GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_BLUE);

	// from sceneuser 
	rCharacterData.nType = GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_ROLETYPE);
	rCharacterData.nExp = GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_EXP);
	rCharacterData.nLandMapId = GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_LAND_ID);
	rCharacterData.nLandX = GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_LAND_X);
	rCharacterData.nLandY = GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_LAND_Y);
	rCharacterData.nInstanceMapId = GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_INSTANCE_ID);
	rCharacterData.nInstanceX = GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_INSTANCE_X);
	rCharacterData.nInstanceY = GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_INSTANCE_Y);

	// quest数据
	StQuestDataTable& rQuestData = sUserAllData.sUserData.sQuestTable;


	// 保存数据,保存成功后会回调 CallBackOfSave 方法 
	SendToDp(&sUserAllData, sUserAllData.GetPackLength());
	
	return true;
	
}

// 保存后的dp回调
void SceneUser::CallBackOfSave(int32 nReceiptID)
{

	// 比较回执编号是否一致 
	if (nReceiptID < 1 || m_vecSaveCallBack.size() < 1)
		return;

	std::vector<StUserSaveCallBack*>::iterator it = m_vecSaveCallBack.begin();
	for (; it < m_vecSaveCallBack.end();++it )
	{
		StUserSaveCallBack* pcb = *it;
		if(pcb->GetReceiptID() == nReceiptID )
		{ 
			pcb->SaveCallBack();
			m_vecSaveCallBack.erase(it);
			S_SAFE_DELETE(pcb);
			break;
		}
	}
	
}


void SceneUser::SaveTransferMapData(uint32 nMapID,int32 nPosX,int32 nPosZ,bool bIsInstance,bool bIsSameSs)
{

	//SaveData();
}


IMPLEMENT_FUNCTION_CHANNEL_BROADCAST(SceneUser,ChannelPosition)
{
	if(rsetValues.size() == 0)
		return;
	for (set<const ChannelPosition*>::const_iterator it = rsetValues.begin(); it != rsetValues.end();++it)
	{
		if(*it == NULL || (*it)->nID == ID())
			continue;
		//m_cNotifyControl.InsertPosition(**it);
	}
}

IMPLEMENT_FUNCTION_CHANNEL_BROADCAST(SceneUser,ChannelCreature)
{
	if(rsetValues.size() == 0)
		return;
	for (set<const ChannelCreature*>::const_iterator it = rsetValues.begin(); it != rsetValues.end();++it)
	{
		if(*it == NULL || (*it)->nID == ID())
			continue;
		//m_cNotifyControl.InsertCreature(**it);
	}
}

// 广播角色穿戴信息 
IMPLEMENT_FUNCTION_CHANNEL_BROADCAST(SceneUser,ChannelWear)
{

}


// 广播角色技能信息 
IMPLEMENT_FUNCTION_CHANNEL_BROADCAST(SceneUser,ChannelSpell)
{
	if(rsetValues.size() == 0)
		return;
	for (set<const ChannelSpell*>::const_iterator it = rsetValues.begin(); it != rsetValues.end();++it)
	{
		if(*it == NULL || (*it)->nID == ID() ||  (*it)->nSpellID == 0)
			continue;
		//m_cNotifyControl.InsertSpell(**it);
	}
}

// 广播角色Buff信息 
IMPLEMENT_FUNCTION_CHANNEL_BROADCAST(SceneUser,ChannelBuff)
{
	if(rsetValues.size() == 0)
		return;
	for (set<const ChannelBuff*>::const_iterator it = rsetValues.begin(); it != rsetValues.end();++it)
	{
		if(*it == NULL) // 包括自己 
			continue;
		//m_cNotifyControl.InsertBuff(**it);
	}
}

void SceneUser::Update(int32 nServerTimes)
{	

	// 检查是否有修改 
	if(nServerTimes % 25 == 0)
	{
		
		//if(m_cUserCtrl.IsModify())
		//{
		//	SaveData();
		//}


	}


}

void SceneUser::UpdateAttribute()
{

}

void SceneUser::OnExpChange(const ValueType& vOldValue, const ValueType& vNewValue)
{

}

void SceneUser::OnMapIDChange(const ValueType& vOldValue, const ValueType& vNewValue)
{

}

void SceneUser::OnClothesChange(const ValueType& vOldValue, const ValueType& vNewValue)
{

}

void SceneUser::OnWeaponChange(const ValueType& vOldValue, const ValueType& vNewValue)
{

}

void SceneUser::SendToClient(NetMsgHead* pMsg, int32 nSize)
{
	if (!IsClientReady())
	{
		FLOG_WARRING(__FUNCTION__,__LINE__,"ClientReady Not Ready");
	}
	SendToFep(pMsg, nSize);
}

void SceneUser::SendToFep(NetMsgHead* pMsg, int32 nSize)
{
	if (m_pClientSession)
	{
		m_pClientSession->SendMsgToFep(pMsg, nSize);
	}
}

void SceneUser::SendToDp(NetMsgHead* pMsg, int32 nSize)
{
	if (m_pClientSession)
	{
		m_pClientSession->SendMsgToDp(pMsg, nSize);
	}
}

void SceneUser::SendToWs(NetMsgHead* pMsg, int32 nSize)
{
	if (m_pClientSession)
	{
		m_pClientSession->SendMsgToWs(pMsg, nSize);
	}
}


void SceneUser::SendMainData()
{

	// 发送人物主要数据 
	// 角色基本数据
	S2CCharacterMainData sMsg;

	sMsg.nCharID = GetUid();
	sMsg.nType = GetType();
	sMsg.nLevel = GetLevel();
	sMsg.nSceneId = GetCurrSceneID();
	sMsg.nSceneX = GetPosition().nX;
	sMsg.nSceneY = GetPosition().nY;
	sMsg.nRed = GetHp();
	sMsg.nBlue = GetMp();

	SendToClient(&sMsg, sMsg.GetPackLength());
	//m_cQuestCtrl.SendMainData();

}

