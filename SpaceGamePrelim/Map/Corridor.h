#pragma once

#include "MapWall.h"
#include "MapInactive.h"
#include "MapRoom.h"

#include <vector>

typedef std::vector<std::vector<MapCell*>> Array;

class Corridor
{
protected:

	enum class Movement { LEFT, RIGHT, UP, DOWN };
	int m_Width;
	int m_Height;
	std::string MapType;
	MapCell*** m_Cells;
	MapRoom* m_RoomAboveOrRight;
	MapRoom* m_RoomBelowOrLeft;

	virtual void AddCell(MapCoordinate CellPosition, MapCell* Cell);

public:
	Corridor();
	Corridor(int Width, int Height);
	virtual ~Corridor();

	MapCell*** GetCells() { return m_Cells; }
	MapRoom* GetRoomAboveOrRight() { return m_RoomAboveOrRight; }
	MapRoom* RoomBelowOrLeft() { return m_RoomBelowOrLeft; }
	virtual Corridor* GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix, MapCoordinate Beg, MapCoordinate End, int DistanceBetween) = 0;
};

