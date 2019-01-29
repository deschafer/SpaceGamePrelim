#include "Scene.h"



Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::Update()
{

	for (int i = 0; i < m_Objects.size(); i++)
	{
		m_Objects[i]->Update();
	}

}
void Scene::Render()
{
	for (int i = 0; i < m_Objects.size(); i++)
	{
		m_Objects[i]->Draw();
	}
}