#pragma once

#include "..\BasicTypes\Vector.h"

class GameEntity;

class Interactable
{ 
private:
	bool m_Interactable;
	size_t m_InteractableIndex;

public:

	virtual bool OnInteraction(GameEntity* InteractingEntity);
	virtual Vector GetInteractablePosition() { return Vector(-1, -1); }
	
	void SetInteractable(bool IsInteractable) { m_Interactable = IsInteractable; }
	
	Interactable(bool IsInteractable);
	Interactable() = delete;
	virtual ~Interactable();
};

