#include "DbMysql.h"
#include "DbRecordSet.h"
#include <iostream>
#include <string.h>

#include "DbResult.h"
#include "DbRequest.h"

DbMysql::DbMysql():m_pMysql( NULL ),m_bOpen( false ) 
{	
	memset(m_arrHost,0,sizeof(m_arrHost));
	memset(m_arrUser,0,sizeof(m_arrUser));
	memset(m_arrPasswd,0,sizeof(m_arrPasswd));
	memset(m_arrDBName,0,sizeof(m_arrDBName));
	static uint32 s_nIncDbID = 0;
	m_nID = ++s_nIncDbID;
}

DbMysql::~DbMysql()
{
	while ( m_queAsyncSQLRequest.size() )
	{
		SSleep(100);
	}
	this->Close();
}

uint32 DbMysql::ID()
{
	return m_nID;
}

void DbMysql::Release()
{ 
	delete this;
}

int32 DbMysql::OnThreadDestroy()
{
	mysql_thread_end();
	return 0;
}

void DbMysql::Close()
{
	m_bOpen = false;
	SSleep(100);
	if (m_pMysql != NULL)
	{
		mysql_close(m_pMysql);
		m_pMysql = NULL;
	}
}

bool DbMysql::Open(const char* pHost, const char* pUsernName, const char* pPassword, const char* pDBName, bool bSQLChk /*= true*/)
{
	strcpy(m_arrHost, pHost);
	strcpy(m_arrUser, pUsernName);
	strcpy(m_arrPasswd, pPassword);
	strcpy(m_arrDBName, pDBName);

	if (!pHost || !pUsernName || !pPassword || !pDBName)
		return false;

	if (m_pMysql = this->Connect( m_arrHost, m_arrUser, m_arrPasswd, m_arrDBName))
	{	
		m_bOpen =  true;
		thread t(boost::bind( &DbMysql::OnThreadCreate, this ));
		return true;
	}else
	{
		printf("ERROR: dbtool connect failed.\n");
		return false;
	}
}

MYSQL* DbMysql::Connect( char* szHost, char* szUser, char* szPasswd, char* szDB, uint32 nPort, char* szSocket, int32 nTimeout)
{	
	if(MYSQL* pHdbc = mysql_init( NULL ))
	{
		mysql_options( pHdbc , MYSQL_READ_DEFAULT_GROUP , "" );
		mysql_options( pHdbc , MYSQL_OPT_CONNECT_TIMEOUT , "" );

		pHdbc->options.read_timeout = nTimeout;
		pHdbc->reconnect = 1;
		pHdbc->free_me = 1;

		if ( mysql_real_connect( pHdbc , szHost , szUser , szPasswd , szDB ,
			nPort , szSocket , CLIENT_MULTI_RESULTS | CLIENT_MULTI_STATEMENTS | CLIENT_FOUND_ROWS | CLIENT_INTERACTIVE ) )
		{
			mysql_set_character_set( pHdbc, "utf8" );
			mysql_set_server_option( pHdbc, MYSQL_OPTION_MULTI_STATEMENTS_ON );
			return pHdbc;
		}else
		{
			printf( "ERROR: dbtool real connect failed: %s \n" , mysql_error(pHdbc) );
			mysql_close(pHdbc);
			return NULL;
		}
	}
	return NULL;
}

uint32 DbMysql::GetRequestSize()
{ 
	return m_queAsyncSQLRequest.size();
}

uint32 DbMysql::GetResultSize()
{ 
	return m_queAsyncSQLResult.size(); 
}

MYSQL* DbMysql::GetDBHandle()
{
	return m_pMysql;
}

bool DbMysql::IsOpen()
{
	return m_bOpen;
}

/*
SQL替换字符串，确保数据库安全 
*/
void DbMysql::wsSQLReplaceStr(char *to,const char *from,unsigned long length)
{
	// 暂时还不知道该替换些啥，先原样返回就好了 
	to[0] = '\0';
	memcpy( to, from, length);
	to[length] = '\0';
}

int32 DbMysql::OnThreadCreate() 
{
	while( m_bOpen && m_pMysql )
	{
		this->OnThreadProcess();
	}
	return 0;
}

int32 DbMysql::OnThreadProcess()
{
	if (m_queAsyncSQLRequest.size())
	{
		m_cQueueMutex.lock();
		DbRequest* pRequest = m_queAsyncSQLRequest.front();
		m_queAsyncSQLRequest.pop();
		m_cQueueMutex.unlock();

		if(pRequest->m_pCallBack == NULL)
		{
			m_cDBMutex.lock();
			if ( mysql_query( m_pMysql, pRequest->m_arrSql) != 0 )
			{
				printf("[MYSQL]Error in database requesting %s\n",mysql_error(m_pMysql));
				S_SAFE_DELETE(pRequest);
				m_cDBMutex.unlock();
				return 0;			
			}
			MYSQL_RES* pRes = mysql_store_result(m_pMysql);
			mysql_free_result( pRes );
			mysql_next_result(m_pMysql); 
			m_cDBMutex.unlock();
			S_SAFE_DELETE(pRequest);
		}else
		{
			MYSQL_RES* pRES = ResultSQL( pRequest->m_arrSql );
			if(pRES == NULL)
			{
				S_SAFE_DELETE(pRequest);
				return NULL;
			}
			DbRecordSet* pRecordSet = new DbRecordSet();
			if (!pRecordSet || !pRecordSet->Create(pRES))
			{
				S_SAFE_DELETE(pRequest);
				S_SAFE_DELETE(pRecordSet);
				return 0;					
			}
			while(mysql_next_result(m_pMysql) == 0)
			{
				MYSQL_RES* pRes = mysql_store_result(m_pMysql);
				mysql_free_result( pRes );
			}

			DbResult* pResult = new DbResult();
			pResult->m_bFlag		= true;
			pResult->m_pRecordSet	= pRecordSet;
			pResult->m_pCallResult = pRequest->m_pCallBack;

			S_SAFE_DELETE(pRequest);

			m_cQueueMutex.lock();
			m_queAsyncSQLResult.push( pResult );
			m_cQueueMutex.unlock();
		}
		return 0;
	}
	else
	{
		SSleep(1);
		return 0;
	}
}

MYSQL_RES* DbMysql::ResultSQL(const char* pszSQL)
{
	m_cDBMutex.lock();
	if(mysql_query(m_pMysql,pszSQL) != 0 )	// =0 suceess, !=0 fail
	{	
		printf("Exec sql Fail,sql=%s, error:%s\n",pszSQL,mysql_error(m_pMysql));
		m_cDBMutex.unlock();
		return NULL;
	}
	MYSQL_RES* pRes = mysql_store_result(m_pMysql);
	if (pRes == NULL) {
		m_cDBMutex.unlock();
		return NULL;
	}
	m_cDBMutex.unlock();
	return pRes; //mysql_free_result(pRes); 在使用完成pRes后，调用释放 
}

int32 DbMysql::QuerySQL(const char* pszSQL )
{
	m_cDBMutex.lock();
#ifdef _DEBUG
	printf("[MYSQL]Exec sql:%s\n", pszSQL);
#endif // _DEBUG
	if(mysql_query(m_pMysql,pszSQL) != 0 )	// =0 suceess, !=0 fail
	{	
		printf("Exec sql Fail,sql=%s, error:%s\n",pszSQL,mysql_error(m_pMysql));
		m_cDBMutex.unlock();
		return -1;
	}
	m_cDBMutex.unlock();
	return 0;
}

void DbMysql::Escape(char* pOutSql,const char* pInSql)
{
	mysql_real_escape_string(m_pMysql,pOutSql,pInSql,::strlen(pInSql));
}

bool DbMysql::ExecAsyncSQL(const char* pszSQL, MyDBCallBack* pCallBack)
{
	if ( !this->IsOpen() )
		return false;

	if ( strlen(pszSQL) == 0 )
	{
		cerr << "sql command is null" << endl;
		return false;
	}

	// chk sql
	if ( !(pszSQL && strlen(pszSQL) > 0 && strlen(pszSQL) < 10240) )
	{
		cerr << "strlen(pszSQL = " << strlen(pszSQL) << endl;
		cerr << "no sql command" << endl;
		return false;
	}

	DbRequest* pRequest = new DbRequest;
	if (pRequest == NULL) {
		cout << "memory (new dbRequest) allocating failed." << endl;
		return false;
	}

	pRequest->m_pCallBack = pCallBack;
	strncpy( pRequest->m_arrSql , pszSQL , ::strlen(pszSQL));

	m_cQueueMutex.lock();
	m_queAsyncSQLRequest.push( pRequest );
	m_cQueueMutex.unlock();

	return true;
}

bool DbMysql::CheckSQL( const char* szSQL )
{
	if ( !szSQL )
		return false;

	char pszSQL[DB_MAX_SQL] = "";
	strcpy( pszSQL , szSQL );
	if ( 0 == strcmp(pszSQL, "UPDATE") )	
	{
		if ( !strstr(pszSQL, "WHERE") || !strstr(pszSQL, "LIMIT") )
		{
			return false;
		}
	}
	if ( 0 == strcmp(pszSQL, "DELETE") )
	{
		if ( !strstr(pszSQL, "WHERE") )
		{
			return false;
		}
	}

	return true;
}

IDbResult* DbMysql::GetAsyncResult()
{
	if (m_queAsyncSQLResult.size())
	{
		m_cQueueMutex.lock();
		IDbResult* pResult = m_queAsyncSQLResult.front();
		m_queAsyncSQLResult.pop();
		m_cQueueMutex.unlock();
		return pResult;
	}
	return NULL;
}

void DbMysql::BinaryToString(char* pBinaryData,char* o_strBinarayData,int nLen)
{
	assert(nLen > 0 && nLen < 65336);
	assert(o_strBinarayData);
	memset(o_strBinarayData,0,nLen);
	mysql_real_escape_string(m_pMysql,o_strBinarayData,pBinaryData,nLen);
}

int32 DbMysql::ExecSelect(const char* tableName,const dbCol *column,const char *where,const char *order,IDbRecordSet& o_rRecordSet)
{

	int retval=-1;
	long retsize=0;
	if (tableName==NULL || column==NULL)
	{
		printf("MysqlClientHandle::exeSelect null pointer error.\n");
		return retval;
	}

	std::string sql = CreateSelectSql(tableName,column,where,order);
	if(sql.empty())
	{
		printf("MysqlClientHandle::FetchSelectSql null pointer error.\n");
		return retval;
	}

	MYSQL_RES* pRES = ResultSQL(sql.c_str());
	if(pRES == NULL || !o_rRecordSet.Create(pRES))
	{
		mysql_free_result( pRES );
		printf("MysqlClientHandle::RecordSet Create null pointer error.\n");
		return retval;
	}

	while(mysql_next_result(m_pMysql) == 0)
	{
		MYSQL_RES* pRes = mysql_store_result(m_pMysql);
		mysql_free_result( pRes );
	}
	return 0;
}

std::string DbMysql::CreateSelectSql(const char* tableName,const dbCol *column,const char *where,const char *order,int32 limit/* = 0*/,int32 limit_from/* = 0*/, bool UseBak/* = false*/)
{
	int32 retsize=0;
	const dbCol *temp;
	bool first=true;
	std::string sql;
	sql+="SELECT ";
	temp = column;
	while(temp->name) 
	{   
		retsize+=temp->size;
		if (strlen(temp->name) > 0)
		{
			if (first)
				first=false;
			else
				sql+=",";
			sql+=temp->name;
		}
		temp++;
	}
	if (strlen(tableName)>0)
	{
		sql+=" FROM `";
		sql+=tableName;
		sql += "`";
	}
	if (where!=NULL && strlen(where)>0)
	{
		sql+=" WHERE ";
		sql+=where;
	}
	if (order!=NULL && strlen(order)>0)
	{
		sql+=" ORDER BY ";
		sql+=order;
	}
	if (limit_from)
	{
	  char tmp[32];
	  _snprintf(tmp,sizeof(tmp) - 1,"%u,%u",limit_from,limit);
	  sql+=" LIMIT ";
	  sql+=tmp;
	}
	else if (limit)
	{
	  char tmp[32];
	  _snprintf(tmp,sizeof(tmp) - 1,"%u",limit);
	  sql+=" LIMIT ";
	  sql+=tmp;
	} 
	return sql; 
}

int32 DbMysql::ExecInsert(const char *tableName,const dbCol *column,const char *data)
{
	const dbCol *temp;
	if (tableName == NULL || data==NULL || column==NULL)
	{
		printf("DbMysql::ExecInsert null pointer error.\n");
		return -1;
	}
	std::ostringstream strSql;
	strSql << "INSERT INTO `";
	strSql << tableName;
	strSql << "` ( ";
	temp = column; 
	bool first=true;
	while(temp->name) 
	{
		int len = strlen(temp->name);
		if (len > 0)
		{       
			if (first)
				first=false;
			else
				strSql << ",";
			strSql << temp->name;
		}
		temp++; 
	}
	strSql << ") VALUES( ";

	first = true;
	temp = column; 
	int offset=0;
	// [ranqd] 需要绑定的二进制数据容器 
	BINDDATA ltBind;
	while(temp->name) 
	{
		if (strlen(temp->name)!=0)
		{       
			if (first)
				first=false;
			else
				strSql << ",";
			switch(temp->type)
			{
			case DB_CHAR:
				{
					short temp = 0x00ff & (*(char *)(data+offset));
					strSql << temp;
				}
				break;
			case DB_BYTE:
				{
					WORD temp = 0x00ff & (*(BYTE*)(data+offset));
					strSql << temp;
				}
				break;
			case DB_WORD:
				strSql << *(int16*)(data+offset);
				break;
			case DB_DWORD:
				strSql << *(int32*)(data+offset);
				break;
			case DB_FLOAT:
				strSql << *(float*)(data+offset);
				break;
			case DB_QWORD:
				strSql << *(int64*)(data+offset);
				break;
			case DB_STR:
				{
					uint32 len = strlen((char *)(data+offset));
					len = (len > temp->size) ? temp->size : len;
					char* strData=new char[len * 2 + 1];
					strData[0] = '\0';
					wsSQLReplaceStr(strData,(char *)(data+offset),len);
					strSql << "\'" << strData << "\'";
					delete[] strData;
				}
				break;
			case DB_BIN:
				{
					// [ranqd] SQLServer对二进制数据的处理不能直接放到SQL语句中，先把数据放到ltBind容器，后面执行SQL前再绑定 
					BDATA td;					
					//td.pData = new char[temp->size * 2 + 1];
					wsSQLReplaceStr( td.pData,(char *)(data+offset),temp->size );
					td.len = temp->size;
					ltBind.push_back(td);
					strSql << "?";
				}
				break;
			default:
				printf("invalid zebra mysql type.\n");
				return (int32)-1;
			}
		}
		if (temp->size==0)
			offset+=(*((int32 *)(data+offset)) + sizeof(int32));
		else
			offset+=temp->size;
		temp++;
	}   
	strSql << ")";
	return QuerySQL(strSql.str().c_str());
}


int32 DbMysql::ExecDelete(const char *tableName,const char *where)
{
	if (tableName==NULL)
	{
		return (DWORD)-1;
	}
	std::string strSql="DELETE FROM ";
	strSql+=tableName;
	if (where)
	{
		strSql+=" WHERE ";
		strSql+=where;
	}
	return QuerySQL(strSql.c_str());
}

int32 DbMysql::ExecUpdate(const char *tableName,const dbCol *column,const char *data,const char *where)
{
	std::ostringstream out_sql;
	const dbCol *temp;
	if (tableName==NULL || column==NULL || data==NULL)
	{
		printf("MysqlClientHandle::exeUpdate null pointer error.\n");
		return (DWORD)-1;
	}
	out_sql << "UPDATE [" << tableName << "] SET ";
	temp = column;
	bool first=true;
	int offset=0;
	// [ranqd] 需要绑定的二进制数据临时存放点 
	BINDDATA ltBind;
	BDATA td;
	while(temp->name)
	{
		int len = strlen(temp->name);
		// [ranqd] CHARID在SQLServer系统中为自动增长的主键，不能被Update 
		if (len > 0 && strcmp(temp->name, "`CHARID`") != 0)
		{
			if (first)
				first=false;
			else
				out_sql << ",";
			out_sql << temp->name << "=";
			switch(temp->type)
			{
			case DB_CHAR:
				{
					short temp = 0x00ff & (*(char *)(data+offset));
					out_sql << temp;
				}
				break;
			case DB_BYTE:
				{
					WORD temp = 0x00ff & (*(BYTE*)(data+offset));
					out_sql << temp;
				}
				break;
			case DB_WORD:
				out_sql << *(int16 *)(data+offset);
				break;
			case DB_DWORD:
				out_sql << *(int32 *)(data+offset);
				break;
			case DB_QWORD:
				out_sql << *(int64 *)(data+offset);
				break;
			case DB_STR:
				{
					DWORD len=strlen((char *)(data+offset));
					len = (len > temp->size) ? temp->size : len;
					char* strData=new char[len * 2 + 1];
					wsSQLReplaceStr( strData,(char *)(data+offset),len );
					out_sql << "\'" << strData << "\'";
					delete[] strData;
				}
				break;
			case DB_BIN:
				{ 
					// [ranqd] SQLServer对二进制数据的处理不能直接放到SQL语句中，先把数据放到ltBind容器，后面执行SQL前再绑定 
					//  td.pData = new char[temp->size * 2 + 1]; 
					wsSQLReplaceStr( td.pData,(char *)(data+offset),temp->size );
					td.len = temp->size;
					ltBind.push_back(td);
					out_sql << "?";
				}
				break;
			case DB_BIN2:
				{
					DWORD size = *((DWORD *)(data+offset));
					size += sizeof(DWORD);
					// [ranqd] SQLServer对二进制数据的处理不能直接放到SQL语句中，先把数据放到ltBind容器，后面执行SQL前再绑定 
					BDATA td;
					//  td.pData = new char[size * 2 + 1];
					wsSQLReplaceStr( td.pData,(char *)(data+offset),size );
					td.len = size;
					ltBind.push_back(td);
					out_sql << "?";
				}
				break;
			default:
				printf("invalid zebra mysql type.\n");
				return (DWORD)-1;
			}
		}
		if (temp->size==0)
			offset+=(*((DWORD *)(data+offset)) + sizeof(DWORD));
		else
			offset+=temp->size;
		temp++;
	}
	if (where!=NULL)
	{
		out_sql << " WHERE " << where;
	}

	printf("update %s\n",out_sql.str().c_str());

	char sql[8196];
	sql[0] = '\0';
	strncpy( sql, out_sql.str().c_str(), sizeof(sql));
	return QuerySQL(sql);
}


bool DbMysql::Commit()
{
	return (0 == QuerySQL("COMMIT"));
}

bool DbMysql::Rollback()
{
	return (0 == QuerySQL("ROLLBACK"));
}


