#pragma once

#include <string>
#include <vector>
#include <map>

#include "..\Objects\GameObject.h"

class Scene
{
protected:

	std::string m_SceneID;
	std::vector<GameObject*> m_Objects;
	std::map<std::string, Callback> m_Handlers;
	
	bool m_PauseScreen;	// Indicates that all other scenes in the 
						// background should be paused when this scene is active.
	bool m_MappedScreen;// Indicates if this screen contains the in use map from MapManager


	Scene();
	Scene(bool Paused, bool Mapped);

public:

	std::string GetSceneID() { return m_SceneID; }
	bool IsPauseScreen() { return m_PauseScreen; }
	bool IsMappedScreend() { return m_MappedScreen; }

	virtual bool Enter() = 0;
	virtual void Exit() = 0;

	virtual void Update();
	virtual void Render();

	virtual ~Scene();
};

