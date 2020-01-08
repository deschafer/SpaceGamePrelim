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
	
	void SetInteractable(bool IsInteractable);
	void ClearState() { m_Interactable = false; m_InteractableIndex = 0; }
	
	Interactable(bool IsInteractable);
	Interactable() = delete;
	virtual ~Interactable();
};

