#include "GameServer_PCH.h"
#include "GeneralScene.h"

#include "SceneUser.h"

GeneralScene::GeneralScene(SceneMap& rSceneMap):m_rSceneMap(rSceneMap)
{
}

GeneralScene::~GeneralScene(void)
{
}

bool GeneralScene::CheckEnter(const stEnterSceneParam& param)
{

	return true;
}

bool GeneralScene::Enter(SceneUser* pUser)
{


	return true;
}

void GeneralScene::Leave(SceneUser* pUser)
{

}
