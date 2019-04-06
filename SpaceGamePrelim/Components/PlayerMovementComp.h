#pragma once
#include "Component.h"


class PlayerMovementComp : public Component
{
public:

	virtual void Execute();

	PlayerMovementComp();
	PlayerMovementComp(GameEntity* Owner);

	virtual ~PlayerMovementComp();
};

class PlayerMovementCompCreator : public ComponentCreator
{
public:
	PlayerMovementCompCreator() {}
	~PlayerMovementCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new PlayerMovementComp(Owner);
	}
};