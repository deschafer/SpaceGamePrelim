#pragma once
#include "Scene.h"

#include <string>

class MenuScene : public Scene
{
public:
	MenuScene();
	MenuScene(std::string SceneID, bool Paused = 0);
	virtual ~MenuScene();

	virtual bool Enter();
	virtual void Exit();

	virtual void Update() override;
	virtual void Render() override;

};

