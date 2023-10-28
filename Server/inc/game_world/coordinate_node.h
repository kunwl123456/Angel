#ifndef __COORDINATE_NODE_H__
#define __COORDINATE_NODE_H__

#include "def/type_def.h"

#ifndef AG_COORD_HAS_Y
#define AG_COORD_HAS_Y (0)
#endif

#define COORD_PRECISION (10000.0)
#define COORD_CONV(u) ((int64_t)((u)*(COORD_PRECISION)))
/*
struct AGRadius
{
	AGRadius(float frad)
		:rad(COORD_CONV(frad)){}
	AGRadius(){}
	AGRadius(uint64_t v)
		:rad(v){}
	AGRadius(int v)
		:rad(v){}

	inline int init() { rad = 0; return 0; }
	inline int fini() { return 0; }
	uint64_t rad;
};

struct AGCoordPos
{
	AGCoordPos(const AGVec3& pos) :x(COORD_CONV(pos.x), y(COORD_CONV(pos.y),z(COORD_CONV(pos.z)) {}
	AGCoordPos(){}
	AGCoordPos(int64_t v)
		:x(v),y(v),z(v){}
	AGCoordPos(int64_t x,int64_t y,int64_t z)
		:x(x),y(y),z(z){}

	inline int init() { x = 0; y = 0; z = 0; return 0; }
	inline int uninit() { return 0; }
	inline int is_valid()
	{
		return (x > INT_MIN) && (x < INT_MAX)
			&& (y > INT_MIN) && (y < INT_MAX)
			&& (z > INT_MIN) && (z < INT_MAX);
	}

	AGCoordPos operator+(AGRadius& rad) const
	{
		return AGCoordPos(x + rad.rad, y + rad.rad, z + rad.rad);
	}

	AGCoordPos operator-(AGRadius& rad) const
	{
		return AGCoordPos(x - rad.rad, y - rad.rad, z - rad.rad);
	}

	AGCoordPos operator-(AGCoordPos& pos) const
	{
		return AGCoordPos(x - pos.x, y - pos.y, z - pos.z);
	}

	AGCoordPos operator+(AGCoordPos& pos) const
	{
		return AGCoordPos(x + pos.x, y + pos.y, z + pos.z);
	}

	bool operator==(AGCoordPos& pos) const
	{
		return x == pos.x && y==pos.y && pos.z ==z ;
	}

	volatile int64_t x;
	volatile int64_t y;
	volatile int64_t z;
};

enum AG_COORD_NODE_TYPE
{
	agcNodeMin,
	
	agcNodeLGuard,  //节点左边视野极限
	agcNodeRGuard,  //节点右边视野极限
	agcNodeAgent,   //
	
	agcNodeMax
};

class AGTerrainAgent;
class AGCoordNode
{
public:
	inline bool is_lguard() {return ntype == agcNodeLGuard;}
	inline bool is_rguard() { return ntype == agcNodeRGuard; }
	inline bool is_agent() { return ntype == agcNodeAgent; }
	inline bool is_guard() { return ntype == agcNodeLGuard || ntype == agcNodeRGuard; }

	inline bool is_valid();
	inline bool is_equal(AGCoordNode* p);
	
	bool was_in_xrange(AGCoordNode* p);
	bool was_in_yrange(AGCoordNode* p);
	bool was_in_zrange(AGCoordNode* p);

	bool is_in_xrange(AGCoordNode* p);
	bool is_in_yrange(AGCoordNode* p);
	bool is_in_zrange(AGCoordNode* p);

	void on_passx(AGCoordNode* node);
	void on_passy(AGCoordNode* node);
	void on_passz(AGCoordNode* node);
	void on_leave(AGCoordNode* node);
	void on_enter(AGCoordNode* node);

	int init();
	int fini();
	void debug();

public:
	AGCoordNode* xPrev;
	AGCoordNode* xNext;

#if AG_COORD_HAS_Y
	AGCoordNode* yPrev;
	AGCoordNode* yNext;
#endif
	AGCoordNode* zPrev;
	AGCoordNode* zNext;

	AGCoordNode* lGuard;
	AGCoordNode* rGuard;
	//AGTerrainAgent* agent;
	AGCoordNode* oWner;

	//半径
	AGRadius old_rad;
	AGRadius cur_rad;

	//
	AGCoordPos old_pos;
	AGCoordPos cur_pos;
	int ntype;
	uint64_t nUid;
	bool install;
	bool is_updated;

};

AGCoordNode* coordinate_node_alloc();
bool coordinate_node_free(AGCoordNode* node);

*/
#endif //__COORDINATE_NODE_H__