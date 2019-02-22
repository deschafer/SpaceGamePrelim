
#include "Player.h"


Player::Player()
{
}


Player::~Player()
{
}

Player::Player(std::string RedTexture, TextureProperties* Prop, std::string TypeID, std::string SpecID,
	Vector InitPosition, Vector InitVelocity, Vector InitAccel) :
	GameEntity(RedTexture, Prop, TypeID, SpecID, InitPosition, InitVelocity, InitAccel)
{

}

void Player::Update()
{
	m_Velocity.setX(0);
	m_Velocity.setY(0);

	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_W))
	{
		GameEntity::m_Velocity.setY(-1);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_A))
	{
		GameEntity::m_Velocity.setX(-1);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_S))
	{
		GameEntity::m_Velocity.setY(1);
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_D))
	{
		GameEntity::m_Velocity.setX(1);
	}

	GameEntity::Update();
}

