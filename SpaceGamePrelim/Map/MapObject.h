#pragma once

#include "MapCoordinate.h"

enum class MapObjectTypes{CELL_WALL, CELL_INACTIVE};

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

