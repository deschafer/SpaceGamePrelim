#include "MapZoomComp.h"
#include "../Collision/CollisionManager.h"
#include "../BasicTypes/EntityDirection.h"
#include "../Frame/ZoomManager.h"


MapZoomComp::MapZoomComp()
{
}

MapZoomComp::~MapZoomComp()
{
}

MapZoomComp::MapZoomComp(GameEntity* Owner) :
	Component(Owner)
{

}

//
// Execute()
//
//
void MapZoomComp::Execute()
{
	if (ZoomManager::Instance()->IsChange())
	{

		// Check if we have actually moved,
		// if not then do not check for collisions
		bool CollisionPresent = false;
		MapCollision* CurrMapCollision;

		Vector CurrentPosition = m_Owner->GetPosition();

		// Get the collisions
		std::vector<Collision*> Collisions = CollisionManager::Instance()->CheckPosition(
			CurrentPosition, CurrentPosition, m_Owner);

		for (size_t i = 0; i < Collisions.size(); i++)
		{
			if (Collisions[i]->GetType() == CollisionType::MapWall)
			{
				HandleMapCollisions(
					static_cast<MapCollision*>(Collisions[i]),
					CurrentPosition);
				CollisionPresent = true;
			}
		}

		if (CollisionPresent)
		{
			m_Owner->SetPosition(CurrentPosition);
		}
	}
}

//
// HandleMapCollisions()
//
//
void MapZoomComp::HandleMapCollisions(MapCollision* NewCollision, Vector &Position)
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
		// Set this to the distance we can travel until we collide
		Position.setX(Position.getX() + Distance);

	}
	if (CollisionAngleVerti)
	{
		// Set this to the distance we can travel until we collide
		Position.setX(Position.getY() + Distance);
	}
}


