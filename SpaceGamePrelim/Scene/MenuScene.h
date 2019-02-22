#pragma once
#include "Scene.h"
#include "..\Objects\Button.h"

#include <string>

class MenuScene : public Scene
{
protected:
	MenuScene();
	MenuScene::MenuScene(std::string SceneID, bool Paused);

public:
	
	virtual ~MenuScene();

	virtual bool Enter() = 0;
	virtual void Exit() = 0;

	virtual void Update() override;
	virtual void Render() override;

};

