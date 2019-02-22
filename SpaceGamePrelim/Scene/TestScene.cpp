#include "TestScene.h"
#include "..\Objects\GameEntity.h"
#include "..\Objects\GameObject.h"
#include "..\TextureCode\TextureManager.h"
#include "..\Frame\MainApplication.h"

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