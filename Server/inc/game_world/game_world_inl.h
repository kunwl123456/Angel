#include "game_world.h"
#ifndef _GAME_WORLD_INL_H_
#define _GAME_WORLD_INL_H_

inline CGameWorld& CGameWorld::instance(void)
{
	return ms_Instance;
}

inline uint64_t CGameWorld::get_current_time(void)
{
	return m_pGWData->m_qwCurrentTime;
}








#endif //_GAME_WORLD_INL_H_






















