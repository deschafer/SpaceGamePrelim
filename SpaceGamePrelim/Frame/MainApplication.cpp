
#include "MainApplication.h"
#include "..\Parsing\InitFactory.h"
#include "..\Parsing\SceneFactory.h"
#include "..\Scene\MapScene.h"
#include "..\Objects\Button.h"

// init static member
MainApplication* MainApplication::m_Instance = nullptr;


MainApplication::MainApplication() : 
	m_IsRunning(true),
	m_WindowHeight(0),
	m_WindowWidth(0)
{}

MainApplication::~MainApplication()
{}

//
//
//
//
bool MainApplication::Initialize(const char *WindowTitle, int TopLeftXPos, int TopLeftYPos,
	int WndWidth, int WndHeight, bool Fullscreen, bool WndCentered) 
{
	int WndFlags = SDL_WINDOW_SHOWN;	// Sets default window style
	int topLeftXPos = TopLeftXPos;		// Default X/Y positions
	int topLeftYPos = TopLeftYPos;		// ... 

	m_WindowHeight = WndHeight;
	m_WindowWidth = WndWidth;
	m_ResolutionChanged = true;

	// Save a pointer to TextureManager instance
	TextureContainer = TextureManager::Instance();

	// First initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Centering the window id applicable
		if (WndCentered)
		{
			topLeftXPos = SDL_WINDOWPOS_CENTERED;
			topLeftYPos = SDL_WINDOWPOS_CENTERED;
		}
		// Checking the fullscreen flag
		if (Fullscreen)
		{
			WndFlags = SDL_WINDOW_FULLSCREEN;
		}

		// Create the window with the new settings
		g_pWindow = SDL_CreateWindow(WindowTitle, topLeftXPos, topLeftYPos, WndWidth, WndHeight, WndFlags);
		//SDL_SetWindowResizable(g_pWindow, SDL_TRUE);

		// if the window creation succeeded create our renderer
		if (g_pWindow != 0)
		{
			std::cout << "Window creation success\n";
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

			if (g_pRenderer != nullptr) // renderer init success
			{
				std::cout << "Renderer creation success\n";
				//SDL_SetRenderDrawColor(g_pRenderer, 105, 165, 200, 255);
				SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 0);
			}
			else
			{
				std::cout << "Render creation failed" << std::endl;
				return false;
			}
		}
		else
		{
			std::cout << "Window was unable to be created." << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "game init failure - " << SDL_GetError() << "\n";
		return false; // sdl could not initialize
	}
	// everything succeeded lets draw the window
	std::cout << "SDL intiialization was successful" << std::endl;

	
	// intialize joysticks/gamepads if supported
	// initialize any other game structures here as well

	InitFactory::Instance()->LoadRoomDefinitions("XML/Map/Room.xml");
	InitFactory::Instance()->LoadTextures("XML/Textures/Textures.xml");


	InitializeScenes();

	return true;
}

//
// HandleEvents()
//
//
//
void MainApplication::HandleEvents()
{
	InputManager::Instance()->HandleEvents();
}

//
// Update()
// updates all the current objects here
// includes map, visible map, player, entities,
// all objects get updated here
//
void MainApplication::Update()
{
	// First update the map objects
	


	// Then update other objects on top of the map objects
	try {
		ActiveSceneManager::Instance()->Update();
	}
	catch (...)
	{

	}

}

//
// Render()
//
//
//
void MainApplication::Render()
{
	SDL_RenderClear(g_pRenderer);

	// Draw map first
	// Draw the objects on the map
	// Then draw the UI elements
	// Then draw any overlayed menus


	try {
		ActiveSceneManager::Instance()->Render();
	}
	catch (...)
	{
	}

	SDL_RenderPresent(g_pRenderer);
}

//
// Clean()
// Called after main loop has been exited,
// cleans all SDL framework objects before closing.
//
void MainApplication::Clean()
{
	std::cout << "Closing..." << std::endl;

	// Clean window
	SDL_DestroyWindow(g_pWindow);
	// Clean renderer
	SDL_DestroyRenderer(g_pRenderer);
	// Finally, closing application
	SDL_Quit();
}

//
//
//
//
bool MainApplication::ResolutionChanged()
{

	if (m_ResolutionChanged)
	{
		m_ResolutionChanged = false;
		return true;
	}
	else return false;
}

//
// InitializeScenes()
// Initial menu scenes will be added here
//
bool MainApplication::InitializeScenes()
{

	// Temporary addition of scenes here
	ActiveSceneManager::Instance()->AddScene(new MapScene("Game Scene", false));

	return true;
}