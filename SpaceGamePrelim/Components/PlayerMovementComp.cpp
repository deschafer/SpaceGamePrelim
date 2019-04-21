
#include "PlayerMovementComp.h"
#include "..\Frame\InputManager.h"
#include "..\BasicTypes\EntityDirection.h"

#include <iostream>

static int MovementSpeed = 2;
static float MovementAccel = 0.30F;
static float MovementDeaccel = 0.10F;
static int MovementMax = 4;

PlayerMovementComp::PlayerMovementComp()
{
}

PlayerMovementComp::PlayerMovementComp(GameEntity* Owner) :
	Component(Owner)
{
}

PlayerMovementComp::~PlayerMovementComp() 
{

}


void PlayerMovementComp::Execute()
{
	Vector CurrentVelocity = m_Owner->GetVelocity();
	Vector Velocity;
	EntityDirection HorizComp = EntityDirection::None;
	EntityDirection VertiComp = EntityDirection::None;;

	Velocity.setX(0);
	Velocity.setY(0);

	float MovementMagnitudeY = (abs(CurrentVelocity.getY()) + MovementAccel);
	float MovementMagnitudeX = (abs(CurrentVelocity.getX()) + MovementAccel);


	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_W))
	{
		Velocity.setY(-(MovementMagnitudeY >= MovementMax ? MovementMax : MovementMagnitudeY));
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_A))
	{
		Velocity.setX(-(MovementMagnitudeX >= MovementMax ? MovementMax : MovementMagnitudeX));
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_S))
	{
		Velocity.setY((MovementMagnitudeY >= MovementMax ? MovementMax : MovementMagnitudeY));
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_D))
	{
		Velocity.setX((MovementMagnitudeX >= MovementMax ? MovementMax : MovementMagnitudeX));

	}

	// If there is not input,
	// then allow for deacceleration
	if (Velocity.getY() == 0 && CurrentVelocity.getY() != 0)
	{
		Velocity.setY(
			(CurrentVelocity.getY() < 0) ? 
			CurrentVelocity.getY() + MovementDeaccel :
			CurrentVelocity.getY() - MovementDeaccel);
	}
	if (Velocity.getX() == 0 && CurrentVelocity.getX() != 0)
	{
		Velocity.setX(
			(CurrentVelocity.getX() < 0) ?
			CurrentVelocity.getX() + MovementDeaccel :
			CurrentVelocity.getX() - MovementDeaccel);
	}

	// Based on direction, set the owning object direction
	if (Velocity.getX() > 0)
	{
		HorizComp = EntityDirection::East;
	}
	else if (Velocity.getX() < 0)
	{
		HorizComp = EntityDirection::West;
	}
	if (Velocity.getY() > 0)
	{
		VertiComp = EntityDirection::South;
	}
	else if (Velocity.getY() < 0)
	{
		VertiComp = EntityDirection::North;
	}

	m_Owner->SetDirection(HorizComp, VertiComp);
	m_Owner->SetVelocity(Velocity);
}