#include "MapWall.h"



MapWall::MapWall()
{
	m_ObjType = MapObjectTypes::CELL_WALL;
}

MapWall::~MapWall()
{
}

MapWall::MapWall(std::vector<std::string> TextureIDs, TextureProperties* Properties, 
	MapCoordinate Position, Cell MapCell) : 
	MapCell(TextureIDs, Properties, Position, MapCell)
{
	MapWall();
}
MapWall::MapWall(TextureProperties* Properties, MapCoordinate Position, Cell CellType) :
	MapCell(Properties, Position, CellType)
{
	MapWall();
}
MapWall::MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell CellType) : 
	MapCell(RedTextureIDs, Position, CellType)
{
	MapWall();
}

void MapWall::Draw(MapCoordinate Coords) 
{
	MapCell::Draw(Coords);
}
