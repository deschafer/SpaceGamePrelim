#pragma once

#include <vector>

#include "Colorable.h"
#include "Locatable.h"
#include "../BasicTypes/BasicTypes.h"
class Scene;

//
// Purpose of this class is to provide an abstract interface to all drawn objects in the game.
// It also serves to provide a standard implementation for drawing.
//
// It only provides an interface to drawn elements, and has some minor modifications on those elements
//

class Drawable : 
	private Colorable,
	public Locatable
{
protected:

	// Texture properties
	bool m_Visible;
	Rect m_Dimensions;	// dimensions in the game space, not the source texture dimensions
	Scene* m_ParentScene;

public:

	Drawable(Rect Dimensions, Scene* ParentScene);
	Drawable() = delete;
	virtual ~Drawable();

	// primary draw function
	virtual bool Draw() { return Draw(0, 0); }
	virtual bool Draw(double X, double Y);
	virtual void Update() = 0;
	// these methods offer a standardized way to get/add textures, but it is not the only way to do so
	virtual void AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames) = 0;
	virtual std::vector<std::string> GetTextures() = 0;
	virtual std::vector<int> GetTextureIndices() = 0;
	virtual std::vector<int> GetAnimationSpeeds() = 0;
	virtual std::vector<int> GetNumberFrames() = 0;
	virtual void ClearTextures() = 0;

	// locatable overrides
	virtual Vector GetLocatableScreenPosition() override { return m_LocatableScreenPosition = Vector((float)m_Dimensions.TopLeftX(), (float)m_Dimensions.TopLeftY()); }
	virtual void SetLocatableScreenPosition(Vector Position) override { m_Dimensions = Rect(m_LocatableScreenPosition = Position, m_Dimensions.Width(), m_Dimensions.Height()); }

	// all inheriting classes need to work with these variables
	void SetVisible(bool Visible) { m_Visible = Visible; }
	Rect GetDimensions() { return m_Dimensions; }
	Rect SetDimensions(Rect Dim) { m_Dimensions = Dim; m_LocatableScreenPosition = Vector((float)Dim.TopLeftX(), (float)Dim.TopLeftY()); }
	void SetParentScene(Scene* ParentScene);
	Scene* GetParentScene() { return m_ParentScene; }

	// same with color, classes need to implement this
	SDL_Color GetColor() { return m_Color; }
	void SetColor(int r, int g, int b, int a) { m_Color.a = a; m_Color.r = r; m_Color.g = g; m_Color.b = b; }
};