#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "TextureManager.h"
#include "GameEntity.h"
#include "ActiveSceneManager.h"
#include "MenuScene.h"
#include "TestScene.h"
#include "InputManager.h"
#include "MapManager.h"
#include "RoomManager.h"

// Singleton class structure
class MainApplication
{
private:

	int m_WindowWidth;
	int m_WindowHeight;
	bool m_ResolutionChanged;

	SDL_Window* g_pWindow;
	SDL_Renderer* g_pRenderer;

	bool m_IsRunning;			
	static MainApplication* m_Instance;
	TextureManager* TextureContainer;
	GameEntity* Player;

	MainApplication();

public:
	
	~MainApplication();

	static MainApplication* Instance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new MainApplication();
			return m_Instance;
		}
		else
		{
			return m_Instance;
		}
	}

	int GetWndWidth() { return m_WindowWidth; }
	int GetWndHeight() { return m_WindowHeight; }
	bool ResolutionChanged();

	bool Initialize(const char * WindowTitle, int TopLeftXPos, int TopLeftYPos,
		int WndWidth, int WndHeight, bool Fullscreen, bool WndCentered = true);
	
	void HandleEvents();
	void Update();
	void Render();

	SDL_Renderer* GetRenderer() { return g_pRenderer; }
	SDL_Window* GetWindow() { return g_pWindow; }

	bool IsRunning() { return m_IsRunning; }

	void Terminate() { m_IsRunning = false; }	// Terminates this game instance
	void Clean();
	
};

