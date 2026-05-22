#include "stdafx.h"

#include "game_world/aoi_grid.h"

CAoiObject::CAoiObject()
	: objectId(0),
	  x(0),
	  y(0),
	  cellX(0),
	  cellY(0)
{
}

CAoiGrid::CAoiGrid()
	: m_CellSize(1000)
{
}

BOOL CAoiGrid::Init(int32_t cellSize)
{
	m_CellSize = cellSize > 0 ? cellSize : 1000;
	m_Objects.clear();
	m_Cells.clear();
	return TRUE;
}

CAoiGrid::CELL_KEY CAoiGrid::CalcCell(int32_t x, int32_t y) const
{
	return CELL_KEY(x / m_CellSize, y / m_CellSize);
}

void CAoiGrid::AddToCell(uint64_t objectId, const CELL_KEY& cell)
{
	m_Cells[cell].insert(objectId);
}

void CAoiGrid::RemoveFromCell(uint64_t objectId, const CELL_KEY& cell)
{
	std::map<CELL_KEY, std::set<uint64_t> >::iterator it = m_Cells.find(cell);
	if (it == m_Cells.end())
	{
		return;
	}
	it->second.erase(objectId);
	if (it->second.empty())
	{
		m_Cells.erase(it);
	}
}

BOOL CAoiGrid::Enter(uint64_t objectId, int32_t x, int32_t y)
{
	if (objectId == 0)
	{
		return FALSE;
	}

	const CELL_KEY cell = CalcCell(x, y);
	CAoiObject& object = m_Objects[objectId];
	if (object.objectId != 0)
	{
		RemoveFromCell(objectId, CELL_KEY(object.cellX, object.cellY));
	}

	object.objectId = objectId;
	object.x = x;
	object.y = y;
	object.cellX = cell.first;
	object.cellY = cell.second;
	AddToCell(objectId, cell);
	return TRUE;
}

BOOL CAoiGrid::Move(uint64_t objectId, const MovementFragment& movement)
{
	std::map<uint64_t, CAoiObject>::iterator it = m_Objects.find(objectId);
	if (it == m_Objects.end())
	{
		return Enter(objectId, movement.nextX, movement.nextY);
	}

	CAoiObject& object = it->second;
	const CELL_KEY oldCell(object.cellX, object.cellY);
	const CELL_KEY newCell = CalcCell(movement.nextX, movement.nextY);
	object.x = movement.nextX;
	object.y = movement.nextY;
	if (oldCell != newCell)
	{
		RemoveFromCell(objectId, oldCell);
		AddToCell(objectId, newCell);
		object.cellX = newCell.first;
		object.cellY = newCell.second;
	}
	return TRUE;
}

BOOL CAoiGrid::Leave(uint64_t objectId)
{
	std::map<uint64_t, CAoiObject>::iterator it = m_Objects.find(objectId);
	if (it == m_Objects.end())
	{
		return TRUE;
	}

	RemoveFromCell(objectId, CELL_KEY(it->second.cellX, it->second.cellY));
	m_Objects.erase(it);
	return TRUE;
}

BOOL CAoiGrid::GetVisible(uint64_t objectId, std::vector<uint64_t>& outObjects) const
{
	outObjects.clear();
	std::map<uint64_t, CAoiObject>::const_iterator objectIt = m_Objects.find(objectId);
	if (objectIt == m_Objects.end())
	{
		return FALSE;
	}

	for (int32_t dx = -1; dx <= 1; ++dx)
	{
		for (int32_t dy = -1; dy <= 1; ++dy)
		{
			const CELL_KEY cell(objectIt->second.cellX + dx, objectIt->second.cellY + dy);
			std::map<CELL_KEY, std::set<uint64_t> >::const_iterator cellIt = m_Cells.find(cell);
			if (cellIt == m_Cells.end())
			{
				continue;
			}
			for (std::set<uint64_t>::const_iterator idIt = cellIt->second.begin(); idIt != cellIt->second.end(); ++idIt)
			{
				if (*idIt != objectId)
				{
					outObjects.push_back(*idIt);
				}
			}
		}
	}
	return TRUE;
}
