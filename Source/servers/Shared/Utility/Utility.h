#ifndef __DY_UTILITY_H_
#define __DY_UTILITY_H_

#include "BaseDefine.h"

/************************************************************************/
/* ͨ��������                                                             */
/************************************************************************/
class Utility
{
public:

	/*
	*
	*	Detail: ���ϵͳʱ���
	*
	*  Created by hzd 2014-10-7
	*
	*/
	inline static uint32 GetCurrentSeconds()
	{
		using namespace boost;
		using namespace boost::gregorian;

		posix_time::ptime epoch(date(1970, Jan, 1));
		posix_time::time_duration time_from_epoch = posix_time::second_clock::universal_time() - epoch;
		return time_from_epoch.total_seconds();
	}

	/*
	*
	*	Detail: ���ϵͳʱ���(΢��)
	*
	*  Created by hzd 2014-10-7
	*
	*/
	inline static uint64 GetCurrentMircoseconds()
	{
		using namespace boost;
		posix_time::ptime epoch(gregorian::date(1970, gregorian::Jan, 1));
		posix_time::time_duration time_from_epoch = posix_time::microsec_clock::universal_time() - epoch;
		return time_from_epoch.total_microseconds();
	}

	/*
	 *	���ʱ������룩����2012��1��1�������
	 */
	inline static int32 NowTime()
	{
		using namespace boost;
		posix_time::ptime epoch(gregorian::date(2012, gregorian::Jan, 1));
		posix_time::time_duration time_from_epoch = posix_time::second_clock::universal_time() - epoch;
		return static_cast<int32>(time_from_epoch.total_seconds());
	}

	/*
	 *	���ʱ��������룩����2012��1��1�������
	 */
	inline static int64 MicroTime()
	{
		using namespace boost;
		posix_time::ptime epoch(gregorian::date(2012, gregorian::Jan, 1));
		posix_time::time_duration time_from_epoch = posix_time::microsec_clock::universal_time() - epoch;
		return static_cast<int64>(time_from_epoch.total_microseconds());
	}

};



#ifndef __TWP_UTIL_UTIL_HEADER__
#define __TWP_UTIL_UTIL_HEADER__


inline int RangedRand(int nMin, int nMax)
{
	return static_cast<int>(nMin + rand() * 1.f / (RAND_MAX + 1) * (nMax - nMin));
}

inline int GBKToUTF8(const char * lpGBKStr, char * lpUTF8Str, int nUTF8StrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if (!lpGBKStr)  // ���GBK�ַ���ΪNULL������˳�  
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_ACP, 0, (char *)lpGBKStr, -1, NULL, NULL);  // ��ȡת����Unicode���������Ҫ���ַ��ռ䳤�� 
	lpUnicodeStr = new WCHAR[nRetLen + 1];  // ΪUnicode�ַ����ռ� 
	nRetLen = ::MultiByteToWideChar(CP_ACP, 0, (char *)lpGBKStr, -1, lpUnicodeStr, nRetLen);  // ת����Unicode���� 
	if (!nRetLen)  // ת��ʧ��������˳�
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, NULL, 0, NULL, NULL);  // ��ȡת����UTF8���������Ҫ���ַ��ռ䳤�� 

	if (!lpUTF8Str)  // ���������Ϊ���򷵻�ת������Ҫ�Ŀռ��С 
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return nRetLen;
	}

	if (nUTF8StrLen < nRetLen)  // ���������������Ȳ������˳� 
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, (char *)lpUTF8Str, nUTF8StrLen, NULL, NULL);  // ת����UTF8���� 

	if (lpUnicodeStr)
		delete[]lpUnicodeStr;

	return nRetLen;
}

inline char* Utf8ToGBK(const char* strUtf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
	unsigned short * wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, (LPWSTR)wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL, NULL);
	return szGBK;
}



inline int CalcGroupCount(int count, int max_pile_count)
{
	if (max_pile_count == 0)
	{
		return 1;
	}
	int group_count = count / max_pile_count;
	if (count % max_pile_count != 0)
	{
		++group_count;
	}
	return group_count;
}

/*
 *	��Map��Valueֵ��ֵ
 */
template <typename MapType>
inline void AddValueInMap(MapType& map_obj, typename MapType::key_type key, typename MapType::mapped_type value)
{
	typename MapType::iterator iter = map_obj.find(key);
	if (iter != map_obj.end())
	{
		iter->second += value;
	}
	else
	{
		map_obj.insert(std::make_pair(key, value));
	}
}

/*
 *	���Map��Valueָ�룬���Key�����ڣ��򷵻�NULL
 */
template <typename MapType>
inline typename MapType::mapped_type* GetValPtrFromMap(MapType& map_obj, typename MapType::key_type& key)
{
	typename MapType::iterator iter = map_obj.find(key);
	return iter == map_obj.end() ? NULL : &iter->second;
}

/*
 * ��Map�в���һ��Key����������򷵻�false���ɹ��򷵻�true
 */
template <typename MapType>
inline bool InsertToMap(MapType& map_obj, typename MapType::key_type& key, typename MapType::mapped_type& value)
{
	typename MapType::iterator iter = map_obj.find(key);
	if (iter != map_obj.end())
		return false;
	map_obj.insert(std::make_pair(key, value));
	return true;
}

/*
*   ֵ����һ��Ϊָ��
*	���Map��Valueָ�룬���Key�����ڣ��򷵻�NULL
*/
template <typename MapType>
inline typename MapType::mapped_type GetValFromMap(MapType& map_obj, typename MapType::key_type key)
{
	typename MapType::iterator iter = map_obj.find(key);
	return iter == map_obj.end() ? NULL : iter->second;
}

/*
*	����һ�������е�һ��ֵ��Ϊelem���±�
*/
template <typename T, unsigned int count>
inline unsigned int FindFirstNotOf(const T(&arr)[count], const T& elem)
{
	unsigned int index = 0;
	for (unsigned int i = 0; i < count; ++i)
	{
		if (elem != arr[i])
		{
			index = i;
			break;
		}
	}
	return index;
}

/*
*	����һ�������е�һ��ֵΪelem���±�
*/
template <typename T, unsigned int count>
inline unsigned int FindFirstOf(const T(&arr)[count], const T& elem)
{
	unsigned int index = 0;
	for (unsigned int i = 0; i < count; ++i)
	{
		if (elem == arr[i])
		{
			index = i;
			break;
		}
	}
	return index;
}


#ifdef TWP_SNPRINTF
# undef	TWP_SNPRINTF
#endif // TWP_SNPRINTF
#if defined(linux) || defined(__linux) || defined(__linux__)
#  define TWP_SNPRINTF snprintf
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#  define TWP_SNPRINTF(str, size, fmt, ...) _snprintf_s(str, size, (size) - 1, fmt, ##__VA_ARGS__)
#endif

/**
* \brief ���ַ�������tokenת��Ϊ����ַ���
*
* ������ʹ�����ӳ���
*    <pre>
*    std::list<string> ls;
*    stringtok (ls," this  \t is\t\n  a test  ");
*    for(std::list<string>const_iterator i = ls.begin(); i != ls.end(); ++i)
*        std::cerr << ':' << (*i) << ":\n";
*     </pre>
*
* \param container ���������ڴ���ַ���
* \param in �����ַ���
* \param delimiters �ָ�����
* \param deep ��ȣ��ָ����ȣ�ȱʡû������
*/
template <typename Container>
inline void stringtok(Container &container, std::string const &in,
	const char * const delimiters = " \t\n",
	const int deep = 0)
{
	const std::string::size_type len = in.length();
	std::string::size_type i = 0;
	int count = 0;

	while (i < len)
	{
		i = in.find_first_not_of(delimiters, i);
		if (i == std::string::npos)
			return;   // nothing left

					  // find the end of the token
		std::string::size_type j = in.find_first_of(delimiters, i);

		count++;
		// push token
		if (j == std::string::npos
			|| (deep > 0 && count > deep)) {
			container.push_back(in.substr(i));
			return;
		}
		else
			container.push_back(in.substr(i, j - i));

		// set up for next loop
		i = j + 1;
	}
}

template<typename T1, typename T2, typename T3> struct my_streble
{
	T1 first;
	T2 second;
	T3 third;
	my_streble() :first(), second(), third(){}
	my_streble(const T1& t1, const T2& t2, const T3& t3) :first(t1), second(t2), third(t3){}
	template<typename U1,typename U2,typename U3> my_streble(const my_streble<U1, U2, U3>& _streble) : first(_streble.first), second(_streble.second), third(_streble.third){}
};
template<typename T1,typename T2,typename T3> inline my_streble<T1, T2, T3> make_my_streble(T1 x,T2 y,T3 z)
{
	return my_streble<T1, T2, T3>(x, y, z);
}

#endif






















#endif

