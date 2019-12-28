#pragma once

#include <string>
#include <SDL.h>

#include "..\BasicTypes\BasicTypes.h"

class TextureProperties
{
private:
	
	Rect m_Dimensions;		// SOURCE rectangle dimensions
	std::string m_TextureID;// Assoc. texture ID (TextureManager), or can be a RedTXTID
	int m_TextIndex;			// Assoc. index with source/red texture in source vector
	int m_NumberFrames;		// Total number of frames for an animated texture
	int m_AnimationSpeed;	// Animation speed, module value (lower == faster)
	SDL_Color m_Color;

public:
	TextureProperties();
	TextureProperties(Rect Dimensions, std::string TextureID, int NumberFrames, int AnimationSpeed);
	TextureProperties(Rect Dimensions, std::string TextureID);


	Rect GetDimensions() { return m_Dimensions; }
	int GetNumberFrames() { return m_NumberFrames; }
	int GetAnimationSpeed() { return m_AnimationSpeed; }
	int GetTextureIndex() { return m_TextIndex; }
	std::string GetTextureID() { return m_TextureID; }

	void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
		m_Color.a = a; 
		m_Color.r = r; 
		m_Color.g = g;
		m_Color.b = b;
	}

	virtual ~TextureProperties();
};


