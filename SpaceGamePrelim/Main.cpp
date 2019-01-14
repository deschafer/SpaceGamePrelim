//
//
//
//

#include <SDL.h>
#include <iostream>

#include "MainApplication.h"

using namespace std;

int main(int argc, char* argv[])
{		

	MainApplication* Application = MainApplication::Instance();	// Getting the app instance

	if (Application->Initialize("SpaceGamePrelim", 100, 100, 1400, 900, false))
	{


		// Main event loop 
		while (Application->IsRunning())
		{

			Application->HandleEvents();
			Application->Update();
			Application->Render();


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