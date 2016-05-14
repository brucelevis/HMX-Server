#include "DbConnManager.h"

DbConnManager* DbConnManager::s_pInstance = NULL;

DbConnManager::DbConnManager():m_pMainDB(NULL)
{

};

DbConnManager* DbConnManager::Instance()
{
	if(!s_pInstance)
		s_pInstance = new DbConnManager;
	return s_pInstance;
}

IDbBase* DbConnManager::AddDatabase(const char* szDBServer,const char* szLoginName,const char* szPassword,const char* szDBName,bool bEnableSQLChk /*= false*/)
{
	DbMysql* pDB = new DbMysql;
	if (pDB->Open(szDBServer, szLoginName, szPassword, szDBName, bEnableSQLChk) )
	{
		m_vecDbBase.push_back(pDB);
		return pDB;
	}
	pDB->Release();	
	return NULL;
}

IDbBase* DbConnManager::GetDatabase(uint32 nDbID)
{
	std::vector<IDbBase*>::iterator it = m_vecDbBase.begin();
	for (; it != m_vecDbBase.end(); ++it)
	{
		if((*it)->ID() == nDbID)
			return *it;
	}
	return NULL;
}

IDbBase* DbConnManager::GetMainDB()
{
	if(m_pMainDB)
		return m_pMainDB;
	if(m_vecDbBase.size())
		return *m_vecDbBase.begin();
	return NULL;
}

void DbConnManager::SetMainDB(IDbBase& rIDataBase)
{
	m_pMainDB = &rIDataBase;
}

void DbConnManager::Update()
{
	IDbBase*  pDbBase = NULL;
	IDbResult* pResult = NULL;
	DbBaseVectorType::iterator it = m_vecDbBase.begin();
	DbBaseVectorType::iterator itEnd = m_vecDbBase.end();
	for (; it != itEnd; ++it)
	{	
		pDbBase = (*it);
		while((pResult = pDbBase->GetAsyncResult()) != NULL)
		{
			pResult->DBCallRead();
			pResult->Release();
		}
	}	
}





