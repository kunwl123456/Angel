#ifndef _SERVER_DEF_H__
#define _SERVER_DEF_H__

#define USE_DBCACHE

#include "def/type_def.h"

#define MAX_GS_COUNT				(256)
#define MAX_DCS_COUNT				(256)

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


#endif //_SERVER_DEF_H__