#ifndef __COMMON_FRIENDS_H_
#define __COMMON_FRIENDS_H_

/*
 *	���ѹ���
 */

#include "BaseDefine.h"

#pragma pack(push,1)

// ����������Ϣ 
struct StFriendInfo
{
	int32 nUniqueID;

	int32 nMyId;		// �Լ�ID 
	int32 nFirendId;	// ����ID 
	int32 nSid;			// ԴID(����˭��˭�Ĺ�ϵ) 
	enum ERelationType
	{
		E_RELATION_NONE = 0,	// û�й�ϵ 
		E_RELATION_STRANGER,	// İ����
		E_RELATION_FRIEND,		// ����
		E_RELATION_BLACKLIST,	// ������
		E_RELATION_MAX,		
	};
	int32 nRelation;			// ��ϵ 
	int32 nCreateTime;			// ʱ��

};

// save use google protobuf protocol


#pragma pack(pop)






















#endif