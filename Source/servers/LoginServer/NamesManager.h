#ifndef __NAME_MANAGER_H_
#define __NAME_MANAGER_H_

#include "SharedIncludes.h"

struct StNameInfo
{
	char  arrName[32];
	int32 nMark; // ʹ�ü�¼ 
	StNameInfo()
	{
		memset(arrName,0,32);
		nMark = 0;
	}
};

class NamesManager : public BaseSingle<NamesManager>
{
public:
	NamesManager(void);
	~NamesManager(void);

	void LoadItems(const char* arrName);

	bool GetRandTtems(char* o_arrName);

private:

	int32					m_nLastMark;
	std::vector<StNameInfo>	m_setNameInfo;
};

#endif


