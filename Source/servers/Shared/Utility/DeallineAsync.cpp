#include "DeallineAsync.h"


DeallineAsync::DeallineAsync(int32 nIntervalTime,int32 nMaxCallTimes,DeallineAsyncHandler pHander)
	:m_nIntervalTime(nIntervalTime),m_nMaxCallTimes(nMaxCallTimes),m_fAsyncHandler(pHander),m_ctimer(*this)
{
	m_nHadCallTimes = 0;
}


DeallineAsync::~DeallineAsync()
{
}

void DeallineAsync::Start()
{
	thread t(boost::bind(&DeallineAsync::HandlerStart, this));
	this_thread::yield();		// Temporarily give up a CPU time , to other threads
	t.swap(m_cServiceThread);	// Swaps the state of the object with that of mServiceThread
}

void DeallineAsync::HandlerStart()
{
	m_ctimer.expires_from_now(posix_time::millisec(m_nIntervalTime));
	m_ctimer.async_wait(boost::bind(&DeallineAsync::DoHandler,this)); 
	io_service::run();
}


void DeallineAsync::Cancel()
{	
	m_ctimer.cancel();
	io_service::stop();
}

void DeallineAsync::DoHandler()
{
	m_ctimer.expires_at(m_ctimer.expires_at() + posix_time::millisec(m_nIntervalTime));
	m_ctimer.async_wait(boost::bind(&DeallineAsync::DoHandler,this));
	(m_fAsyncHandler)(m_nHadCallTimes++);
	if(m_nMaxCallTimes > 0 && m_nMaxCallTimes == m_nHadCallTimes)
	{
		m_ctimer.cancel();
		io_service::stop();
	}
}


