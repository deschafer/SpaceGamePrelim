#include "MapScene.h"
#include "../Parsing/SceneFactory.h"
#include "../Objects/Button.h"
#include "../Map/MapManager.h"

#include <iostream>

const std::string ButtonHandlerStr = "ButtonHandler";

MapScene::MapScene()
{
}


MapScene::~MapScene()
{
}

MapScene::MapScene(std::string SceneID, bool Paused) :
	GameScene(SceneID, Paused)
{

	m_Handlers[ButtonHandlerStr] = s_ButtonHandler;
}


//
//
//
//
bool MapScene::Enter()
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
void MapScene::Exit()
{

#ifdef _DEBUG
	std::cout << "Exiting a gamescene\n";
#endif // DEBUG

}

//
//
//
//
void MapScene::Update()
{
	GameScene::Update();
}

//
//
//
//
void MapScene::Render()
{
	GameScene::Render();

}


void MapScene::s_ButtonHandler()
{
	std::cout << "Menu button has been clicked\n";
	MapManager::Instance()->ResetMap();
}