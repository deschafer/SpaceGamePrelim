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

MapWall::MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position, Rect DestRect, Cell CellType) :
	MapCell(RedTextureIDs, Position, DestRect, CellType)
{
	MapWall();
}

//
// OnCollision()
// Returns true if there is a no-movement 
// Collision. Does what it needs to the player
// here by modifying its data.
//
bool MapWall::OnCollision(GameEntity* Enitity)
{
	// We can do other things here like damage the player, or slow their movement,
	// change textures, etc..

	return true; // A wall means no more movement
}

