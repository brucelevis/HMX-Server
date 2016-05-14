#ifndef __DB_CONN_MANAGER_H_
#define __DB_CONN_MANAGER_H_

#include "DbConfig.h"
#include "DbMysql.h"

/*-------------------------------------------------------------------
 * @Brief : 多个数据库管理类 
 * 
 * @Author:hzd 2015:12:28
 *------------------------------------------------------------------*/
class DbConnManager
{
	typedef std::vector<IDbBase*> DbBaseVectorType;
public:
	
	// 获得单例实体 
	static DbConnManager* Instance();
	
	// 添加一个数据库连接
	IDbBase* AddDatabase(const char* szDBServer, const char* szLoginName, const char* szPassword, const char* szDBName, bool bEnableSQLChk = false);

	// 获得某数据库连接
	IDbBase* GetDatabase(uint32 nDbID);

	// 获得主DB(默认为第一个连接)
	IDbBase* GetMainDB();

	// 设置主DB
	void SetMainDB(IDbBase& rIDataBase);

	// 处理
	void Update();

private:

	DbConnManager();

private:

	static DbConnManager*	s_pInstance;	// 单例实体 
	IDbBase*				m_pMainDB;		// 主DB第一个连接 
	DbBaseVectorType		m_vecDbBase;	// 数据连接容器  
};



#endif


