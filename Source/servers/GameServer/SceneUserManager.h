#ifndef __SCENE_USER_MANAGER_H_
#define __SCENE_USER_MANAGER_H_

#include "SharedIncludes.h"

#include "Creature.h"
#include "UnorderedSet.h"
#include "UnorderedMap.h"
#include "ClientSession.h"
#include "SpellControlEx.h"

#include "SceneUserCtrl.h"
#include "QuestCtrl.h"
#include "character.pb.h"

/*
 *
 *	Detail: 角色类 
 *   
 *  Created by hzd 2014-12-17
 *
 */

/*-------------------------------------------------------------------
 * @Brief : 角色管理类 
 * 
 * @Author:hzd 2016:1:6
 *------------------------------------------------------------------*/
class SceneUserManager : public BaseSingle<SceneUserManager> 
{
public:

	// 添加角色 
	SceneUser* AddUser(ClientSession* pClientSession,const ::protobuf::Character& charactor);

	// 删除角色 
	void RemoveUser(int64 nUID);

	// 获得Character 
	SceneUser* GetUserByUID(int64 nUID);

	// 
	SceneUser* GetUserByCSID(int32 nCSID);

	// 获得Character* 
	SceneUser* GetUserByEntityID(int64 nEntityID);



	//更新
	void Update(int32 nServerTimes);	


private:

	UNORDERED_SET<SceneUser*>			m_setPlayerExs;			// 玩家容器vector 
	UNORDERED_MAP<int64,SceneUser*>		m_umapUsers;		// characerID=>Character*  
	UNORDERED_MAP<int64,SceneUser*>		m_umapEntityUser;	// 实体ID对应Character* 
	UNORDERED_MAP<int32,SceneUser*>		m_umapCSIDUser;	// sessionID对应Character* 
	
	static ObjPool<SceneUser>			s_cUserFactory;	// 角色工厂 

};

#endif



