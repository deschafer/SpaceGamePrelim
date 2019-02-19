#include "Scene.h"
#include "../Map/MapManager.h"


Scene::Scene()
{
}

Scene::Scene(bool Paused, bool Mapped) :
	m_PauseScreen(Paused),
	m_MappedScreen(Mapped)
{
}

Scene::~Scene()
{
}

void Scene::Update()
{

	for (size_t i = 0; i < m_Objects.size(); i++)
	{
		m_Objects[i]->Update();
	}

}
void Scene::Render()
{
	if (m_MappedScreen)
	{
		MapManager::Instance()->Draw();
	}

	for (size_t i = 0; i < m_Objects.size(); i++)
	{
		m_Objects[i]->Draw();
	}
}