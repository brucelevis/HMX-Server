#ifndef __QUEST_CTROL_H_
#define __QUEST_CTROL_H_


#include "Base.h"
#include "CommonCharacter.h"
#include "AttributeOffsetC.h"

class SceneUser;

/*-------------------------------------------------------------------
 * @Brief : 任务控制器 
 * 
 * @Author:hzd 2015:11:26
 *------------------------------------------------------------------*/

// 数据内存地址偏移位置，内存还在在m_sData中，只是方便SET和GET 

enum QuestAttribute
{
	QUEST_ATTR_CHAR_ID = 0,
	QUEST_ATTR_MAIN_LAST_ID,
	QUEST_ATTR_MAIN_CURR_ID,
	QUEST_ATTR_MAIN_RESULT_0,
	QUEST_ATTR_MAIN_RESULT_1,
	QUEST_ATTR_MAX,

};

struct StQuestInfo
{
	int32 nQuestID;
	int32 nResult0;
	int32 nResult1;
};


class QuestCtrl : /*public BaseCtrl<StQuestDataTable> ,*/ public AttributeOffsetC<QuestAttribute,QUEST_ATTR_MAX>
{
public:
	QuestCtrl(SceneUser* pUser);
	~QuestCtrl(void);

	void InData(const StQuestDataTable& rData);
	void OutData(StQuestDataTable& rData);

	void SendMainData();

	void InitAttribute();


public:

	// 接受新任务 
	bool AcceptNewQuest(int32 nQuestID);
	
	// 已接可接 
	void RefreshList(); 

	// 交任务 
	bool SubmitQuest(int32 nQuestID);

private:

	//bool HadThisQuest(int32 nQuestID); // 是否已经接或完成 

	//StQuestDataTable::QuestInfo* GetQuestInfo(int32 nQuestID);

private:

	void OnMainLastIDChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnMainCurrIDChange(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnMainResult0Change(const ValueType& vOldValue, const ValueType& vNewValue);
	void OnMainResult1Change(const ValueType& vOldValue, const ValueType& vNewValue);

private:

	SceneUser*			m_pUser;

	//StQuestDataTable m_sData;

	std::vector<StQuestInfo> m_vecHadAccept;
	std::vector<StQuestInfo> m_vecCanAccept;

};


#endif

