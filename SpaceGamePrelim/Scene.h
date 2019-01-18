#pragma once

#include <string>
#include <vector>

#include "GameObject.h"

class Scene
{
protected:

	std::string m_SceneID;
	std::vector<GameObject*> m_Objects;
	
	bool m_PauseScreen;	// Indicates that all other scenes in the 
						// background should be paused when this scene is active.

public:

	std::string GetSceneID() { return m_SceneID; }
	bool IsPauseScreen() { return m_PauseScreen; }

	virtual bool Enter() = 0;
	virtual void Exit() = 0;

	virtual void Update();
	virtual void Render();

	Scene();
	virtual ~Scene();
};

