#include "CommonCharacter.h"


//  select `char_id`,`server_id`,`account_id`,`name`,`type`,`exp`,`level`,`land_mapid`,`land_x`,`land_y`,`instance_mapid`,`instance_x`,`instance_y`,`red`,`blue`,`bin_data` from `user_info` where `char_id`=i_char_id; 

const dbCol StCharacterTable::fields[] = {

	{ "char_id",	DB_QWORD, sizeof(nCharID) },
	{ "server_id" , DB_DWORD, sizeof(nServerID) },
	{ "account_id",DB_QWORD ,sizeof(nAccountID) },
	{ "name",DB_STR ,sizeof(arrName) },
	{ "type",DB_DWORD ,sizeof(nType) },
	{ "exp",DB_DWORD ,sizeof(nExp) },
	{ "level",DB_DWORD ,sizeof(nLevel) },
	{ "land_mapid",DB_DWORD ,sizeof(nLandMapId) },
	{ "land_x",	DB_DWORD, sizeof(nLandX) },
	{ "land_y" , DB_DWORD, sizeof(nLandY) },
	{ "instance_mapid",DB_DWORD ,sizeof(nInstanceMapId) },
	{ "instance_x",DB_DWORD ,sizeof(nInstanceX) },
	{ "instance_y",DB_DWORD ,sizeof(nInstanceY) },
	{ "red",DB_DWORD ,sizeof(nRed) },
	{ "blue",DB_DWORD ,sizeof(nBlue) },
	{ "bin_data",DB_BIN2 ,sizeof(binData) },
	{NULL,0,0}
};









