#ifndef _SERVER_DEF_H__
#define _SERVER_DEF_H__

#define USE_DBCACHE

#include "def/type_def.h"
#include <functional>
#include <list>


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
	
	svrGW = 2,      // gateway_server: client access and C2S/S2C relay
	svrGS = 3,      // game_server: core gameplay and SS/DB coordination
	svrWS = 4,      // world_server: GameCenter/lobby/world routing
	svrSS = 5,      // scene_server: scene instance, movement, AOI
	svrPS = 6,      // platform_server: account, SDK, payment/platform integration
	svrTS = 7,      // team_server: team, matchmaking, queueing
	svrCS = 8,      // cross_server: cross-zone gameplay
	svrMCS = 9,     // master_control_server: cluster orchestration and routing
	svrFS = 10,     // friend_server: friend/social data
	svrGuild = 11,  // guild_server: guild data and guild gameplay
	svrGlobal = 12, // global_server: global singleton data and services


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
    CONFIG_TYPE_NONE                 = 0, // ????
    CONFIG_TYPE_MAIN                 = 1, // ????????
    CONFIG_TYPE_WHITELIST            = 2, // gamesvr ??????
    CONFIG_TYPE_TESTACNT             = 3, // ???????
    CONFIG_TYPE_MSGFREQLIST          = 4, // ??????????
    CONFIG_TYPE_SSMSGFREQLIST        = 5, // SS??????????
    CONFIG_TYPE_DBOPFREQLIST         = 6, // DB???????????
    CONFIG_TYPE_GAMETOOS             = 7, // gametools ????
    CONFIG_TYPE_CLIENTRESFOLDERS     = 8, // client resource folders
    CONFIG_TYPE_LOCAL_RELAY_TESTACNT = 9, // local relay test account whitelist
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