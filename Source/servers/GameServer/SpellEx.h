#ifndef __SPELLEX_H_
#define __SPELLEX_H_

#include "def_buff.h"

// 技能控制器使用的数据单个技能数据 

struct SpellEx
{

	//SpellEx(uint32 _nSpellID,const SpellRes& rRes)
	//	:nSpellID(_nSpellID),rSpellRes(&rRes)
	//{
	//}

	// 是否携带中 
	bool IsCarried() const ;	

//	const SpellRes* rSpellRes;	// 该技能配置 
	uint32 nSpellID;			// 技能ID 
	int32 nCdTime;				// 冷切毫秒(配置中的时间 - CD减免情况),0时代表该技能没有CD 
	int32 nTimeElapse;			// 已持续时间毫秒,当该值>= nCDTime时，则将nCDTime置为0 

};



#endif


