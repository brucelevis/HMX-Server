
#ifndef PROTOCOL_DEFIEN_H_
#define PROTOCOL_DEFIEN_H_

#include "BaseDefine.h"
#include <memory>


/*
 * 通用值类型 	
 */
union ValueType
{
	ValueType()
	{
		memset(this, 0, sizeof(*this));
	}

	template<typename T>
	ValueType(T v)
	{
		if (!(sizeof(*this) < sizeof(T)))
		{
			memcpy(this, &v, sizeof(T));
		}
	}

	int8	nInt8;
	int16	nInt16;
	int32	nInt32;
	int64	nInt64;
	uint8	nUint8;
	uint16	nUint16;
	uint32	nUint32;
	uint64	nUint64;
	float32	fFloat;
};


typedef boost::function<void(const ValueType& vOldValue, const ValueType& vNewValue)> AttributeHandler;

// 属性偏移结构 
struct AttributeOffset
{
	void*				nOffset;				// 属性位置 
	int32				nSize;					// 属性大小 
	bool				bCanSet;				// 是否可设置 
	AttributeHandler	hHandler;				// 属性修改回调
};



typedef vector< string > StringVector;
typedef vector< string >::iterator StringIter;

typedef vector< int32 > IntVector;
typedef vector< int32 >::iterator IntIter;

#define EncryptDefualt "1234567890ABCDEFGHIJKLMNOPQSRTWX"
#define RUN_PRINT_TIMES 2000

#define CFG_DIR		"../../Config/"
#define RES_DIR		"E:/NEW_SERVER/Resource/"
#define ZONE_DIR	"E:/NEW_SERVER/Resource/Zone/"
#define SCENE_DIR	"E:/NEW_SERVER/Resource/Scene/"


#define SERVER_TIMEOUT				300 // 5分钟内未收到连接的任何数据，则会断开连接  
#define PING_TO_S_INTERVAL			300 // 60秒进行ping一次	
#define MAX_SCENE_NUM				60	// 最大场景数量(单个服) 

#define MAX_SERVER_NUM				100  // 最大服务数量(所有连接ws的server数量，只能记录在运行状态) 

#define INTEVAL_COMMON_TIME			200  // 共用定时回调ms 
#define INTEVAL_NETIO_TIME			100  // IO事件，业务定时回调ms 
#define INTEVAL_SCENEMAP_TIME       100  // 地图刷新ms 


/* 常用常量定义 */
enum 
{
	MAX_ACCOUNT_LENG				= 32,	// 平台账号最大长度 
	MAX_PASSWORD_LENG				= 32,   // 平台密码最大长度 
	MAX_NICK_LENGTH					= 32,	// 昵称最大长度 
	MAX_ROLE_TYPE_COUNT				= 4,	// 一个平台帐号最多允许创建角色数量 
	MAX_CHAT_MSG_LENGTH				= 128,	// 聊天内容最大长度 
	MAX_ENCRYPT_LENTH				= 32,	// 密钥长度 
	MAX_SERVERINFO_LIST				= 32,   // 最大服务器数量(除ws外) 
	MAX_CHAR_WORLD_MSG_LENGTH		= 64,	// 聊天最多可以说64个字符 
	MAX_SKILL_COUNT					= 5,	// 技能最大数量
	MAX_QUEST_LIST_COUNT			= 10,	// 最大接受任务数量 
	MAX_BINARY_SIZE					= 4096,	// 每个数据表最大的二进制大小(不可修改)
};


// 客户端当前状态 
enum ClientStatus
{
	CS_INVALID  = 0,
	CS_LOGIN_ING,			//	登录中 
	CS_LOGIN_OK	,			//	登录完成 
	CS_REGISTER_ING,		//  注册中 
	CS_REGISTER_OK,			//	注册成功 
	CS_LOADDATA_ING,		//	初始化数据中 
	CS_LOADDATA_OK,			//	初始化数据完成 
	CS_IN_GAME	,			//	进入游戏(选择或注册角色后) 
	CS_RES_OK	,			//	前端资源加载完成 
};


// 服务器类型定义 
enum EServerType
{
	ESERVER_TYPE_NULL	= 0,
	ESERVER_TYPE_FEP	= 1,
	ESERVER_TYPE_LS		= 2,
	ESERVER_TYPE_WS		= 3,
	ESERVER_TYPE_SS		= 4,
	ESERVER_TYPE_DP		= 5,
	ESERVER_TYPE_LOG	= 6,
	ESERVER_TYPE_MAX,
};

// session 类型 
enum ESessionType
{
	ESESSION_TYPE_NULL	= 0,
	ESESSION_TYPE_CLINET ,
	ESESSION_TYPE_SERVER,
};

// 数据库表 
enum ETableType
{
	E_TABLE_NULL = 0 ,
	E_TABLE_USER,
	E_TABLE_QUEST,
};

#pragma pack(push,1)

// 请求登录到ws的通用数据结构 
struct LoginData2Ws
{
	LoginData2Ws()
	{
	}
	char arrServerAddr[32]; // 服务器地址 
	int32 nServerPort;		// 端口 
	int32 nServerID;		// 服务器标识ID 
};

// 服务器信息 
struct ServerInfoData
{
	ServerInfoData()
	{
		memset(this,0,sizeof(*this));
	}
	char arrHost[32];
	int32 nPort;
	int32 nServerType;
	int32 nServerID;
};

typedef std::vector<ServerInfoData> ServerInfoDataVecType;


// 协议号范围管理 

enum 
{
	// 内部协议server与server之前的通用通信协议 
	PRO_INTERNAL_BASE = 100,

	// 固定值(1)+类型form+类型to+两位自定义 
	PRO_C2F_BASE = 10100,
	PRO_C2L_BASE = 10200,
	PRO_C2W_BASE = 10300,
	PRO_C2S_BASE = 10400,
	
	PRO_F2C_BASE = 11000,
	PRO_F2L_BASE = 11200,
	PRO_F2W_BASE = 11300,
	PRO_F2S_BASE = 11400,

	PRO_L2C_BASE = 12000,
	PRO_L2F_BASE = 12100,
	PRO_L2W_BASE = 12300,
	PRO_L2D_BASE = 12500,

	PRO_W2C_BASE = 13000,
	PRO_W2F_BASE = 13100,
	PRO_W2L_BASE = 13200,
	PRO_W2S_BASE = 13400,
	PRO_W2D_BASE = 13500,

	PRO_S2C_BASE = 14000,
	PRO_S2L_BASE = 14100,
	PRO_S2W_BASE = 14300,
	PRO_S2D_BASE = 14500,

	PRO_D2L_BASE = 15200,
	PRO_D2W_BASE = 15300,
	PRO_D2S_BASE = 15400,

};

// 角色类型 
enum ERoleType
{
	E_ROLE_TYPE_NULL,
	E_ROLE_TYPE_1,
	E_ROLE_TYPE_2,
	E_ROLE_TYPE_3,
	E_ROLE_TYPE_4,
	E_ROLE_TYPE_MAX,
};


// 进入场景条件参数
struct stEnterSceneParam
{
	int32 nSubIdx; // 动态地图唯一ID 
	int32 nPram1;
	int32 nPram2;
	stEnterSceneParam()
	{
		memset(this, 0, sizeof(*this));
	}
};



#pragma pack(pop)



// 定时常用定义 
#define __INTERAVAL_ONE_SECOND__	(nSrvTime % 5 == 0)
#define __INTERAVAL_TWO_SECOND__	(nSrvTime % 10 == 0)
#define __INTERAVAL_THREE_SECOND__  (nSrvTime % 15 == 0)
#define __INTERAVAL_FOUR_SECOND__	(nSrvTime % 20 == 0)
#define __INTERAVAL_FIVE_SECOND__	(nSrvTime % 25 == 0)
#define __INTERAVAL_ONE_MINUTE__	(nSrvTime % 300 == 0)

// 异步事件唯一编号 
enum SocketEventCode
{
	SOCKET_EVENT_CODE_SAVE_CHANGE_MAP = 1,
};


#endif


