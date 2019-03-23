//
//
//
//

#include <SDL.h>
#include <iostream>
#include <chrono>

#include "MainApplication.h"

#define Debug

using namespace std;

int main(int argc, char* argv[])
{		
	Uint32 frameStart = 0,
		frameTime = 0;

	const int FPS = 60;
	const int DELAY_TIME = 1000.0f / FPS;

	// Time seed generation
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto value = now_ms.time_since_epoch();
	long duration = value.count();

	std::chrono::milliseconds dur(duration);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);

	srand(duration);
	//srand(1);


	MainApplication* Application = MainApplication::Instance();	// Getting the app instance
	// 1408 x 896
	//if (Application->Initialize("SpaceGamePrelim", 100, 100, 1440, 900, false))
	if (Application->Initialize("SpaceGamePrelim", 100, 100, 1920, 1080, true))
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
