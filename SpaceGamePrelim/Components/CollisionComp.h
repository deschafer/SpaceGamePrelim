#pragma once
#include "Component.h"

class CollisionComp : public Component
{
public:
	virtual void Execute();

	CollisionComp();
	CollisionComp(GameEntity* Owner);

	virtual ~CollisionComp();
};


class CollisionCompCreator : public ComponentCreator
{
public:
	CollisionCompCreator() {}
	~CollisionCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new CollisionComp(Owner);
	}
};

