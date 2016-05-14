#ifndef __COMMON_S2W_H_
#define __COMMON_S2W_H_


#include "CommonDefine.h"
#include "NetConfig.h"
#include "CommonCharacter.h"


#pragma pack(push,1)


enum EProS2W
{

	PRO_S2W_REGISTER_SCENE		= PRO_S2W_BASE + 0, // ע�ⳡ�� 
	PRO_S2W_CANCEL_SCENE		= PRO_S2W_BASE + 1, // ע������ 
	PRO_S2W_ENTER_SCENE_RESULT	= PRO_S2W_BASE + 2,	// ���볡�����
	PRO_S2W_CHANGE_SCENE		= PRO_S2W_BASE + 3, // �����л����� 


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

	// �Ƿ�Ϊ�����1�ǣ�����Ҫ֪ͨԭ������0������Ҫ���������� 
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

�л���������ss��ws�����û��Ȼ���뵽��ʱ������
Ȼ������ṩ�Ļ�������������ȼ���Ƿ���Խ��룬
������Խ��룬����泡������ؽ�ɫ�ڴ��������ݣ�
֮������һ����ȼ�飬�������ܽ��룬���������Խ���
��ʧ����ɹ�������лص���ԭ���ĳ�����ʧ�ܣ���֪ͨǰ�ˣ�
�ɹ���ɾ�����������ڴ�ok

*/
struct S2WChangeScene : public NetMsgHead
{
	int64 nCharID;
	int32 nSceneID;
	int32 nPram0; // ����1 
	int32 nPram1; // ����2
	int32 nPram2; // ����3
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

	PRO_D2W_GLOBAL_INFO			= PRO_D2W_BASE + 0,	// ȫ����Ϣ 
	PRO_D2W_SELECT_ROLE_RESULT	= PRO_D2W_BASE + 1,	// ѡ���ɫ��� 

};

struct D2WGlobalInfo : public NetMsgHead
{

	// �ڼ������� 
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
 * @Brief : ѡ���ɫ��Ϣ������Ϣ����ɹ���������֪ͨ��Client��ʧ��
 *			���ͣ��WS����WS������Client�ٴη���ѡ���ɫ 
 * @Author:hzd 2015:11:6
 *------------------------------------------------------------------*/
struct D2WSelectRoleResult : public NetMsgHead
{
	enum{
		
		E_SELECT_SUCCESS = 0,
		E_SELECT_LOADING,	// ������ 
		E_SELECT_NOT_FOUND, // �Ҳ���
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

