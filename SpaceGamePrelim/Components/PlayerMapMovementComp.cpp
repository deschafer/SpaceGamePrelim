
#include "PlayerMapMovementComp.h"
#include "../Map/MapManager.h"

#include <iostream>

PlayerMapMovementComp::PlayerMapMovementComp()
{
}

PlayerMapMovementComp::PlayerMapMovementComp(GameEntity* Owner) : 
	Component(Owner)
{
}

PlayerMapMovementComp::~PlayerMapMovementComp()
{
}

//
// Execute()
// Moves the map offsets by the indicated movement from this object
//
void PlayerMapMovementComp::Execute()
{
	// Need to incorporate acceleration

	Vector Velocity = m_Owner->GetVelocity();
	MapManager::Instance()->OffsetMap(Velocity);
	std::cout << "Velocity set " << Velocity.getX() << " " << Velocity.getY() << std::endl;
}