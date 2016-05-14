#if 0
#ifndef __BUFF_H_
#define __BUFF_H_

#include "CommonDefine.h"
#include "def_buff.h"
#include "ObjPool.h"

class Creature;

struct Buff
{
	Buff(const BuffData* rRes, Creature* _pFrom,int32 nTimeElapse = 0/* 下线Buff加载进来需要用到 */)
		: rResData(*rRes), pFrom(_pFrom), nTimeElapse(0) {}

	const BuffData&					rResData;									// 资源信息 
	Creature*						pFrom;										// 施法者 
	int32							nTimeElapse;								// 已持续时间 
	std::map<uint64,int32>			arrReverseValue[MAX_BUFF_EFFECT_NUM];		// 可逆值,实体ID=>改变的值 
	
};

void Reverse(Buff& rBuff, Creature* pOwner);

extern ObjPool<Buff> g_cBuffFactory;

#endif
#endif
