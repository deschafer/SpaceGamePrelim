#pragma once
#include "GameScene.h"
class MapScene :
	public GameScene
{
public:
	MapScene();
	virtual ~MapScene();

	MapScene::MapScene(std::string SceneID, bool Paused);


	virtual bool Enter();
	virtual void Exit();

	virtual void Update() override;
	virtual void Render() override;

	static void s_ButtonHandler();
};

