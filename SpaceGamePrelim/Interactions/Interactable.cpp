#include "Interactable.h"
#include "InteractionManager.h"

#include <iostream>

bool Interactable::OnInteraction(GameEntity * InteractingEntity)
{
	return m_Interactable;
}

Interactable::Interactable(bool IsInteractable) :
	m_Interactable(IsInteractable)
{
	// add this interactable to the InteractionManager
	try 
	{
		m_InteractableIndex = InteractionManager::Instance()->AddInteractable(this);
	}
	catch (...) 
	{
		std::cout << "Expection thrown, Interactable::Interactable(bool arg)\n";
	}
}

Interactable::~Interactable()
{
	InteractionManager::Instance()->RemoveInteractable(m_InteractableIndex);
}
 