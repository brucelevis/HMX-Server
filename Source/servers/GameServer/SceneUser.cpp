#include "SceneUser.h"
#include "SceneMap.h"
#include "ResourceMgr.h"
#include "def_entity.h"
#include "def_resource.h"

SceneUser::SceneUser(int64 nUserID,ClientSession* pClientSession)
	:Creature(ENTITY_TYPE_PLAYER, 0,true,E_CHANNEL_REGIST_TYPE_POS_B | E_CHANNEL_REGIST_TYPE_CREATURE_B | E_CHANNEL_REGIST_TYPE_SPELL_B | E_CHANNEL_REGIST_TYPE_BUFF_B)
	,m_pCSession(pClientSession)
	,m_cUserCtrl(this),m_cQuestCtrl(this)
{
	ASSERT(nUserID);
	ASSERT(pClientSession);
	m_nClientStatus = 0;
	m_nSaveStatue = 0;
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

// 保存数据 
bool SceneUser::Serialize(int32 nScoketEventCode)
{
	if (m_nSaveStatue > 0)
	{
		printf("[ERROR]:m_nSaveStatue =======1\n");
		ASSERT(0);
		return false;
	}

	m_nSaveStatue = 1;

	S2DSaveCharacter sMsg;

	::protobuf::Character character;
	character.set_char_id(GetUid());
	character.set_name(m_sAttribute.arrName);
	character.set_level(GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_LEVEL));
	character.set_red(GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_RED));
	character.set_blue(GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_BLUE));
	character.set_type(GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_ROLETYPE));
	character.set_exp(GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_EXP));
	character.set_land_mapid(GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_LAND_ID));
	character.set_land_x(GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_LAND_X));
	character.set_land_y(GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_LAND_Y));
	character.set_instance_mapid(GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_INSTANCE_ID));
	character.set_instance_x(GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_INSTANCE_X));
	character.set_instance_y(GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_INSTANCE_Y));

	sMsg.nByteSize = character.ByteSize();
	character.SerializeToArray(sMsg.arrByte, character.ByteSize());

	if (nScoketEventCode)
	{
		int32 nSceneID = character.instance_mapid() > 0 ? character.instance_mapid() : character.land_mapid();
		sMsg.stEvent = make_streble(EVENT_REMOTE_REVC_AFTER_MSG, nScoketEventCode, GetCSID(), nSceneID, GetUid());
	}
	
	SendToDp(&sMsg, sMsg.GetPackLength());
	return true;

}

// 初始化角色基本数据 
void SceneUser::UnSerialize(const ::protobuf::Character& character)
{

/*
关于修改数据是用SetXXXAttribute还是用m_sAttribute方式，如果数据可以修改且数据变化会影响其他变化，则用SetXXXAttribute方式
如果数据不可以设置，则用m_sAttribute方式，往往这种是初始化数据，或者特殊情况，
如果又要强行修改，又要影响其他，则用SetXXXAttribute，将第三个参数传入true
*/

	int32 nType = character.type();
	int32 nLevel = character.level();

/////////////////////////////////基础数据，是不需要保存的/////////////////////////////////////////
	// 加载基础数据 
	if(const StRoleTypeInitCfg* pRoleInfo = ResourceMgr::Instance()->GetRoleTypeInitInfoRes(nType))
	{
		SetEntityAttribute(ENTITY_ATTRIBUTE_INVIEWRANGE,pRoleInfo->nInViewRange);
		SetEntityAttribute(ENTITY_ATTRIBUTE_OUTVIEWRANGE,pRoleInfo->nOutViewRange);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_ATTACKTYPE,pRoleInfo->nAttackType);
		SetCreatureAttribute(CREATURE_ATTRIBUTE_SPEED,pRoleInfo->nSpeed);
	}

	if(const StCharacterLevelCfg* pBaseInfo = ResourceMgr::Instance()->GetCharacterLevelBase(nType,nLevel))
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
	int32 nInstanceMapId = character.instance_mapid();
	int32 nInstanceX = character.instance_x();
	int32 nInstanceY = character.instance_y();
	int32 nLandMapId = character.land_mapid();
	int32 nLandX = character.land_x();
	int32 nLandY = character.land_y();
	int32 nRed = character.red();
	int32 nBlue = character.blue();

	// 设置Entry属性 坐标
	if (nInstanceMapId > 0)
	{
		Point2D sPoint(nInstanceX, nInstanceY);
		SetPosition(sPoint);
	}
	else if (nLandMapId > 0)
	{
		Point2D sPoint(nLandX, nLandY);
		SetPosition(sPoint);
	}

	// 设置Creatrue属性 等级，红蓝
	SetCreatureAttribute(CREATURE_ATTRIBUTE_STATUS, 0); // 该状态需要后面的其他功能计算再覆盖 
	SetCreatureAttribute(CREATURE_ATTRIBUTE_LEVEL, nLevel);
	SetCreatureAttribute(CREATURE_ATTRIBUTE_RED, nRed);
	SetCreatureAttribute(CREATURE_ATTRIBUTE_BLUE, nBlue);

	int64 nCharID = character.char_id();

	// 设置角色SceneUser属性 
	m_sAttribute.nUid = nCharID;
	m_sAttribute.nRoleType = nType;

	int32 nExp = character.exp();

	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_EXP, nExp);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_LAND_ID, nLandMapId);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_LAND_X, nLandX);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_LAND_Y, nLandY);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_INSTANCE_ID, nInstanceMapId);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_INSTANCE_X, nInstanceX);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_INSTANCE_Y, nInstanceY);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_CLOTHESID, 0);
	SetSceneUserAttribute(SCENEUSER_ATTRIBUTE_WEAPONID, 0);

	strncpy(m_sAttribute.arrName, character.name().c_str(), MAX_NICK_LENGTH);

	// 加载Buff数据
	

	// 加载其他数据 


	//------------------------------------------ 其它功能控制器数据加载 ------------------------------------------   


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
	if (GetClientStatus())
	{
		FLOG_WARRING("ClientReady Not Ready");
	}
	SendToFep(pMsg, nSize);
}

void SceneUser::SendToFep(NetMsgHead* pMsg, int32 nSize)
{
	if (m_pCSession)
	{
		m_pCSession->SendMsgToFep(pMsg, nSize);
	}
}

void SceneUser::SendToDp(NetMsgHead* pMsg, int32 nSize)
{
	if (m_pCSession)
	{
		m_pCSession->SendMsgToDp(pMsg, nSize);
	}
}

void SceneUser::SendToWs(NetMsgHead* pMsg, int32 nSize)
{
	if (m_pCSession)
	{
		m_pCSession->SendMsgToWs(pMsg, nSize);
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

