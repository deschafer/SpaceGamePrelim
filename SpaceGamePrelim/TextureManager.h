#pragma once

#include <map>
#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include "BasicTypes.h"

// singleton class structure
class TextureManager
{
private:

	static TextureManager *m_Instance;
	std::map<std::string, SDL_Texture*> m_TextureContainer;

public:
	TextureManager();
	~TextureManager();

	bool load(std::string Pathname, std::string TextureID, SDL_Renderer* pRenderer);

	
	
	//void DrawCurrentFrame(std::string TextureID, int TopLeftX, int TopLeftY, int Width,
	//	int Height,)
};

