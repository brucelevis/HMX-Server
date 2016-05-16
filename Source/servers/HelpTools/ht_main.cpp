#include "HelpTools_PCH.h"
#include "../DyNetMysql/DbIncludes.h"
#include "../DyNetSocket/NetIncludes.h"
#include "../Shared/CommonDef/SharedIncludes.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>

#include "StockCrawl.h"

/*
*
*  Detail: ��������� 936�ַ� 
*
*  Created by hzd 2014-11-28
*
*/

char SQL_BUFFER[ MAX_SQL_BUFFER ];

DbConnManager* gDbServerMgr = DbConnManager::Instance(); // db 

bool gIsRun = true;

int HttpGet(const char* host,const char* path,std::string& o_reponse);

bool Init();

void CreateNickData(IDbBase* pDb);

void SinaStock(const char* nCode);

int main(int argc, const char * argv[])
{
	// ��ʼ������������Ҫ׼���õ����� 
	bool bResult = Init();

	if(!bResult)
	{
		//FLOG_INFO("Init Fail");
		ASSERT(0);
		return 0;
	}

	return 0;
}

#pragma pack(push,1)

struct MixNumber
{
	int32 type;
	int64 value;
};

#pragma pack(pop)

bool Init()
{
	bool bResult = NetServerOpt::Instance()->LoadServerConfig("ht_cfg.xml",ESERVER_TYPE_DP);
	if(!bResult)
	{
		printf("Load cfg fail\n");
		return false;
	}

	// �������ݿ�  
	DatabaseInfoOpt& rDbInfo = NetServerOpt::Instance()->GetDatabaseInfo();
	IDbBase* pDb = gDbServerMgr->AddDatabase(rDbInfo.strHost.c_str(),rDbInfo.strUser.c_str(),rDbInfo.strPass.c_str(),rDbInfo.strDbname.c_str());
	if(pDb == NULL)
	{
		printf("Connnect to Database fail!\n");
		return false;
	}

	//CreateNickData(pDb);

	static const dbCol count_define[] = {
		{ "`type`",DB_DWORD,sizeof(int32) },
		{ "`value`",DB_QWORD,sizeof(int64) },
		{ "`time`",DB_DWORD,sizeof(int32) },
		{ "`name`",DB_STR,32 },
		{ NULL,0,0}
	};

	//DbRecordSet recordset;
	//pDb->ExecSelect("mixitem_number_info",count_define,NULL,NULL,recordset);
	//for (int32 i = 0; i < recordset.CountRecord();++i)
	//{
	//	const MixNumber* num = static_cast<const MixNumber*>(recordset.GetRecordData(i));
	//	int32 aa = 1;
	//}

#pragma pack(push,1)

	struct StAAInsert
	{
		int32 nType;
		int64 nValue;
		int32 nTime;
		char name[32];
		StAAInsert()
		{
			memset(name,0,32);
		}
	};
#pragma pack(pop)

	// ��ҳ����GBK���룬������Ҫת�룬Ҫ��Ҫת�룬�����Լ���ʵ����������� 
	//StAAInsert a;
	//a.nType = 7;
	//a.nValue = 10;
	//a.nTime = 1300001;

	//char arrUtf8[32];
	//memset(arrUtf8,0,32);
	//GBKToUTF8("�й�",arrUtf8,32);
	//arrUtf8[31] = '\0';
	//memcpy(a.name,arrUtf8,32);

	//uint64 nID = pDb->ExecInsert("mixitem_number_info",count_define,(char*)&a);
	//std::string reponse;
	//	int32 n = HttpGet("hq.sinajs.cn","/list=sh601006",reponse);


	// ���stock_info�������еĹ�Ʊ���룬�ٱ���ÿ����ȥ��� 

	//SinaStock("601006");


	StockCrawl::Instance()->UpdaeStockInfo();

	return true;

}

void CreateNickData(IDbBase* pDb)
{
	std::string arrXing[] = {"��","��","��","��","��","��","��","ŷ��","�Ϲ�","����"};
	int32 nXingSize = sizeof(arrXing) / sizeof(arrXing[0]);

	std::string arrMing[] = {"����","����","��","���","��Զ","�Ĳ�","Ӣ��","ò��","����","���",\
		"�ž�","����","���","����","����","ٻѩ","��ܿ","��ѩ","����","����"};
	int32 nMingSize = sizeof(arrMing) / sizeof(arrMing[0]);

	// �������ּ��뵽���ݿ��� 
	for (int32 i = 0 ; i < nXingSize; ++i)
	{
		for (int32 j = 0; j < nMingSize; ++j)
		{
			std::stringstream strName;
			strName << arrXing[i] << arrMing[j];
			char arrUtf8[32];
			memset(arrUtf8,0,32);
			GBKToUTF8(strName.str().c_str(),arrUtf8,32);
			arrUtf8[31] = '\0';
			memset(SQL_BUFFER,0,MAX_SQL_BUFFER);
			SPRINTF(SQL_BUFFER,"INSERT INTO `swa_data`.`names_info` (`name`) VALUES ('%s');",arrUtf8);
			SQL_BUFFER[ MAX_SQL_BUFFER - 1 ] = '\0';
			pDb->ResultSQL(SQL_BUFFER);
		}
	}

}

void SinaStock(const char* nCode)
{



}


