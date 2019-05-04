#include "ActiveSceneManager.h"
#include "..\Frame\MainApplication.h"

ActiveSceneManager* ActiveSceneManager::m_sInstance = nullptr;


ActiveSceneManager::ActiveSceneManager() : 
	m_ActiveScene(nullptr)
{
}

ActiveSceneManager::~ActiveSceneManager()
{
}


//
//
// Adds scene to the top of the stack
//
int ActiveSceneManager::AddScene(Scene* NewScene)
{
	if (NewScene == nullptr) throw "NewScene was nullptr";
	try {
		m_OpenScenes.push_back(NewScene);
	}
	catch (const std::bad_alloc &) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector push_back bad allocation -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}
	catch (const std::exception &) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector push_back excpetion -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}
	catch (...) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector push_back other error (...) -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}

	// Get a ptr to the current, active scene
	m_ActiveScene = m_OpenScenes.back();

	// Initialiaze the state
	m_ActiveScene->Enter();
	return 0;
}

//
//
// Removes the active scene, scene at the top of the stack
//
int ActiveSceneManager::RemoveActiveScene()
{
	m_ActiveScene->Exit();

	try {
		if(!m_OpenScenes.empty()) m_OpenScenes.pop_back();
	}
	catch (const std::bad_alloc &) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector pop_back bad allocation -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}
	catch (const std::exception &) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector pop_back excpetion -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}
	catch (...) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector pop_back other error (...) -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}

	// Get a new ptr to the new active scene
	if(!m_OpenScenes.empty()) m_ActiveScene = m_OpenScenes.back();
	else m_ActiveScene = nullptr;
	return 0;

}

//
//
// Completely empties the stack of active scenes
//
int ActiveSceneManager::Purge()
{

	try {
		// May need to do more here to clear all memory in the scenes
		if(!m_OpenScenes.empty()) m_OpenScenes.clear();
	}
	catch (const std::bad_alloc &) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector clear bad allocation -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}
	catch (const std::exception &) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector clear excpetion -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}
	catch (...) {
		SDL_ShowSimpleMessageBox(0, "Error", "std vector clear other error (...) -- AddScene",
			MainApplication::Instance()->GetWindow());
		return -1;
	}

	// No active scene, so set as nullptr
	m_ActiveScene = nullptr;
	return 0;
}

//
//
//
//
void ActiveSceneManager::Update()
{
	bool Update = true;

	// Iterate through all the scenes under the active scene
	// located at the rear of the vector

	// Only update until a non paused screen
	for (size_t i = 0; i < m_OpenScenes.size() && Update; i++)
	{
		if(m_OpenScenes[i] != m_ActiveScene) m_OpenScenes[i]->Update();
		if (m_OpenScenes[i]->IsPauseScreen()) Update = false;
	}


	if (m_ActiveScene != nullptr) m_ActiveScene->Update();
	else
	{
		throw "ActiveScene was nullptr";
	}
}

//
//
//
//
void ActiveSceneManager::Render()
{
	/*
	if(m_ActiveScene != nullptr)
		m_ActiveScene->Render();
	else
	{
		throw "ActiveScene was nullptr";
	}
	*/

	// Iterate through all the scenes in the vector
	// drawing them in the correct order
	if (!m_OpenScenes.empty())
	{
		for (size_t i = 0; i < m_OpenScenes.size(); i++)
		{
			m_OpenScenes[i]->Render();
		}
	}


}