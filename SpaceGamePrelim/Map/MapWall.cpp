#include "MapWall.h"



MapWall::MapWall()
{
}

MapWall::~MapWall()
{
}

MapWall::MapWall(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position) : 
	MapCell(TextureIDs, Properties, Position)
{
}
MapWall::MapWall(TextureProperties* Properties, MapCoordinate Position) :
	MapCell(Properties, Position)
{
}
MapWall::MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position) : 
	MapCell(RedTextureIDs, Position)
{
}

void MapWall::Draw(MapCoordinate Coords) 
{
	MapCell::Draw(Coords);
}
