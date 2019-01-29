#pragma once

#include "GameEntity.h"
#include "..\Frame\InputManager.h"
#include "..\BasicTypes\BasicTypes.h"
#include "..\TextureCode\TextureManager.h"

class Player : public GameEntity
{
public:

	Player(TextureProperties* Properties,
		Vector InitPosition, Vector InitVelocity, Vector InitAccel);

	virtual void Update() override;

	Player();
	~Player();
};

