#pragma once

#include <string>

#include "..\BasicTypes\BasicTypes.h"

class TextureProperties
{
private:
	
	Rect m_Dimensions;		// SOURCE rectangle dimensions
	std::string m_TextureID;// Assoc. texture ID (TextureManager)
	int m_CurrentFrame;		// Current frame for an animated texture, current column in sprite sheet, dst rect
	int m_NumberFrames;		// Total number of frames for an animated texture
	int m_AnimationSpeed;	// Animation speed, module value (lower == faster)
	int m_CurrentRow;		// Current row in the sprite sheet, for dst rect, for animation

public:
	TextureProperties();
	TextureProperties(Rect Dimensions, std::string TextureID, int CurrentFrame, 
		int NumberFrames, int AnimationSpeed, int CurrentRow);
	TextureProperties(Rect Dimensions, std::string TextureID);

	Rect GetDimensions() { return m_Dimensions; }
	int GetCurrentFrame() { return m_CurrentFrame; }
	int GetNumberFrames() { return m_NumberFrames; }
	int GetAnimationSpeed() { return m_AnimationSpeed; }
	int GetCurrentRow() { return m_CurrentRow; }
	std::string GetTextureID() { return m_TextureID; }

	virtual ~TextureProperties();

};

