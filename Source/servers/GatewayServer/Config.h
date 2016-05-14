#ifndef FEP_CONFIG_H_
#define FEP_CONFIG_H_

/*-------------------------------------------------------------------
 * @Brief : FEP服务器常用常量定义 
 * 
 * @Author:hzd 2013/04/03
 *------------------------------------------------------------------*/

// 最大客户端（玩家）连接数
#ifdef _DEBUG
#define MAX_FEP_SOCKET_CONNECT 100
#else
#define MAX_FEP_SOCKET_CONNECT 2048
#endif // _DEBUG



#endif
 