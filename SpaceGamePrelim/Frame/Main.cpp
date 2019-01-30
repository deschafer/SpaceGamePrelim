//
//
//
//

#include <SDL.h>
#include <iostream>

#include "MainApplication.h"

#define Debug

using namespace std;

int main(int argc, char* argv[])
{		
	Uint32 frameStart = 0,
		frameTime = 0;

	const int FPS = 60;
	const int DELAY_TIME = 1000.0f / FPS;

	int seed = 78;

	//cin >> seed;
	srand(seed);

	MainApplication* Application = MainApplication::Instance();	// Getting the app instance

	if (Application->Initialize("SpaceGamePrelim", 100, 100, 1408, 896, false))
	{

		// Main event loop 
		while (Application->IsRunning())
		{
			frameStart = SDL_GetTicks();

			Application->HandleEvents();
			Application->Update();
			Application->Render();


			// Setting framerate
			frameTime = SDL_GetTicks() - frameStart;

			if (frameTime< DELAY_TIME)
			{
				SDL_Delay((int)(DELAY_TIME - frameTime));
			}

		}

		
	}
	else
	{
		cout << "SDL was not initialized from main" << endl;
	}


	// Shutting down process(es)
	cout << "Closing main..." << endl;
	Application->Clean();

	return 0;
}