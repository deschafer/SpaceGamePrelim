#pragma once
#include "Component.h"
#include "../Collision/MapCollision.h"



class MapZoomComp : public Component
{
private:
	void HandleMapCollisions(MapCollision* NewCollision, Vector &Position);

public:
	virtual void Execute();

	MapZoomComp();
	MapZoomComp(GameEntity* Owner);

	virtual ~MapZoomComp();
};

class MapZoomCompCreator : public ComponentCreator
{
public:
	MapZoomCompCreator() {}
	~MapZoomCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new MapZoomComp(Owner);
	}
};
