#include "MenuScene.h"
#include "..\Objects\GameEntity.h"
#include "..\TextureCode\TextureManager.h"
#include "..\Frame\MainApplication.h"
#include "..\Map\MapManager.h"
#include "..\Parsing\SceneFactory.h"

#include <iostream>

using namespace std;

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

bool MenuScene::Enter()
{


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


