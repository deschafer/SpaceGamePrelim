#include "Scene.h"
#include "../Map/MapManager.h"

#include "..\Map\MapCell.h"


Scene::Scene()
{
}

Scene::Scene(bool Paused) :
	m_PauseScreen(Paused)
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

	for (size_t i = 0; i < m_Objects.size(); i++)
	{
		m_Objects[i]->Draw();
	}
}