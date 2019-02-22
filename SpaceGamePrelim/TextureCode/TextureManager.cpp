#include "TextureManager.h"

#include <random>

using namespace std;

TextureManager* TextureManager::m_Instance = nullptr;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

//
// NOTE: Rect class that's a part of TextureProperties is for the SOURCE rectangle, not the destination
//

//
// load()
// Loads specefied asset and saves a handle to it into this object
//
bool TextureManager::SetTexture(std::string Pathname, std::string TextureID, SDL_Renderer* pRenderer)
{
	// Creating and verifying the surface
	SDL_Surface* TempSurface = IMG_Load(Pathname.c_str());
	if (TempSurface == nullptr)
	{
		return false;
	}

	// Creating the texture
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(pRenderer, TempSurface);

	// Freeing the surface, as we don't need it anymore
	SDL_FreeSurface(TempSurface);

	if (Texture == nullptr)
	{
		return false;
	}
	else
	{
		// Saving a handle to the texture that was just loaded
		m_TextureContainer[TextureID] = Texture;
	}

#ifdef _DEBUG
	cout << "New texture " << TextureID << " loaded successfully." << endl;
#endif // DEBUG
}

//
// SetReducedTexture()
// Sets a reduced texture -- a small section of a loaded in
// texture or sprite sheet that corresponds to a single
// meaningful section of that texture.
//
void TextureManager::SetReducedTexture(std::string ID, TextureProperties* Properties)
{

	if (!Properties)
	{
#ifdef _DEBUG
		cout << "Properties passed was nullptr" << endl;
#endif // DEBUG
	}
	else
	{
		m_ReducedTextureDefinitions[ID] = Properties;
	}

}

void TextureManager::DrawCurrentFrame(int X, int Y, std::string RedTxtID, SDL_RendererFlip Flip,
	SDL_Renderer *pRenderer, int CurrentRow, int CurrentFrame)
{

	TextureProperties* Properties = m_ReducedTextureDefinitions[RedTxtID];
	if (!Properties)
	{
#ifdef _DEBUG
		cout << "No reduced texture formatted for this ID " << RedTxtID << endl;
#endif // DEBUG
		return;
	}

	Rect Dim = Properties->GetDimensions();
	SDL_Rect SourceRect;
	SDL_Rect DestRect;

	// Setting information to draw this frame correctly
	SourceRect.x = Dim.Width() * CurrentFrame;
	SourceRect.y = Dim.Height() * (CurrentRow - 1);
	SourceRect.w = DestRect.w = Dim.Width();
	SourceRect.h = DestRect.h = Dim.Height();
	DestRect.x = X;
	DestRect.y = Y;

	SDL_RenderCopyEx(pRenderer, m_TextureContainer[Properties->GetTextureID()], &SourceRect,
		&DestRect, 0, 0, Flip);

}

//
// DrawStaticFrame() 
// A simplified function for drawing textures that stay in a static state
// and do not flip, changed position in the sprite sheet, and have
// a reduced texture definition.
//
void TextureManager::DrawStaticFrame(int X, int Y, std::string RedTxtID, SDL_Renderer *pRenderer)
{
	TextureProperties* Properties = m_ReducedTextureDefinitions[RedTxtID];
	if (!Properties)
	{
#ifdef _DEBUG
		cout << "No reduced texture formatted for this ID " << RedTxtID << endl;
#endif // DEBUG
		return;
	}

	Rect Dim = Properties->GetDimensions();
	SDL_Rect SourceRect;
	SDL_Rect DestRect;

	// Setting information to draw this frame correctly
	SourceRect.x = Dim.TopLeftX();
	SourceRect.y = Dim.TopLeftY();
	SourceRect.w = DestRect.w = Dim.Width();
	SourceRect.h = DestRect.h = Dim.Height();
	DestRect.x = X;
	DestRect.y = Y;

	SDL_RenderCopyEx(pRenderer, m_TextureContainer[Properties->GetTextureID()], &SourceRect,
		&DestRect, 0, 0, SDL_FLIP_NONE);
}

//
// RemoveTexture()
// Removes the texture associated with the ID given
//
int TextureManager::RemoveTexture(std::string TextureID) 
{
	m_TextureContainer.erase(TextureID);

	return 0;
}

//
// AddTextureGroup()
// Adds a list of reduced textures to an associated texture group
//
void TextureManager::AddTextureGroup(std::string GroupID, vector<std::string> ReducedTextures)
{

	for (size_t i = 0; i < ReducedTextures.size(); i++)
	{
		m_TextureGroups[GroupID].push_back(ReducedTextures[i]);
	}

}

//
// GetReducedFromTextureGrp()
// Returns a random texture from the texture group, and returns its reduced text. ID
//
std::string TextureManager::GetReducedFromTextureGrp(std::string TextureGroupID)
{

	int Number = m_TextureGroups[TextureGroupID].size();
	if (Number == 0) return 0;
	int Random = rand() % Number;

	return m_TextureGroups[TextureGroupID][Random];
	
}