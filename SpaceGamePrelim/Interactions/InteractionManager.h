#pragma once

#include <mutex>
#include <vector>

#include "..\Objects\GameEntity.h"
#include "..\Map\MapManager.h"

class Interactable;

class InteractionManager
{
private:

	static std::mutex m_InstanceMutex;			// mut excl. used for the Instance() method
	static std::mutex m_GetMutex;				// mut excl. usef for the Get methods
	static InteractionManager* m_Instance;		

	static const int m_InteractionDistance = MapManager::CellWidthSrc + MapManager::CellWidthSrc / 2;

	std::vector<Interactable*> m_Interactables;

	InteractionManager();

public:
	
	static InteractionManager* Instance() {
		
		m_InstanceMutex.lock();
		if (!m_Instance) 
		{
			m_Instance = new InteractionManager;
		}
		m_InstanceMutex.unlock();
		return m_Instance;
	}

	size_t AddInteractable(Interactable* NewInteractable);
	void RemoveInteractable(size_t Index);
	void InteractWithSurroundings(GameEntity* InteractingEntity);
	

	virtual ~InteractionManager();
};


