#pragma once

#include "../Objects/GameEntity.h"

//
// A component is a small and basic routine that affects its owning gamentity in some
// way. The idea is to use indivudal components so we can improve reusability of code.
// Each component will be derived from this class and be its own unique class with its own 
// Execute function.
//

// Base class for all component types
class Component
{
protected:
	GameEntity* m_Owner;

	Component(GameEntity* Owner);

public:

	virtual void Execute() = 0;

	Component();
	virtual ~Component();
};

class ComponentCreator
{
public:
	ComponentCreator() {}
	~ComponentCreator() {}

	virtual Component* Create(GameEntity* Owner) = 0;
};


