#pragma once
#include "Component.h"


class ZoomMovementComp : public Component
{
public:
	virtual void Execute();

	ZoomMovementComp();
	ZoomMovementComp(GameEntity* Owner);

	virtual ~ZoomMovementComp();
};

class ZoomMovementCompCreator : public ComponentCreator
{
public:
	ZoomMovementCompCreator() {}
	~ZoomMovementCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new ZoomMovementComp(Owner);
	}
};