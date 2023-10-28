#ifndef _constant_h__
#define _constant_h__

#ifndef ARR_LEN
#define ARR_LEN(a) (sizeof(a) / sizeof(*(a)))
#endif //ARR_LEN

#ifndef PI
#define PI (3.1415926536f)
#endif //PI

enum DIRECTION_TYPE
{
	dirInvalid = -1,
	
	dirEast,
	dirSouthEast,
	dirSouth,
	dirSouthWest,
	dirWest,
	dirNorthWest,
	dirNorth,
	dirNorthEast,

	dirTotal,

};

#endif //_constant_h__