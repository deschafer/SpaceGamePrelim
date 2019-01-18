#include "TestScene.h"
#include "GameEntity.h"
#include "GameObject.h"
#include "TextureManager.h"
#include "MainApplication.h"

using namespace std;

TestScene::TestScene()
{
}


TestScene::~TestScene()
{
}


TestScene::TestScene(std::string SceneID, bool Paused)
{
	Scene::m_SceneID = SceneID;
	Scene::m_PauseScreen = Paused;
}


bool TestScene::Enter()
{
	GameEntity *Temp = new GameEntity(new TextureProperties(Rect(0, 0, 100, 100),
		"Source", 1, 3, 60, 1), Vector(100, 100),
		Vector(.5, 0), Vector(0, 0));

	TextureManager::Instance()->load("Assets/arc2.png", "Source",
		MainApplication::Instance()->GetRenderer());

	Scene::m_Objects.push_back(Temp);

	Temp = new Player(new TextureProperties(Rect(-220, -200, 100, 100),
		"tank", 1, 3, 60, 1), Vector(200, 100),
		Vector(0, 0), Vector(0, 0));

	TextureManager::Instance()->load("Assets/tankbrigade.png", "tank",
		MainApplication::Instance()->GetRenderer());

	Scene::m_Objects.push_back(Temp);

	cout << "Entering a testscene\n";
	return true;
}
void TestScene::Exit()
{

	cout << "exiting menuscene\n";

}

void TestScene::Update()
{
	Scene::Update();
}

void TestScene::Render()
{
	Scene::Render();
}