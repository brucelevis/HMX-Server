//
//  logManager.cpp
//  test
//
//  Created by fox on 13-1-9.
//
//

#include "LogManager.h"
#include "Utility.h"

LogManagerMgr::LogManagerMgr()
{
	m_pLogServer = NULL;
}

LogManagerMgr::~LogManagerMgr()
{

}

void LogManagerMgr::Init(ServerSession* pServerSession,int32 nServerType,int32 nStartTimes)
{
	ASSERT(pServerSession);
	m_pLogServer = pServerSession;
	m_nServerType = nServerType;
	m_nStartTimes = nStartTimes;
}


void LogManagerMgr::Normal(const char* str , ...)
{
	boost::mutex::scoped_lock lock( gMutex );    
	va_list va;
	static char sstr[255];
	static char sbuf[255];
	memset( sstr , 0 , 255 );
	memset( sbuf , 0 , 255 );

	va_start( va , str );
	vsprintf( sstr , str , va );
	va_end(va);
	printf( "LOG:%s \n", sstr );
	lock.unlock();

	SaveToDb(LOG_LEVEL_NORMAL,sstr);	

}

void LogManagerMgr::Info(const char* str , ...)
{
	boost::mutex::scoped_lock lock( gMutex );    
	va_list va;
	static char sstr[255];
	static char sbuf[255];
	memset( sstr , 0 , 255 );
	memset( sbuf , 0 , 255 );

	va_start( va , str );
	vsprintf( sstr , str , va );
	va_end(va);
	printf( "LOG:%s \n", sstr );
	lock.unlock();
	SaveToDb(LOG_LEVEL_INFO,sstr);
}

void LogManagerMgr::Warring(const char* str , ...)
{
	boost::mutex::scoped_lock lock( gMutex );    
	va_list va;
	static char sstr[255];
	static char sbuf[255];
	memset( sstr , 0 , 255 );
	memset( sbuf , 0 , 255 );

	va_start( va , str );
	vsprintf( sstr , str , va );
	va_end(va);
	printf( "LOG:%s \n", sstr );
	lock.unlock();
	SaveToDb(LOG_LEVEL_WARRING,sstr);

}

void LogManagerMgr::Error(const char* str , ...)
{
	boost::mutex::scoped_lock lock( gMutex );    
	va_list va;
	static char sstr[255];
	static char sbuf[255];
	memset( sstr , 0 , 255 );
	memset( sbuf , 0 , 255 );

	va_start( va , str );
	vsprintf( sstr , str , va );
	va_end(va);
	printf( "LOG:%s \n", sstr );
	lock.unlock();

	SaveToDb(LOG_LEVEL_ERROR,sstr);
}

void LogManagerMgr::SaveToDb(ELogLevel eLevel,const char* detail)
{

	S2LogMsg sMsg;
	sMsg.nServerType = m_nServerType;
	sMsg.nStartTimes = m_nStartTimes;
	sMsg.nLevel = eLevel;
	strcpy(sMsg.arrDetail,detail);
	sMsg.nRowNum = 0;
	sMsg.nTime = Utility::NowTime();

	if(m_pLogServer)
	{
		if(m_vecRecordTmp.size())
		{
			std::vector<S2LogMsg>::iterator it = m_vecRecordTmp.begin();
			for (; it != m_vecRecordTmp.end();++it)
			{
				m_pLogServer->SendMsg(&(*it),(*it).GetLength());
			}
			m_vecRecordTmp.clear();
		}
		m_pLogServer->SendMsg(&sMsg,sMsg.GetLength());
	}
	else
	{
		// 缓存下来，下次再存 
		m_vecRecordTmp.push_back(sMsg);
		printf("LOG Server ISNOT OPEN!\n");
	}

}



