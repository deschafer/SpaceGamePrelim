#pragma once
#include "Scene.h"
#include "..\Objects\Button.h"

#include <string>

class MenuScene : public Scene
{
private:


public:
	MenuScene();
	MenuScene(std::string SceneID, bool Paused = 0);
	virtual ~MenuScene();

	virtual bool Enter();
	virtual void Exit();

	virtual void Update() override;
	virtual void Render() override;

	static void s_ButtonHandler();

};

