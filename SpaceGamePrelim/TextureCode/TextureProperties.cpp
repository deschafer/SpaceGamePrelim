#include "TextureProperties.h"



TextureProperties::TextureProperties()
{
}


TextureProperties::~TextureProperties()
{
}

TextureProperties::TextureProperties(Rect Dimensions, std::string TextureID,
	int NumberFrames, int AnimationSpeed) : 
	m_Dimensions(Dimensions), 
	m_TextureID(TextureID),
	m_NumberFrames(NumberFrames), 
	m_AnimationSpeed(AnimationSpeed)
{
}

TextureProperties::TextureProperties(Rect Dimensions, std::string TextureID) :
	m_Dimensions(Dimensions),
	m_TextureID(TextureID),
	m_NumberFrames(1),
	m_AnimationSpeed(0)
{
}