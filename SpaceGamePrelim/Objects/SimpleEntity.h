#pragma once

#include "Drawable.h"

#include <vector>
#include <string>

// NOTE: only works with single row sprite sheets

class SimpleEntity : public Drawable
{
private:

	// single set of textures drawn on top of each other
	// can be added to and removed from
	std::vector<std::string> m_TextureSet;
	std::vector<int> m_CurrentFrameSet;
	std::vector<int> m_TextureIndicesSet;
	std::vector<int> m_AnimationSpeedSet;
	std::vector<int> m_NumberFramesSet;

public:
	void Update();
	
	virtual bool Draw(double X, double Y) override;
	virtual void AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames) override;
	virtual std::vector<std::string> GetTextures() override { return m_TextureSet; }
	virtual void ClearTextures() override;
	virtual std::vector<int> GetTextureIndices() override { return m_TextureIndicesSet; }
};

