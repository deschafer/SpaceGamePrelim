#include "Interactable.h"
#include "InteractionManager.h"

#include <iostream>

bool Interactable::OnInteraction(GameEntity * InteractingEntity)
{
	return m_Interactable;
}

void Interactable::SetInteractable(bool IsInteractable)
{
	if (IsInteractable && !m_Interactable)
	{
		m_Interactable = IsInteractable;

		// then we need to add to the interactable list
		InteractionManager::Instance()->AddInteractable(this);
	} 
	else if (!IsInteractable && m_Interactable)
	{
		m_Interactable = IsInteractable;

		// then we need to add to the interactable list
		InteractionManager::Instance()->RemoveInteractable(m_InteractableIndex);
	}
}

Interactable::Interactable(bool IsInteractable) :
	m_Interactable(IsInteractable),
	m_InteractableIndex(0)
{
	if (m_Interactable)
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
}

Interactable::~Interactable()
{
	InteractionManager::Instance()->RemoveInteractable(m_InteractableIndex);
}
 