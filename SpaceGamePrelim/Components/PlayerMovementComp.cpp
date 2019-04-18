
#include "PlayerMovementComp.h"
#include "..\Frame\InputManager.h"
#include "..\BasicTypes\EntityDirection.h"

static int MovementSpeed = 8;

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
		Velocity.setY(-MovementSpeed);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_A))
	{
		Velocity.setX(-MovementSpeed);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_S))
	{
		Velocity.setY(MovementSpeed);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_D))
	{
		Velocity.setX(MovementSpeed);
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