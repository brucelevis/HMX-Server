#include "Entity.h"

// 生成一个自增的实体ID  
static uint64 IncreaseEntityID()
{
	static uint64 nIncreaseEnitiyId = 0; // 实体自增ID  
	++nIncreaseEnitiyId;
	if(nIncreaseEnitiyId == 0)
		nIncreaseEnitiyId = 1;
	return nIncreaseEnitiyId;
}

Entity::Entity(EntityType eEntityType,int32 nTypeID,bool bListen, int32 nRegistChannelFlags)
	: m_nID(0), m_bListen(bListen), m_nChannelRegistFlag(nRegistChannelFlags | E_CHANNEL_REGIST_TYPE_POS_R)
{
	m_nID = IncreaseEntityID();
	m_sChannelPosition.nID = m_nID;
	m_sChannelPosition.nType = eEntityType;
	m_sChannelPosition.nTypeID = nTypeID;
	EntityInitAttributeOffet();
}

bool Entity::SetEntityAttribute(EntityAttributeType eAttributeType, const ValueType& vNewValue)
{
	// 检测是否有偏移值   
	if (m_sAttributeOffset[eAttributeType].nOffset && m_sAttributeOffset[eAttributeType].bCanSet)
	{
		SetEntityAttribute(eAttributeType, vNewValue, false);
		return true;
	}
	return false;
}

bool Entity::SetEntityAttribute(EntityAttributeType eAttributeType, const ValueType& vNewValue, bool bEnforce)
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

bool Entity::GetEntityAttribute(EntityAttributeType i_eAttributeType, ValueType& o_vValue)
{
	// 检测是否有偏移值 
	if (m_sAttributeOffset[i_eAttributeType].nOffset)
	{
		memcpy(&o_vValue, m_sAttributeOffset[i_eAttributeType].nOffset, m_sAttributeOffset[i_eAttributeType].nSize);
		return true;
	}
	return false;
}

int32 Entity::GetEntityAttributeInt32(EntityAttributeType i_eAttributeType)
{
	ValueType o_vValue;
	GetEntityAttribute(i_eAttributeType,o_vValue);
	return o_vValue.nInt32;
}

int64 Entity::GetEntityAttributeInt64(EntityAttributeType i_eAttributeType)
{
	ValueType o_vValue;
	GetEntityAttribute(i_eAttributeType,o_vValue);
	return o_vValue.nInt64;
}


void Entity::EntityInitAttributeOffet()
{
	// 清空偏移值 
	memset(m_sAttributeOffset, 0, sizeof(m_sAttributeOffset));

#define	INIT_ATTRIBUTE_OFFSET(index, name, set, handler)	\
					{\
			m_sAttributeOffset[index].nOffset = &(this->name);\
			m_sAttributeOffset[index].nSize = sizeof(name);\
			m_sAttributeOffset[index].bCanSet = set;\
			m_sAttributeOffset[index].hHandler = handler;\
					}

	// 偏移信息 
	INIT_ATTRIBUTE_OFFSET(ENTITY_ATTRIBUTE_MAPID, m_sAttribute.nMapID, true, NULL);
	INIT_ATTRIBUTE_OFFSET(ENTITY_ATTRIBUTE_POS_X, m_sAttribute.sPos.nX, true, NULL);
	INIT_ATTRIBUTE_OFFSET(ENTITY_ATTRIBUTE_POS_Y, m_sAttribute.sPos.nY, true, NULL);
	INIT_ATTRIBUTE_OFFSET(ENTITY_ATTRIBUTE_INVIEWRANGE, m_sAttribute.nInViewRange, true, boost::bind(&Entity::OnInViewChange, this, _1, _2));
	INIT_ATTRIBUTE_OFFSET(ENTITY_ATTRIBUTE_OUTVIEWRANGE, m_sAttribute.nInViewRange, true, boost::bind(&Entity::OnOutViewChange, this, _1, _2));

#undef INIT_ATTRIBUTE_OFFSET
}

void Entity::OnInViewChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	// 相差半径值 
	int32 nLess = vNewValue.nInt32 - vOldValue.nInt32;
	m_sInViewRange.sTopLeft.nX		= m_sAttribute.sPos.nX - nLess;
	m_sInViewRange.sTopLeft.nY		= m_sAttribute.sPos.nY - nLess;
	m_sInViewRange.sBottomRight.nX	= m_sAttribute.sPos.nX + nLess;
	m_sInViewRange.sBottomRight.nY	= m_sAttribute.sPos.nY + nLess;
}

void Entity::OnOutViewChange(const ValueType& vOldValue, const ValueType& vNewValue)
{
	// 相差半径值 
	int32 nLess = vNewValue.nInt32 - vOldValue.nInt32;
	m_sOutViewRangle.sTopLeft.nX		= m_sAttribute.sPos.nX - nLess;
	m_sOutViewRangle.sTopLeft.nY		= m_sAttribute.sPos.nY - nLess;
	m_sOutViewRangle.sBottomRight.nX	= m_sAttribute.sPos.nX + nLess;
	m_sOutViewRangle.sBottomRight.nY	= m_sAttribute.sPos.nY + nLess;
}

