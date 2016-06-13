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
class WorldUser
{
public:

	WorldUser(ClientSession* pClientSession,const StUserDataForWs* fromDpData);

	~WorldUser();

	int64 GetUserID() 
	{ 
		return m_nCharID;
	}

	int32 GetSessionID() 
	{
		return m_pCSession ? m_pCSession->GetSessionID() : 0;
	}

	int32 GetCurSceneID()
	{
		return m_sData.nLandMapid;
	}

	void SetCurSceneID(int32 nSceneID)
	{
		m_sData.nLandMapid = nSceneID;
	}

	void SendToFep(NetMsgHead* pMsg,int32 nSize);

private:

	int64				m_nCharID;
	ClientSession*		m_pCSession;
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


	WorldUser* AddWorldUser(ClientSession* pClientSession,const StUserDataForWs* fromDpData);

	void RemoveWorldUser(int64 nCharID);

	WorldUser* GetUserByCharID(int64 nCharID);

	int64 GetUserIDBySessionID(int32 nSessionID);

private:

	void AddSessionID(int32 nSessionID,int64 nCharID);

	void RemoveSessionID(int32 nSessionID);
	
private:

	typedef std::map<int64,WorldUser*>	UserMapType;
	typedef std::map<int32, int64>		SessionIDUserIDMapType;

	UserMapType					m_mapUser;			// 在线用户缓存 
	static ObjPool<WorldUser>	g_cUserFactory;
	SessionIDUserIDMapType		m_mapSessionIDUserID;
};


#endif


