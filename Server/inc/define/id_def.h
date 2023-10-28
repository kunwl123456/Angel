#pragma once

#include "define/server_def.h"
#include "def/type_def.h"

#define SAFE_ADVANCED_TIME (8)

#define OBJECT_ID_TIME_LEN     (31)
#define OBJECT_ID_GSINDEX_LEN  (8)
#define OBJECT_ID_INDEX_LEN    (12)
#define OBJECT_ID_AREA_LEN     (6)
#define OBJECT_ID_TYPE_LEN     (7)
const uint32_t global_objectid_type_pos = OBJECT_ID_TIME_LEN + OBJECT_ID_GSINDEX_LEN + OBJECT_ID_INDEX_LEN + OBJECT_ID_AREA_LEN;

#pragma pack(push,1)
union OBJECT_ID
{
	struct
	{
		uint64_t uTime : OBJECT_ID_TIME_LEN;
		uint64_t uGSIndex : OBJECT_ID_GSINDEX_LEN;
		uint64_t uIndex : OBJECT_ID_INDEX_LEN;
		uint64_t uArea : OBJECT_ID_AREA_LEN;
		uint64_t uType : OBJECT_ID_TYPE_LEN;//use 6
	};
	uint64_t qwID;
};
#pragma pack(pop)


enum OBJECT_TYPE
{
	otInvalid = 0,
	otRole,
	otNpc,
	otDoodad,
	otMissile,
	otBuild,
	otSceneItem,
	otVolume,
	otTotal
};

enum OBJ_TYPE_MASK
{
	otmRole = 1 << otRole,
	otmNpc = 1 << otNpc
};

inline int32_t GET_OBJECT_TYPE(const uint64_t& qwObjID)
{
	return ((OBJECT_ID*)&qwObjID)->uType;
}



#pragma once












