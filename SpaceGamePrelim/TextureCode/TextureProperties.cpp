#include "TextureProperties.h"
#include "TextureManager.h"


TextureProperties::TextureProperties()
{
	m_Color.a = 1;
	m_Color.r = 1;
	m_Color.g = 1;
	m_Color.b = 1;
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
	m_TextIndex = TextureManager::Instance()->GetSourceTextureIndex(TextureID);
}

TextureProperties::TextureProperties(Rect Dimensions, std::string TextureID) :
	m_Dimensions(Dimensions),
	m_TextureID(TextureID),
	m_NumberFrames(1),
	m_AnimationSpeed(0)
{
	m_TextIndex = TextureManager::Instance()->GetSourceTextureIndex(TextureID);
}
