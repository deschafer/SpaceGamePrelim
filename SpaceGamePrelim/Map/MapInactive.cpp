#include "MapInactive.h"



MapInactive::MapInactive()
{
	m_ObjType = MapObjectTypes::CELL_INACTIVE;
}

MapInactive::~MapInactive()
{
}

MapInactive::MapInactive(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position) :
	MapCell(TextureIDs, Properties, Position)
{
	MapInactive();
}

MapInactive::MapInactive(TextureProperties* Properties, MapCoordinate Position) :
	MapCell(Properties, Position)
{
	MapInactive();
}

MapInactive::MapInactive(std::vector<std::string> RedTextureIDs, MapCoordinate Position) :
	MapCell(RedTextureIDs, Position)
{
	MapInactive();
}

void MapInactive::Draw(MapCoordinate Coords)
{
	MapCell::Draw(Coords);
}