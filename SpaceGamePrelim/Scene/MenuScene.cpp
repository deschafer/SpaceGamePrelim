#include "MenuScene.h"
#include "..\Objects\GameEntity.h"
#include "..\TextureCode\TextureManager.h"
#include "..\Frame\MainApplication.h"

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
	

	TextureManager::Instance()->load("Assets/expl2.png", "Explosion", 
		MainApplication::Instance()->GetRenderer());

	GameEntity *Temp = new Button(new TextureProperties(Rect(0, 0, 32, 31),
		"Explosion", 1, 3, 1, 1), Vector(100, 100),
		Vector(0, 0), Vector(0, 0), s_ButtonHandler);


	m_Objects.push_back(Temp);

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

void MenuScene::s_ButtonHandler()
{
	cout << "Menu button has been clicked\n";
}