#ifndef __COMMON_S2W_H_
#define __COMMON_S2W_H_


#include "CommonDefine.h"
#include "NetConfig.h"
#include "CommonCharacter.h"


#pragma pack(push,1)


enum EProS2W
{

	PRO_S2W_REGISTER_SCENE		= PRO_S2W_BASE + 0, // 注意场景 
	PRO_S2W_CANCEL_SCENE		= PRO_S2W_BASE + 1, // 注销场景 
	PRO_S2W_ENTER_SCENE_RESULT	= PRO_S2W_BASE + 2,	// 进入场景结果
	PRO_S2W_CHANGE_SCENE		= PRO_S2W_BASE + 3, // 请求切换场景 


};


struct S2WRegisterScene : public NetMsgHead
{

	S2WRegisterScene():NetMsgHead(PRO_S2W_REGISTER_SCENE)
	{

	}
	int32 nSceneNum;
	int32 arrSceneID[MAX_SCENE_NUM];
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

struct S2WReqCancelScene : public NetMsgHead
{
	S2WReqCancelScene():NetMsgHead(PRO_S2W_CANCEL_SCENE)
	{

	}
	int32 nSceneNum;
	int32 arrSceneID[MAX_SCENE_NUM];
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};


struct S2WEnterSceneResult : public NetMsgHead
{

	int32 nSceneID;

	// 是否为跨服，1是，则需要通知原场景，0否，则需要进入主场景 
	int32 nCross;	
	enum 
	{
		E_ENTER_SUCCESS = 0,
		E_ENTER_FAIL,
	};

	int32 nResult;

	S2WEnterSceneResult():NetMsgHead(PRO_S2W_ENTER_SCENE_RESULT)
	{
		nSceneID = nCross = nResult = 0;
	}
	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

/*

切换场景，由ss向ws请求，用户先会进入到临时场景，
然后根据提供的基本参数进行轻度检查是否可以进入，
如果可以进入，则对面场景会加载角色内存所有数据，
之后再做一次深度检查，如果这次能进入，则真正可以进入
，失败与成功都会进行回调到原来的场景，失败，则通知前端，
成功则删除本场景的内存ok

*/
struct S2WChangeScene : public NetMsgHead
{
	int64 nCharID;
	int32 nSceneID;
	int32 nPram0; // 参数1 
	int32 nPram1; // 参数2
	int32 nPram2; // 参数3
	S2WChangeScene() :NetMsgHead(PRO_S2W_CHANGE_SCENE)
	{
		nCharID = nSceneID = nPram0 = nPram1 = nPram2 = 0;
	}

	inline int32 GetPackLength()const
	{
		return sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////

enum EProD2W
{

	PRO_D2W_GLOBAL_INFO			= PRO_D2W_BASE + 0,	// 全局信息 
	PRO_D2W_SELECT_ROLE_RESULT	= PRO_D2W_BASE + 1,	// 选择角色结果 

};

struct D2WGlobalInfo : public NetMsgHead
{

	// 第几次启动 
	int32 nStartTimes;
	// 

	D2WGlobalInfo():NetMsgHead(PRO_D2W_GLOBAL_INFO)
	{

	}
	inline int32 GetLength()
	{
		return sizeof(*this);
	}
};

/*-------------------------------------------------------------------
 * @Brief : 选择角色信息，本信息如果成功会有数据通知到Client，失败
 *			则会停在WS，由WS处理，等Client再次发起选择角色 
 * @Author:hzd 2015:11:6
 *------------------------------------------------------------------*/
struct D2WSelectRoleResult : public NetMsgHead
{
	enum{
		
		E_SELECT_SUCCESS = 0,
		E_SELECT_LOADING,	// 加载中 
		E_SELECT_NOT_FOUND, // 找不到
	};
	int32 nResult;
	StUserDataForWs sUserData;
	D2WSelectRoleResult():NetMsgHead(PRO_D2W_SELECT_ROLE_RESULT)
	{
		nResult = 0;
	}
	inline int32 GetLength()
	{
		return sizeof(*this);
	}
};

#pragma pack(pop)




#endif

