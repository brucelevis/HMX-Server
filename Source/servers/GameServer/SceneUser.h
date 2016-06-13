#ifndef __CHARACTER_H_
#define __CHARACTER_H_

#include "SharedIncludes.h"

#include "BaseSingle.h"
#include "Creature.h"
#include "UnorderedSet.h"
#include "UnorderedMap.h"
#include "ClientSession.h"
#include "SpellControlEx.h"

#include "SceneUserCtrl.h"
#include "QuestCtrl.h"
#include "character.pb.h"

/*
 *
 *	Detail: 角色类 
 *   
 *  Created by hzd 2014-12-17
 *
 */

class ConnAppServer;

class SceneUser : public Creature , public IChannelOperate(ChannelWear)
{

	friend class SceneUserCtrl;

public:

	SceneUser(int64 nUID,ClientSession* pClientSession);
	~SceneUser(void){};

		// 属性修改 
	bool SetSceneUserAttribute(SceneUserAttributeType eAttributeType, const ValueType& vNewValue);
	bool GetSceneUserAttribute(SceneUserAttributeType i_eAttributeType, ValueType& o_vValue);
	int32 GetSceneUserAttributeInt32(SceneUserAttributeType i_eAttributeType);
	int64 GetSceneUserAttributeInt64(SceneUserAttributeType i_eAttributeType);
	bool SetSceneUserAttribute(SceneUserAttributeType eAttributeType, const ValueType& vNewValue, bool bEnforce);

private:
	void SceneUserInitAttributeOffet();

private:
	
	SceneUserAttribute	m_sAttribute;				
	AttributeOffset		m_sAttributeOffset[SCENEUSER_ATTRIBUTE_MAX];// 

public:

	// 保存数据 
	bool Serialize(int32 nScoketEventCode = 0);

	// 加载该玩家所有数据 
	void UnSerialize(const ::protobuf::Character& character);

	// 位置信息广播 
	DEFINE_FUNCTION_CHANNEL_BROADCAST(ChannelPosition);

	// 生物体信息广播 
	DEFINE_FUNCTION_CHANNEL_BROADCAST(ChannelCreature);

	// 装备信息广播 
	DEFINE_FUNCTION_CHANNEL_BROADCAST(ChannelWear);

	// 技能信息广播 
	DEFINE_FUNCTION_CHANNEL_BROADCAST(ChannelSpell);

	// Buff信息广播 
	DEFINE_FUNCTION_CHANNEL_BROADCAST(ChannelBuff);

	// 保存切线地图数据 
	void SaveTransferMapData(uint32 nMapID,int32 nPosX,int32 nPosZ,bool bIsInstance,bool bIsSameSs);

	// 消息更新 
	void Update(int32 nServerTimes);

	ClientSession* GetClientSession(){	return m_pCSession;}

	int32 GetCSID()
	{
		return m_pCSession ? m_pCSession->GetSessionID() : 0;
	}


public:

	// 进入场景需要发送数据 
	void SendMainData();

	int64 GetUid()
	{ 
		return GetSceneUserAttributeInt64(SCENEUSER_ATTRIBUTE_UID);
	}

	int32 GetLevel() 
	{
		return GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_LEVEL);
	}

	int32 GetType()
	{
		return GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_ROLETYPE);
	}

	int32 GetCurrSceneID()
	{
		return GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_LAND_ID);
	}

	int32 GetMainSceneID()
	{
		return GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_LAND_ID);
	}

	int32 GetInstanceSceneID()
	{
		return GetSceneUserAttributeInt32(SCENEUSER_ATTRIBUTE_INSTANCE_ID);
	}

	int32 GetHp()
	{
		return GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_RED);
	}

	int32 GetMp()
	{
		return GetCreatureAttributeInt32(CREATURE_ATTRIBUTE_BLUE);
	}

	void SendToClient(NetMsgHead* pMsg, int32 nSize);

	void SendToFep(NetMsgHead* pMsg, int32 nSize);

	void SendToDp(NetMsgHead* pMsg, int32 nSize);

	void SendToWs(NetMsgHead* pMsg, int32 nSize);
	
//-------------------------------属性更改回调函数------------------------------------------
private:

	void OnExpChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnMapIDChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnClothesChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnWeaponChange(const ValueType& vOldValue, const ValueType& vNewValue);

	// 同步自己的属性(检查有变动才发送更新) 
	void UpdateAttribute();


public:
	void SetClientStatus(int32 nStatue)
	{
		m_nClientStatus = nStatue;
	}
	int32 GetClientStatus()
	{
		return m_nClientStatus;
	}

private:

	ClientSession*		m_pCSession;		// 会话   
	int32				m_nClientStatus;	// 前端状态 0 ok,1 加载资源中 
	int32				m_nSaveStatue;		// 0 空闲中，1进行中，不可再保存 

//----------------------------------控制器管理------------------------------------------
public:

	SceneUserCtrl& GetUserCtrl(){return m_cUserCtrl;}

	QuestCtrl& GetQuestCtrl(){ return m_cQuestCtrl;}

private:

	// 角色，其他模块拿角色相关数据，尽量使用CharCtrl去拿 
	SceneUserCtrl		m_cUserCtrl;

	// 任务 控制器 
	QuestCtrl			m_cQuestCtrl;

	// 计数器(支持每时每日每周每月等清零处理)

};

#endif



