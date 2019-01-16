#pragma once

#include <string>

#include "BasicTypes.h"

class TextureProperties
{
private:
	
	Rect m_Dimensions;
	std::string m_TextureID;
	int m_CurrentFrame;
	int m_NumberFrames;
	int m_AnimationSpeed;
	int m_CurrentRow;

public:
	TextureProperties();
	TextureProperties(Rect Dimensions, std::string TextureID, int CurrentFrame, 
		int NumberFrames, int AnimationSpeed, int CurrentRow);

	Rect GetDimensions() { return m_Dimensions; }
	int GetCurrentFrame() { return m_CurrentFrame; }
	int GetNumberFrames() { return m_NumberFrames; }
	int GetAnimationSpeed() { return m_AnimationSpeed; }
	int GetCurrentRow() { return m_CurrentRow; }
	std::string GetTextureID() { return m_TextureID; }

	virtual ~TextureProperties();

};

