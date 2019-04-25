#pragma once
#include "Component.h"


class ZoomComp : public Component
{
public:
	virtual void Execute();

	ZoomComp();
	ZoomComp(GameEntity* Owner);

	virtual ~ZoomComp();
};

class ZoomCompCreator : public ComponentCreator
{
public:
	ZoomCompCreator() {}
	~ZoomCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new ZoomComp(Owner);
	}
};
