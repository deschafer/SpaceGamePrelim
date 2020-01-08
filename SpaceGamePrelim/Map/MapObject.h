#pragma once

#include "MapCoordinate.h"
#include "..\Objects\Colorable.h"
#include "..\Objects\Locatable.h"
#include "..\Objects\Drawable.h"
#include "..\BasicTypes\BasicTypes.h"

// Map Object types -- not sure if it will be staying
enum class MapObjectTypes{CELL_WALL, CELL_INACTIVE, N_A};

// Base class for all map objects such as MapCells
class MapObject : public Drawable
{
protected:

	MapObjectTypes m_ObjType;
	MapCoordinate m_Position;

public:

	MapObject(Rect Dimensions, Scene* ParentScene);
	MapObject() = delete;

	virtual	~MapObject();
};

