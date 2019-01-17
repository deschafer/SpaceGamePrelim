#include "MenuScene.h"
#include "GameEntity.h"
#include "TextureManager.h"
#include "MainApplication.h"

#include <iostream>

using namespace std;

MenuScene::MenuScene()
{
}

MenuScene::MenuScene(std::string SceneID)
{
	Scene:m_SceneID = SceneID;
}


MenuScene::~MenuScene()
{
}

bool MenuScene::Enter()
{
	GameEntity *Temp = new GameEntity(new TextureProperties(Rect(0, 0, 100, 100),
		"Source", 1, 3, 1, 3), Vector(100, 100),
		Vector(.05, 0), Vector(.5, 0));

	TextureManager::Instance()->load("Assets/arc2.png", "Source", 
		MainApplication::Instance()->GetRenderer());

	Scene::m_Objects.push_back(Temp);

	cout << "Entering a menuscene\n";
	return true;
}
void MenuScene::Exit()
{

}

void MenuScene::Update()
{

}
void MenuScene::Render()
{

}