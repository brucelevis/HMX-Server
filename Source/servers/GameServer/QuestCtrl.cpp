#include "GameServer_PCH.h"
#include "QuestCtrl.h"

#include "SharedIncludes.h"

#include "SceneUser.h"

QuestCtrl::QuestCtrl(SceneUser* pUser):m_pUser(pUser)
{

	InitAttribute();
}

QuestCtrl::~QuestCtrl(void)
{
}

void QuestCtrl::InData(const StQuestDataTable& i_data)
{
	//memcpy(&m_sData,&i_data,sizeof(m_sData));
}

void QuestCtrl::OutData(StQuestDataTable& o_Data)
{
	//memcpy(&o_Data,&m_sData,sizeof(o_Data));
}

void QuestCtrl::SendMainData()
{

	S2CQuestMainData sMsg;

	


	m_pUser->GetClientSession()->SendMsgToFep(&sMsg,sMsg.GetPackLength());

}

void QuestCtrl::InitAttribute()
{
	memset(m_sAttributeOffset, 0, sizeof(m_sAttributeOffset));

#define	INIT_ATTRIBUTE_OFFSET(index, name, set, handler)	\
	{\
	m_sAttributeOffset[index].nOffset = &(this->m_sData.name);\
	m_sAttributeOffset[index].nSize = sizeof(this->m_sData.name);\
	m_sAttributeOffset[index].bCanSet = set;\
	m_sAttributeOffset[index].hHandler = handler;\
	}

	// 偏移信息 
	//INIT_ATTRIBUTE_OFFSET(QUEST_ATTR_CHAR_ID, nCharID, false, NULL);
	//INIT_ATTRIBUTE_OFFSET(QUEST_ATTR_MAIN_LAST_ID, nMainLastID, true,  boost::bind(&QuestCtrl::OnMainLastIDChange, this, _1, _2));

#undef INIT_ATTRIBUTE_OFFSET
}



void QuestCtrl::OnMainLastIDChange(const ValueType& vOldValue, const ValueType& vNewValue)
{

}

void QuestCtrl::OnMainCurrIDChange(const ValueType& vOldValue, const ValueType& vNewValue)
{

}

void QuestCtrl::OnMainResult0Change(const ValueType& vOldValue, const ValueType& vNewValue)
{

}

void QuestCtrl::OnMainResult1Change(const ValueType& vOldValue, const ValueType& vNewValue)
{

}


//////////////////////////////////////////////////////////////////////////

//bool QuestCtrl::AcceptNewQuest(int32 nQuestID)
//{
//
//	
//
//	return true;
//}


//void QuestCtrl::RefreshList();
//{
//	// 检查已接任务 
//
//
//
//	// 检查可是否有可接任务 
//
//
//
//}




