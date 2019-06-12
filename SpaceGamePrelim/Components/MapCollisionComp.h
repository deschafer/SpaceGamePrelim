#pragma once
#include "Component.h"

class MapCollision;

class MapCollisionComp : public Component
{
private:

	void HandleMapCollisions(MapCollision* NewCollision, Vector &SetVelocity);

public:
	virtual void Execute();

	MapCollisionComp();
	MapCollisionComp(GameEntity* Owner);

	virtual ~MapCollisionComp();
};


class MapCollisionCompCreator : public ComponentCreator
{
public:
	MapCollisionCompCreator() {}
	~MapCollisionCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new MapCollisionComp(Owner);
	}
};

