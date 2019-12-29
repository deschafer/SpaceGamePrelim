#include "MapScene.h"
#include "../Parsing/SceneFactory.h"
#include "../Objects/Button.h"
#include "../Objects/Player.h"
#include "../Map/MapManager.h"
#include "../Map/MapAssetManager.h"
#include "../Map/MapAssets/TestingAsset.h"
#include "../Map/MapAssets/Door.h"

#include "../Components/PlayerMovementComp.h"
#include "../Components/ButtonInteractComp.h"
#include "../Components/PlayerMapMovementComp.h"
#include "../Components/MapCollisionComp.h"
#include "../Components/CenterPosComp.h"
#include "../Components/ZoomComp.h"
#include "../Components/MapZoomComp.h"
#include "../Components/ZoomMovementComp.h"
#include "../Components/PlayerInteractComp.h"

#include "../Parsing/RoomDefinitionParser.h"
#include "../Parsing/FallbackRoomParser.h"


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
	SceneFactory::Instance()->RegisterNewComponent("MapCollision", new MapCollisionCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("PlayerMapMovement", new PlayerMapMovementCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("CenteredPos", new CenterPosCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("Zoom", new ZoomCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("MapZoom", new MapZoomCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("ZoomMovement", new ZoomMovementCompCreator());
	SceneFactory::Instance()->RegisterNewComponent("PlayerInteraction", new PlayerInteractCompCreator());

	SceneFactory::Instance()->LoadNewScene("./XML/Scenes/Test.xml", this);

	// Now setting up the associated assets with the map
	// This should probably be moved somewhere into MapManager

	// Register before parsing source file
	MapAssetManager::Instance()->RegisterAsset("Testing", new TestingAsset());
	MapAssetManager::Instance()->RegisterAsset("Door", new Door());

	// Then load in the file that contains the defs for the registered assets
	MapAssetManager::Instance()->AddAssetSourceFile("./XML/Map/Assets.xml");

	// Need to create the room definitions first
	RoomDefinitionParser::Instance()->LoadRoomDefinitions("./XML/Map/Room.xml");

	// Then we can decide which of this are going to be fallbacks
	FallbackRoomParser::Instance()->LoadFallbackRooms("./XML/Map/Fallback.xml");

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