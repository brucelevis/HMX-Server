#if 0
#ifndef __SCRIPT_H_
#define __SCRIPT_H_

#include "def_buff.h"

class SceneUser;
struct SpellEx;

namespace Script
{
	extern BuffEffect arrBuffEffect[BUFF_EFFECT_TYPE_MAX+1];

	// 获得范围内有效生物体 
	template<typename _InIt, typename _OutIt>
	static _OutIt GetEffectCreature(const Point2D& i_rPosition, const Vector2DEx& i_rHeading, EffectRangeType eRangeType,
		int32 i_nDist, float32 i_nCosValue,
		_InIt i_rInputFirst, _InIt i_rInputEnd, _OutIt o_rOutputFirst);

	// 技能效果释放给对象 
	int32 SetSpellEffectEntry(SceneUser* pFrom, SpellEx* pSpell);

	// 对应的不同buffer的效果函数 
	void ShouJi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void JiDao(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void FuKong(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void JiTui(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void MiWu(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void ShengMingXiuGaiPuTongGongJi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void ShengMingXiuGaiJiNengGongJi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void ShengMingXiuGaiZiJieYiChu(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void ChenMo(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void DingShen(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void HunLuan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void LiuXie(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void HuanMan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void XiYin(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangShouji(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangJiDao(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangFuKong(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangMiWu(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangDingShen(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangHunLuan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangXuanYun(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangHuanMan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangJiTui(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangChenMo(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void WuGong(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void NeiGong(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void ShengMing(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void WuFang(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void NeiFang(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void DaDuan(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void BaoJiLv(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void ShanBiLv(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);
	
	void GeDangLv(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void MingZhongLv(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void BaojiZhi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void KangLiuXue(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

	void GeDangZhi(Creature* pFrom, Creature* pTo, EffectAimType eAimType, EffectRangeType eRangeType, int32 nParam, int32 nDist, float32 nCosValue,int32 nEffectNum,std::map<uint64,int32>& oi_nReverseVal,bool bReverse);

}


#endif

#endif
