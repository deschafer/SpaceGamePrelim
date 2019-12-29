#pragma once

#include "MapCoordinate.h"
#include "..\Objects\Colorable.h"

// Map Object types -- not sure if it will be staying
enum class MapObjectTypes{CELL_WALL, CELL_INACTIVE};

// Base class for all map objects such as MapCells
class MapObject : public Colorable
{
protected:

	MapObjectTypes m_ObjType;
	MapCoordinate m_Position;

public:

	virtual void Draw(MapCoordinate Coords) = 0;
	virtual void Update() = 0;

	MapObject();
	virtual	~MapObject();
};

