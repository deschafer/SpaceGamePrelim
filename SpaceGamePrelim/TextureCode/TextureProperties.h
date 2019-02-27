#pragma once

#include <string>

#include "..\BasicTypes\BasicTypes.h"

class TextureProperties
{
private:
	
	Rect m_Dimensions;		// SOURCE rectangle dimensions
	std::string m_TextureID;// Assoc. texture ID (TextureManager), or can be a RedTXTID
	int m_TextIndex;			// Assoc. index with source/red texture in source vector
	int m_NumberFrames;		// Total number of frames for an animated texture
	int m_AnimationSpeed;	// Animation speed, module value (lower == faster)

public:
	TextureProperties();
	TextureProperties(Rect Dimensions, std::string TextureID, int NumberFrames, int AnimationSpeed);
	TextureProperties(Rect Dimensions, std::string TextureID, int TxtIndex, int NumberFrames, int AnimationSpeed);
	TextureProperties(Rect Dimensions, std::string TextureID);
	TextureProperties(Rect Dimensions, std::string TextureID, int TxtIndex);


	Rect GetDimensions() { return m_Dimensions; }
	int GetNumberFrames() { return m_NumberFrames; }
	int GetAnimationSpeed() { return m_AnimationSpeed; }
	int GetTextureIndex() { return m_TextIndex; }
	std::string GetTextureID() { return m_TextureID; }

	virtual ~TextureProperties();

};


