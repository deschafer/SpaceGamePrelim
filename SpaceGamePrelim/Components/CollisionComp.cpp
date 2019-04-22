#include "CollisionComp.h"
#include "../Collision/CollisionManager.h"
#include "../BasicTypes/EntityDirection.h"
#include "../Collision/MapCollision.h"

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
		PositionAfterMovement, CurrentPosition, m_Owner);

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
	}
	else if (NewCollision->GetDir() == MapCollisionDir::Verti)
	{
		CollisionAngleVerti = true;
	}
	else if (NewCollision->GetDir() == MapCollisionDir::Diagonal)
	{
		CollisionAngleHoriz = true;
		CollisionAngleVerti = true;
	}

	int Distance = NewCollision->GetDistance();
	if (abs(Distance) == 1) Distance = 0;

	if (CollisionAngleHoriz)
	{
		if (Horiz == EntityDirection::East && (SetVelocity.getX() > 0))
		{
			// Set this to the distance we can travel until we collide
			SetVelocity.setX(Distance);
		}
		else if (Horiz == EntityDirection::West && (SetVelocity.getX() < 0))
		{
			// Set this to the distance we can travel until we collide
			SetVelocity.setX(-Distance);
		}
	}
	if (CollisionAngleVerti)
	{
		if (Verti == EntityDirection::South && (SetVelocity.getY() > 0))
		{
			// Set this to the distance we can travel until we collide
			SetVelocity.setY(Distance);
		}
		else if (Verti == EntityDirection::North && (SetVelocity.getY() < 0))
		{
			// Set this to the distance we can travel until we collide
			SetVelocity.setY(-Distance);
		}
	}
}


