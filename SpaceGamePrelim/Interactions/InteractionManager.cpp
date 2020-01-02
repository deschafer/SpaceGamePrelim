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

void InteractionManager::AddInteractable(Interactable* NewInteractable) {
	m_GetMutex.lock();

	// based on the screen position, we add it to a localized region

	m_Interactables.push_back(NewInteractable);
	m_GetMutex.unlock();
}

void InteractionManager::RemoveInteractable(Interactable* Object) {
	m_GetMutex.lock();
	m_Interactables.remove(Object);
	m_GetMutex.unlock();
}

void InteractionManager::InteractWithSurroundings(GameEntity* InteractingEntity) {

	std::vector<Interactable*> InteractingObjects;

	// we iterate through all of our interactables
	for (Interactable* CurrentInteractable : m_Interactables)
	{
		Vector Difference = InteractingEntity->GetLocatableScreenPosition() - CurrentInteractable->GetInteractablePosition();
		
		// then get the magnitude of the difference vector
		if (Difference.length() <= m_InteractionDistance && Difference.length() > 0)
		{
			// then add this to the InteractingObjects vector
			InteractingObjects.push_back(CurrentInteractable);
		}
	}

	// then if we have any objects in our vector, we interate through it and 
	// call the OnInteraction method for each object
	for (Interactable* CurrentInteractable : InteractingObjects)
	{
		// then also get the map cell at this position
		Door* NewDoor = (Door*)CurrentInteractable;

		Map* AssetMap = NewDoor->GetParentMap();
		MapCell* CurrentCell = nullptr;
		MapCoordinate AssetMapPosition = NewDoor->GetPosition();

		if (AssetMap && 
			(CurrentCell = (MapCell*)AssetMap->GetCell(AssetMapPosition.GetPositionX(), AssetMapPosition.GetPositionY())))
		{
			// get the cell at this position if it exists
			// highlight it 
			CurrentCell->SetColor(255, 0, 0, 255);

			AssetMap->SetCell(AssetMapPosition.GetPositionX(), AssetMapPosition.GetPositionY(), nullptr);
		}

		if (NewDoor) {
			NewDoor->SetColor(255, 0, 0, 255);
		}

		CurrentInteractable->OnInteraction(InteractingEntity);
	}
}