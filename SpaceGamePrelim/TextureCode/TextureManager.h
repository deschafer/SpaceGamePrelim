#pragma once

#include <unordered_map>
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

	std::unordered_map<std::string, int> m_TextureContainer;
	std::unordered_map<std::string, int> m_ReducedTextureDefinitions;

	std::unordered_map<std::string, std::vector<std::string>> m_TextureGroups;

	// Source Vectors
	std::vector<SDL_Texture*> m_SourceTextures;		
	std::vector<TextureProperties*> m_RedTextures;

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

	void DrawCurrentFrame(
		int X, 
		int Y, 
		int RedIndex, 
		SDL_RendererFlip Flip, 
		SDL_Renderer *pRenderer, 
		Rect DestRect,
		int CurrentRow, 
		int CurrentFrame = 1);
	void DrawCurrentFrame(
		int X, 
		int Y, 
		int RedIndex, 
		SDL_RendererFlip Flip,
		SDL_Renderer *pRenderer, 
		int CurrentRow, 
		int CurrentFrame = 1);
	void DrawStaticFrame(
		int X, 
		int Y, 
		int RedIndex, 
		SDL_Renderer *pRenderer);
	void DrawStaticFrame(
		int X, 
		int Y, 
		int RedIndex, 
		Rect DestDimesnions,
		SDL_Renderer *pRenderer);
	void FastDrawFrame(
		int X, 
		int Y, 
		TextureProperties* Properties, 
		SDL_Texture* Texture, 
		SDL_Renderer* Renderer);

	void AddTextureGroup(std::string GroupID, std::vector<std::string> ReducedTextures);
	std::string GetReducedFromTextureGrp(std::string TextureGroupID);
	TextureProperties* GetRoomProperties(int RedID) { return m_RedTextures[RedID]; }

	int GetRedTextureIndex(std::string ID) { return m_ReducedTextureDefinitions[ID]; }
	int GetSourceTextureIndex(std::string SourceID) { return m_TextureContainer[SourceID]; }
};

