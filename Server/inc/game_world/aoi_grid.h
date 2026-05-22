#ifndef ANGEL_AOI_GRID_H_
#define ANGEL_AOI_GRID_H_

#include <map>
#include <set>
#include <vector>

#include "base_lib/def/type_def.h"
#include "game_world/movement_def.h"

struct CAoiObject
{
	CAoiObject();

	uint64_t objectId;
	int32_t x;
	int32_t y;
	int32_t cellX;
	int32_t cellY;
};

class CAoiGrid
{
public:
	CAoiGrid();

	BOOL Init(int32_t cellSize);
	BOOL Enter(uint64_t objectId, int32_t x, int32_t y);
	BOOL Move(uint64_t objectId, const MovementFragment& movement);
	BOOL Leave(uint64_t objectId);
	BOOL GetVisible(uint64_t objectId, std::vector<uint64_t>& outObjects) const;
	size_t GetObjectCount() const { return m_Objects.size(); }

private:
	typedef std::pair<int32_t, int32_t> CELL_KEY;

	CELL_KEY CalcCell(int32_t x, int32_t y) const;
	void AddToCell(uint64_t objectId, const CELL_KEY& cell);
	void RemoveFromCell(uint64_t objectId, const CELL_KEY& cell);

private:
	int32_t m_CellSize;
	std::map<uint64_t, CAoiObject> m_Objects;
	std::map<CELL_KEY, std::set<uint64_t> > m_Cells;
};

#endif // ANGEL_AOI_GRID_H_
