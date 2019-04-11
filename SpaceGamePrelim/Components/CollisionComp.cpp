#include "CollisionComp.h"
#include "../Collision/CollisionManager.h"
#include "../BasicTypes/EntityDirection.h"

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

void CollisionComp::Execute()
{
	// Cheeck if we have actually moved,
	// if not then do not check for collisions
	bool CollisionPresent = false;
	bool CollisionAngleHoriz = false;
	bool CollisionAngleVerti = false;

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
			CollisionPresent = true;
		}
		if (Collisions[i]->GetDir() == CollisionDir::Horiz)
		{
			CollisionAngleHoriz = true;
			std::cout << "Horiz collision" << std::endl;
		}
		else if (Collisions[i]->GetDir() == CollisionDir::Verti)
		{
			CollisionAngleVerti = true;
			std::cout << "Verti collision" << std::endl;
		}
		else if(Collisions[i]->GetDir() == CollisionDir::Diagonal)
		{
			CollisionAngleHoriz = true;
			CollisionAngleVerti = true;
			std::cout << "Diag collision" << std::endl;
		}
	}

	// If collisions are present, 
	// do something

	// for now just stop movement for testing
	if (CollisionPresent)
	{
		EntityDirection Horiz = m_Owner->GetDirectionHoriz();
		EntityDirection Verti = m_Owner->GetDirectionVerti();
	

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
		std::cout << "Vel: " << SetVelocity.getX() << " " << SetVelocity.getY() << std::endl;


		m_Owner->SetVelocity(SetVelocity);
	}
	// Get the last direction moved, and if the velocity is in this direction, set it to zero



	// Otherwise, do nothing
}



