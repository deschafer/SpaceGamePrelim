#include "MapInactive.h"


MapInactive::MapInactive()
{
	m_ObjType = MapObjectTypes::CELL_INACTIVE;
}

MapInactive::~MapInactive()
{
}

MapInactive::MapInactive(std::vector<std::string> RedTextureIDs, std::vector<TextureProperties*> Properties,
	MapCoordinate Position, Cell CellType) :
	MapCell(RedTextureIDs, Properties, Position, CellType)
{
	MapInactive();
}

MapInactive::MapInactive(std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell CellType) :
	MapCell(RedTextureIDs, Position, CellType)
{
	MapInactive();
}

void MapInactive::Draw(MapCoordinate Coords)
{
	MapCell::Draw(Coords);
}