#ifndef _GAME_WORLD_H__
#define _GAME_WORLD_H__

#include <vector>

#include "define/id_def.h"
#include "game_world/aoi_grid.h"

class CGameWorld
{
public:
	CGameWorld() : m_pGWData(NULL), m_nGSIndex(0), m_nAreaID(0) {}
	~CGameWorld() {}
	inline static CGameWorld& instance(void);

	BOOL init(void);
	BOOL uninit(void);
	BOOL mainloop(void);
	BOOL reload(const char* pcszModule, BOOL bForce);

	inline uint64_t get_current_time(void);

	uint64_t generate_obj_id(int32_t eObjType);
	BOOL enter_world_object(uint64_t objectId, int32_t x, int32_t y);
	BOOL update_object_movement(uint64_t objectId, const MovementFragment& movement);
	BOOL leave_world_object(uint64_t objectId);
	BOOL get_visible_objects(uint64_t objectId, std::vector<uint64_t>& outObjects) const;

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
	CAoiGrid					m_AoiGrid;


};

#include "game_world_inl.h"

#endif //_GAME_WORLD_H__

