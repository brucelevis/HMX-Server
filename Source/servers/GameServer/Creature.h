#ifndef __CREATURE_H_
#define __CREATURE_H_

#include <map>

#include "Entity.h"
#include "UnorderedSet.h"
#include "BaseSingle.h"

struct Buff;

class Creature : public Entity, public IChannelOperate(ChannelCreature), public IChannelOperate(ChannelSpell) , public IChannelOperate(ChannelBuff)
{
public:
	Creature(EntityType eEntityType,uint32 nEntityTypeID,bool bListen,int32 registChannelFlags);

	bool SetCreatureAttribute(CreatureAttributeType eAttributeType, const ValueType& vNewValue);
	bool GetCreatureAttribute(CreatureAttributeType i_eAttributeType, ValueType& o_vValue);
	int32 GetCreatureAttributeInt32(CreatureAttributeType i_eAttributeType);
	int64 GetCreatureAttributeInt64(CreatureAttributeType i_eAttributeType);
	bool SetCreatureAttribute(CreatureAttributeType eAttributeType, const ValueType& vNewValue, bool bEnforce);

private:
	void CreatureInitAttributeOffet();

private:
	CreatureAttribute	m_sAttribute;	// 生物体属性 
	AttributeOffset		m_sAttributeOffset[CREATURE_ATTRIBUTE_MAX];// 

public:

	// 添加Buff 
	void AddBuff(Creature* pFrom, int32 nBuffID);

	// 加载Buff 
	void LoadBuff(Creature* pFrom, int32 nBuffID,int32 nTimeElapse);

	// 更新生物体上的所有Buff(nTimeElapse单位:毫秒) 
	void UpdateBuff(int32 nTimeElapse);

	// 打断生物体上可被打断的Buff 
	void InterruptBuff();

	// 抵抗（删除）某个buff 
	void RemoveBuff(int32 nBuffID,bool bIsAll/* false移除第一个,true移除所有这种 */);

	// 添加技能到释放队列 
	void AddSpellQuene(uint32 nSpellID);
protected:

	// 添加Buff 
	void AddBuff(Creature* pFrom, int32 nBuffID,int32 nTimeElapse);

protected:

	std::multimap<int32, Buff*>		m_mapBuffs;					// 身上所带Buff 
	std::queue<uint32>				m_queSpell;					// 技能施放队列 

	uint64							m_nFlagsForClient;			// 属性变化的位标记 

private:

	void OnStatusChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnRedChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnRedMaxChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnBlueChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnBlueMaxChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnLevelChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnSpeedChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnRedRecoverChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnBlueRecoverChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnPhysiAttackChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnPhysicDefendChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnSpellAttackChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnSpellDefendChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnWuliChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnShenshouChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnNeiliChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnAttackTypeChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnMingzhonglvChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnShanbilvChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnBaojiChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnBaojilvChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnGedangChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnGedanglvChange(const ValueType& vOldValue, const ValueType& vNewValue);
	//void OnDeAttackChange(const ValueType& vOldValue, const ValueType& vNewValue);
	//void OnDeMeleeAttackChange(const ValueType& vOldValue, const ValueType& vNewValue);
	//void OnDeRemoteAttackChange(const ValueType& vOldValue, const ValueType& vNewValue);
	//void OnDeSpellCDChange(const ValueType& vOldValue, const ValueType& vNewValue);
	//void OnAttackRedRecoverChange(const ValueType& vOldValue, const ValueType& vNewValue);
	//void OnAttackBlueRecoverChange(const ValueType& vOldValue, const ValueType& vNewValue);

	// 给Buff频道添加一个Buff 
	void AddChannelBuff(uint32 nBuffID);
	// 给Buff频道删除一个Buff 
	void DelChannelBuff(uint32 nBuffID);

};


class CreatureManager : public BaseSingle<CreatureManager>
{
	typedef UNORDERED_SET<Creature*> CreatureUSetType;
public:
	CreatureManager(){}
	~CreatureManager(){}
	
	// 生物体添加 
	void AddCreature(Creature& rCreature);

	// 生物体消失 
	void DelCreature(Creature& rCreature);

	// 更新(nTimeElapse单位:毫秒) 
	void Update(int32 nTimeElapse);

private:
	
	CreatureUSetType	m_usetCreaturies;			// 所有生物体容器 

};

#endif
