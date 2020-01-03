#include "InteractionManager.h"

#include <vector>
#include <iostream>

#include "..\Map\MapAssets\Door.h"

std::mutex InteractionManager::m_InstanceMutex;
std::mutex InteractionManager::m_GetMutex;
InteractionManager* InteractionManager::m_Instance = nullptr;

InteractionManager::InteractionManager()
{
}

InteractionManager::~InteractionManager()
{
}


//
// AddInteractable
// Adds the given interactable to the master list so it can be interacted with
// NOTE: The return value is the index of the storage list. This should be saved.
//
size_t InteractionManager::AddInteractable(Interactable* NewInteractable) {

	m_GetMutex.lock();
	// add the new interactable
	m_Interactables.push_back(NewInteractable);
	size_t Size = m_Interactables.size() - 1;
	m_GetMutex.unlock();
	
	// then return the index
	return Size;
}

void InteractionManager::RemoveInteractable(size_t Index) {
	m_GetMutex.lock();
	m_Interactables[Index] = nullptr;
	m_GetMutex.unlock();
}

void InteractionManager::InteractWithSurroundings(GameEntity* InteractingEntity) {

	std::vector<Interactable*> InteractingObjects;

	// we iterate through all of our interactables
	for (Interactable* CurrentInteractable : m_Interactables)
	{
		if (CurrentInteractable)
		{
			Vector Difference = InteractingEntity->GetLocatableScreenPosition() - CurrentInteractable->GetInteractablePosition();

			// then get the magnitude of the difference vector
			if (Difference.length() <= m_InteractionDistance && Difference.length() >= 0)
			{
				// then add this to the InteractingObjects vector
				InteractingObjects.push_back(CurrentInteractable);
			}
		}
	}

	// then if we have any objects in our vector, we interate through it and 
	// call the OnInteraction method for each object
	for (Interactable* CurrentInteractable : InteractingObjects)
	{
		CurrentInteractable->OnInteraction(InteractingEntity);
	}
}