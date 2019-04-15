
#include "PlayerMapMovementComp.h"
#include "../Map/MapManager.h"

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

	// Need to incorporate velocity
	// Change from the current position

	Vector Velocity = m_Owner->GetVelocity();

	MapManager::Instance()->OffsetMap(Velocity);
	

}