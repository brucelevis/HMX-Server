#ifndef __DEF_ENTITY_H_
#define __DEF_ENTITY_H_

#include "CommonDefine.h"
#include "Geometry2D.h" 

// 实体类型 
enum EntityType
{
	ENTITY_TYPE_NPC			= 1,
	ENTITY_TYPE_PLAYER		= 2,
	ENTITY_TYPE_MASTER		= 3,
	ENTITY_TYPE_TRANSFER	= 4,
};

#define MAX_SPELL_CONTROL_NUM 4



// 生物体类型 
/*-------------------------------------------------------------------
 * @Brief : 本类属性，是指在地图上使用的，共用的，也就是地图上的PK相关 
 *			的数据，而功能属性不注册在这里 
 * @Author:hzd 2015:11:28
 *------------------------------------------------------------------*/
enum EntityAttributeType
{
	////////////////////////////////////Entry共用属性//////////////////////////////////////
	ENTITY_ATTRIBUTE_MAPID,
	ENTITY_ATTRIBUTE_POS_X,
	ENTITY_ATTRIBUTE_POS_Y,
	ENTITY_ATTRIBUTE_INVIEWRANGE,
	ENTITY_ATTRIBUTE_OUTVIEWRANGE,
	ENTITY_ATTRIBUTE_MAX,
};

struct EntityAttribute
{
	int32 nMapID;					// 地图ID 
	Point2D sPos;					// 坐标 
	int32 nInViewRange;				// 进入视野距离 
	int32 nOutViewRangle;			// 离开视野距离 
	EntityAttribute()
	{
		sPos.nX = sPos.nY = 0;
		nInViewRange = nOutViewRangle = 0;
	}
};

/*-------------------------------------------------------------------
* @Brief : 生物体的大部分属性
*
* @Author:hzd 2015:11:12
*------------------------------------------------------------------*/
////////////////////////////////////Creature共用属性 start//////////////////////////////////////
struct CreatureAttribute
{
	int32		nStatus;							// 状态(CreatureStatus二进制位运算值) 
	int32		nLevel;								// 等级 

	int32		nRed;								// 红(血) 
	int32		nRedMax;							// 红上限 
	int32		nBlue;								// 蓝(法术) 
	int32		nBlueMax;							// 蓝上限 

	int32		nSpeed;								// 速度 
	int32		nRedRecover;						// 红恢复 
	int32		nBlueRecover;						// 蓝恢复 

	int32		nPhysicAttack;						// 物理攻击 
	int32		nPhysicDefend;						// 物理防御 
	int32		nSpellAttack;						// 法术攻击 
	int32		nSpellDefend;						// 法术防御 

	int32		nAttackType;						// 攻击类型(CreatureAttackType二进制与或运算值) 

	int32		nMingzhonglv;						// 命中率 
	int32		nShanbilv;							// 闪避率 
	int32		nBaoji;								// 暴击 
	int32		nBaojilv;							// 暴击率 
	int32		nGedang;							// 隔挡 
	int32		nGedanglv;							// 隔挡率 

};


enum CreatureAttributeType
{
	CREATURE_ATTRIBUTE_STATUS,			// 状态 
	CREATURE_ATTRIBUTE_LEVEL,			// 等级 
	CREATURE_ATTRIBUTE_RED,				// 红 
	CREATURE_ATTRIBUTE_REDMAX,			// 红MAX
	CREATURE_ATTRIBUTE_BLUE,			// 蓝 
	CREATURE_ATTRIBUTE_BLUEMAX,			// 蓝MAX
	CREATURE_ATTRIBUTE_SPEED,			// 速度
	CREATURE_ATTRIBUTE_REDRECOVER,		// 红恢复 
	CREATURE_ATTRIBUTE_BLUERECOVER,		// 蓝恢复 
	CREATURE_ATTRIBUTE_PHYSICATTACK,	// 物理攻击 
	CREATURE_ATTRIBUTE_PHYSICDEFEND,	// 物理防御 
	CREATURE_ATTRIBUTE_SPELLATTACK,		// 法术攻击 
	CREATURE_ATTRIBUTE_SPELLDEFEND,		// 法术防御 
	CREATURE_ATTRIBUTE_ATTACKTYPE,		// 攻击类型(CreatureAttackType二进制与或运算值) 
	CREATURE_ATTRIBUTE_MINGZHONGLV,		// 命中率 
	CREATURE_ATTRIBUTE_SHANBILV,		// 闪避率 
	CREATURE_ATTRIBUTE_BAOJI,			// 暴击 
	CREATURE_ATTRIBUTE_BAOJILV,			// 暴击率 
	CREATURE_ATTRIBUTE_GEDANG,			// 隔挡 
	CREATURE_ATTRIBUTE_GEDANGLV,		// 隔挡率 
	CREATURE_ATTRIBUTE_MAX,
};

enum CreatureStatus
{
	CREATURE_STATUS_NULL,

	CREATURE_STATUS_ALIVE = 0x00000001,	// 活着 
	CREATURE_STATUS_DEAD = 0x00000002,	// 死亡 
	  
	CREATURE_STATUS_DEHUANMAN	= 0x00000008,	// 抗缓慢 
	CREATURE_STATUS_XUANYUN		= 0x00000010,	// 眩晕 
	CREATURE_STATUS_DEXUANYUN	= 0x00000020,	// 抗眩晕 
	CREATURE_STATUS_HUNLUAN		= 0x00000040,	// 混乱 
	CREATURE_STATUS_DEHUNLUAN	= 0x00000080,	// 抗混乱 
	CREATURE_STATUS_DINGSHEN	= 0x00000100,	// 定身 
	CREATURE_STATUS_DEDINGSHEN	= 0x00000200,	// 抗定身 
	CREATURE_STATUS_CHENMO		= 0x00000400,	// 沉默 
	CREATURE_STATUS_DECHENMO	= 0x00000800,	// 抗沉默 
	CREATURE_STATUS_MIWU		= 0x00001000,	// 迷雾 
	CREATURE_STATUS_DEMIWU		= 0x00002000,	// 抗迷雾 
	CREATURE_STATUS_FUKONG		= 0x00004000,	// 浮空 
	CREATURE_STATUS_DEFUKONG	= 0x00008000,	// 抗浮空 
	CREATURE_STATUS_JIDAO		= 0x00010000,	// 击倒 
	CREATURE_STATUS_DEJIDAO		= 0x00020000,	// 抗击倒 
	CREATURE_STATUS_SHOUJI		= 0x00040000,	// 受击 
	CREATURE_STATUS_DESHOUJI	= 0x00080000,	// 抗受击 
	CREATURE_STATUS_DEJITUI		= 0x00200000,	// 抗击退 
};

enum CreatureAttackType
{
	CREATURE_ATTACK_TYPE_NULL,

	CREATURE_ATTACK_TYPE_Melee		= 0x00000001,	// 近战 
	CREATURE_ATTACK_TYPE_Remote		= 0x00000002,	// 远程 
	CREATURE_ATTACK_TYPE_PHYSIC		= 0x00000004,	// 物理 
	CREATURE_ATTACK_TYPE_SPELL		= 0x00000008,	// 法术 
};

////////////////////////////////////Creature共用属性end//////////////////////////////////////

struct SpellData
{
	uint32 nSpellID;	// 技能ID 
	int32 nPosition;    // 携带位置 (-1未携带,0 - 4 携带在对应的位置上) 
};

enum CharacterType
{
	CHARACTER_TYPE_NULL		= 0,    // 非法 
	CHARACTER_TYPE_SHAOLIN	= 1,	// 少林 
	CHARACTER_TYPE_MOJIAO	= 2,	// 魔教 
	CHARACTER_TYPE_GUMU		= 3,	// 古墓 
	CHARACTER_TYPE_XIAOYAO	= 4,	// 逍遥 
};

enum CharacterConst
{
	MAX_CHARACTER_NAME_LENGTH		= 32,			// 角色名最大长度 
	MAX_CHARACTER_NUM				= 8,			// 最大角色数量 
};

#pragma pack(push, 1)

//////////////////////////////////角色////////////////////////////////////////
// 角色专有的基本数据，大部分数据已经继承于生物类  
struct SceneUserAttribute
{
	int64 nUid;					// UID
	int32 nExp;					// 经验 
	int32 nLandID;				// 地图ID(副本或主地图) 加载时会先检查副本地图，再检查主地图 
	int32 nLandX;
	int32 nLandY;
	int32 nInstanceID;			// 副本ID 
	int32 nInstanceX;
	int32 nInstanceY;
	int32 nRoleType;			// 角色类型 
	int32 nClothesID;			// 衣物装备ID 
	int32 nWeaponID;			// 武器装备ID 
};

enum SceneUserAttributeType
{
	SCENEUSER_ATTRIBUTE_UID,
	SCENEUSER_ATTRIBUTE_EXP,
	SCENEUSER_ATTRIBUTE_LAND_ID, // 主场景ID
	SCENEUSER_ATTRIBUTE_LAND_X,
	SCENEUSER_ATTRIBUTE_LAND_Y,
	SCENEUSER_ATTRIBUTE_INSTANCE_ID, // 副本场景ID 
	SCENEUSER_ATTRIBUTE_INSTANCE_X,
	SCENEUSER_ATTRIBUTE_INSTANCE_Y,
	SCENEUSER_ATTRIBUTE_ROLETYPE,
	SCENEUSER_ATTRIBUTE_CLOTHESID,
	SCENEUSER_ATTRIBUTE_WEAPONID,
	SCENEUSER_ATTRIBUTE_MAX
};

//////////////////////////////////////////////////////////////////////////


// 角色身上穿的三件装备 
struct CharacterWearData
{
	uint64 nCharacterID;	// 角色ID  
	uint32 nClothesID;		// 衣服ID 
	uint32 nWeaponID;		// 武器ID 
};

#endif

