#include "SimpleEntity.h"
#include "../Frame/MainApplication.h"

void SimpleEntity::Update()
{
	// for each of the textures assoc. with this object
	for (size_t i = 0; i < m_TextureSet.size(); i++)
	{
		m_CurrentFrameSet[i] = int(((SDL_GetTicks() / m_AnimationSpeedSet[i]) % m_NumberFramesSet[i]));
	}
}

bool SimpleEntity::Draw(double X, double Y)
{
	// only continue if it's visible and there exists textures to draw
	if (m_Visible && m_TextureIndicesSet.size())
	{
		SDL_Renderer* Temp = MainApplication::Instance()->GetRenderer();

		// for each of the textures assoc. with this object, it gets drawn
		for (size_t i = 0; i < m_TextureSet.size(); i++)
		{
			TextureManager::Instance()->DrawCurrentFrame(
				m_Dimensions.TopLeftX(),
				m_Dimensions.TopLeftX(),
				m_TextureIndicesSet[i],
				SDL_FLIP_NONE,
				MainApplication::Instance()->GetRenderer(),
				m_Dimensions,
				GetColor(),
				1,	// single row sprite sheet
				m_CurrentFrameSet[i]);
		}
	}
	return m_Visible;
}

void SimpleEntity::AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames)
{
	m_TextureSet.push_back(RedTextureID);
	m_TextureIndicesSet.push_back(RedTextureIndex);
	m_NumberFramesSet.push_back(NumberFrames);
	m_AnimationSpeedSet.push_back(AnimationSpeed);
	m_CurrentFrameSet.push_back(0);
}

void SimpleEntity::ClearTextures()
{
	m_TextureSet.clear();
	m_TextureIndicesSet.clear();
	m_NumberFramesSet.clear();
	m_AnimationSpeedSet.clear();
	m_CurrentFrameSet.clear();
}

std::vector<int> SimpleEntity::GetAnimationSpeeds()
{
	return m_AnimationSpeedSet;
}

std::vector<int> SimpleEntity::GetNumberFrames()
{
	return m_NumberFramesSet;
}
