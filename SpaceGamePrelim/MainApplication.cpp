#include "MainApplication.h"

// init static member
MainApplication* MainApplication::m_Instance = nullptr;


MainApplication::MainApplication() : m_IsRunning(true)
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

		// if the window creation succeeded create our renderer
		if (g_pWindow != 0)
		{
			std::cout << "Window creation success\n";

			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);

			if (g_pRenderer != nullptr) // renderer init success
			{
				std::cout << "Renderer creation success\n";
				SDL_SetRenderDrawColor(g_pRenderer, 105, 165, 200, 255);
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

	TextureContainer->load("Assets/arc2.png", "Source", g_pRenderer);

	// intialize joysticks/gamepads if supported
	// initialize any other game structures here as well

	return true;
}

//
// HandleEvents()
//
//
//
void MainApplication::HandleEvents()
{

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			Terminate();
			break;
		default:
			break;
		}
	}
}

//
// Update()
// updates all the current objects here
// includes map, visible map, player, entities,
// all objects get updated here
//
void MainApplication::Update()
{



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
	TextureContainer->DrawCurrentFrame("Source", Rect(20, 20, 200, 200), SDL_FLIP_NONE, g_pRenderer, 1);

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