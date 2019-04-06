
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

