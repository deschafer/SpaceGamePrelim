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
std::vector<Collision*> CollisionManager::CheckPosition(Vector PositionAfter, Vector PositionBefore)
{
	std::vector<Collision*> Collisions;

	// Get collisions from the map
	Collisions = MapManager::Instance()->CheckCollisions(PositionAfter, PositionBefore);

	return Collisions;
}

//
// CheckMapForCollisions()
//
//
void CollisionManager::CheckMapForCollisions(std::vector<Collision*> &Collisions, Vector PositionAfter, Vector PositionBefore)
{
	/*
	Cell CellType = MapManager::Instance()->GetCellType(Position);

	if (CellType != Cell::Floor)
	{
		Collisions.push_back(new Collision(CollisionType::MapWall));
	}
	*/

	// Get collisions from the map
	std::vector<Collision*> CurrCollisions = MapManager::Instance()->CheckCollisions(PositionAfter, PositionBefore);


	/*
	switch (CellType)
	{
	case Cell::Floor:
		std::cout << "Cell: Floor" << std::endl;
		break;
	case Cell::Wall_Top:
		std::cout << "Cell: Wall_Top" << std::endl;

		break;
	case Cell::Wall_Bottom:
		std::cout << "Cell: Wall_Bottom" << std::endl;

		break;
	case Cell::Wall_Left:
		std::cout << "Cell: Wall_Left" << std::endl;

		break;
	case Cell::Wall_Right:
		std::cout << "Cell: Wall_Right" << std::endl;

		break;
	case Cell::Wall_Corner_Left:
		std::cout << "Cell: Wall_Corner_Left" << std::endl;

		break;
	case Cell::Wall_Corner_Right:
		std::cout << "Cell: vWall_Corner_Right" << std::endl;

		break;
	case Cell::Wall_Corner_Left_Bottom:
		std::cout << "Cell: Wall_Corner_Left_Bottom" << std::endl;

		break;
	case Cell::Wall_Corner_Right_Bottom:
		std::cout << "Cell: Wall_Corner_Right_Bottom" << std::endl;

		break;
	case Cell::Default:
		std::cout << "Cell: Default" << std::endl;

		break;
	case Cell::Empty:
		std::cout << "Cell: Empty" << std::endl;

		break;
	default:
		break;
	}
	*/

}