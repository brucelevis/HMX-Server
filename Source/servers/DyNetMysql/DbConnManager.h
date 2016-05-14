#ifndef __DB_CONN_MANAGER_H_
#define __DB_CONN_MANAGER_H_

#include "DbConfig.h"
#include "DbMysql.h"

/*-------------------------------------------------------------------
 * @Brief : ������ݿ������ 
 * 
 * @Author:hzd 2015:12:28
 *------------------------------------------------------------------*/
class DbConnManager
{
	typedef std::vector<IDbBase*> DbBaseVectorType;
public:
	
	// ��õ���ʵ�� 
	static DbConnManager* Instance();
	
	// ���һ�����ݿ�����
	IDbBase* AddDatabase(const char* szDBServer, const char* szLoginName, const char* szPassword, const char* szDBName, bool bEnableSQLChk = false);

	// ���ĳ���ݿ�����
	IDbBase* GetDatabase(uint32 nDbID);

	// �����DB(Ĭ��Ϊ��һ������)
	IDbBase* GetMainDB();

	// ������DB
	void SetMainDB(IDbBase& rIDataBase);

	// ����
	void Update();

private:

	DbConnManager();

private:

	static DbConnManager*	s_pInstance;	// ����ʵ�� 
	IDbBase*				m_pMainDB;		// ��DB��һ������ 
	DbBaseVectorType		m_vecDbBase;	// ������������  
};



#endif


