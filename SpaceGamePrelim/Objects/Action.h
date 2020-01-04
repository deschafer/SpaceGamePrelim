#pragma once

#include <string>
#include <vector>

#include "..\BasicTypes\BasicTypes.h"
#include "Drawable.h"

class Action : public Drawable
{
private:
	bool m_Start;
	std::string m_Name;
	double m_Duration;
	Rect m_Dimensions;
	Drawable *m_DrawableObject;

	// drawn texture data
	// NOTE: this object draws a single texture every frame
	// it is manipulated by modifying the data of this texture by derived classes.
	std::string m_DrawnRedTextureID;
	int m_DrawnRectTextureIndex;
	int m_DrawnAnimationSpeed;
	int m_DrawnNumberFrames;
	int m_CurrentFrame;
	int m_CurrentRow;

public:

	Action(Drawable* DrawableObject, std::string Name, double Duration, Rect Dimensions);

	std::string GetName() { return m_Name; }


	// this is implementation dependent
	virtual void Start();
	virtual void Stop();

	virtual void Update();

	// drawable method overrides
	virtual bool Draw(double X, double Y) override;
	virtual void AddReducedTexture(std::string RedTextureID, int RedTextIndex, int AnimationSpeed, int NumberFrames) override;
	virtual void ClearTextures() override;
	virtual std::vector<std::string> GetTextures() override;
	virtual std::vector<int> GetTextureIndices() override;

};

