
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
	Vector Velocity(0,0);
	EntityDirection HorizComp = EntityDirection::None;
	EntityDirection VertiComp = EntityDirection::None;;

	float MovementMagnitudeY = (abs(CurrentVelocity.getY()) + MovementAccel);
	float MovementMagnitudeX = (abs(CurrentVelocity.getX()) + MovementAccel);

	float LeftMovement = CurrentVelocity.getX() - MovementAccel;
	float RightMovement = CurrentVelocity.getX() + MovementAccel;
	float NorthMovement = CurrentVelocity.getY() - MovementAccel;
	float SouthMovement = CurrentVelocity.getY() + MovementAccel;


	bool W = InputManager::Instance()->IsKeyDown(SDL_SCANCODE_W);
	bool A = InputManager::Instance()->IsKeyDown(SDL_SCANCODE_A);
	bool S = InputManager::Instance()->IsKeyDown(SDL_SCANCODE_S);
	bool D = InputManager::Instance()->IsKeyDown(SDL_SCANCODE_D);

	if (W && !S)
	{
		Velocity.setY(NorthMovement <= -MovementMax ? -MovementMax : NorthMovement);
	}
	if (A && !D)
	{
		Velocity.setX(LeftMovement <= -MovementMax ? -MovementMax : LeftMovement);
	}
	if (S && !W)
	{
		Velocity.setY((SouthMovement >= MovementMax ? MovementMax : SouthMovement));
	}
	if (D && !A)
	{
		Velocity.setX(RightMovement >= MovementMax ? MovementMax : RightMovement);
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