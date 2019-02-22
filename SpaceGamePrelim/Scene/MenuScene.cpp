#include "MenuScene.h"


MenuScene::MenuScene()
{
}

MenuScene::MenuScene(std::string SceneID, bool Paused) : 
	Scene(Paused)
{
	Scene::m_SceneID = SceneID;
}

MenuScene::~MenuScene()
{
}


void MenuScene::Update()
{
	Scene::Update();
}

void MenuScene::Render()
{
	Scene::Render();
}


