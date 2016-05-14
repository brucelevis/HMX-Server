#include "ResourceMgr.h"

#include "def_buff.h"
#include "ResourceBuffer.h"

#include "SceneMap.h"

#define XMLATTR(KEY) "<xmlattr>."#KEY 


using namespace twp::app;
using namespace boost::property_tree;

ResourceMgr::ResourceMgr()
{
}

ResourceMgr::~ResourceMgr()
{
}

//-----------------------------加载资源---------------------------------------------------

bool ResourceMgr::LoadAllResources()
{
	bool bResult = true;
	if(!LoadCharacterLevelBaseResource(RES_DIR "character_level_base.xml"))
	{
		bResult =  false;
	}

	if(!LoadMapInfoResource(RES_DIR "map_info.xml"))
	{
		bResult = false;
	}

	if(!LoadRoleTypeInitResource(RES_DIR "role_create_info.xml"))
	{
		bResult = false;
	}

	if(!LoadQuestResource(RES_DIR "quest_info.xml"))
	{
		bResult = false;
	}

	// TODO 创建场景地图，初始化各模块准备数据 
	
	//m_mapZoneInfo

	SceneMapManager& rSceneMapMgr = *SceneMapManager::Instance();
	MapInfoResMapType::iterator it = m_mapMapInfo.begin();
	for (; it != m_mapMapInfo.end(); ++it )
	{
		StMapInfoResCfg& rMapInfo = it->second;
		int32 nMapID = rMapInfo.nMapId;	
		int32 nSceneType = rMapInfo.nSceneType;

		const StMapZoneResCfg* zoneRes = GetMapZoneRes(nMapID);
		if(zoneRes == NULL)
		{
			ASSERT(zoneRes);
			return false;
		}

		int32 nXAmount = zoneRes->nXAmount;
		int32 nZAmount = zoneRes->nYAmount;
		int32 nXCellLength = zoneRes->nXCellLength;
		int32 nZCellLength = zoneRes->nYCellLength;
		/*
			Y
			|
			|
			--------->X
		*/
		Point2D sTopLeftCoordinate(0, nZAmount * nZCellLength);
		rSceneMapMgr.AddSceneMap(nMapID,nSceneType,sTopLeftCoordinate,nXAmount,nZAmount,nXCellLength,nZCellLength,zoneRes->regionValue);
	}
	return bResult;
}

const StMapZoneResCfg* ResourceMgr::GetMapZoneRes(int32 nMapID)
{
	MapZoneResMapType::iterator it = m_mapZoneInfo.find(nMapID);
	if( it != m_mapZoneInfo.end())
	{
		return &(it->second);
	}
	return NULL;
}


// 角色等级 
bool ResourceMgr::LoadCharacterLevelBaseResource(const std::string& strFileName)
{
	ptree cPtreeDoc;
	ptree cRootNode;
	read_xml(strFileName, cPtreeDoc, xml_parser::trim_whitespace);
	cRootNode = cPtreeDoc.get_child("root");
	for(BOOST_AUTO(pos,cRootNode.begin());pos != cRootNode.end();++pos)  //boost中的auto     
	{    
		ptree& itemNode = pos->second;
		StCharacterLevelCfg sItem;
		sItem.nIdx = itemNode.get<int32>(XMLATTR(idx));
		sItem.nCharType = itemNode.get<int32>(XMLATTR(char_type));
		sItem.nLevel = itemNode.get<int32>(XMLATTR(level));
		sItem.nUpLevelExp = itemNode.get<int32>(XMLATTR(exp_max));
		sItem.nRedMax = itemNode.get<int32>(XMLATTR(red_max));
		sItem.nRedRecover = itemNode.get<int32>(XMLATTR(red_recover));
		sItem.nBlueMax = itemNode.get<int32>(XMLATTR(blue_max));
		sItem.nBlueRecover = itemNode.get<int32>(XMLATTR(blue_recover));
		sItem.nPhysicAttack = itemNode.get<int32>(XMLATTR(attack));
		sItem.nPhysicDefend = itemNode.get<int32>(XMLATTR(defend));
		sItem.nSpellAttack = itemNode.get<int32>(XMLATTR(inattack));
		sItem.nSpellDefend = itemNode.get<int32>(XMLATTR(indefend));
		sItem.nBaojilv = itemNode.get<int32>(XMLATTR(crit_rate));
		sItem.nBaoji = itemNode.get<int32>(XMLATTR(crit_val));
		sItem.nGedanglv = itemNode.get<int32>(XMLATTR(block_rate));
		sItem.nGedang = itemNode.get<int32>(XMLATTR(block_val));
		sItem.nShanbilv = itemNode.get<int32>(XMLATTR(dodge_rate));
		sItem.nMingZhonglv = itemNode.get<int32>(XMLATTR(hit_rate));
		m_mapCharacterLevelBase.insert(std::make_pair(sItem.nIdx,sItem));

	}    
	return true;
}


bool ResourceMgr::LoadMapInfoResource(const std::string& strFileName)
{
	ptree cPtreeDoc;
	ptree cRootNode;
	read_xml(strFileName, cPtreeDoc, xml_parser::trim_whitespace);
	cRootNode = cPtreeDoc.get_child("root");
	for(BOOST_AUTO(pos,cRootNode.begin());pos != cRootNode.end();++pos)  //boost中的auto     
	{    
		ptree& itemNode = pos->second;
		StMapInfoResCfg sItem;
		sItem.nMapId = itemNode.get<int32>(XMLATTR(id));
		sItem.nSceneType = itemNode.get<int32>(XMLATTR(scene_type));
		sItem.strSceneFileName = itemNode.get<std::string>(XMLATTR(scenefile));
		sItem.strZoneFileName = itemNode.get<std::string>(XMLATTR(zonefile));
		m_mapMapInfo.insert(std::make_pair(sItem.nMapId,sItem));

	}

	// 加载地图区域与阻挡 
	MapInfoResMapType::iterator it = m_mapMapInfo.begin();
	for (; it != m_mapMapInfo.end();++it)
	{
		int32 nMapId = it->first;
		StMapInfoResCfg rItem = it->second;
		if(!LoadMapZoneResource(nMapId,std::string(ZONE_DIR + rItem.strZoneFileName)))
		{
			return false;
		}

		if(!LoadMapSceneResource(nMapId,std::string(SCENE_DIR + rItem.strSceneFileName)))
		{
			return false;
		}
	}

	return true;
}

/*------------------------------------------------
* 加载地图区域资源 
*-----------------------------------------------*/
bool ResourceMgr::LoadMapZoneResource(int32 nMapId,const std::string& strFileName)
{
	ptree cPtreeDoc;
	ptree cMapNode;
	//ptree cBlockNode;
	//ptree cNpcNode;
	ptree cData;

	read_xml(strFileName, cPtreeDoc, xml_parser::trim_whitespace);
	cMapNode = cPtreeDoc.get_child("map");

	StMapZoneResCfg sItem;

	sItem.nMapId = cMapNode.get<int32>(XMLATTR(id));
	if(sItem.nMapId != nMapId)
	{
		return false;
	}

	sItem.nXAmount = cMapNode.get<int32>(XMLATTR(maxX));
	sItem.nYAmount = cMapNode.get<int32>(XMLATTR(maxY));
	sItem.nXCellLength = cMapNode.get<int32>(XMLATTR(gridSizeX));
	sItem.nYCellLength = cMapNode.get<int32>(XMLATTR(gridSizeY));
	sItem.nBlock = cMapNode.get<int32>(XMLATTR(block));
	sItem.nNpc = cMapNode.get<int32>(XMLATTR(npc));

//	sItem.strData = cMapNode.get<string>(XMLATTR(data));


	//cBlockNode = cMapNode.get_child("block");
	//cNpcNode = cMapNode.get_child("npc");
	cData = cMapNode.get_child("data");
	sItem.strData = cData.data();

	ASSERT(sItem.strData.length() < 10240 );

	static char arrValues[10240];
	memset(arrValues,0,sizeof(10240));
	strncpy(arrValues,sItem.strData.c_str(), sItem.nXAmount * sItem.nYAmount);
	//// 将data=000022226666的数据进行小格子化 
	for(int32 j = 0 ; j < sItem.nYAmount; ++j)
	{
		std::vector<int8> items_vec;
		for (int32 i = 0 ; i < sItem.nXAmount; ++i)
		{
			int32 nIndex = j * sItem.nYAmount + i;
			char  a =  arrValues[nIndex];
			items_vec.push_back(atoi(&a));

		}
		sItem.regionValue.push_back(items_vec);
	}  

	m_mapZoneInfo.insert(std::make_pair(sItem.nMapId,sItem));
	return true;
}

/*------------------------------------------------
* 加载地图阻挡资源
*-----------------------------------------------*/
bool ResourceMgr::LoadMapSceneResource(int32 nMapId,const std::string& strFileName)
{

	ptree cPtreeDoc;
	ptree cMapNode;
	ptree cData;
	//ptree cBlockNode;
	//ptree cNpcNode;

	read_xml(strFileName, cPtreeDoc, xml_parser::trim_whitespace);
	cMapNode = cPtreeDoc.get_child("map");

	StMapSceneResCfg sItem;

	sItem.nMapId = cMapNode.get<int32>(XMLATTR(id));
	if(sItem.nMapId != nMapId)
	{
		return false;
	}

	sItem.nXAmount = cMapNode.get<int32>(XMLATTR(maxX));
	sItem.nYAmount = cMapNode.get<int32>(XMLATTR(maxY));
	sItem.nXCellLength = cMapNode.get<int32>(XMLATTR(gridSizeX));
	sItem.nYCellLength = cMapNode.get<int32>(XMLATTR(gridSizeY));

	//cBlockNode = cMapNode.get_child("block");
	//cNpcNode = cMapNode.get_child("npc");

	cData = cMapNode.get_child("data");
	sItem.strData = cData.data();

	ASSERT(sItem.strData.length() < 10240);

	static char arrValues[10240];
	memset(arrValues, 0, sizeof(10240));
	strncpy(arrValues, sItem.strData.c_str(), sItem.nXAmount * sItem.nYAmount);
	//// 将data=000022226666的数据进行小格子化 
	for (int32 j = 0; j < sItem.nYAmount; ++j)
	{
		std::vector<int8> items_vec;
		for (int32 i = 0; i < sItem.nXAmount; ++i)
		{
			int32 nIndex = j * sItem.nYAmount + i;
			char  a = arrValues[nIndex];
			items_vec.push_back(atoi(&a));

		}
		sItem.regionValue.push_back(items_vec);
	}
	m_mapSceneInfo.insert(std::make_pair(sItem.nMapId,sItem));

	return true;
}

bool ResourceMgr::LoadRoleTypeInitResource(const std::string& strFileName)
{
	ptree cPtreeDoc;
	ptree cRootNode;
	read_xml(strFileName, cPtreeDoc, xml_parser::trim_whitespace);
	cRootNode = cPtreeDoc.get_child("root");
	for(BOOST_AUTO(pos,cRootNode.begin());pos != cRootNode.end();++pos)  //boost中的auto     
	{    
		ptree& itemNode = pos->second;
		StRoleTypeInitCfg  sItem;
		sItem.nRoleType = itemNode.get<int32>(XMLATTR(id));
		sItem.arrSkillId[0] = itemNode.get<int32>(XMLATTR(Gskill_1));
		sItem.arrSkillId[1] = itemNode.get<int32>(XMLATTR(Gskill_2));
		sItem.arrSkillId[2] = itemNode.get<int32>(XMLATTR(Gskill_3));
		sItem.arrSkillId[3] = itemNode.get<int32>(XMLATTR(Gskill_4));
		sItem.arrSkillId[4] = itemNode.get<int32>(XMLATTR(Gskill_5));
		sItem.nLandMapId = itemNode.get<int32>(XMLATTR(first_map_id));
		sItem.nLandX = itemNode.get<int32>(XMLATTR(cin_x));
		sItem.nLandY = itemNode.get<int32>(XMLATTR(cin_z));
		sItem.nInViewRange = itemNode.get<int32>(XMLATTR(come_into_area));
		sItem.nOutViewRange = itemNode.get<int32>(XMLATTR(be_off_area));
		sItem.nInitRed = itemNode.get<int32>(XMLATTR(init_red));
		sItem.nInitBlue = itemNode.get<int32>(XMLATTR(init_blue));
		sItem.nAttackType = itemNode.get<int32>(XMLATTR(attack_type));
		sItem.nSpeed = itemNode.get<int32>(XMLATTR(speed));
		m_mapRoleTypeInitInfo.insert(std::make_pair(sItem.nRoleType,sItem));
	}    
	return true;
}

bool ResourceMgr::LoadQuestResource(const std::string& strFileName)
{
	ptree cPtreeDoc;
	ptree cRootNode;
	read_xml(strFileName, cPtreeDoc, xml_parser::trim_whitespace);
	cRootNode = cPtreeDoc.get_child("root");
	for(BOOST_AUTO(pos,cRootNode.begin());pos != cRootNode.end();++pos)  //boost中的auto     
	{    
		ptree& itemNode = pos->second;
		StQuestInfoResCfg  sItem;
		sItem.nId = itemNode.get<int32>(XMLATTR(id));
		sItem.nType = itemNode.get<int32>(XMLATTR(type));
		sItem.nSubType = itemNode.get<int32>(XMLATTR(subtype));
		//sItem.strTitle = itemNode.get<std::string>(XMLATTR(title));
		//sItem.strContent = itemNode.get<std::string>(XMLATTR(content));
	
		sItem.accept[0].nType = itemNode.get<int32>(XMLATTR(apt_type_0));
		sItem.accept[0].nArg0 = itemNode.get<int32>(XMLATTR(apt_arg_0_0));
		sItem.accept[0].nArg1 = itemNode.get<int32>(XMLATTR(apt_arg_0_1));
		sItem.accept[1].nType = itemNode.get<int32>(XMLATTR(apt_type_1));
		sItem.accept[1].nArg0 = itemNode.get<int32>(XMLATTR(apt_arg_1_0));
		sItem.accept[1].nArg1 = itemNode.get<int32>(XMLATTR(apt_arg_1_1));
		sItem.accept[2].nType = itemNode.get<int32>(XMLATTR(apt_type_2));
		sItem.accept[2].nArg0 = itemNode.get<int32>(XMLATTR(apt_arg_2_0));
		sItem.accept[2].nArg1 = itemNode.get<int32>(XMLATTR(apt_arg_2_1));
		sItem.accept[3].nType = itemNode.get<int32>(XMLATTR(apt_type_3));
		sItem.accept[3].nArg0 = itemNode.get<int32>(XMLATTR(apt_arg_3_0));
		sItem.accept[3].nArg1 = itemNode.get<int32>(XMLATTR(apt_arg_3_1));

		sItem.finish[0].nType = itemNode.get<int32>(XMLATTR(fsh_type_0));
		sItem.finish[0].nArg0 = itemNode.get<int32>(XMLATTR(fsh_arg_0_0));
		sItem.finish[0].nArg1 = itemNode.get<int32>(XMLATTR(fsh_arg_0_1));
		sItem.finish[1].nType = itemNode.get<int32>(XMLATTR(fsh_type_1));
		sItem.finish[1].nArg0 = itemNode.get<int32>(XMLATTR(fsh_arg_1_0));
		sItem.finish[1].nArg1 = itemNode.get<int32>(XMLATTR(fsh_arg_1_1));
		sItem.finish[2].nType = itemNode.get<int32>(XMLATTR(fsh_type_2));
		sItem.finish[2].nArg0 = itemNode.get<int32>(XMLATTR(fsh_arg_2_0));
		sItem.finish[2].nArg1 = itemNode.get<int32>(XMLATTR(fsh_arg_2_1));
		sItem.finish[3].nType = itemNode.get<int32>(XMLATTR(fsh_type_3));
		sItem.finish[3].nArg0 = itemNode.get<int32>(XMLATTR(fsh_arg_3_0));
		sItem.finish[3].nArg1 = itemNode.get<int32>(XMLATTR(fsh_arg_3_1));

		sItem.award[0].nType = itemNode.get<int32>(XMLATTR(ard_type_0));
		sItem.award[0].nArg0 = itemNode.get<int32>(XMLATTR(ard_arg_0_0));
		sItem.award[0].nArg1 = itemNode.get<int32>(XMLATTR(ard_arg_0_1));
		sItem.award[1].nType = itemNode.get<int32>(XMLATTR(ard_type_1));
		sItem.award[1].nArg0 = itemNode.get<int32>(XMLATTR(ard_arg_1_0));
		sItem.award[1].nArg1 = itemNode.get<int32>(XMLATTR(ard_arg_1_1));
		sItem.award[2].nType = itemNode.get<int32>(XMLATTR(ard_type_2));
		sItem.award[2].nArg0 = itemNode.get<int32>(XMLATTR(ard_arg_2_0));
		sItem.award[2].nArg1 = itemNode.get<int32>(XMLATTR(ard_arg_2_1));
		sItem.award[3].nType = itemNode.get<int32>(XMLATTR(ard_type_3));
		sItem.award[3].nArg0 = itemNode.get<int32>(XMLATTR(ard_arg_3_0));
		sItem.award[3].nArg1 = itemNode.get<int32>(XMLATTR(ard_arg_3_1));

		m_mapQuestResInfo.insert(std::make_pair(sItem.nId,sItem));
	}    
	return true;
}


/////////////////////////////////////GET/////////////////////////////////////

const StCharacterLevelCfg* ResourceMgr::GetCharacterLevelBase(int32 nRoleType,int32 nLevel)
{
	CharacterLevelMapType::iterator it = m_mapCharacterLevelBase.find(nRoleType * 10000 + nLevel);
	if( it != m_mapCharacterLevelBase.end())
	{
		return &(it->second);
	}
	return NULL;
}


const StRoleTypeInitCfg* ResourceMgr::GetRoleTypeInitInfoRes(int32 nRoleType)
{
	RoleTypeInitMapType::iterator it = m_mapRoleTypeInitInfo.find(nRoleType);
	if( it != m_mapRoleTypeInitInfo.end())
	{
		return &(it->second);
	}
	return NULL;
}


