#pragma once

#include "Component.h"

class CenterPosComp : public Component
{
public:
	virtual void Execute();

	CenterPosComp();
	CenterPosComp(GameEntity* Owner);

	virtual ~CenterPosComp();
};


class CenterPosCompCreator : public ComponentCreator
{
public:
	CenterPosCompCreator() {}
	~CenterPosCompCreator() {}

	virtual Component* Create(GameEntity* Owner)
	{
		return new CenterPosComp(Owner);
	}
};
