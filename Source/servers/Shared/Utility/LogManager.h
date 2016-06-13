//
//  LogManager.h
//  test
//
//  Created by fox on 13-1-9.
//
//

#ifndef SWALLOW_LOG_MANAGER_H_
#define SWALLOW_LOG_MANAGER_H_

#include <stdarg.h>

#include "BaseDefine.h"
#include "BaseSingle.h"
#include "ServerSession.h"


enum ELogLevel
{
	LOG_LEVEL_NORMAL = 0,// 普通，记录一些统计用数据 
	LOG_LEVEL_INFO,		 // 记录一些关键性数据，这些数据有可能会引起程序出错 
	LOG_LEVEL_WARRING,   // 该数据有异常，可能是设计如此，也可能是BUG
    LOG_LEVEL_ERROR,     // 出错，使程序功能无法正确运行 
};

enum
{

	PRO_S2LOG_MSG = 9999, // 记录日记协议 
	MAX_LOG_DETAIL = 200,
	MAX_LOG_FILENAME = 100,

};

#pragma pack(push,1)

struct S2LogMsg : public NetMsgHead
{
	int32 nServerType; // 来自哪个进程 
	int32 nStartTimes; // 第几次启动 
	int32 nLevel;
	char arrFunctionName[MAX_LOG_FILENAME];
	int32 nRowNum;
	char arrDetail[MAX_LOG_DETAIL];
	int32 nTime; 

	S2LogMsg():NetMsgHead(PRO_S2LOG_MSG)
	{
		nLevel = nRowNum = nTime = 0;
		memset(arrFunctionName,0,MAX_LOG_FILENAME);
		memset(arrDetail,0,MAX_LOG_DETAIL);
	}

	inline int32 GetLength()
	{
		return sizeof(*this);
	}
};


#pragma pack(pop)
    
/*------------------------------------------------------------------
 *
 * @Brief : 日志管理类，可以记录本地文件，也可记录到LogServer
 *
 * @Author: hzd 
 * @File  : LogManager.h
 * @Date  : 2015/10/22 23:18
 * @Copyright (c) 2015,hzd, All rights reserved.
 *-----------------------------------------------------------------*/
class LogManagerMgr : public BaseSingle<LogManagerMgr>
{
public:

	LogManagerMgr();
	~LogManagerMgr();

	void Init(ServerSession* pServerSession,int32 nServerType,int32 nStartTimes);

	void Normal(const char* str , ...);

	void Info(const char* str , ...);

	void Warring(const char* str , ...);

	void Error(const char* str , ...);

	void SaveToDb(ELogLevel eLevel,const char* detail);

private:

	boost::mutex gMutex;

	int32 m_nServerType;

	int32 m_nStartTimes;

	/*-------------------------------------------------------------------
	 * @Brief : 日志服务器Session
	 * 
	 * @Author:hzd 2015:10:23
	 *------------------------------------------------------------------*/
	ServerSession* m_pLogServer;

	/*------------------------------------------------
	 * 临时保存，等待下次保存保存 
	 *-----------------------------------------------*/
	std::vector<S2LogMsg> m_vecRecordTmp;

};


#ifdef WIN32
#define FLOG_NORMAL( fmt , ... ) LogManagerMgr::Instance()->Normal( fmt , __VA_ARGS__ );
#define FLOG_INFO( fmt , ... ) LogManagerMgr::Instance()->Info(fmt , __VA_ARGS__ );
#define FLOG_WARRING(fmt , ... ) LogManagerMgr::Instance()->Warring(fmt , __VA_ARGS__ );
#define FLOG_ERROR(fmt , ... ) LogManagerMgr::Instance()->Error(fmt , __VA_ARGS__ );
#else
#define FLOG_NORMAL( fmt , a... ) LogManagerMgr::Instance()->Normal( fmt , ##a );
#define FLOG_INFO( fmt , a... ) LogManagerMgr::Instance()->Info( fmt , ##a );
#define FLOG_WARRING(fmt , a... ) LogManagerMgr::Instance()->Warring(fmt , ##a );
#define FLOG_ERROR(fmt , a... ) LogManagerMgr::Instance()->Error(fmt , ##a );
#endif

#endif


