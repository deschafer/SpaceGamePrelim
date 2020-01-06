#include "Scene.h"
#include "../Map/MapManager.h"
#include "..\Map\MapCell.h"
#include "..\Objects\Drawable.h"


Scene::Scene() :
	m_Drawables(0)
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

	for (Drawable* CurrDrawable : m_Drawables)
	{
		CurrDrawable->Update();
	}

}
void Scene::Render()
{

	for (size_t i = 0; i < m_Objects.size(); i++)
	{
		m_Objects[i]->Draw();
	}

	for (Drawable* CurrDrawable : m_Drawables)
	{
		CurrDrawable->Draw();
	}
}