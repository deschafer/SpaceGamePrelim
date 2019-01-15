#include "TextureProperties.h"



TextureProperties::TextureProperties()
{
}


TextureProperties::~TextureProperties()
{
}

TextureProperties::TextureProperties(Rect Dimensions, std::string TextureID, int CurrentFrame,
	int NumberFrames, int AnimationSpeed) : m_Dimensions(Dimensions), m_TextureID(TextureID),
	m_CurrentFrame(CurrentFrame), m_NumberFrames(NumberFrames), m_AnimationSpeed(AnimationSpeed)
{
}
