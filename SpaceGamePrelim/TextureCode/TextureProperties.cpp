#include "TextureProperties.h"
#include "TextureManager.h"


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
