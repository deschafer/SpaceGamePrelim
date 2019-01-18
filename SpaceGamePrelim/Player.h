#pragma once

#include "GameEntity.h"
#include "InputManager.h"
#include "BasicTypes.h"
#include "TextureProperties.h"

class Player : public GameEntity
{
public:

	Player(TextureProperties* Properties,
		Vector InitPosition, Vector InitVelocity, Vector InitAccel);

	virtual void Update() override;

	Player();
	~Player();
};

