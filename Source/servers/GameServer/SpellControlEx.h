#ifndef __SPELLCONTROLEX_H_
#define __SPELLCONTROLEX_H_

#include "SpellEx.h"
#define  MAX_SPELL_POSITION_NUM 6


class SceneUser;
/*
 *
 *	Detail: 技能控制器类 
 *   
 *  Created by hzd 2015-1-8
 *
 */

class SpellControlEx
{
public:

	SpellControlEx(SceneUser& rCharacter);

	~SpellControlEx(void);


	////加载职业技能 
	//void LoadJobSpell();

	//// 加载技能数据 
	////void Load(const std::vector<uint32>& vecSpellID);

	//void Load(const uint32 *pSpell,uint32 nCount);

	//// 加载位置信息 
	//void LoadPosition(const CharacterSpellPosition& rSpellPosition);

	//// 添加一个技能 
	//void AddSpell(uint32 nSpellID);

	//// 删除一个技能 
	//void DelSpell(uint32 sSpellID);

	//// 保存数据位置到数据库用） 
	//void SaveSpellPositionData();

	//// 技能更新(CD更新) nDelay为毫秒,为施放中的技能更新持续时间 
	//void Update(int32 nDelay);

	//// 设置为施放该技能,true成功，false失败 
	//bool SpellDo(uint32 nSpellID);

	////	获取技能 
	//SpellEx* GetSpell(uint32 nSpellID);

	////设置技能位置 
	//bool SetSpell(uint32 nPos,uint32 nSpellID);

	//// 学习技能 
	//bool LearnSpell(uint32 nSpellID);

	//// 发送技能列表 
	//void SendSpellList();
private:

	//// 是否CD中 
	//bool IsCloding(const SpellEx& rSpellEx, bool bAll = true /* 是否与所有的CD有影响 */);

	//// true在cd  
	//bool IsCD(uint32 nSpellID);

	//// 是否足够蓝（恕气） 
	//bool IsEnoughBlue(const SpellEx& rSpellEx);

	//// 是否携带 
	//bool IsCarried(const SpellEx& rSpellEx);

	//// 是否被限制中(受击，沉默中，定身，倒地，浮空，混乱，吸引) 
	//bool IsLimitting();

private:

	uint32					m_nLastSpellID;						// 用于前置技能判断 
	SceneUser&				m_rCharactter;						// 所属角色引用 
	uint32					nPosition[MAX_SPELL_POSITION_NUM];	//对应位置的技能 	 		
	std::vector<SpellEx>	m_vecSpellData;						// 玩家拥有的技能数据 

};


#endif

