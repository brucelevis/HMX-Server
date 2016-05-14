#include "SceneChannel.h"


ObjPool<SceneChannel(ChannelPosition)>	g_cPositionChannelFactory;
ObjPool<SceneChannel(ChannelCreature)>	g_cCreatureChannelFactory;
ObjPool<SceneChannel(ChannelWear)>		g_cEquipmentChannelFactory;	
ObjPool<SceneChannel(ChannelSpell)>		g_cSpellChannelFactory;
ObjPool<SceneChannel(ChannelBuff)>		g_cBuffChannelFactory;

// 增加频道 
void SceneChannelManager::AddSceneChannel(Channel& rChannel)
{
	m_usetChannels.insert(&rChannel);
}

// 更新所有频道 
void SceneChannelManager::Update()
{
	UNORDERED_SET<Channel*>::iterator it = m_usetChannels.begin();
	UNORDERED_SET<Channel*>::iterator itEnd = m_usetChannels.end();
	for (;it != itEnd; ++it)
	{
		Channel* pTemp = *it;
		pTemp->Update();
	}
}



