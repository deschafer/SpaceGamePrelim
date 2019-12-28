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
	int m_CellWidth;
	int m_CellHeight;
	std::string MapType;
	MapCell*** m_Cells;
	MapAsset*** m_Assets;
	MapRoom* m_RoomAboveOrRight;
	MapRoom* m_RoomBelowOrLeft;

	virtual void AddCell(MapCoordinate CellPosition, MapCell* Cell);
	virtual void Corridor::AddAsset(MapCoordinate CellPosition, MapAsset* Cell);

public:
	Corridor();
	Corridor(int Width, int Height, MapRoom* RoomOne, MapRoom* RoomTwo);
	virtual ~Corridor();

	MapCell*** GetCells() { return m_Cells; }
	MapAsset*** GetAssets() { return m_Assets; }
	MapRoom* GetRoomAboveOrRight() { return m_RoomAboveOrRight; }
	MapRoom* GetRoomBelowOrLeft() { return m_RoomBelowOrLeft; }
	virtual Corridor* GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix, MapCoordinate Beg, MapCoordinate End, int DistanceBetween) = 0;
};

