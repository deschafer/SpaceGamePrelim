#pragma once
#include "Component.h"

class MapCollision;

class CollisionComp : public Component
{
private:

	void HandleMapCollisions(MapCollision* NewCollision, Vector &SetVelocity);

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

