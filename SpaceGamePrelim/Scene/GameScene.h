#pragma once
#include "Scene.h"

// GameScenes always have a map associated with it
class GameScene : public Scene
{
protected:
	GameScene::GameScene(std::string SceneID, bool Paused);
	GameScene();

public:
	virtual ~GameScene();


	virtual bool Enter() = 0;
	virtual void Exit() = 0;

	virtual void Update() override;
	virtual void Render() override;

};
