#include "CollisionManager.h"
#include "..\Map\MapManager.h"

CollisionManager* CollisionManager::m_Instance = nullptr;

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

//
// CheckPosition()
// Checks the current position for any collisions, and if any are present,
// it returns a list of objects representing the collisions
//
std::vector<Collision*> CollisionManager::CheckPosition(Vector PositionAfter, Vector PositionBefore, GameEntity* Entity)
{
	std::vector<Collision*> Collisions;

	// Get collisions from the map
	Collisions = MapManager::Instance()->CheckCollisions(PositionAfter, PositionBefore, Entity);

	return Collisions;
}