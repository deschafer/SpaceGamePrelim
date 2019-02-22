#include "GameScene.h"
#include "../Parsing/SceneFactory.h"
#include "../Objects/Button.h"
#include "../Map/MapManager.h"

#include <iostream>

const std::string ButtonHandlerStr = "ButtonHandler";


GameScene::GameScene()
{
}

GameScene::GameScene(std::string SceneID, bool Paused) :
	Scene(Paused)
{
	Scene::m_SceneID = SceneID;

	m_Handlers[ButtonHandlerStr] = s_ButtonHandler;
}


GameScene::~GameScene()
{
}

//
//
//
//
bool GameScene::Enter()
{
	// Registering the types assoc with this scene
	SceneFactory::Instance()->RegisterNewObject("Button", new ButtonCreator());

	SceneFactory::Instance()->LoadNewScene("XML/Scenes/Test.xml", this);

	#ifdef _DEBUG
	std::cout << "Entering a gamescene\n";
	#endif // DEBUG

	return true;
}

//
//
//
//
void GameScene::Exit()
{

	#ifdef _DEBUG
	std::cout << "Exiting a gamescene\n";
	#endif // DEBUG

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


void GameScene::s_ButtonHandler()
{
	std::cout << "Menu button has been clicked\n";
	MapManager::Instance()->ResetMap();
}