#include "stdafx.h"
#include "game_world/game_world.h"

#include "define/id_def.h"

#include "base_lib/process_error.h"

CGameWorld CGameWorld::ms_Instance;

BOOL CGameWorld::init(void)
{
	int32_t nRetCode = 0;


	m_pGWData = NULL;


	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGameWorld::uninit(void)
{
	m_pGWData = NULL;

	return TRUE;
Exit0:
	return FALSE;
}


uint64_t CGameWorld::generate_obj_id(int32_t eObjType)
{
	OBJECT_ID id;
	uint32_t dwCurrentTime = get_current_time();

	LOG_PROCESS_ERROR(eObjType >= otRole && eObjType < otTotal);

	if (dwCurrentTime > m_pGWData->m_dwGenIDLastTime[eObjType])
	{
		m_pGWData->m_dwIDCounter[eObjType] = 0;
		m_pGWData->m_dwGenIDLastTime[eObjType] = dwCurrentTime;
	}
	else
	{
		m_pGWData->m_dwIDCounter[eObjType]++;
		if (m_pGWData->m_dwIDCounter[eObjType] >= (1 << OBJECT_ID_INDEX_LEN) - 1)
		{
			//CRI("object id %d exceeds limit" ,eObjType);
			m_pGWData->m_dwIDCounter[eObjType] = 0;
			m_pGWData->m_dwGenIDLastTime[eObjType]++;
			LOG_PROCESS_ERROR(m_pGWData->m_dwGenIDLastTime[eObjType] - dwCurrentTime < SAFE_ADVANCED_TIME);
		}
	}

	id.uType = eObjType;
	id.uArea = m_nAreaID;
	id.uGSIndex = m_nGSIndex;
	id.uIndex = m_pGWData->m_dwIDCounter[eObjType];
	id.uTime = m_pGWData->m_dwGenIDLastTime[eObjType];

	return id.qwID;
Exit0:
	return 0;

}








