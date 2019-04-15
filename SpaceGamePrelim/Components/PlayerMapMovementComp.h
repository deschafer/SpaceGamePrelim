#pragma once

#include "Component.h"


class PlayerMapMovementComp : public Component
{
private:


public:
	virtual void Execute();

	PlayerMapMovementComp();
	PlayerMapMovementComp(GameEntity* Owner);
	virtual ~PlayerMapMovementComp();
};

class PlayerMapMovementCompCreator : public ComponentCreator
{
public:
	PlayerMapMovementCompCreator() {}
	~PlayerMapMovementCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new PlayerMapMovementComp(Owner);
	}
};