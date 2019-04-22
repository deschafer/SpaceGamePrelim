#include "MapScene.h"
#include "../Parsing/SceneFactory.h"
#include "../Objects/Button.h"
#include "../Objects/Player.h"
#include "../Map/MapManager.h"
#include "../Components/PlayerMovementComp.h"
#include "../Components/ButtonInteractComp.h"
#include "../Components/PlayerMapMovementComp.h"
#include "../Components/CollisionComp.h"
#include "../Components/CenterPosComp.h"

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
	SceneFactory::Instance()->RegisterNewComponent("ButtonInteract", new ButtonInteractCompCreator());

	SceneFactory::Instance()->RegisterNewObject("Player", new PlayerCreator());
	SceneFactory::Instance()->RegisterNewComponent("PlayerMovement", new PlayerMovementCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("Collision", new CollisionCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("PlayerMapMovement", new PlayerMapMovementCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("CenteredPos", new CenterPosCompCreator());

	SceneFactory::Instance()->LoadNewScene("./XML/Scenes/Test.xml", this);

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
	MapManager::Instance()->Update();
	Scene::Update();
}

//
//
//
//
void MapScene::Render()
{
	MapManager::Instance()->Draw();
	Scene::Render();
}

//
//
//
//
void MapScene::s_ButtonHandler()
{
	//MapManager::Instance()->ResetMap();
}