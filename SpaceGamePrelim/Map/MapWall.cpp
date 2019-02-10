#include "MapWall.h"



MapWall::MapWall()
{
	m_ObjType = MapObjectTypes::CELL_WALL;
}

MapWall::~MapWall()
{
}

MapWall::MapWall(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position) : 
	MapCell(TextureIDs, Properties, Position)
{
	MapWall();
}
MapWall::MapWall(TextureProperties* Properties, MapCoordinate Position) :
	MapCell(Properties, Position)
{
	MapWall();
}
MapWall::MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position) : 
	MapCell(RedTextureIDs, Position)
{
	MapWall();
}

void MapWall::Draw(MapCoordinate Coords) 
{
	MapCell::Draw(Coords);
}
