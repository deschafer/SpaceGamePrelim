#include "ZoomMovementComp.h"
#include "../Frame/ZoomManager.h"


ZoomMovementComp::ZoomMovementComp()
{
}

ZoomMovementComp::ZoomMovementComp(GameEntity* Owner) :
	Component(Owner)
{
}

ZoomMovementComp::~ZoomMovementComp()
{
}

//
// Execute()
//
//
void ZoomMovementComp::Execute()
{
	Vector CurrentVelocity = m_Owner->GetVelocity();

	// Normalize the set velocity by the current zoom scale
	Vector NewVelocity(
		CurrentVelocity.getX() * (float)ZoomManager::Instance()->GetScale(),
		CurrentVelocity.getY() * (float)ZoomManager::Instance()->GetScale());

	// Trying to eliminate garbage, small numbers from the floats
	if (abs(NewVelocity.getX()) < 0.00001) NewVelocity.setX(0.0);
	if (abs(NewVelocity.getY()) < 0.00001) NewVelocity.setY(0.0);

	m_Owner->SetVelocity(NewVelocity);
}