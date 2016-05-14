#include "SpellControlEx.h"

#include "SceneUser.h"
#include "ResourceMgr.h"
#include "Script.h"

SpellControlEx::SpellControlEx(SceneUser& rCharacter):m_rCharactter(rCharacter)
{
	
}

SpellControlEx::~SpellControlEx(void)
{
}
//void SpellControlEx::LoadJobSpell()
//{
//	const unit::RoleTypeInitInfo* RoleBase = g_cResourceMgr.GetRoleTypeInitInfoRes(m_rCharactter.GetRole());
//	if (!RoleBase)
//	{
//		return;
//	}
//	for (uint32 i = 0 ;i<unit::LIMIT_SKILL_COUNT ;i++)
//	{
//		AddSpell(RoleBase->arrSkillId[i]);
//	}
//}
//
//
//void SpellControlEx::Load(const uint32 *pSpell,uint32 nCount)
//{
//	for (uint32 i = 0;i<nCount;i++)
//	{
//		AddSpell(pSpell[i]);
//	}
//}
//
//
//
//void SpellControlEx::LoadPosition(const CharacterSpellPosition& rSpellPosition)
//{
//	for (uint32 i = 0;i < MAX_SPELL_POSITION_NUM;i++)
//	{
//		nPosition[i]=rSpellPosition.arrPosSpell[i];
//	}
//}
//void SpellControlEx::SaveSpellPositionData()
//{
//
//}
//
//
//void SpellControlEx::AddSpell(uint32 nSpellID)
//{
//	const SpellRes* cRes = g_cResourceMgr.GetSpellInfo(nSpellID);
//	if (!cRes)
//	{
//		return;
//	}
//	m_vecSpellData.push_back(SpellEx(nSpellID,*cRes));
//}
//
//// 删除一个技能
//void SpellControlEx::DelSpell(uint32 sSpellID)
//{
//	for (std::vector<SpellEx>::iterator it = m_vecSpellData.begin();it!=m_vecSpellData.end();it++)
//	{
//
//		if (it->nSpellID == sSpellID)
//		{
//			m_vecSpellData.erase(it);
//			break;
//		}
//	}
//}
//
//
//// 技能更新(CD更新) nDelay为毫秒,为施放中的技能更新持续时间
//void SpellControlEx::Update(int32 nDelay)
//{
//	for (std::vector<SpellEx>::iterator it = m_vecSpellData.begin();it!=m_vecSpellData.end();it++)
//	{
//		if (it->nCdTime > 0)
//		{
//			if (it->nCdTime <= it->nTimeElapse)
//			{
//				it->nTimeElapse += nDelay;
//			}
//		}
//	}
//}
//
//// 是否CD中
//bool SpellControlEx::IsCloding(const SpellEx& rSpellEx, bool bAll /*是否与所有的CD有影响*/)
//{
//	return false;
//}
//bool SpellControlEx::IsCD(uint32 nSpellID)
//{
//	SpellEx* Spell = GetSpell(nSpellID);
//	//判断冷却时间
//	if (Spell&&Spell->nCdTime > 0)
//	{
//		if (Spell->nCdTime > Spell->nTimeElapse)
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}	
//	}
//	return false;
//}
//bool SpellControlEx::SpellDo(uint32 nSpellID)
//{
//	if (SpellEx* pSpell = GetSpell(nSpellID))
//	{
//		//检测冷却时间
//		if (pSpell->nCdTime > 0)
//		{
//			return false;
//		}
//		//检测前置技能
//		if(pSpell->rSpellRes->nPreSpellID !=0 && pSpell->rSpellRes->nPreSpellID == pSpell->rSpellRes->nPreSpellID)
//		{
//			return false;
//		}
//		//检测蓝
//		if (IsEnoughBlue(*pSpell))
//		{
//			return false;
//		}
//		pSpell->nCdTime = pSpell->rSpellRes->nCd;
//
//		//扣蓝
//		m_rCharactter.SetAttribute(ATTRIBUTE_BLUE,(0-pSpell->rSpellRes->nBlue));
//
//		m_rCharactter.AddSpellQuene(nSpellID);
//
//		//添加buffer
//		Script::SetSpellEffectEntry(&m_rCharactter,pSpell);
//		return true;
//		/*
//		for (uint32 i =0 ;i < MAX_SPELL_BUFF_NUM ;i++ )
//		{
//			if(pSpell->rSpellRes->arrBuff[i].nBuffID!=0)
//			{
//				switch (pSpell->rSpellRes->arrBuff[i].eAimType)
//				{
//				case EFFECT_AIM_TYPE_FROM:
//					m_rCharactter.AddBuff(&m_rCharactter,  pSpell->rSpellRes->arrBuff[i].nBuffID);
//					break;
//				case EFFECT_AIM_TYPE_FROM_RANGE:
//					{
//						
//						SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)&m_rCharactter));
//						if (pScene  == NULL)
//						{
//							return false;
//						}
//						std::vector<Entity*>  around_player;
//						pScene->GetNeighborsEntity(m_rCharactter,around_player);
//
//						std::vector<Entity*> out_player(around_player.size());
//						std::vector<Entity*>::const_iterator it_left = around_player.begin();
//						std::vector<Entity*>::const_iterator it_right = around_player.end();
//
//						//Script::GetEffectCreature(m_rCharactter.Position(),m_rCharactter.Heading(),EFFECT_RANGE_TYPE_AROUND,pSpell->rSpellRes->arrBuff[i].nArea,pSpell->rSpellRes->arrBuff[i].nAngle,it_left,it_right,around_player.begin());
//						Script::GetEffectCreatureOut(m_rCharactter.Position(),m_rCharactter.Heading(),EFFECT_RANGE_TYPE_AROUND,pSpell->rSpellRes->arrBuff[i].nArea,pSpell->rSpellRes->arrBuff[i].nAngle,it_left,it_right,around_player.begin());
//						for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
//						{
//							Character* pOther = (Character*)(*itr);
//							pOther->AddBuff(&m_rCharactter, pSpell->rSpellRes->arrBuff[i].nBuffID);
//						}
//						
//					}
//					break;
//				default:
//					break;
//				}
//			}
//			*/
//		//}
//		
//	}
//	return false;
//}
//
//SpellEx* SpellControlEx::GetSpell(uint32 nSpellID)
//{
//	for (std::vector<SpellEx>::iterator it = m_vecSpellData.begin();it!=m_vecSpellData.end();it++)
//	{
//		if (it->nSpellID == nSpellID)
//		{
//			return &(*it);
//		}
//	}
//	return NULL;
//}
//
//bool SpellControlEx::IsEnoughBlue(const SpellEx& rSpellEx)
//{
//	ValueType blue = 0;
//	m_rCharactter.GetAttribute(ATTRIBUTE_BLUE,blue);
//	return rSpellEx.rSpellRes->nBlue > blue.nInt32;
//}
//
//bool SpellControlEx::SetSpell(uint32 nPos,uint32 nSpellID)
//{
//	//设置技能位置
//	if (nPos >= MAX_SPELL_POSITION_NUM )
//	{
//		return false;
//	}
//
//	if (SpellEx* pSpell = GetSpell(nSpellID))
//	{
//		nPosition[nPos] = nSpellID;
//		return true;
//	}
//	return false;
//}
//
//
//void SpellControlEx::SendSpellList()
//{
//	ss2c::SS2C_SPELL_POSITION sSpellMsg;
//	sSpellMsg.client_uid = m_rCharactter.GetClientUID();
//	uint32 i =0;
//	for ( ;i< MAX_SPELL_POSITION_NUM;i++)
//	{
//		if ( nPosition[i] == 0)
//		{
//			continue;
//		}
//		sSpellMsg.nSpell[i] = nPosition[i];
//	}
//	sSpellMsg.nCount =i;
//	m_rCharactter.SendMsg(&sSpellMsg,sSpellMsg.get_pak_length());
//}

