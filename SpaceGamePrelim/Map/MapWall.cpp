#include "MapWall.h"

MapWall::MapWall(Rect Dimensions, Scene* Parent) : 
	MapCell(Dimensions, Parent)
{
	m_ObjType = MapObjectTypes::CELL_WALL;
	m_Collidable = false;
}

MapWall::~MapWall()
{
}

MapWall::MapWall(Rect Dimensions, Scene* Parent, std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell CellType) :
	MapCell(Dimensions, Parent, RedTextureIDs, Position, CellType)
{
	m_ObjType = MapObjectTypes::CELL_WALL;
	m_Collidable = false;
}

MapWall::MapWall(Rect Dimensions, Scene* Parent, std::vector<std::string> RedTextureIDs, MapCoordinate Position, Rect DestRect, Cell CellType) :
	MapCell(Dimensions, Parent, RedTextureIDs, Position, DestRect, CellType)
{
	m_ObjType = MapObjectTypes::CELL_WALL;
	m_Collidable = false;
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

