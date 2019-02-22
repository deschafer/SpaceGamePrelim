#pragma once
#include "Scene.h"

// GameScenes always have a map associated with it
class GameScene : public Scene
{
public:
	GameScene();
	virtual ~GameScene();

	GameScene::GameScene(std::string SceneID, bool Paused);


	virtual bool Enter();
	virtual void Exit();

	virtual void Update() override;
	virtual void Render() override;

	static void s_ButtonHandler();
};
