#ifndef _GAME_WORLD_H__
#define _GAME_WORLD_H__

#include "define/id_def.h"

class CGameWorld
{
public:
	CGameWorld() {}
	~CGameWorld() {}
	inline static CGameWorld& instance(void);

	BOOL init(void);
	BOOL uninit(void);
	BOOL mainloop(void);
	BOOL reload(const char* pcszModule, BOOL bForce);

	inline uint64_t get_current_time(void);

	uint64_t generate_obj_id(int32_t eObjType);

private:
	BOOL _restore_game_world_data(void);
	BOOL _create_game_world_data(void);

private:
	static CGameWorld          ms_Instance;

	struct GAME_WORLD_DATA
	{
		uint64_t						m_qwGameFrame;
		uint64_t						m_qwStartTick;
		uint64_t						m_qwCurrentTick;
		time_t							m_qwStartTime;
		time_t							m_qwCurrentTime;

		uint32_t						m_dwIDCounter[otTotal];
		uint32_t						m_dwGenIDLastTime[otTotal];
	};

	GAME_WORLD_DATA*			m_pGWData;
	int32_t						m_nGSIndex;
	int32_t						m_nAreaID;


};

#include "game_world_inl.h"

#endif //_GAME_WORLD_H__

