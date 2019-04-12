#include "CollisionComp.h"
#include "../Collision/CollisionManager.h"
#include "../BasicTypes/EntityDirection.h"
#include "../Collision/MapCollision.h"

#include <iostream>

CollisionComp::CollisionComp()
{
}

CollisionComp::~CollisionComp()
{
}

CollisionComp::CollisionComp(GameEntity* Owner) :
	Component(Owner)
{
}

//
//
//
//
void CollisionComp::Execute()
{
	// Check if we have actually moved,
	// if not then do not check for collisions
	bool CollisionPresent = false;
	MapCollision* CurrMapCollision;

	Vector CurrentPosition = m_Owner->GetPosition();
	Vector SetVelocity = m_Owner->GetVelocity();
	Vector PositionAfterMovement = Vector(
		CurrentPosition.getX() + SetVelocity.getX(),
		CurrentPosition.getY() + SetVelocity.getY());

	// Get the collisions
	std::vector<Collision*> Collisions = CollisionManager::Instance()->CheckPosition(
		CurrentPosition, PositionAfterMovement, m_Owner);

	for(size_t i = 0; i < Collisions.size(); i++)
	{ 
		if (Collisions[i]->GetType() == CollisionType::MapWall)
		{
			HandleMapCollisions(
				static_cast<MapCollision*>(Collisions[i]),
				SetVelocity);
			CollisionPresent = true;
		}	
	}

	if (CollisionPresent)
	{
		std::cout << "Vel: " << SetVelocity.getX() << " " << SetVelocity.getY() << std::endl;

		m_Owner->SetVelocity(SetVelocity);
	}
}

//
// HandleMapCollisions()
//
//
void CollisionComp::HandleMapCollisions(MapCollision* NewCollision, Vector &SetVelocity) 
{
	bool CollisionAngleHoriz = false;
	bool CollisionAngleVerti = false;

	EntityDirection Horiz = m_Owner->GetDirectionHoriz();
	EntityDirection Verti = m_Owner->GetDirectionVerti();

	if (NewCollision->GetDir() == MapCollisionDir::Horiz)
	{
		CollisionAngleHoriz = true;
		std::cout << "Horiz collision" << std::endl;
	}
	else if (NewCollision->GetDir() == MapCollisionDir::Verti)
	{
		CollisionAngleVerti = true;
		std::cout << "Verti collision" << std::endl;
	}
	else if (NewCollision->GetDir() == MapCollisionDir::Diagonal)
	{
		CollisionAngleHoriz = true;
		CollisionAngleVerti = true;
		std::cout << "Diag collision" << std::endl;
	}

	if (CollisionAngleHoriz)
	{
		if (Horiz == EntityDirection::East && (SetVelocity.getX() > 0))
		{
			// Then prevent eastward movement
			SetVelocity.setX(0);
		}
		else if (Horiz == EntityDirection::West && (SetVelocity.getX() < 0))
		{
			// Then prevent westward movement
			SetVelocity.setX(0);
		}
	}
	if (CollisionAngleVerti)
	{
		if (Verti == EntityDirection::South && (SetVelocity.getY() > 0))
		{
			// Then prevent eastward movement
			SetVelocity.setY(0);
		}
		else if (Verti == EntityDirection::North && (SetVelocity.getY() < 0))
		{
			// Then prevent westward movement
			SetVelocity.setY(0);
		}
	}
}


