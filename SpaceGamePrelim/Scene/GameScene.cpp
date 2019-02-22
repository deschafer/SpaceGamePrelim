#include "GameScene.h"
#include "../Map/MapManager.h"



GameScene::GameScene()
{
}

GameScene::GameScene(std::string SceneID, bool Paused) :
	Scene(Paused)
{
	Scene::m_SceneID = SceneID;
}


GameScene::~GameScene()
{
}


//
//
//
//
void GameScene::Update()
{
	MapManager::Instance()->Update();
	Scene::Update();
}

//
//
//
//
void GameScene::Render()
{
	MapManager::Instance()->Draw();
	Scene::Render();
}

