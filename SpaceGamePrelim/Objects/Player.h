#pragma once

#include "GameEntity.h"
#include "..\Frame\InputManager.h"
#include "..\BasicTypes\BasicTypes.h"
#include "..\TextureCode\TextureManager.h"
#include "BaseCreator.h"

class Player : public GameEntity
{
public:

	Player(Scene* ParentScene, std::string RedTexture, TextureProperties* Prop, std::string TypeID, std::string SpecID,
		Vector InitPosition, Vector InitVelocity, Vector InitAccel);

	Player();
	~Player();
};

class PlayerCreator : public BaseCreator
{
public:

	PlayerCreator() {}
	virtual ~PlayerCreator() {}

	virtual GameObject* CreateObject() override
	{
		return new Player;
	}

};
