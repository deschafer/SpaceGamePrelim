#pragma once
#include "Scene.h"



class TestScene : public Scene
{
public:
	TestScene();
	TestScene(std::string SceneID, bool Paused = 0);
	virtual ~TestScene();

	virtual bool Enter();
	virtual void Exit();

	virtual void Update() override;
	virtual void Render() override;

};

