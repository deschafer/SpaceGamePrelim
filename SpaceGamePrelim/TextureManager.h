#pragma once

#include <map>
#include <string>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#include "BasicTypes.h"

// singleton class structure
class TextureManager
{
private:
	TextureManager();

	static TextureManager *m_Instance;
	std::map<std::string, SDL_Texture*> m_TextureContainer;

public:
	~TextureManager();

	static TextureManager* Instance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new TextureManager();
			return m_Instance;
		}
		else
		{
			return m_Instance;
		}
	}

	bool load(std::string Pathname, std::string TextureID, SDL_Renderer* pRenderer);
	void DrawCurrentFrame(std::string TextureID, Rect Dimensions, SDL_RendererFlip Flip, 
		 SDL_Renderer *pRenderer, int CurrentRow, int CurrentFrame = 1);
	void TextureManager::DrawCurrentFrame(std::string TextureID, int X, int Y, Rect Dim, SDL_RendererFlip Flip,
		SDL_Renderer *pRenderer, int CurrentRow, int CurrentFrame);
	int RemoveTexture(std::string TextureID);
};

