#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#include "..\BasicTypes\BasicTypes.h"


// singleton class structure
class TextureManager
{
private:
	TextureManager();

	static TextureManager *m_Instance;
	std::map<std::string, SDL_Texture*> m_TextureContainer;
	std::map<std::string, TextureProperties*> m_ReducedTextureDefinitions;

	std::map<std::string, std::vector<std::string>> m_TextureGroups;

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

	bool SetTexture(std::string Pathname, std::string TextureID, SDL_Renderer* pRenderer);
	int RemoveTexture(std::string TextureID);
	void SetReducedTexture(std::string ID, TextureProperties* Properties);

	void DrawCurrentFrame(int X, int Y, std::string RedTxtID, SDL_RendererFlip Flip, 
		SDL_Renderer *pRenderer, int CurrentRow, int CurrentFrame = 1);
	void DrawStaticFrame(int X, int Y, std::string RedTxtID, SDL_Renderer *pRenderer);


	void AddTextureGroup(std::string GroupID, std::vector<std::string> ReducedTextures);
	std::string GetReducedFromTextureGrp(std::string TextureGroupID);
	TextureProperties* GetRoomProperties(std::string ReducedTextureID) { return m_ReducedTextureDefinitions[ReducedTextureID]; }
};

