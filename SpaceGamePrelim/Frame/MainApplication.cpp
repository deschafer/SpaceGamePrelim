
#include "MainApplication.h"


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

		// if the window creation succeeded create our renderer
		if (g_pWindow != 0)
		{
			std::cout << "Window creation success\n";
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);

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

	InitializeRoomDefinitions();
	InitializeScenes();
	InitializeTextures();

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

	MapManager::Instance()->DrawGrid();

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
// InitializeRoomDefinitions()
// Current version has hard-coded room definitions
// Future version will parse in the definitions from
// a file and load them into the game.
//
bool MainApplication::InitializeRoomDefinitions()
{
	std::vector<int> Sides;
	std::vector<bool> SideDef;
	std::vector<char> Turns;

	// Top side
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);
	// Right side
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);
	// Bottom side
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);
	// Last side ending
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	// Right Side
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	// Bottom side
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	// Last side ending
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"Complex");

	SideDef.clear();
	Turns.clear();

	SideDef.push_back(1);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);

	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('R');

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"Inv_L_Rect");

	SideDef.clear();
	Turns.clear();

	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"L_Rect");

	SideDef.clear();
	Turns.clear();

	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('R');

	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"Rect");

	SideDef.clear();
	Turns.clear();

	std::vector<int> StaticSides;

	StaticSides.push_back(0);
	StaticSides.push_back(8);
	StaticSides.push_back(5);
	StaticSides.push_back(3);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);


	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');

	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);

	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, StaticSides, 5, 5),
		"Complex_Top");

	StaticSides.clear();
	SideDef.clear();
	Turns.clear();

	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');


	SideDef.push_back(1);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);

	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"Complex_Right");

	SideDef.clear();
	Turns.clear();

	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	Turns.push_back('R');



	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);

	SideDef.push_back(1);
	SideDef.push_back(1);

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"Complex_Bottom");

	SideDef.clear();
	Turns.clear();

	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);

	SideDef.push_back(1);

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"Complex_Left");

	SideDef.clear();
	Turns.clear();

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');

	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);

	SideDef.push_back(1);

	SideDef.push_back(1);

	SideDef.push_back(1);


	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"Stair_Down");

	SideDef.clear();
	Turns.clear();

	Turns.push_back('R');

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');

	SideDef.push_back(1);

	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);

	SideDef.push_back(1);

	SideDef.push_back(1);




	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, 5, 5),
		"Stair_Down_Reflected");


	SideDef.clear();
	Turns.clear();
	StaticSides.clear();

	StaticSides.push_back(8);
	StaticSides.push_back(5);
	StaticSides.push_back(0);

	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);


	StaticSides.push_back(0);

	StaticSides.push_back(0);


	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);
	SideDef.push_back(1);
	SideDef.push_back(1);

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('R');

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	Turns.push_back('R');

	Turns.push_back('R');

	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, StaticSides, 5, 5),
		"L_Rect_Static");

	SideDef.clear();
	Turns.clear();
	StaticSides.clear();

	StaticSides.push_back(5);
	StaticSides.push_back(3);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(3);
	StaticSides.push_back(5);

	StaticSides.push_back(5);
	StaticSides.push_back(3);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(3);
	StaticSides.push_back(5);

	StaticSides.push_back(5);
	StaticSides.push_back(3);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(3);
	StaticSides.push_back(5);

	StaticSides.push_back(5);
	StaticSides.push_back(3);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(0);
	StaticSides.push_back(3);
	StaticSides.push_back(5);

	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);

	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);

	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);

	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(0);
	SideDef.push_back(1);

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');

	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');
	Turns.push_back('L');
	Turns.push_back('L');
	Turns.push_back('R');
	Turns.push_back('R');


	RoomManager::Instance()->RegisterRoomType(new RoomProperties(SideDef, Turns, StaticSides, 5, 5),
		"Static_Complex");


	return true;
}

//
// InitializeTextures()
// All base textures are loaded into the game here
//
bool MainApplication::InitializeTextures()
{

	if (!TextureContainer->load("Assets/room2.png", "Room", g_pRenderer))
	{

		return false;
	}

	TextureContainer->SetReducedTexture("Wall", new TextureProperties(Rect(32, 0, 32, 32), "Room", 1, 0, 0, 1));
	TextureContainer->SetReducedTexture("Wall_Side_Right", new TextureProperties(Rect(0, 0, 32, 32), "Room", 1, 0, 0, 1));
	TextureContainer->SetReducedTexture("Wall_Side_Left", new TextureProperties(Rect(160, 0, 32, 32), "Room", 1, 0, 0, 1));
	TextureContainer->SetReducedTexture("Wall_Bottom", new TextureProperties(Rect(64, 128, 32, 32), "Room", 1, 0, 0, 1));
	TextureContainer->SetReducedTexture("Wall_Corner_Right", new TextureProperties(Rect(160, 128, 32, 32), "Room", 1, 0, 0, 1));
	TextureContainer->SetReducedTexture("Wall_Corner_Left", new TextureProperties(Rect(0, 128, 32, 32), "Room", 1, 0, 0, 1));
	TextureContainer->SetReducedTexture("Floor", new TextureProperties(Rect(64, 32, 32, 32), "Room", 1, 0, 0, 1));



	return true;
}

//
// InitializeScenes()
// Initial menu scenes will be added here
//
bool MainApplication::InitializeScenes()
{

	// Temporary addition of scenes here
	ActiveSceneManager::Instance()->AddScene(new MenuScene("menu scene"));
	//ActiveSceneManager::Instance()->AddScene(new TestScene("test scene", true));
	//ActiveSceneManager::Instance()->AddScene(new MenuScene("menu scene", false));

	return true;
}