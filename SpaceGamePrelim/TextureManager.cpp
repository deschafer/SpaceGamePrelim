#include "TextureManager.h"

using namespace std;

TextureManager* TextureManager::m_Instance = nullptr;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

//
// load()
// Loads specefied asset and saves a handle to it into this object
//
bool TextureManager::load(std::string Pathname, std::string TextureID, SDL_Renderer* pRenderer)
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

	cout << "New texture " << TextureID << " loaded successfully." << endl;
}

//
// DrawCurrentFrame()
// Draws the current frame for a given object associated with an textureIF
//
void TextureManager::DrawCurrentFrame(std::string TextureID, Rect Dim, SDL_RendererFlip Flip, 
	SDL_Renderer *pRenderer, int CurrentRow, int CurrentFrame)
{

	// Local vars
	SDL_Rect SourceRect;
	SDL_Rect DestRect;

	// Setting information to draw this frame correctly
	SourceRect.x = Dim.Width() * CurrentFrame;
	SourceRect.y = Dim.Height() * (CurrentRow - 1);
	SourceRect.w = DestRect.w = Dim.Width();
	SourceRect.h = DestRect.h = Dim.Height();
	DestRect.x = Dim.TopLeftX();
	DestRect.y = Dim.TopLeftY();

	SDL_RenderCopyEx(pRenderer, m_TextureContainer[TextureID], &SourceRect,
		&DestRect, 0, 0, Flip);
}

void TextureManager::DrawCurrentFrame(std::string TextureID, int X, int Y, Rect Dim, SDL_RendererFlip Flip,
	SDL_Renderer *pRenderer, int CurrentRow, int CurrentFrame)
{

	// Local vars
	SDL_Rect SourceRect;
	SDL_Rect DestRect;

	// Setting information to draw this frame correctly
	SourceRect.x = Dim.Width() * CurrentFrame;
	SourceRect.y = Dim.Height() * (CurrentRow - 1);
	SourceRect.w = DestRect.w = Dim.Width();
	SourceRect.h = DestRect.h = Dim.Height();
	DestRect.x = X;
	DestRect.y = Y;

	SDL_RenderCopyEx(pRenderer, m_TextureContainer[TextureID], &SourceRect,
		&DestRect, 0, 0, Flip);
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