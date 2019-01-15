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


public:
	TextureProperties();
	TextureProperties(Rect Dimensions, std::string TextureID, int CurrentFrame, 
		int NumberFrames, int AnimationSpeed);

	virtual ~TextureProperties();

};

