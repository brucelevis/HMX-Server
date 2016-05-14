#ifndef __RSYNC_TOOLS_H_
#define __RSYNC_TOOLS_H_

/*-------------------------------------------------------------------
 * @Brief : ͬ�������࣬��ʱ���浽���ݿ⣬��ʱ��������Ƿ��и��£�
 *			����֪ͨ���������Ϣ
 * 
 * @Author:hzd 2015:12:22
 *------------------------------------------------------------------*/

#include "SharedIncludes.h"
#include "DataRsyncStruct.h"

class RsyncTools : public BaseSingle<RsyncTools>
{
public:
	RsyncTools(void);
	~RsyncTools(void);

	void LoadDb();

	void Update(int32 nSrvTime);

	void SaveMixItemNumber(int32 eType,int64 nValue);

	void SaveMixItemBinary(int32 eType,char* data,int32 nLen);

private:


	std::map<int32,int64> m_mapMixItemNumber;


	//std::map<int32,char*> m_mapMixItemNumber;

};


#endif

