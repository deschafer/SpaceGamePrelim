#pragma once

#include "..\BasicTypes\Vector.h"

class GameEntity;

class Interactable
{ 
private:
	bool m_Interactable;

protected:
	void* m_HasDerived;

public:

	virtual bool OnInteraction(GameEntity* InteractingEntity);
	virtual Vector GetInteractablePosition() { return Vector(-1, -1); }
	
	void SetDerived(void* Mem) { m_HasDerived = Mem; }
	void* HasDerived() { return m_HasDerived; }
	void SetInteractable(bool IsInteractable) { m_Interactable = IsInteractable; }
	
	Interactable(bool IsInteractable);
	Interactable() = delete;
	virtual ~Interactable();
};

