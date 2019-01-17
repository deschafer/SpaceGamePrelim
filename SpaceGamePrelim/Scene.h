#pragma once

#include <string>
#include <vector>

#include "GameObject.h"

class Scene
{
protected:

	std::string m_SceneID;
	std::vector<GameObject*> m_Objects;
	
public:

	std::string GetSceneID() { return m_SceneID; }

	virtual bool Enter() = 0;
	virtual void Exit() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;

	Scene();
	virtual ~Scene();
};

