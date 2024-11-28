#ifndef _SERVER_DEF_H__
#define _SERVER_DEF_H__

#define USE_DBCACHE

#include "def/type_def.h"
#include <functional>


#define MAX_GS_COUNT				(256)
#define MAX_DCS_COUNT				(256)
#define MAX_SVR_NAME_LEN            (64)
#define MAX_SVR_CONFIG_DIR          (256)
#define MAX_SVR_DETIAL_NAME         (256)
#define MAX_SVR_RPOC_CFG             (64)
#define SVR_PATH_MAX                 (256)

typedef std::function<bool(const char*, const char*)> CheckProcCfgFun;


enum SERVER_TYPE
{
	svrInvalid = 0,
	
	svrGW = 2,
	svrGS = 3,
	svrWS = 4,
	svrSS = 5,
	svrPS = 6,
	svrTS = 7,
	svrCS = 8,
	svrMCS = 9,
	svrFS = 10,
	svrGuild = 11,
	svrGlobal = 12,


	svrTotal
};

enum GAME_SERVER_STATE_TYPE
{
	gsstInvalid,

	gsstReady,
	gsstStopWaitForC2gMsgClear,
	gsstStopWaitForServerReply,
	gsstStopWaitForSaveRole,
	gsstStopWaitForSaveManor,
	gsstStopWaitForPoolClear,
	gsstStopComplete,
	gsstClosed,
	gsstStopWaitForRoleChangeScene,
	gsstStopWaitForMsgProcess,
	gsstStopWaitForRoleKicked,

	gsstTotal
};

enum CONFIG_TYPE
{
    CONFIG_TYPE_NONE                 = 0, // 无效
    CONFIG_TYPE_MAIN                 = 1, // 进程配置
    CONFIG_TYPE_WHITELIST            = 2, // gamesvr 白名单
    CONFIG_TYPE_TESTACNT             = 3, // 测试账号
    CONFIG_TYPE_MSGFREQLIST          = 4, // 消息频率限制
    CONFIG_TYPE_SSMSGFREQLIST        = 5, // SS消息频率限制
    CONFIG_TYPE_DBOPFREQLIST         = 6, // DB操作频率限制
    CONFIG_TYPE_GAMETOOS             = 7, // gametools 配置
    CONFIG_TYPE_CLIENTRESFOLDERS     = 8, //需要服务器加载并下发给客户端的资源目录 
    CONFIG_TYPE_LOCAL_RELAY_TESTACNT = 9, //本地服白名单配置 
    //TODO:other type
};


inline int32_t get_tbus_area(const char* pcszAddr)
{
	int32_t nAddr = 0;
	if (pcszAddr == NULL)
		return 0;
	while (*pcszAddr)
	{
		if (*pcszAddr == '.')
			break;
		if (isdigit(*pcszAddr))
			nAddr = nAddr * 10 + *pcszAddr - '0';
		pcszAddr++;
	}
	return nAddr;
}

inline BOOL get_tbus_info(const char* pcszAddr, int32_t nInfo[4])
{
	int32_t nAddr = 0;
	const char* p = pcszAddr;

	if (pcszAddr == NULL)
		return FALSE;

	sscanf(pcszAddr, "%d.%d.%d.%d",&nInfo[0],&nInfo[1],&nInfo[2],&nInfo[3]);

	return TRUE;
}

//--------------------------------

struct ProcConfNode
{
    CONFIG_TYPE     emConType;
    char            szConfFilePath[MAX_SVR_CONFIG_DIR];
    char            szMetalibName[MAX_SVR_DETIAL_NAME];
    char            szMetalibProcConf[MAX_SVR_RPOC_CFG];
    ProcConfNode()
    {
        emConType = CONFIG_TYPE_NONE;
		memset(&szMetalibProcConf, 0, sizeof(szMetalibProcConf));
		memset(&szMetalibName, 0, sizeof(szMetalibName));
		memset(&szConfFilePath, 0, sizeof(szConfFilePath));
    }
};

struct LoadCfgByConfigParam
{
	char                    szSvrName[MAX_SVR_NAME_LEN];

	int8_t                  nReloadFailProcExit;
	CheckProcCfgFun         pCheckFunc;
	std::list<ProcConfNode> confList;
	BOOL                    bPrintLog;
	int32_t                 nSleepSec;
	LoadCfgByConfigParam()
	{
		nReloadFailProcExit = 0;
		nSleepSec = 0;
		memset(&szSvrName, 0, sizeof(szSvrName));
	}

};


#endif //_SERVER_DEF_H__