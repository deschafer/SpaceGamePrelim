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
	for (Drawable* CurrDrawable : m_Drawables)
	{
		CurrDrawable->Update();
	}

}
void Scene::Render()
{
	for (Drawable* CurrDrawable : m_Drawables)
	{
		CurrDrawable->Draw();
	}
}