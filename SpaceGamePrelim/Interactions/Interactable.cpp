#include "Interactable.h"
#include "InteractionManager.h"

#include <iostream>

bool Interactable::OnInteraction(GameEntity * InteractingEntity)
{
	return m_Interactable;
}

Interactable::Interactable(bool IsInteractable) :
	m_Interactable(IsInteractable),
	m_HasDerived(nullptr)
{
	// add this interactable to the InteractionManager
	try 
	{
		InteractionManager::Instance()->AddInteractable(this);
	}
	catch (...) 
	{
		std::cout << "Expection thrown, Interactable::Interactable(bool arg)\n";
	}
}

Interactable::~Interactable()
{
	// remove this interactable from the InteractionManager
	InteractionManager::Instance()->RemoveInteractable(this);
}
 