#include "MapWall.h"

MapWall::MapWall()
{
	m_ObjType = MapObjectTypes::CELL_WALL;
}

MapWall::~MapWall()
{
}

MapWall::MapWall(std::vector<std::string> RedTextureIDs, std::vector<TextureProperties*> Properties,
	MapCoordinate Position, Cell CellType) : 
	MapCell(RedTextureIDs, Properties, Position, CellType)
{
	MapWall();
}

MapWall::MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell CellType) : 
	MapCell(RedTextureIDs, Position, CellType)
{
	MapWall();
}

