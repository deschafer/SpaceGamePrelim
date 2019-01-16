#include "TextureProperties.h"



TextureProperties::TextureProperties()
{
}


TextureProperties::~TextureProperties()
{
}

TextureProperties::TextureProperties(Rect Dimensions, std::string TextureID, int CurrentFrame,
	int NumberFrames, int AnimationSpeed, int CurrentRow) : m_Dimensions(Dimensions), m_TextureID(TextureID),
	m_CurrentFrame(CurrentFrame), m_NumberFrames(NumberFrames), m_AnimationSpeed(AnimationSpeed), m_CurrentRow(CurrentRow)
{
}
