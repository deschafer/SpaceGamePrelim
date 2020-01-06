#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>

#include "..\Objects\GameObject.h"

class Drawable;

class MapCell;

class Scene
{
protected:

	std::string m_SceneID;
	std::vector<GameObject*> m_Objects;
	std::list<Drawable*> m_Drawables;
	std::map<std::string, Callback> m_Handlers;
	
	bool m_PauseScreen;	// Indicates that all other scenes in the 
						// background should be paused when this scene is active.

	Scene();
	Scene(bool Paused);

public:

	std::string GetSceneID() { return m_SceneID; }
	bool IsPauseScreen() { return m_PauseScreen; }
	void SetGameObjects(std::vector<GameObject*> Objects) {
		m_Objects.clear();
		for (size_t i = 0; i < Objects.size(); i++)
			m_Objects.push_back(Objects[i]);
	}
	void AddDrawable(Drawable* NewDrawable) { m_Drawables.push_back(NewDrawable); }
	void RemoveDrawable(Drawable* SelDrawable) { m_Drawables.remove(SelDrawable); }

	Callback GetCallback(std::string HandlerID) { return m_Handlers[HandlerID]; }

	virtual bool Enter() = 0;
	virtual void Exit() = 0;

	virtual void Update();
	virtual void Render();

	virtual ~Scene();
};

