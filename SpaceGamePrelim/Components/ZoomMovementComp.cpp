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
		CurrentVelocity.getX() * ZoomManager::Instance()->GetScale(),
		CurrentVelocity.getY() * ZoomManager::Instance()->GetScale());

	m_Owner->SetVelocity(NewVelocity);

}