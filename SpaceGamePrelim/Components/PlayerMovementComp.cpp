
#include "PlayerMovementComp.h"
#include "..\Frame\InputManager.h"

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

	m_Owner->SetVelocity(Velocity);
}