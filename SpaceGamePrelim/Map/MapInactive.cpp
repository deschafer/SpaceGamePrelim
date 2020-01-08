#include "MapInactive.h"

MapInactive::MapInactive(Rect Dimensions, Scene* ParentScene) : 
	MapCell(Dimensions, ParentScene)
{
	m_ObjType = MapObjectTypes::CELL_INACTIVE;
}

MapInactive::~MapInactive()
{
}

MapInactive::MapInactive(Rect Dimensions, Scene* ParentScene, std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell CellType) :
	MapCell(Dimensions, ParentScene, RedTextureIDs, Position, CellType)
{
	m_ObjType = MapObjectTypes::CELL_INACTIVE;
}

MapInactive::MapInactive(Rect Dimensions, Scene* ParentScene, std::vector<std::string> RedTextureIDs, MapCoordinate Position, Rect DestRect, Cell CellType) :
	MapCell(Dimensions, ParentScene, RedTextureIDs, Position, DestRect, CellType)
{
	m_ObjType = MapObjectTypes::CELL_INACTIVE;
}

