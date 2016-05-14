#ifndef __USER_MANAGER_H_
#define __USER_MANAGER_H_

#include "BaseDefine.h"
#include "BaseSingle.h"
#include "ObjPool.h"

#include "SharedIncludes.h"

class ClientSession;

/*------------------------------------------------------------------
 *
 * @Brief : 玩家对象 
 *
 * @Author: hzd 
 * @File  : UserManager.h
 * @Date  : 2015/11/08 19:57
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
class User
{
public:

	User(int32 _nCSID, ClientSession* pClientSession,const StUserDataForWs* fromDpData);

	~User();

	void EnterScene(int32 nSceneID,int32 nPram0 = 0,int32 nPram1 = 0,int32 nPram2 = 0);

	int32 GetCurSceneID();


private:

	int32				m_nCSID;
	int64				m_nCharID;

	ClientSession*		m_pClientSession;

	StUserDataForWs		m_sData;
	
};



/*------------------------------------------------------------------
 *
 * @Brief : 玩家对象管理器 
 *
 * @Author: hzd 
 * @File  : UserManager.h
 * @Date  : 2015/11/06 23:33
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/

class UserManager : public BaseSingle<UserManager>
{
public:

	UserManager(void);
	~UserManager(void);


	User* LoginUser(int32 nCSID, ClientSession* pClientSession,const StUserDataForWs* fromDpData);

	void LogoutUser(int32 nCSID);

	int64 GetCharIDByCSID(int32 nCSID);

	User* GetUserByCharID(int64 nCharID);

	User* GetUserByCSID(int32 nCSID);
	

private:

	typedef std::map<int64,User*>	UserMapType;
	typedef std::map<int32,int64>	CSIDCharIDMapType;

	UserMapType				m_mapUser;			// 在线用户缓存 
	CSIDCharIDMapType		m_mapCSIDCharID;	// ClientS
	static ObjPool<User>	g_cUserFactory;

};


#endif


