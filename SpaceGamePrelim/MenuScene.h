#pragma once
#include "Scene.h"

#include <string>

class MenuScene : public Scene
{
public:
	MenuScene();
	MenuScene(std::string SceneID);
	virtual ~MenuScene();

	virtual bool Enter();
	virtual void Exit();

	virtual void Update();
	virtual void Render();

};

