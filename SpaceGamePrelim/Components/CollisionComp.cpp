#include "CollisionComp.h"
#include "../Collision/CollisionManager.h"
#include "../BasicTypes/EntityDirection.h"

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


	// Get the owners position
	std::vector<Collision*> Collisions = CollisionManager::Instance()->CheckPosition(m_Owner->GetPosition());

	for(size_t i = 0; i < Collisions.size(); i++)
	{ 
		if (Collisions[i]->GetType() == CollisionType::MapWall)
		{
			CollisionPresent = true;
		}
	}

	// If collisions are present, 
	// do something

	// for now just stop movement for testing
	if (CollisionPresent)
	{
		m_Owner->SetDirection(EntityDirection::None, EntityDirection::None);
		m_Owner->SetVelocity(Vector(0.0, 0.0));
	}
	// Get the last direction moved, and if the velocity is in this direction, set it to zero


	// Otherwise, do nothing
}



