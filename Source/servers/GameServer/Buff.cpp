#if 0
#include "Buff.h"
#include "Script.h"


ObjPool<Buff> g_cBuffFactory;

void Reverse(Buff& rBuff, Creature* pOwner)
{
	for (int32 i = 0; i < MAX_BUFF_EFFECT_NUM; ++i)
	{
		const BuffData::Effect& sEffect = rBuff.rResData.arrEffect[i];
		if(sEffect.nEffectID == BUFF_EFFECT_TYPE_NULL)
			break;
		BuffEffect& rCallback = Script::arrBuffEffect[sEffect.nEffectID];
		//rCallback.pCallBack(rBuff.pFrom, pOwner, sEffect.eAimType, sEffect.eRangeType,
		//	sEffect.nParam, sEffect.nDist, sEffect.fCosAngle,rBuff.rResData.nEffectNum,rBuff.arrReverseValue[i],true);
	}
}
#endif
