
#include "Player.h"


Player::Player() : 
	GameEntity(nullptr)
{
}

Player::~Player()
{
}

Player::Player(Scene* ParentScene, std::string RedTexture, TextureProperties* Prop, std::string TypeID, std::string SpecID,
	Vector InitPosition, Vector InitVelocity, Vector InitAccel) :
	GameEntity(ParentScene, RedTexture, Prop, TypeID, SpecID, InitPosition, InitVelocity, InitAccel)
{
}

