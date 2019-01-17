#include "ActiveSceneManager.h"
#include "MainApplication.h"

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
	return 0;
}

//
//
// Removes the active scene, scene at the top of the stack
//
int ActiveSceneManager::RemoveActiveScene()
{

	try {
		m_OpenScenes.pop_back();
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
		m_OpenScenes.clear();
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

	m_ActiveScene->Update();

}

//
//
//
//
void ActiveSceneManager::Render()
{

	m_ActiveScene->Render();

}