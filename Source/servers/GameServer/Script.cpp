#if 0

#include "Script.h"

#include "Creature.h"
#include "SceneMap.h"
#include "Geometry2D.h"
#include "SceneUser.h"
#include "SceneUserManager.h"

namespace Script
{
	//通用buffer效果 buffer调用
	void BufferCommonEffect(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,CreatureStatus eBufferType,CreatureStatus eDeBufferType)
	{
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
//			if(!pFrom->StatusExist(eDeBufferType))
//				pFrom->StatusAdd(eBufferType);
			break;
		case EFFECT_AIM_TYPE_TO:
//			if(!pTo->StatusExist(eDeBufferType))
//				pTo->StatusAdd(eBufferType);
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{	
				SceneMap *pScene = NULL /* g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom))*/;
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);
				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != it_out ; itr++)
				{
				//	if(!(*itr)->StatusExist(eDeBufferType))
				//		(*itr)->StatusAdd(eBufferType);
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = NULL/*g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo))*/;
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				it_out = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != it_out ; itr++)
				{
				//	if(!(*itr)->StatusExist(eDeBufferType))
				//		(*itr)->StatusAdd(eBufferType);
				}

			}
			break;
		default:
			break;
		}
	}
	
	//通用buffer效果 debuffer调用
	void BufferCommonEffect(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,CreatureStatus eBufferType)
	{
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			//	pFrom->StatusAdd(eBufferType);
			break;
		case EFFECT_AIM_TYPE_TO:
			//	pTo->StatusAdd(eBufferType);
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{	
				SceneMap *pScene = NULL/*g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom))*/;
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);
				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != it_out ; itr++)
				{
				//	(*itr)->StatusAdd(eBufferType);
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene =NULL/* g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo))*/;
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				it_out = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != it_out ; itr++)
				{
				//	(*itr)->StatusAdd(eBufferType);
				}

			}
			break;
		default:
			break;
		}
	}
	
	//buffer修改属性通用函数
	void BufferCommonAttribute(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,AttributeType eArrributeType)
	{
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetEntityAttribute(eArrributeType,att_val);
				int32 ReservseValue = (nParam/10000) * att_val.nInt32;
				pFrom->SetEntityAttribute(eArrributeType,ReservseValue+att_val.nInt32);
				oi_nReverseVal.insert(std::make_pair(pFrom->ID(),ReservseValue));
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{		
				ValueType att_val;
				pTo->GetEntityAttribute(eArrributeType,att_val);
				int32 ReservseValue = (nParam/10000) * att_val.nInt32;
				pTo->SetEntityAttribute(eArrributeType,ReservseValue+att_val.nInt32);
				oi_nReverseVal.insert(std::make_pair(pTo->ID(),ReservseValue));
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene =NULL/* g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom))*/;
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(eArrributeType,att_val);
					int32 ReservseValue = (nParam/10000) * att_val.nInt32;
					(*itr)->SetAttribute(eArrributeType,ReservseValue+att_val.nInt32);
					oi_nReverseVal.insert(std::make_pair((*itr)->ID(),ReservseValue));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = NULL/*g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo))*/;
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(eArrributeType,att_val);
					int32 ReservseValue = (nParam/10000) * att_val.nInt32;
					(*itr)->SetAttribute(eArrributeType,ReservseValue+att_val.nInt32);
					oi_nReverseVal.insert(std::make_pair((*itr)->ID(),ReservseValue));
				}
			}
			break;
		default:
			break;
		}
	}
	
	// 可逆buffer属性操作 
	void ReverseCommonAttribute(std::map<uint64,int32>& oi_nReverseVal,AttributeType eArrributeType)
	{
		for (std::map<uint64,int32>::iterator it = oi_nReverseVal.begin();it!=oi_nReverseVal.end();it++)
		{
			ValueType att_val;
			SceneUser *pChar = SceneUserManager::Instance()->GetUserByEntityID(it->first);
			pChar->GetEntityAttribute(eArrributeType,att_val);
			if ( att_val.nInt32 >= it->second)
			{
				pChar->SetEntityAttribute(eArrributeType, att_val.nInt32 - it->second);
			}
			else
			{
				pChar->SetEntityAttribute(eArrributeType,0);
			}
		}

	}

#define BUFFER_SCRIPT_STATUES_FUNC(BUFFER_TYPE,DEBUFFER_TYPE) if (bReverse)\
	{\
	return;\
	} \
	BufferCommonEffect(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal,BUFFER_TYPE,DEBUFFER_TYPE);


#define BUFFER_SCRIPT_DESTATUES_FUNC(BUFFER_TYPE) if (bReverse)\
	{\
	return;\
	} \
	BufferCommonEffect(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal,BUFFER_TYPE);

//	#define  MAX_ATTACKED_PLAYER 10


	void ReduceRed(Entity *i_rPlayer,uint32 i_nNum)
	{
		i_nNum=1;
		ValueType Red = 0;
		i_rPlayer->GetEntityAttribute(CREATURE_ATTRIBUTE_RED,Red);

		if (Red.nUint32 > i_nNum)
		{
			i_rPlayer->SetEntityAttribute(CREATURE_ATTRIBUTE_RED,Red.nUint32 - i_nNum);
		}
		else
		{
			i_rPlayer->SetEntityAttribute(CREATURE_ATTRIBUTE_RED,0);
		}
	}

	// 获得夹角范围内有效生物体 
	template<typename _InIt, typename _OutIt>
	static _OutIt GetAngleCreature(const Point2DEx& i_rPosition, const Vector2DEx& i_rHeading, int32 i_nDist, float32 i_nCosValue,
		_InIt i_rInputFirst, _InIt i_rInputEnd, _OutIt o_rOutputFirst)
	{
		//_InIt1 it = i_rInputFirst;
		while (i_rInputFirst != i_rInputEnd)
		{
			Entity * pCreature =(Entity *) (*i_rInputFirst);
			pCreature->Position();

			Point2DEx creature_pos(pCreature->Position());
			Vector2DEx back_vec_s = i_rPosition - creature_pos;
			back_vec_s.Normalize();
			float angle = i_rHeading.Dot(back_vec_s);
			if (angle <=  i_nDist)
			{
				//*o_rOutputFirst = *i_rInputFirst;
				//*o_rOutputFirst = static_cast <void*>(*i_rInputFirst);
				memcpy(*o_rOutputFirst,*i_rInputFirst,sizeof(void*));
				o_rOutputFirst++;
			}
			i_rInputFirst++;
		}
		return o_rOutputFirst;
	}

	// 获得矩形范围内有效生物体 
	template<typename _InIt, typename _OutIt>
	static _OutIt GetAroundCreature(const Point2D& i_rPosition, int32 i_nDist,
		_InIt i_rInputFirst, _InIt i_rInputEnd, _OutIt o_rOutputFirst)
	{
		//_InIt1 it = i_rInputFirst;
		while (i_rInputFirst != i_rInputEnd)
		{
			Entity * pCreature =(Entity *) (*i_rInputFirst);
			int32 dis = DistanceSq(i_rPosition,pCreature->Position());
			if (dis < i_nDist)
			{
				*o_rOutputFirst = *i_rInputFirst;
				o_rOutputFirst++;
			}
			i_rInputFirst++;
		}
		return o_rOutputFirst;
	}

	// 获得范围内有效生物体 
	template<typename _InIt, typename _OutIt>
	static _OutIt GetEffectCreature(const Point2D& i_rPosition/*, const Point2DEx& i_rBackPosition*/, const Vector2DEx& i_rHeading, EffectRangeType eRangeType,
		int32 i_nDist, float32 i_nCosValue,
		_InIt i_rInputFirst, _InIt i_rInputEnd, _OutIt o_rOutputFirst)
	{

		switch (eRangeType)
		{
		case EFFECT_RANGE_TYPE_AROUND:
			{
				return GetAroundCreature(i_rPosition,i_nDist,i_rInputFirst,i_rInputEnd,o_rOutputFirst);
			} 
			break;
		case EFFECT_RANGE_TYPE_Angle:
			{
				_OutIt begin_itr = o_rOutputFirst;
				Point2DEx BackPoint =i_rPosition + i_rHeading;				
				_OutIt end_itr = GetAroundCreature(i_rPosition,i_nDist,i_rInputFirst,i_rInputEnd,o_rOutputFirst);
				std::vector<void*> t_filter(begin_itr, end_itr);
				_OutIt out_itr = GetAngleCreature(BackPoint,i_rHeading,i_nDist,i_nCosValue,t_filter.begin(),t_filter.end(),begin_itr);

				return out_itr;
			}
			break;
		default:
			{
				return o_rOutputFirst;
			}
		}
	}

	struct EntryDis
	{
		Entity* entry;
		uint32  dis; // 实体的相对比较距离 
	};

	// 距离比较函数 
	bool CompDistance(EntryDis &a,EntryDis &b)
	{
		return !(a.dis > b.dis);
	};

	// 对周围玩家距离排序 
	void sort_around(Creature *pFrom,std::vector<Entity*>::iterator i_begin,std::vector<Entity*>::iterator i_end,std::vector<Entity*> &o_setEnities,int32 num)
	{
		std::vector<EntryDis> vecEnities;

		const Point2D t_self = pFrom->Position();

		for (std::vector<Entity*>::iterator it = i_begin;it!=i_end;it++)
		{
			EntryDis t_entry;
			t_entry.entry = (*it);
			t_entry.dis =  DistanceSq(pFrom->Position(),(*it)->Position());

			if(vecEnities.size() >= num)
			{
				std::pop_heap(vecEnities.begin(),vecEnities.end(),CompDistance);
				vecEnities.push_back(t_entry);
				std::push_heap(vecEnities.begin(),vecEnities.end(),CompDistance);
			}
			else
			{
				vecEnities.push_back(t_entry);
			}

			if (vecEnities.size() == num)
			{
				std::make_heap(vecEnities.begin(),vecEnities.end(),CompDistance);
			}
		}

		for (std::vector<EntryDis>::iterator it = vecEnities.begin();it!=vecEnities.end();it++)
		{
			o_setEnities.push_back(it->entry);
		}
	}

	/*
	void sort_around(Creature *pFrom,std::vector<Entity*> &i_setEnities,std::vector<Entity*> &o_setEnities,int32 num)
	{
		std::vector<EntryDis> vecEnities;

		const Point2D t_self = pFrom->Position();

		for (std::vector<Entity*>::iterator it = i_setEnities.begin();it!=i_setEnities.end();it++)
		{
			EntryDis t_entry;
			t_entry.entry = (*it);
			t_entry.dis =  DistanceSq(pFrom->Position(),(*it)->Position());

			if(vecEnities.size() >= num)
			{
				std::pop_heap(vecEnities.begin(),vecEnities.end(),CompDistance);
				vecEnities.push_back(t_entry);
				std::push_heap(vecEnities.begin(),vecEnities.end(),CompDistance);
			}
			else
			{
				vecEnities.push_back(t_entry);
			}

			if (vecEnities.size() == num)
			{
				std::make_heap(vecEnities.begin(),vecEnities.end(),CompDistance);
			}
		}

		for (std::vector<EntryDis>::iterator it = vecEnities.begin();it!=vecEnities.end();it++)
		{
			o_setEnities.push_back(it->entry);
		}

	}
	*/
	
	int32 SetSpellEffectEntry(SceneUser* pFrom, SpellEx* pSpell)
	{
		/*for (uint32 i =0 ;i < MAX_SPELL_BUFF_NUM ;i++ )
		{
			if(pSpell->rSpellRes->arrBuff[i].nBuffID!=0)
			{
				switch (pSpell->rSpellRes->arrBuff[i].eAimType)
				{
				case EFFECT_AIM_TYPE_FROM:
					pFrom->AddBuff(pFrom,  pSpell->rSpellRes->arrBuff[i].nBuffID);
					break;
				case EFFECT_AIM_TYPE_FROM_RANGE:
					{
						SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)&pFrom));
						if (pScene  == NULL)
						{
							return false;
						}
						std::vector<Entity*>  around_player;
						pScene->GetNeighborsEntity(*pFrom,around_player);

						std::vector<Entity*> out_player(around_player.size());
						std::vector<Entity*>::const_iterator it_left = around_player.begin();
						std::vector<Entity*>::const_iterator it_right = around_player.end();
						Script::GetEffectCreature(pFrom->Position(),pFrom->Heading(),EFFECT_RANGE_TYPE_AROUND,pSpell->rSpellRes->arrBuff[i].nArea,pSpell->rSpellRes->arrBuff[i].nAngle,it_left,it_right,around_player.begin());
						for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
						{
							Character* pOther = (Character*)(*itr);
							pOther->AddBuff(pFrom, pSpell->rSpellRes->arrBuff[i].nBuffID);
						}
					}
					break;
				default:
					break;
				}
			}
		}*/
		return 0;
	}

	// 计算普通血量伤害 
	static int32 CalculateCommonRedEffect(Creature* pFrom, Creature* pTo, int32 nParam)
	{
		/*ValueType attr_flag_from  =0 ,spell_att = 0, physical_att = 0 , deatt =0 , phy_defense = 0,spell_defense = 0;

		ValueType gedanglv = 0,gedang = 0,defense=0;
		pFrom->GetAttribute(ATTRIBUTE_GEDANGLV,gedanglv);
		uint32 rand_gedang = twp::common::utility::RandMax32(10000);
		if (rand_gedang < gedanglv.nInt32)
		{
			pFrom->GetAttribute(ATTRIBUTE_GEDANG,gedang);
		}

		ValueType baojilv = 0,baoji = 0;
		pFrom->GetAttribute(ATTRIBUTE_BAOJILV,baojilv);
		uint32 rand_baoji = twp::common::utility::RandMax32(10000);
		if (rand_baoji < baojilv.nInt32)
		{
			pFrom->GetAttribute(ATTRIBUTE_BAOJI,baoji);
		}

		pFrom->GetAttribute(ATTRIBUTE_ATTACKTYPE,attr_flag_from);
		pFrom->GetAttribute(ATTRIBUTE_SPELLATTACK,spell_att);
		pFrom->GetAttribute(ATTRIBUTE_PHYSICATTACK,physical_att);
		pFrom->GetAttribute(ATTRIBUTE_PHYSICDEFEND,phy_defense);
		pFrom->GetAttribute(ATTRIBUTE_SPELLDEFEND,spell_defense);


		if (HAS_FLAG(attr_flag_from.nInt32, CREATURE_ATTACK_TYPE_PHYSIC))
		{
			pTo->GetAttribute(ATTRIBUTE_PHYSICDEFEND,defense);
		}
		else if (HAS_FLAG(attr_flag_from.nInt32, CREATURE_ATTACK_TYPE_SPELL))
		{
			pTo->GetAttribute(ATTRIBUTE_SPELLDEFEND,defense);
		}
		return (((physical_att.nInt32 + spell_att.nInt32/2 ) * (1+ baojilv.nInt32/10000 * baoji.nInt32 ) - (phy_defense.nInt32 + spell_defense.nInt32/2)) )/2;*/
		return 0;
	}

	// 计算技能血量伤害 
	static int32 CalculateSpellRedEffect(Creature* pFrom, Creature* pTo, int32 nParam)
	{
		ValueType  attr_flag_from;

		ValueType spell_att = 0, physical_att = 0 , deatt =0 , phy_defense = 0,spell_defense = 0;
		pFrom->GetEntityAttribute(CREATURE_ATTRIBUTE_ATTACKTYPE,attr_flag_from);
		pFrom->GetEntityAttribute(CREATURE_ATTRIBUTE_SPELLATTACK,spell_att);
		pFrom->GetEntityAttribute(CREATURE_ATTRIBUTE_PHYSICATTACK,physical_att);
		pFrom->GetEntityAttribute(CREATURE_ATTRIBUTE_PHYSICDEFEND,phy_defense);
		pFrom->GetEntityAttribute(CREATURE_ATTRIBUTE_SPELLDEFEND,spell_defense);
		/*
		ValueType gedanglv = 0,gedang = 0;
		pFrom->GetAttribute(ATTRIBUTE_GEDANGLV,gedanglv);
		uint32 rand_gedang = twp::common::utility::RandMax32(10000);
		if (rand_gedang < gedanglv.nInt32)
		{
			pFrom->GetAttribute(ATTRIBUTE_GEDANG,gedang);
		}

		ValueType baojilv = 0,baoji = 0;
		pFrom->GetAttribute(ATTRIBUTE_BAOJILV,baojilv);
		uint32 rand_baoji = twp::common::utility::RandMax32(10000);
		if (rand_baoji < baojilv.nInt32)
		{
			pFrom->GetAttribute(ATTRIBUTE_BAOJI,baoji);
		}
		*/
		return (physical_att.nInt32 /2  + spell_att.nInt32 )* nParam - (phy_defense.nInt32 /2 + spell_defense.nInt32) ;
		//ValueType attr_flag_from;
		//pFrom->GetAttribute(ATTRIBUTE_ATTACKTYPE, attr_flag_from);
		//uint32 gedang_val = 0, baoji_val =1 ,defense =0/*防御力全体减伤*/, hurt_reduce =0 ;/*防御对应攻击类型减伤*/
		//if (HAS_FLAG(attr_flag_from.nInt32,CREATURE_ATTACK_TYPE_Melee))
		//{
		//	hurt_reduce = pTo->GetAttribute(ATTRIBUTE_DEMELEEATTACK);
		//}
		//else if (HAS_FLAG(attr_flag_from.nInt32, CREATURE_ATTACK_TYPE_Remote))
		//{
		//	hurt_reduce = pTo->GetAttribute(ATTRIBUTE_DEREMOTEATTACK);
		//}
		//if (HAS_FLAG(attr_flag_from.nInt32, CREATURE_ATTACK_TYPE_PHYSIC))
		//{
		//	defense = pTo->GetAttribute(ATTRIBUTE_PHYSICDEFEND);
		//}
		//else if (HAS_FLAG(attr_flag_from.nInt32, CREATURE_ATTACK_TYPE_SPELL))
		//{
		//	defense = pTo->GetAttribute(ATTRIBUTE_SPELLDEFEND);
		//}

		////gedang 计算
		//uint32 rand_gedang = twp::common::utility::RandMax32(10000);
		//if (rand_gedang < pFrom->GetAttribute(ATTRIBUTE_GEDANGLV))
		//{
		//	gedang_val = pFrom->GetAttribute(ATTRIBUTE_GEDANG);
		//}

		//uint32 rand_baoji = twp::common::utility::RandMax32(10000);
		//if (rand_baoji < pFrom->GetAttribute(ATTRIBUTE_BAOJILV))
		//{
		//	baoji_val = pFrom->GetAttribute(ATTRIBUTE_BAOJI);
		//}

		//float part1 = (pFrom->GetAttribute(ATTRIBUTE_SPELLATTACK)+0.5 * pFrom->GetAttribute(ATTRIBUTE_PHYSICATTACK) - defense)*(baoji_val - gedang_val);

		//float part2 = (1 - pFrom->GetAttribute(ATTRIBUTE_DEATTACK)/10000) * (1 - hurt_reduce/10000) * nParam;

		//return part1*part2;
		return 0;
	}

	void ShouJi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_STATUES_FUNC(CREATURE_STATUS_SHOUJI,CREATURE_STATUS_DESHOUJI);
	}
	
	void JiDao(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_STATUES_FUNC(CREATURE_STATUS_JIDAO,CREATURE_STATUS_DEJIDAO);
	}

	void FuKong(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_STATUES_FUNC(CREATURE_STATUS_FUKONG,CREATURE_STATUS_DEFUKONG);
	}

	void JiTui(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				Point2D SelfPoint = pFrom->Position();
				Point2DEx SelfEx(SelfPoint);
				Point2D ToPoint= pTo->Position();
				Point2DEx ToEx(ToPoint);
				Vector2DEx DirVec = SelfPoint - ToPoint  ;
				DirVec.Normalize();
				pTo->SetPosition(Point2D(ToPoint - DirVec * nParam));
			}

			break;


		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					Point2D SelfPoint = pFrom->Position();
					Point2DEx SelfEx(SelfPoint);
					Point2D ToPoint= (*itr)->Position();
					Point2DEx ToEx(ToPoint);
					Vector2DEx DirVec = SelfPoint - ToPoint  ;
					DirVec.Normalize();
					pTo->SetPosition(Point2D(ToPoint - DirVec * nParam));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					Point2D SelfPoint = pFrom->Position();
					Point2DEx SelfEx(SelfPoint);
					Point2D ToPoint= (*itr)->Position();
					Point2DEx ToEx(ToPoint);
					Vector2DEx DirVec = SelfPoint - ToPoint  ;
					DirVec.Normalize();
					pTo->SetPosition(Point2D(ToPoint - DirVec * nParam));
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void MiWu(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_STATUES_FUNC(CREATURE_STATUS_MIWU,CREATURE_STATUS_DEMIWU);
	}

	void ShengMingXiuGaiPuTongGongJi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		//if (bReverse)
		//{
		//	return;
		//}
		//switch (eAimType)
		//{
		//case EFFECT_AIM_TYPE_FROM:
		//	{
		//		int32 hurt = CalculateCommonRedEffect(pFrom, pFrom, nParam);
		//		pFrom->SetAttribute(ATTRIBUTE_RED, ValueType(0 - hurt));
		//	}
		//	break;
		//case EFFECT_AIM_TYPE_TO:
		//	{
		//		int32 hurt = CalculateCommonRedEffect(pFrom, pTo, nParam);
		//		pTo->SetAttribute(ATTRIBUTE_RED, ValueType(0 - hurt));
		//	}

		//	break;
		//case EFFECT_AIM_TYPE_FROM_RANGE:
		//	{
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}
		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pFrom,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		std::vector<Entity*>::iterator  it_end = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());

		//		std::vector<Entity*> closet_player;
		//		sort_around(pFrom,out_player.begin(),it_end,closet_player,nEffectNum);

		//		int32 i=1;
		//		for (std::vector<Entity*>::iterator itr = closet_player.begin();itr != closet_player.end() ; itr++,i++)
		//		{
		//			Creature  *pReltivePlayer = (Creature*)(*itr);
		//			int32 hurt = CalculateCommonRedEffect(pFrom, pReltivePlayer, nParam);


		//			const DamageReduce *damage_info = g_cResourceMgr.get_damage_reduce_info(i);
		//			if (damage_info  = NULL)
		//			{
		//				damage_info = g_cResourceMgr.GetMaxDamageInfo();
		//				if (!damage_info)
		//				{
		//					ReduceRed(pReltivePlayer, hurt);
		//					return;
		//				}
		//			}
		//			ReduceRed(pReltivePlayer,damage_info->reduce_param/100 * hurt);

		//			//pReltivePlayer->SetAttribute(ATTRIBUTE_RED, ValueType(0 -damage_info->reduce_param * hurt));
		//		}
		//	}
		//	break;
		//case EFFECT_AIM_TYPE_TO_RANGE:
		//	{
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}

		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pTo,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		std::vector<Entity*>::iterator it_out = GetEffectCreature(pTo->Position(),pTo->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());

		//		std::vector<Entity*> closet_player;
		//		sort_around(pTo,out_player.begin(),it_out,closet_player,nEffectNum);
		//		int32 i=1;
		//		for (std::vector<Entity*>::iterator itr = closet_player.begin();itr != closet_player.end() ; itr++,i++)
		//		{
		//			Creature  *pReltivePlayer = (Creature*)(*itr);
		//			int32 hurt = CalculateCommonRedEffect(pFrom, pReltivePlayer, nParam);

		//			const DamageReduce *damage_info = g_cResourceMgr.get_damage_reduce_info(i);
		//			if (!damage_info)
		//			{
		//				damage_info = g_cResourceMgr.GetMaxDamageInfo();
		//				if (!damage_info)
		//				{
		//					ReduceRed(pReltivePlayer, hurt);
		//					return;
		//				}
		//			}
		//			//pReltivePlayer->SetAttribute(ATTRIBUTE_RED, ValueType(0 - damage_info->reduce_param/100*hurt));
		//			ReduceRed(pReltivePlayer,damage_info->reduce_param/100 * hurt);
		//		}
		//	}
		//	break;
		//default:
		//	break;
		//}
	}

	void ShengMingXiuGaiJiNengGongJi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		//if (bReverse)
		//{
		//	return;
		//}
		//switch (eAimType)
		//{
		//case EFFECT_AIM_TYPE_FROM:
		//	{
		//		int32 hurt = CalculateSpellRedEffect(pFrom, pFrom, nParam);
		//		pFrom->SetAttribute(ATTRIBUTE_RED, ValueType(0 - hurt));
		//	}

		//	break;
		//case EFFECT_AIM_TYPE_TO:
		//	{
		//		int32 hurt = CalculateSpellRedEffect(pFrom, pTo, nParam);
		//		pTo->SetAttribute(ATTRIBUTE_RED, ValueType(0 - hurt));
		//	}
		//	break;
		//case EFFECT_AIM_TYPE_FROM_RANGE:
		//	//获取map 相关玩家
		//	{			
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}
		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pFrom,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());

		//		std::vector<Entity*>::iterator  it_end = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());
		//		std::vector<Entity*> closet_player;
		//		sort_around(pFrom,out_player.begin(),it_end,closet_player,nEffectNum);
		//		int32 i = 0;
		//		for (std::vector<Entity*>::iterator itr = closet_player.begin();itr != closet_player.end() ; itr++,i++)
		//		{

		//			Creature  *pReltivePlayer = (Creature*)(*itr);
		//			int32 hurt = CalculateSpellRedEffect(pFrom, pReltivePlayer, nParam);
		//			pReltivePlayer->SetAttribute(ATTRIBUTE_RED, ValueType(0 - hurt));
		//		}
		//	}
		//case EFFECT_AIM_TYPE_TO_RANGE:
		//	{
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}

		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pTo,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		GetEffectCreature(pTo->Position(),pTo->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());

		//		std::vector<Entity*>::iterator  it_end = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());
		//		std::vector<Entity*> closet_player;
		//		sort_around(pFrom,out_player.begin(),it_end,closet_player,nEffectNum);
		//		for (std::vector<Entity*>::iterator itr = closet_player.begin();itr != closet_player.end() ; itr++)
		//		{
		//			Creature  *pReltivePlayer = (Creature*)(*itr);
		//			int32 hurt = CalculateCommonRedEffect(pFrom, pReltivePlayer, nParam);
		//			pReltivePlayer->SetAttribute(ATTRIBUTE_RED, ValueType(0 - hurt));
		//		}
		//	}
		//}
	}

	void ShengMingXiuGaiZiJieYiChu(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		//if (bReverse)
		//{
		//	return;
		//}
		//switch (eAimType)
		//{
		//case EFFECT_AIM_TYPE_FROM:
		//	{
		//		pFrom->SetAttribute(ATTRIBUTE_RED, ValueType(0 - nParam));
		//	}

		//	break;
		//case EFFECT_AIM_TYPE_TO:
		//	{
		//		pTo->SetAttribute(ATTRIBUTE_RED,ValueType(0 - nParam));
		//	}
		//	break;
		//case EFFECT_AIM_TYPE_FROM_RANGE:
		//	//获取map 相关玩家
		//	{			
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}
		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pFrom,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());

		//		std::vector<Entity*>::iterator  it_end = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());
		//		std::vector<Entity*> closet_player;
		//		sort_around(pFrom,out_player.begin(),it_end,closet_player,nEffectNum);
		//		int32 i = 0;
		//		for (std::vector<Entity*>::iterator itr = closet_player.begin();itr != closet_player.end() ; itr++,i++)
		//		{

		//			Creature  *pReltivePlayer = (Creature*)(*itr);
		//			pReltivePlayer->SetAttribute(ATTRIBUTE_RED, ValueType(0 - nParam));
		//		}
		//	}
		//case EFFECT_AIM_TYPE_TO_RANGE:
		//	{
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}

		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pTo,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		GetEffectCreature(pTo->Position(),pTo->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());

		//		std::vector<Entity*>::iterator  it_end = GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,out_player.begin());
		//		std::vector<Entity*> closet_player;
		//		sort_around(pFrom,out_player.begin(),it_end,closet_player,nEffectNum);
		//		for (std::vector<Entity*>::iterator itr = closet_player.begin();itr != closet_player.end() ; itr++)
		//		{
		//			Creature  *pReltivePlayer = (Creature*)(*itr);
		//			pReltivePlayer->SetAttribute(ATTRIBUTE_RED, ValueType(0 - nParam));
		//		}
		//	}
		//}
	}
	
	void ChenMo(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_STATUES_FUNC(CREATURE_STATUS_CHENMO,CREATURE_STATUS_DECHENMO);
	}

	void DingShen(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_STATUES_FUNC(CREATURE_STATUS_DINGSHEN,CREATURE_STATUS_DEDINGSHEN);
	}

	void HunLuan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_STATUES_FUNC(CREATURE_STATUS_HUNLUAN,CREATURE_STATUS_DEHUNLUAN);
	}

	void XuanYun(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_STATUES_FUNC(CREATURE_STATUS_XUANYUN,CREATURE_STATUS_DEXUANYUN);
	}

	void LiuXie(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		//暂无该buffer
	}

	void HuanMan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{

		//switch (eAimType)
		//{
		//case EFFECT_AIM_TYPE_FROM:
		//	pFrom->SetAttribute(ATTRIBUTE_SPEED,(0-nParam));
		//	break;
		//case EFFECT_AIM_TYPE_TO:
		//	pTo->SetAttribute(ATTRIBUTE_SPEED,(0-nParam));
		//	break;
		//case EFFECT_AIM_TYPE_FROM_RANGE:
		//	//获取map 相关玩家
		//	{			
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}
		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pFrom,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		std::vector<Entity*>::iterator it_out= out_player.begin();
		//		GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
		//		for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
		//		{
		//			(*itr)->SetAttribute(ATTRIBUTE_SPEED,(0-nParam));
		//		}
		//	}
		//	break;
		//case EFFECT_AIM_TYPE_TO_RANGE:
		//	{
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}

		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pTo,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		std::vector<Entity*>::iterator it_out= out_player.begin();
		//		GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
		//		for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
		//		{
		//			(*itr)->SetAttribute(ATTRIBUTE_SPEED,(0-nParam));
		//		}
		//	}
		//	break;
		//default:
		//	break;
		//}
	}

	void XiYin(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			//pFrom->StatusAdd(CREATURE_STATUS_XUANYUN);
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				Point2D SelfPoint = pFrom->Position();
				Point2DEx SelfEx(SelfPoint);
				Point2D ToPoint= pTo->Position();
				Point2DEx ToEx(ToPoint);
				Vector2DEx DirVec = ToEx - SelfEx  ;
				DirVec.Normalize();
				pTo->SetPosition(Point2D(ToPoint - DirVec * nParam));
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					Point2D SelfPoint = pFrom->Position();
					Point2DEx SelfEx(SelfPoint);
					Point2D ToPoint= pTo->Position();
					Point2DEx ToEx(ToPoint);
					Vector2DEx DirVec = ToEx - SelfEx  ;
					DirVec.Normalize();
					(*itr)->SetPosition(Point2D(ToPoint - DirVec * nParam));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					Point2D SelfPoint = pFrom->Position();
					Point2DEx SelfEx(SelfPoint);
					Point2D ToPoint= pTo->Position();
					Point2DEx ToEx(ToPoint);
					Vector2DEx DirVec = ToEx - SelfEx;
					DirVec.Normalize();
					(*itr)->SetPosition(Point2D(ToPoint - DirVec * nParam));
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void KangShouji(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DESHOUJI)
	}

	void KangJiDao(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DEJIDAO)
	}

	void KangFuKong(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DEFUKONG)
	}

	void KangMiWu(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DEMIWU)
	}

	void KangDingShen(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DEDINGSHEN)
	}

	void KangHunLuan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DEHUNLUAN)
	}

	void KangXuanYun(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DEXUANYUN)
	}

	void KangHuanMan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DEHUANMAN)
	}

	void KangJiTui(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DEJITUI)
	}

	void KangChenMo(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		BUFFER_SCRIPT_DESTATUES_FUNC(CREATURE_STATUS_DECHENMO)
	}

	void WuGong(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_PHYSICATTACK);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_PHYSICATTACK);
		
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
			ValueType att_val;
			pFrom->GetAttribute(ATTRIBUTE_PHYSICATTACK,att_val);
			pFrom->SetAttribute(ATTRIBUTE_PHYSICATTACK,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{		
			ValueType att_val;
			pTo->GetAttribute(ATTRIBUTE_PHYSICATTACK,att_val);
			pTo->SetAttribute(ATTRIBUTE_PHYSICATTACK,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_PHYSICATTACK,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_PHYSICATTACK,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_PHYSICATTACK,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_PHYSICATTACK,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void NeiGong(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_SPELLATTACK);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_SPELLATTACK);

		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
			ValueType att_val;
			pFrom->GetAttribute(ATTRIBUTE_SPELLATTACK,att_val);
			pFrom->SetAttribute(ATTRIBUTE_SPELLATTACK,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
			ValueType att_val;
			pTo->GetAttribute(ATTRIBUTE_SPELLATTACK,att_val);
			pTo->SetAttribute(ATTRIBUTE_SPELLATTACK,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_SPELLATTACK,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_SPELLATTACK,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_SPELLATTACK,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_SPELLATTACK,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void ShengMing(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_RED);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_RED);
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_RED,att_val);
				pFrom->SetAttribute(ATTRIBUTE_RED,(nParam/10000+1) * att_val.nInt32);
			}

			break;
		case EFFECT_AIM_TYPE_TO:
			{		
				ValueType att_val;
				pTo->GetAttribute(ATTRIBUTE_RED,att_val);
				pTo->SetAttribute(ATTRIBUTE_RED,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_RED,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_RED,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_RED,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_RED,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void WuFang(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_PHYSICDEFEND);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_PHYSICDEFEND);

		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_PHYSICDEFEND,att_val);
				pFrom->SetAttribute(ATTRIBUTE_PHYSICDEFEND,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
			ValueType att_val;
			pTo->GetAttribute(ATTRIBUTE_PHYSICDEFEND,att_val);
			pTo->SetAttribute(ATTRIBUTE_PHYSICDEFEND,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_PHYSICDEFEND,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_PHYSICDEFEND,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_PHYSICDEFEND,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_PHYSICDEFEND,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void NeiFang(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_SPELLDEFEND);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_SPELLDEFEND);
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_SPELLDEFEND,att_val);
				pFrom->SetAttribute(ATTRIBUTE_SPELLDEFEND,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				ValueType att_val;
				pTo->GetAttribute(ATTRIBUTE_SPELLDEFEND,att_val);
				pTo->SetAttribute(ATTRIBUTE_SPELLDEFEND,(nParam/10000+1) * att_val.nInt32);
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_SPELLDEFEND,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_SPELLDEFEND,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_SPELLDEFEND,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_SPELLDEFEND,(nParam/10000+1) * att_val.nInt32);
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void DaDuan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		//if(bReverse)
		//{
		//	return;
		//}
		//switch (eAimType)
		//{
		//case EFFECT_AIM_TYPE_FROM:
		//	pFrom->InterruptBuff();
		//	break;
		//case EFFECT_AIM_TYPE_TO:
		//	pTo->InterruptBuff();
		//	break;
		//case EFFECT_AIM_TYPE_FROM_RANGE:
		//	//获取map 相关玩家
		//	{			
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}
		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pFrom,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		std::vector<Entity*>::iterator it_out= out_player.begin();
		//		GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
		//		for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
		//		{
		//			Creature *p = (Creature*)(*itr);
		//			p->InterruptBuff();
		//		}
		//	}
		//	break;
		//case EFFECT_AIM_TYPE_TO_RANGE:
		//	{
		//		SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
		//		if (pScene  == NULL)
		//		{
		//			return;
		//		}

		//		std::vector<Entity*>  around_player;
		//		pScene->GetNeighborsEntity(*pTo,around_player);

		//		std::vector<Entity*> out_player(around_player.size());
		//		std::vector<Entity*>::const_iterator it_left = around_player.begin();
		//		std::vector<Entity*>::const_iterator it_right = around_player.end();

		//		std::vector<Entity*>::iterator it_out= out_player.begin();
		//		GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
		//		for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
		//		{
		//			Creature *p = (Creature*)(*itr);
		//			p->InterruptBuff();
		//		}
		//	}
		//	break;
		//default:
		//	break;
		//}
	}

	void BaoJiLv(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_BAOJILV);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_BAOJILV);

		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_BAOJILV,att_val);
				pFrom->SetAttribute(ATTRIBUTE_BAOJILV,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				ValueType att_val;
				pTo->GetAttribute(ATTRIBUTE_BAOJILV,att_val);
				pTo->SetAttribute(ATTRIBUTE_BAOJILV,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_BAOJILV,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_BAOJILV,(nParam + att_val.nInt32));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_BAOJILV,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_BAOJILV,(nParam + att_val.nInt32));
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void ShanBiLv(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_SHANBILV);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_SHANBILV);
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_SHANBILV,att_val);
				pFrom->SetAttribute(ATTRIBUTE_SHANBILV,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				ValueType att_val;
				pTo->GetAttribute(ATTRIBUTE_SHANBILV,att_val);
				pTo->SetAttribute(ATTRIBUTE_SHANBILV,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_SHANBILV,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_SHANBILV,(nParam + att_val.nInt32));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_SHANBILV,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_SHANBILV,(nParam + att_val.nInt32));
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void GeDangLv(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_GEDANGLV);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_GEDANGLV);
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_GEDANGLV,att_val);
				pFrom->SetAttribute(ATTRIBUTE_GEDANGLV,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				ValueType att_val;
				pTo->GetAttribute(ATTRIBUTE_GEDANGLV,att_val);
				pTo->SetAttribute(ATTRIBUTE_GEDANGLV,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_GEDANGLV,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_GEDANGLV,(nParam + att_val.nInt32));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_GEDANGLV,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_GEDANGLV,(nParam + att_val.nInt32));
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void MingZhongLv(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_MINGZHONGLV);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_MINGZHONGLV);
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_MINGZHONGLV,att_val);
				pFrom->SetAttribute(ATTRIBUTE_MINGZHONGLV,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				ValueType att_val;
				pTo->GetAttribute(ATTRIBUTE_MINGZHONGLV,att_val);
				pTo->SetAttribute(ATTRIBUTE_MINGZHONGLV,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_MINGZHONGLV,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_MINGZHONGLV,(nParam + att_val.nInt32));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_MINGZHONGLV,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_MINGZHONGLV,(nParam + att_val.nInt32));
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	void BaojiZhi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_BAOJI);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_BAOJI);
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_BAOJI,att_val);
				pFrom->SetAttribute(ATTRIBUTE_BAOJI,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				ValueType att_val;
				pTo->GetAttribute(ATTRIBUTE_BAOJI,att_val);
				pTo->SetAttribute(ATTRIBUTE_BAOJI,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_BAOJI,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_BAOJI,(nParam + att_val.nInt32));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_BAOJI,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_BAOJI,(nParam + att_val.nInt32));
				}
			}
			break;
		default:
			break;
		}
		*/
	}
	void KangLiuXue(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{}
	void GeDangZhi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse)
	{
		if(bReverse)
		{
			ReverseCommonAttribute(oi_nReverseVal, CREATURE_ATTRIBUTE_GEDANG);
			return;
		}
		BufferCommonAttribute(pFrom,pTo,eAimType,eRangeType,nParam,nDist,nCosValue,nEffectNum,oi_nReverseVal, CREATURE_ATTRIBUTE_GEDANG);
		/*
		switch (eAimType)
		{
		case EFFECT_AIM_TYPE_FROM:
			{
				ValueType att_val;
				pFrom->GetAttribute(ATTRIBUTE_GEDANG,att_val);
				pFrom->SetAttribute(ATTRIBUTE_GEDANG,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_TO:
			{
				ValueType att_val;
				pTo->GetAttribute(ATTRIBUTE_GEDANG,att_val);
				pTo->SetAttribute(ATTRIBUTE_GEDANG,(nParam + att_val.nInt32));
			}
			break;
		case EFFECT_AIM_TYPE_FROM_RANGE:
			//获取map 相关玩家
			{			
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pFrom));
				if (pScene  == NULL)
				{
					return;
				}
				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pFrom,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_GEDANG,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_GEDANG,(nParam + att_val.nInt32));
				}
			}
			break;
		case EFFECT_AIM_TYPE_TO_RANGE:
			{
				SceneMap *pScene = g_rSceneMapManager.GetEntitySceneMap(*((Entity*)pTo));
				if (pScene  == NULL)
				{
					return;
				}

				std::vector<Entity*>  around_player;
				pScene->GetNeighborsEntity(*pTo,around_player);

				std::vector<Entity*> out_player(around_player.size());
				std::vector<Entity*>::const_iterator it_left = around_player.begin();
				std::vector<Entity*>::const_iterator it_right = around_player.end();

				std::vector<Entity*>::iterator it_out= out_player.begin();
				GetEffectCreature(pFrom->Position(),pFrom->Heading(),eRangeType,nDist,nCosValue,it_left,it_right,it_out);
				for (std::vector<Entity*>::iterator itr = out_player.begin();itr != out_player.end() ; itr++)
				{
					ValueType att_val;
					(*itr)->GetAttribute(ATTRIBUTE_GEDANG,att_val);
					(*itr)->SetAttribute(ATTRIBUTE_GEDANG,(nParam + att_val.nInt32));
				}
			}
			break;
		default:
			break;
		}
		*/
	}

	BuffEffect arrBuffEffect[BUFF_EFFECT_TYPE_MAX + 1] =
	{
		{BuffEffectType(0),				0,		 0			},
		//{BuffEffectType(1),				0, ShouJi			},
		//{BuffEffectType(2), 			0, JiDao 			},
		//{BuffEffectType(3), 			0, FuKong			},
		//{BuffEffectType(4), 			0, JiTui 			},
		//{BuffEffectType(5),				0, MiWu 			},
		//{BuffEffectType(6), 			0, ChenMo			},
		//{BuffEffectType(7), 			0, DingShen			},
		//{BuffEffectType(8), 			0, HunLuan			},
		//{BuffEffectType(9), 			0, XuanYun			},
		//{BuffEffectType(10), 			0, LiuXie			},
		//{BuffEffectType(11), 			0, HuanMan			},
		//{BuffEffectType(12), 			0, XiYin			},
		//{BuffEffectType(13), 			0, KangShouji		},
		//{BuffEffectType(14), 			0, KangJiDao		},
		//{BuffEffectType(15), 			0, KangFuKong		},
		//{BuffEffectType(16), 			0, KangMiWu			},
		//{BuffEffectType(17), 			0, KangChenMo		},
		//{BuffEffectType(18), 			0, KangDingShen		},
		//{BuffEffectType(19), 			0, KangHunLuan		},
		//{BuffEffectType(20), 			0, KangXuanYun		},
		//{BuffEffectType(21), 			0, KangHuanMan		},
		//{BuffEffectType(22), 			0, KangLiuXue		},
		//{BuffEffectType(23), 			0, BaoJiLv			},
		//{BuffEffectType(24), 			0, ShanBiLv			},
		//{BuffEffectType(25), 			0, GeDangLv			},
		//{BuffEffectType(26), 			0, MingZhongLv		},
		//{BuffEffectType(27), 			0, BaojiZhi			},
		//{BuffEffectType(28), 			0, GeDangZhi		},
		//{BuffEffectType(29), 			0, WuGong			},
		//{BuffEffectType(30), 			0, NeiGong			},		
		//{BuffEffectType(31), 			0, ShengMing		},
		//{BuffEffectType(32), 			0, WuFang			},
		//{BuffEffectType(33), 			0, NeiFang			},
		//{BuffEffectType(34), 			0, ShengMingXiuGaiZiJieYiChu	},
		//{BuffEffectType(35), 			0, DaDuan			},
		//{BuffEffectType(36), 			0, ShengMingXiuGaiPuTongGongJi	},
		//{BuffEffectType(37), 			0, KangJiTui		},
		//{BuffEffectType(38), 			0, ShengMingXiuGaiJiNengGongJi	},
	};
}

#endif