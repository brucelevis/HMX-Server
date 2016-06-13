/*
 *
 *	Detail: Mysql数据库配置类
 *   
 * Copyright (c) Created by hzd 2014-4-21.All rights reserved
 *
 */

#ifndef SWA_NETCONFIG_H_
#define SWA_NETCONFIG_H_

#include <stdio.h>

#include <map>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <string>
#include <utility>

#ifdef WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <mswsock.h>
#include <Windows.h>
#else
#include <stdint.h>
#endif

#include <boost/timer.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/container/set.hpp>
#include <boost/thread/thread.hpp>

using namespace std;

using namespace boost;
using namespace boost::system;
using namespace boost::asio::ip;
using namespace boost::asio;

	
#ifdef WIN32
	typedef __int64                     int64;
#else
	typedef int64_t                     int64;
#endif
	typedef unsigned long long          uint64;
	typedef wchar_t						wchar;
	typedef unsigned char				uchar;
	typedef char						int8;
	typedef unsigned char				uint8;
	typedef short						int16;
	typedef unsigned short				uint16;
	typedef int							int32;
	typedef unsigned int				uint32;
	typedef float						float32;
	typedef double						float64;
	
#define S_SAFE_DELETE(a) if( a ){ delete a;a=NULL; }
#define S_SAFE_RELEASE(a) if( a ){ a->Release(); }
	
#define S_USE_PRINT

#define STRCMP strcmp
#define STRCPY strcpy
#define ATOF atof
#define ATOI atoi
#define ATOL atol

#ifdef WIN32
#define SSleep(ms) Sleep(ms)
#else
#define SSleep(ms) usleep(ms)
#endif

#define RAND rand

#ifdef ASSERT
#undef ASSERT
#endif 
#define ASSERT assert

typedef vector< string > StringVector;
typedef vector< string >::iterator StringIter;
typedef vector< int32 > IntVector;
typedef vector< int32 >::iterator IntIter;



#define MAX_THREAD 4

#pragma pack(push, 1)

// 包头大小设置为4个字节，表示后面body的长度   
#define PACKAGE_HEADER_SIZE 4



template<typename T1, typename T2, typename T3, typename T4, typename T5> struct EventStreble
{
	T1 first;
	T2 second;
	T3 third;
	T4 fourth;
	T5 fifth;
	EventStreble() :first(), second(), third(), fourth(), fifth(){}
	EventStreble(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5) :first(t1), second(t2), third(t3), fourth(t4),fifth(t5) {}
	template<typename U1, typename U2, typename U3, typename U4, typename U5> EventStreble(const EventStreble<U1, U2, U3,U4,U5>& _streble) : first(_streble.first), second(_streble.second), third(_streble.third), fourth(_streble.fourth), fifth(_streble.fifth) {}
	template<typename U1, typename U2, typename U3, typename U4, typename U5> void Init(U1 x, U2 y, U3 z, U4 w, U5 v) {
		first = x; second = y; third = z; fourth = w; fifth = v;
	}
	template<typename U1, typename U2, typename U3, typename U4, typename U5> bool operator==(const EventStreble<U1, U2, U3, U4, U5>& _streble)
	{
		return first == _streble.first && second == _streble.second && third == _streble.third && fourth == _streble.fourth && fifth == _streble.fifth;
	}
};

/*

普通事件->参数列表
int32, int32, int32, int32, int64
事件，唯一编号，客户端sessionid,参数1，参数2

特殊事件(带Only)->参数列表
事件，监听事件，客户端sessionid,参数1，参数2

*/
template<typename T1, typename T2, typename T3, typename T4, typename T5> inline EventStreble<T1, T2, T3, T4, T5> make_streble(T1 x, T2 y, T3 z, T4 w, T5 v)
{
	return EventStreble<T1, T2, T3, T4, T5>(x, y, z, w, v);
}

typedef EventStreble<int32, int32, int32, int32, int64> SocketEvent;

struct NetMsgHead
{
	NetMsgHead(uint32 _nType) :nType(_nType)
	{
		nSessionID = 0;
		stEvent.Init(0, 0, 0, 0, 0);
	}
	int32 nType;			// 协议  
	int32 nSessionID;		// 客户端 sessionID 
	SocketEvent stEvent;	// 设置远程事件  
};

#pragma pack(pop)

#endif
