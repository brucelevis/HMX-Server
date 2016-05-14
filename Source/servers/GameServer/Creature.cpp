#include "Creature.h"

#include "Buff.h"
#include "Script.h"
#include "Utility.h"
#include "UnorderedMap.h"
#include "ResourceMgr.h"

Creature::Creature(EntityType eEntityType,uint32 nEntityTypeID,bool bListen,int32 registChannelFlags)
	: Entity(eEntityType,nEntityTypeID,bListen,registChannelFlags | E_CHANNEL_REGIST_TYPE_CREATURE_R | E_CHANNEL_REGIST_TYPE_SPELL_R | E_CHANNEL_REGIST_TYPE_BUFF_R)
{
	m_sChannelCreature.nID = ID();
	m_sChannelSpell.nID = ID();
	m_sChannelBuff.nID = ID();
	m_nFlagsForClient = 0;
	CreatureInitAttributeOffet();
}

bool Creature::SetCreatureAttribute(CreatureAttributeType eAttributeType, const ValueType& vNewValue)
{
	// 检测是否有偏移值   
	if (m_sAttributeOffset[eAttributeType].nOffset && m_sAttributeOffset[eAttributeType].bCanSet)
	{
		SetCreatureAttribute(eAttributeType, vNewValue, false);
		return true;
	}
	return false;
}

bool Creature::SetCreatureAttribute(CreatureAttributeType eAttributeType, const ValueType& vNewValue, bool bEnforce)
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

bool Creature::GetCreatureAttribute(CreatureAttributeType i_eAttributeType, ValueType& o_vValue)
{
	// 检测是否有偏移值 
	if (m_sAttributeOffset[i_eAttributeType].nOffset)
	{
		memcpy(&o_vValue, m_sAttributeOffset[i_eAttributeType].nOffset, m_sAttributeOffset[i_eAttributeType].nSize);
		return true;
	}
	return false;
}

int32 Creature::GetCreatureAttributeInt32(CreatureAttributeType i_eAttributeType)
{
	ValueType o_vValue;
	GetCreatureAttribute(i_eAttributeType, o_vValue);
	return o_vValue.nInt32;
}

int64 Creature::GetCreatureAttributeInt64(CreatureAttributeType i_eAttributeType)
{
	ValueType o_vValue;
	GetCreatureAttribute(i_eAttributeType, o_vValue);
	return o_vValue.nInt64;
}

// 添加Buff
void Creature::AddBuff(Creature* pFrom, int32 nBuffID)
{
	AddBuff(pFrom,nBuffID,0);
}

void Creature::LoadBuff(Creature* pFrom, int32 nBuffID,int32 nTimeElapse)
{
	AddBuff(pFrom,nBuffID,nTimeElapse);
}

void Creature::AddBuff(Creature* pFrom, int32 nBuffID,int32 nTimeElapse)
{
	// 查找Buff配置 
	//const BuffData* pBuffData = NULL/*g_cResourceMgr.GetBuffData(nBuffID)*/;
	//if (!pBuffData) return;

	//// 检测是否已存在同ID Buff 
	//multimap<int32, Buff*>::iterator iter = m_mapBuffs.find(pBuffData->nBuffID);
	//if (iter != m_mapBuffs.end())
	//{
	//	// 检测是否可叠加 
	//	if (pBuffData->nLimitFlag & BUFF_EFFECT_FLAG_STACKABLE)
	//	{
	//		// 添加Buff 
	//		if (Buff* pNewBuff = g_cBuffFactory.CreateObj(pBuffData, pFrom))
	//		{
	//			m_mapBuffs.insert(pair<int32, Buff*>(pBuffData->nBuffID, pNewBuff));
	//			AddChannelBuff(nBuffID);
	//			return;
	//		}
	//	}

	//	// 检测是否可替换 
	//	if (pBuffData->nLimitFlag & BUFF_EFFECT_FLAG_REPLACEABLE)
	//	{
	//		if (Buff* pNewBuff = g_cBuffFactory.CreateObj(pBuffData, pFrom))
	//		{
	//			// 逆运算 
	//			if(pBuffData->nLimitFlag & BUFF_EFFECT_FLAG_REVERSIBLE)
	//				Reverse(*iter->second, this);

	//			// 回收Buff对象 
	//			g_cBuffFactory.DestroyObj(iter->second);

	//			// 替换 
	//			iter->second = pNewBuff;
	//			
	//			return;
	//		}
	//	}
	//}
	//else
	//{
	//	// 添加Buff 
	//	if (Buff* pNewBuff = g_cBuffFactory.CreateObj(pBuffData, pFrom,nTimeElapse))
	//	{
	//		const BuffData *pAdd = &(pNewBuff->rResData);

	//		m_mapBuffs.insert(pair<int32, Buff*>(pBuffData->nBuffID, pNewBuff));
	//		AddChannelBuff(nBuffID);
	//		std::multimap<int32, Buff*>	::iterator pTest = m_mapBuffs.find(pBuffData->nBuffID);

	//		Buff *pBuffer =  pTest->second;

	//		return;
	//	}
	//}
}

void Creature::UpdateBuff(int32 nTimeElapse)
{

	//multimap<int32, Buff*>::iterator iter = m_mapBuffs.begin();
	//for (; iter != m_mapBuffs.end();)
	//{
	//	Buff& rBuff = *iter->second;

	//	// 当前Buff被删除 
	//	bool bDelete = false;

	//	for (int32 i = 0; i < MAX_BUFF_EFFECT_NUM; ++i)
	//	{
	//		// 效果配置 
	//		const BuffData::Effect& sEffect = rBuff.rResData.arrEffect[i];
	//		if(sEffect.nEffectID == BUFF_EFFECT_TYPE_NULL)
	//			break;

	//		// 时间点检测 
	//		if (rBuff.nTimeElapse <= sEffect.nTimePoint && sEffect.nTimePoint < rBuff.nTimeElapse + nTimeElapse)
	//		{
	//			// 是否随机 
	//			if (sEffect.nProbability)
	//			{
	//				if (!(RangedRand(0, 100) < sEffect.nProbability))
	//				{// 不发生 
	//					return;
	//				}
	//			}

	//			// 回调函数 
	//			BuffEffect& rCallback = Script::arrBuffEffect[sEffect.nEffectID];

	//			// 检测效果限制 
	//			if (rCallback.nLimitFlag != (rCallback.nLimitFlag & rBuff.rResData.nLimitFlag))
	//			{
	//				return;
	//			}

	//			if(rCallback.eType == (int32)BUFF_EFFECT_TYPE_DADUAN &&  ( sEffect.eAimType == EFFECT_AIM_TYPE_TO || rBuff.pFrom == this )/* 是打断自己的效果 */)
	//			{
	//				if(!(rCallback.nLimitFlag & BUFF_EFFECT_FLAG_INTERRUPTIBLE) /* 当前Buff不可被打断 */)
	//				{
	//					// 暂时记录当前BuffId，与Buff指针 
	//					int32 nBuffIdTmp = iter->first;
	//					Buff* pBuffTmp = iter->second;
	//					// 遍历删除可被打断的buff 
	//					for (iter = m_mapBuffs.begin(); iter != m_mapBuffs.end();)
	//					{
	//						Buff& rBuff = *iter->second;
	//						if (rBuff.rResData.nLimitFlag & BUFF_EFFECT_FLAG_INTERRUPTIBLE)
	//						{
	//							// 是否可逆运算 
	//							if(rBuff.rResData.nLimitFlag & BUFF_EFFECT_FLAG_REVERSIBLE)
	//								Reverse(rBuff, this);

	//							DelChannelBuff(rBuff.rResData.nBuffID);

	//							g_cBuffFactory.DestroyObj(&rBuff);
	//							m_mapBuffs.erase(iter++);

	//						}
	//						else
	//						{
	//							++iter;
	//						}
	//					}
	//					// 重置迭代器 
	//					multimap<int32, Buff*>::iterator itTmp = m_mapBuffs.lower_bound(nBuffIdTmp) ;
	//					if(itTmp == m_mapBuffs.upper_bound(nBuffIdTmp))
	//					{
	//						assert(0);
	//						continue;
	//					}
	//					for (; itTmp != m_mapBuffs.end();++itTmp )
	//					{
	//						if(itTmp->second == pBuffTmp)
	//						{
	//							iter = itTmp;
	//						}
	//					}
	//				}
	//				else
	//				{
	//					// 找到下一个不可被打断Buff 
	//					bool bFindNextTerrup = false;
	//					iter++;
	//					for (;iter != m_mapBuffs.end();)
	//					{
	//						Buff& rBuff = *iter->second;
	//						const BuffData& sEffect = (*iter->second).rResData;
	//						if (!(sEffect.nLimitFlag & BUFF_EFFECT_FLAG_INTERRUPTIBLE))
	//						{
	//							bFindNextTerrup = true;
	//							break;
	//						}
	//						iter++;
	//					}

	//					if(bFindNextTerrup/* 找到下一个不可被打断Buff **/)
	//					{
	//						// 暂时记录当前BuffId，与Buff指针 
	//						int32 nBuffIdTmp = iter->first;
	//						Buff* pBuffTmp = iter->second;
	//						// 遍历删除可被打断的buff 
	//						for (iter = m_mapBuffs.begin(); iter != m_mapBuffs.end();)
	//						{
	//							Buff& rBuff = *iter->second;
	//							if (rBuff.rResData.nLimitFlag & BUFF_EFFECT_FLAG_INTERRUPTIBLE)
	//							{
	//								// 逆运算 
	//								if(rBuff.rResData.nLimitFlag & BUFF_EFFECT_FLAG_REVERSIBLE)
	//									Reverse(rBuff, this);

	//								DelChannelBuff(rBuff.rResData.nBuffID);

	//								g_cBuffFactory.DestroyObj(&rBuff);
	//								m_mapBuffs.erase(iter++);
	//							}
	//							else
	//							{
	//								++iter;
	//							}
	//						}
	//						// 重置迭代器 
	//						multimap<int32, Buff*>::iterator itTmp = m_mapBuffs.lower_bound(nBuffIdTmp) ;
	//						if(itTmp == m_mapBuffs.upper_bound(nBuffIdTmp))
	//						{
	//							assert(0);
	//							continue;
	//						}
	//						for (; itTmp != m_mapBuffs.end();++itTmp )
	//						{
	//							if(itTmp->second == pBuffTmp)
	//							{
	//								iter = itTmp;
	//							}
	//						}
	//					}
	//					else
	//					{
	//						// 重置迭代器=end; 
	//						iter = m_mapBuffs.end();
	//					}

	//					bDelete = true;
	//					break;//for (int32 i = 0; i < MAX_BUFF_EFFECT_NUM; ++i)
	//				}
	//			}
	//			else
	//			{
	//				// 调用Buff效果 
	//				if(rCallback.eType == BUFF_EFFECT_TYPE_DADUAN )
	//				{
	//					rBuff.pFrom->InterruptBuff(); // 打断buff特别处理 
	//				}
	//				else if (rCallback.pCallBack)
	//				{
	//				//	rCallback.pCallBack(rBuff.pFrom, this, sEffect.eAimType, sEffect.eRangeType,
	//				//		sEffect.nParam, sEffect.nDist, sEffect.fCosAngle,rBuff.rResData.nEffectNum,rBuff.arrReverseValue[i],false);
	//				}
	//			}
	//		}
	//	}

	//	if(!bDelete)
	//	{
	//		rBuff.nTimeElapse += nTimeElapse;

	//		// 销毁Buff 
	//		if (rBuff.rResData.nLimitFlag & BUFF_EFFECT_FLAG_MOMENTARY /* 瞬时只执行一次 */ || rBuff.rResData.nLifeTime < rBuff.nTimeElapse)
	//		{
	//			// 逆运算 
	//			if(rBuff.rResData.nLimitFlag & BUFF_EFFECT_FLAG_REVERSIBLE)
	//				Reverse(rBuff, this);

	//			DelChannelBuff(rBuff.rResData.nBuffID);

	//			g_cBuffFactory.DestroyObj(&rBuff);
	//			m_mapBuffs.erase(iter++);
	//			
	//		}
	//		else
	//		{
	//			++iter;
	//		}
	//	}
	//}
}

void Creature::InterruptBuff()
{
	/*multimap<int32, Buff*>::iterator iter = m_mapBuffs.begin();
	for (; iter != m_mapBuffs.end();)
	{
		Buff& rBuff = *iter->second;
		if (rBuff.rResData.nLimitFlag & BUFF_EFFECT_FLAG_INTERRUPTIBLE)
		{
			g_cBuffFactory.DestroyObj(&rBuff);
			m_mapBuffs.erase(iter++);
			DelChannelBuff(rBuff.rResData.nBuffID);
		}
		else
		{
			++iter;
		}
	}*/
}


void Creature::CreatureInitAttributeOffet()
{

#define	INIT_ATTRIBUTE_OFFSET(index, name, set, handler)	\
						{\
			m_sAttributeOffset[index].nOffset = &(this->m_sAttribute.name);\
			m_sAttributeOffset[index].nSize = sizeof(this->m_sAttribute.name);\
			m_sAttributeOffset[index].bCanSet = set;\
			m_sAttributeOffset[index].hHandler = handler;\
						}

	// 偏移信息 
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_STATUS				, nStatus				, true		, boost::bind(&Creature::OnStatusChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_LEVEL				, nLevel				, true		, boost::bind(&Creature::OnLevelChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_RED				, nRed					, true		, boost::bind(&Creature::OnRedChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_REDMAX				, nRedMax				, true		, boost::bind(&Creature::OnRedMaxChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_BLUE				, nBlue					, true		, boost::bind(&Creature::OnBlueChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_BLUEMAX			, nBlueMax				, true		, boost::bind(&Creature::OnBlueMaxChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_SPEED				, nSpeed				, true		, boost::bind(&Creature::OnSpeedChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_REDRECOVER			, nRedRecover			, true		, boost::bind(&Creature::OnRedRecoverChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_BLUERECOVER		, nBlueRecover			, true		, boost::bind(&Creature::OnBlueRecoverChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_PHYSICATTACK		, nPhysicAttack			, true		, boost::bind(&Creature::OnPhysiAttackChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_PHYSICDEFEND		, nPhysicDefend			, true		, boost::bind(&Creature::OnPhysicDefendChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_SPELLATTACK		, nSpellAttack			, true		, boost::bind(&Creature::OnSpellAttackChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_SPELLDEFEND		, nSpellDefend			, true		, boost::bind(&Creature::OnSpellDefendChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_ATTACKTYPE			, nAttackType			, true		, boost::bind(&Creature::OnAttackTypeChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_MINGZHONGLV		, nMingzhonglv			, true		, boost::bind(&Creature::OnMingzhonglvChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_SHANBILV			, nShanbilv				, true		, boost::bind(&Creature::OnShanbilvChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_BAOJI				, nBaoji				, true		, boost::bind(&Creature::OnBaojiChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_BAOJILV			, nBaojilv				, true		, boost::bind(&Creature::OnBaojilvChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_GEDANG				, nGedang				, true		, boost::bind(&Creature::OnGedangChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(CREATURE_ATTRIBUTE_GEDANGLV			, nGedanglv				, true		, boost::bind(&Creature::OnGedanglvChange, this, _1, _2));
#undef INIT_ATTRIBUTE_OFFSET
}

void Creature::OnStatusChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	if((vOldValue.nInt32 & CREATURE_STATUS_ALIVE) != (vNewValue.nInt32 & CREATURE_STATUS_ALIVE) ) // 判断是否仅为修改为活着的 
	{
		m_nFlagsForClient |= CREATURE_ATTRIBUTE_STATUS;
	}
}

void Creature::OnRedChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_RED;
	m_sChannelCreature.nRed = vNewValue.nInt32;
	if(m_sChannelCreature.nRed > 0)
	{
		//StatusAdd(CREATURE_STATUS_ALIVE);
	}else
	{
		if(m_sAttribute.nStatus & CREATURE_STATUS_ALIVE)
		{
			//StatusToggle(CREATURE_STATUS_ALIVE);
		}
	}
}
void Creature::OnRedMaxChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_REDMAX;
	m_sChannelCreature.nRedMax = vNewValue.nInt32;
}
void Creature::OnBlueChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_BLUE;
	m_sChannelCreature.nBlue = vNewValue.nInt32;
}
void Creature::OnBlueMaxChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_BLUEMAX;
	m_sChannelCreature.nBlueMax = vNewValue.nInt32;
}
void Creature::OnLevelChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_LEVEL;
}

void Creature::OnSpeedChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_SPEED;
	//m_sChannelPosition.nSpeed = vNewValue.nInt32;
}
void Creature::OnRedRecoverChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_REDRECOVER;
}
void Creature::OnBlueRecoverChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_BLUERECOVER;
}
void Creature::OnPhysiAttackChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_PHYSICATTACK;
}
void Creature::OnPhysicDefendChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_PHYSICDEFEND;
}
void Creature::OnSpellAttackChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_SPELLATTACK;
}
void Creature::OnSpellDefendChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_SPELLDEFEND;
}
//void Creature::OnWuliChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_WULI;
//}
//void Creature::OnShenshouChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_SHENSHOU;
//}
//void Creature::OnNeiliChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_NEILI;
//}
void Creature::OnAttackTypeChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_ATTACKTYPE;
}
void Creature::OnMingzhonglvChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_MINGZHONGLV;
}
void Creature::OnShanbilvChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_SHANBILV;
}
void Creature::OnBaojiChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_BAOJI;
}
void Creature::OnBaojilvChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_BAOJILV;
}
void Creature::OnGedangChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_GEDANG;
}
void Creature::OnGedanglvChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	m_nFlagsForClient |= CREATURE_ATTRIBUTE_GEDANGLV;
}
//void Creature::OnDeAttackChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_DEATTACK;
//}
//void Creature::OnDeMeleeAttackChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_DEMELEEATTACK;
//}
//void Creature::OnDeRemoteAttackChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_DEREMOTEATTACK;
//}
//void Creature::OnDeSpellCDChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_DESPELLCD;
//}
//void Creature::OnAttackRedRecoverChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_ATTACKREDRECOVER;
//}
//void Creature::OnAttackBlueRecoverChange(const ValueType& vOldValue, const ValueType& vNewValue)
//{
//	m_nFlagsForClient |= ATTRIBUTE_ATTACKBLUERECOVER;
//}

// 给Buff频道添加一个Buff
void Creature::AddChannelBuff(uint32 nBuffID)
{
	/*if(m_sChannelBuff.nNum >= CHANNEL_BUFF_MAX)
		return;*/
	m_sChannelBuff.arrBuffID[m_sChannelBuff.nNum] = nBuffID;
	m_sChannelBuff.nNum++;
}

// 给Buff频道删除一个Buff 
void Creature::DelChannelBuff(uint32 nBuffID)
{
	for (int32 nIndex = 0; nIndex < m_sChannelBuff.nNum; ++nIndex)
	{
		if(m_sChannelBuff.arrBuffID[nIndex] == nBuffID)
		{
			if(nIndex + 1 < m_sChannelBuff.nNum)
			{
				m_sChannelBuff.arrBuffID[nIndex] = m_sChannelBuff.arrBuffID[m_sChannelBuff.nNum - 1];// 将最后一位给该位赋值即可 
			}
			m_sChannelBuff.nNum -= 1;
			return;
		}
	}
}

void CreatureManager::AddCreature(Creature& rCreature)
{
	m_usetCreaturies.insert(&rCreature);
}

void CreatureManager::DelCreature(Creature& rCreature)
{
	m_usetCreaturies.erase(&rCreature);
}

void CreatureManager::Update(int32 nTimeElapse)
{

}


void Creature::AddSpellQuene(uint32 nSpellID)
{
	m_queSpell.push(nSpellID);
}


