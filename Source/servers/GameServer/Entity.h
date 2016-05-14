#ifndef __ENTITY_H_
#define __ENTITY_H_

#include <bitset>
#include <assert.h>

#include "ObjPool.h"
#include "UnorderedSet.h"
#include "def_entity.h"
#include "Geometry2D.h"
#include "SceneChannel.h"

class SceneRegion;
class SceneMap;

/*------------------------------------------------------------------
 *
 * @Brief : 实体父类，指在地图上可存在的实体，如NPC，Buff,玩家，怪物等
 *			它们都会有实际的位置，同时也会在场景中出现，所以需要继承场景
 *			频道注册接口，需要实现位置频道接口
 * @Author: hzd 
 * @File  : Entity.h
 * @Date  : 2015/09/19 00:16
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
class Entity : public IChannelRegist, public IChannelOperate(ChannelPosition)
{
public:
	Entity(EntityType eEntityType,int32 nResId,bool bListen, int32 nRegistChannelFlags);

	/*--------------------------------------------
	 *  @brief    :	实现注册场景
	 *  @input	  : eType为所在场景传入的可提供的频道类型
	 *  @return   :	
	 *-------------------------------------------*/
	virtual bool IsRegist(EChannelRegistType eType) const
	{
		return (m_nChannelRegistFlag & eType) == eType;
	}

	const uint64 ID() const
	{
		return m_nID;
	}

	const Vector2DEx& Heading() const
	{
		return m_sHeading;
	}

	void SetHeading(const Vector2D& sHeading)
	{
		// 取保正方向为单位向量 
		m_sHeading = sHeading;
		m_sHeading.Normalize();
		m_sChannelPosition.nHeadingX = sHeading.nX;
		m_sChannelPosition.nHeadingZ = sHeading.nY;
	}

	const Point2D& GetPosition() const
	{
		return m_sAttribute.sPos;
	}

	void SetPosition(const Point2D& sPosition)
	{
		m_sAttribute.sPos = sPosition;
		int32 nX = m_sAttribute.sPos.nX;
		int32 nY = m_sAttribute.sPos.nY;
		int32 nInViewRange = m_sAttribute.nInViewRange;
		int32 nOutViewRange = m_sAttribute.nOutViewRangle;

		m_sChannelPosition.nPositionX = nX;
		m_sChannelPosition.nPositionZ = nY;

		m_sInViewRange.sTopLeft.nX		= nX - nInViewRange;
		m_sInViewRange.sTopLeft.nY		= nY - nInViewRange;
		m_sInViewRange.sBottomRight.nX	= nX + nInViewRange;
		m_sInViewRange.sBottomRight.nY	= nY + nInViewRange;

		m_sOutViewRangle.sTopLeft.nX		= nX - nOutViewRange;
		m_sOutViewRangle.sTopLeft.nY		= nY - nOutViewRange;
		m_sOutViewRangle.sBottomRight.nX	= nX + nOutViewRange;
		m_sOutViewRangle.sBottomRight.nY	= nY + nOutViewRange;

	}

	const RectangleRange& InViewRange() const
	{
		return m_sInViewRange;
	}

	const  RectangleRange& OutViewRange() const
	{
		return m_sOutViewRangle;
	}

	bool IsListen() const
	{
		return m_bListen;
	}

	// 属性修改 
	bool SetEntityAttribute(EntityAttributeType eAttributeType, const ValueType& vNewValue);
	bool GetEntityAttribute(EntityAttributeType i_eAttributeType, ValueType& o_vValue);
	int32 GetEntityAttributeInt32(EntityAttributeType i_eAttributeType);
	int64 GetEntityAttributeInt64(EntityAttributeType i_eAttributeType);
	bool SetEntityAttribute(EntityAttributeType eAttributeType, const ValueType& vNewValue, bool bEnforce);

private:
	void EntityInitAttributeOffet();

private:
	
	EntityAttribute		m_sAttribute;				// 位置坐标，视野范围 
	AttributeOffset		m_sAttributeOffset[ENTITY_ATTRIBUTE_MAX];// 

	////////////////////////////////地图方面的信息//////////////////////////////////////////////

public:
	SceneMap* GetSceneMap() { return m_pSceneMap; }
	

private:

	friend class SceneMap;
	void SetSceneMap(SceneMap* pSceneMap) 
	{
		m_pSceneMap = pSceneMap;
	}

	// 进入视野范围修改 
	void OnInViewChange(const ValueType& vOldValue, const ValueType& vNewValue);

	// 离开视野范围修改 
	void OnOutViewChange(const ValueType& vOldValue, const ValueType& vNewValue);

private:

	SceneMap*				m_pSceneMap;				// 所在的场景地图 

	uint64					m_nID;						// 实体ID	
	
	Vector2DEx				m_sHeading;					// 朝向(保证朝向为单位向量) 
	RectangleRange			m_sInViewRange;				// 进入视野范围 
	RectangleRange			m_sOutViewRangle;			// 离开视野范围 

	const bool				m_bListen;					// 是否监听频道 
	int64					m_nChannelRegistFlag;		// 注册标志 
};

#endif

