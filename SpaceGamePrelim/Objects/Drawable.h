#pragma once

#include <vector>

#include "Colorable.h"
#include "../BasicTypes/BasicTypes.h"

//
// Purpose of this class is to provide an abstract interface to all drawn objects in the game.
// It also serves to provide a standard implementation for drawing.
//
// It only provides an interface to drawn elements, and has some minor modifications on those elements
//

class Drawable : private Colorable
{
protected:

	// Texture properties
	bool m_Visible;
	Rect m_Dimensions;	// dimensions in the game space, not the source texture dimensions

public:

	Drawable(Rect Dimensions);
	Drawable() = delete;

	// primary draw function
	virtual bool Draw(double X, double Y);
	// these methods offer a standardized way to get/add textures, but it is not the only way to do so
	virtual void AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames) = 0;
	virtual std::vector<std::string> GetTextures() = 0;
	virtual std::vector<int> GetTextureIndices() = 0;
	virtual void ClearTextures() = 0;

	// all inheriting classes need to work with these variables
	void SetVisible(bool Visible) { m_Visible = Visible; }
	Rect GetDimensions() { return m_Dimensions; }
	Rect SetDimensions(Rect Dim) { m_Dimensions = Dim; }

	// same with color, classes need to implement this
	SDL_Color GetColor() { return m_Color; }
	void SetColor(int r, int g, int b, int a) { m_Color.a = a; m_Color.r = r; m_Color.g = g; m_Color.b = b; }
};