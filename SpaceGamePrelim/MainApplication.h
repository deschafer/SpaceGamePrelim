#pragma once

#include <SDL.h>
#include <string>
#include <iostream>
#include <SDL_image.h>

// Singleton class structure
class MainApplication
{
private:

	SDL_Window* g_pWindow = 0;
	SDL_Renderer* g_pRenderer = 0;

	bool m_IsRunning;			// Indicates that the current game instance is running

	static MainApplication* m_Instance;

	MainApplication();
	~MainApplication();

public:
	

	static MainApplication* Instance()
	{
		if (m_Instance == nullptr)
		{
			return new MainApplication();
		}
		else
		{
			return m_Instance;
		}
	}

	bool Initialize(const char * WindowTitle, int TopLeftXPos, int TopLeftYPos,
		int WndWidth, int WndHeight, bool Fullscreen, bool WndCentered = true);
	
	void HandleEvents();
	void Update();
	void Render();

	bool IsRunning() { return m_IsRunning; }

	void Terminate() { m_IsRunning = false; }	// Terminates this game instance
	void Clean();
	


};

