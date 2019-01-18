#include "MenuScene.h"
#include "GameEntity.h"
#include "TextureManager.h"
#include "MainApplication.h"

#include <iostream>

using namespace std;

MenuScene::MenuScene()
{
}

MenuScene::MenuScene(std::string SceneID, bool Paused)
{
	Scene::m_SceneID = SceneID;
	Scene::m_PauseScreen = Paused;
}


MenuScene::~MenuScene()
{
}

bool MenuScene::Enter()
{
	GameEntity *Temp = new GameEntity(new TextureProperties(Rect(0, 0, 100, 100),
		"Source", 1, 3, 1, 3), Vector(100, 100),
		Vector(.5, .5), Vector(0, 0));

	TextureManager::Instance()->load("Assets/arc2.png", "Source", 
		MainApplication::Instance()->GetRenderer());

	Scene::m_Objects.push_back(Temp);

	Temp = new GameEntity(new TextureProperties(Rect(-220, -200, 100, 100),
		"Source", 1, 3, 1, 3), Vector(200, 100),
		Vector(.5, 0), Vector(0, 0));

	Scene::m_Objects.push_back(Temp);

	cout << "Entering a menuscene\n";
	return true;
}
void MenuScene::Exit()
{

	cout << "exiting menuscene\n";

}

void MenuScene::Update()
{

	Scene::Update();

}
void MenuScene::Render()
{

	Scene::Render();

}