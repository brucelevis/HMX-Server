#ifndef __TEMPORARY_SCENE_H_
#define __TEMPORARY_SCENE_H_

#include "Base.h"
#include "BaseDefine.h"
#include "BaseSingle.h"
/*-------------------------------------------------------------------
 * @Brief : 如 果是跨服务器进入场景，先是由玩家进入临时场景，再由临时
 *			场景进入指定场景（避免指定场景限时等特殊情况），如果是本服
 *			的场景跳转，则不需要经过临时场景
 * @Author:hzd 2015:11:9
 *------------------------------------------------------------------*/

struct StTempUserInfo
{
	int32 nCSID;
	int64 nCharID;
	int32 nSceneID; // 将要进入的场景 
	int32 nDpServerID;
	int32 nFepServerID;
	int64 nReqTime; // 请求时间 
	bool bCrossSs;	// 是否跨服 
	enum
	{
		E_STEP_NULL = 0,
		E_STEP_SQL_CHARACTER, 
		E_STEP_REV_CHARACTER,
		E_STEP_ENTER_SCENE,
	};

	int32 nStep;

	StTempUserInfo(int32 _nCSID,int64 _nCharID,int32 _nScene)
	{
		nCSID= _nCSID;
		nCharID = _nCharID;
		nSceneID = _nScene;
		nStep = E_STEP_NULL;
		nReqTime = 0;
		bCrossSs = true;
	}
};

class TemporaryScene : public BaseSingle<TemporaryScene>
{
public:
	TemporaryScene(void);
	~TemporaryScene(void);

	// 预前检查，是否可以进入指定场景 
	bool PreEnterUser(int32 nSceneID,int32 nPram0,int32 nPram1,int32 nPram2);

	/*
	由WS请求过来的进入该场景 
	*/
	void EnterUser(int32 nCSID,int64 nCharID,int32 nSceneID,int32 nDpServerID,int32 nFepServerID,int32 nStep = StTempUserInfo::E_STEP_SQL_CHARACTER,bool bCrossSs = true);

	/*

	由本地切换场景，本地已经有了角色的内存 

	 */
	void EnterUserLocal(int32 nCSID, int64 nCharID, int32 nSceneID);

	/*--------------------------------------------
	 *  @brief  :	
	 *  @input	: 
	 *  @return : false 删除tempuser,true继续等待
	 *-------------------------------------------*/
	bool DbLoadData(int32 nCSID,const void* data);

private:

	StTempUserInfo* GetTempUserInfo(int32 nCSID);

	void RemoveTempUser(int32 nCSID);

	//void CheckAndDelTempUser(int32 nCSID);

private:

	std::map<int32,StTempUserInfo>  m_mapUserInfo; //


};


#endif



