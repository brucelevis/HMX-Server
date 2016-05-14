#ifndef __COMMON_FRIENDS_H_
#define __COMMON_FRIENDS_H_

/*
 *	好友功能
 */

#include "BaseDefine.h"

#pragma pack(push,1)

// 单个好友信息 
struct StFriendInfo
{
	int32 nUniqueID;

	int32 nMyId;		// 自己ID 
	int32 nFirendId;	// 好友ID 
	int32 nSid;			// 源ID(即是谁加谁的关系) 
	enum ERelationType
	{
		E_RELATION_NONE = 0,	// 没有关系 
		E_RELATION_STRANGER,	// 陌生人
		E_RELATION_FRIEND,		// 好友
		E_RELATION_BLACKLIST,	// 黑名单
		E_RELATION_MAX,		
	};
	int32 nRelation;			// 关系 
	int32 nCreateTime;			// 时间

};

// save use google protobuf protocol


#pragma pack(pop)






















#endif