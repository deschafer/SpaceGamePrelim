#pragma once

#include "MapCoordinate.h"

// Map Object types -- not sure if it will be staying
enum class MapObjectTypes{CELL_WALL, CELL_INACTIVE};

// Base class for all map objects such as MapCells
class MapObject
{
protected:

	MapObjectTypes m_ObjType;
	MapCoordinate m_Position;

public:

	virtual void Draw(MapCoordinate Coords) = 0;

	MapObject();
	virtual	~MapObject();
};

