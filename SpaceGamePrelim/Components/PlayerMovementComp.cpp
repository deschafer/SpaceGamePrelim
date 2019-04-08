
#include "PlayerMovementComp.h"
#include "..\Frame\InputManager.h"
#include "..\BasicTypes\EntityDirection.h"

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
	Vector Velocity;
	EntityDirection HorizComp = EntityDirection::None;
	EntityDirection VertiComp = EntityDirection::None;;

	Velocity.setX(0);
	Velocity.setY(0);

	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_W))
	{
		Velocity.setY(-1);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_A))
	{
		Velocity.setX(-1);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_S))
	{
		Velocity.setY(1);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_D))
	{
		Velocity.setX(1);
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