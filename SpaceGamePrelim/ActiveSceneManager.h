#pragma once

#include <vector>

#include <SDL.h>

#include "Scene.h"

// Singleton class structure
class ActiveSceneManager
{
private:

	ActiveSceneManager();

	std::vector<Scene*> m_OpenScenes;	// Stack-like structure containing the open scenes
	Scene* m_ActiveScene;				// ptr to the active scene
	static ActiveSceneManager* m_sInstance;

public:
	
	static ActiveSceneManager* Instance()
	{
		if (m_sInstance == nullptr)
		{
			m_sInstance = new ActiveSceneManager();
			return m_sInstance;
		}
		else return m_sInstance;
	}

	int AddScene(Scene* NewScene);	// Adds scene to the top of the stack
	int RemoveActiveScene();	// Removes the active scene, scene at the top of the stack
	int Purge();				// Completely empties the stack of active scenes

	void Update();
	void Render();

	Scene* GetActiveScene() { return m_ActiveScene; }

	virtual ~ActiveSceneManager();
};

