#ifndef _MOVEMENT_DEF_H__
#define _MOVEMENT_DEF_H__

#include "def/type_def.h"

// A compact movement segment for MMORPG state sync prediction/interpolation.
struct MovementFragment
{
	int16_t x;
	int16_t y;
	int16_t nextX;
	int16_t nextY;
	int16_t duration;
};

#endif // _MOVEMENT_DEF_H__
