#pragma once

#include "..\BasicTypes\Vector.h"

class GameEntity;

class Interactable
{ 
private:

	bool m_Interactable;

public:

	virtual bool OnInteraction(GameEntity* InteractingEntity);
	virtual Vector GetScreenPosition() = 0;

	void SetInteractable(bool IsInteractable) { m_Interactable = IsInteractable; }
	
	Interactable(bool IsInteractable);
	Interactable() = delete;
	virtual ~Interactable();
};

