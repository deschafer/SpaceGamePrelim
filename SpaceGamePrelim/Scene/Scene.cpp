#include "Scene.h"



Scene::Scene()
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