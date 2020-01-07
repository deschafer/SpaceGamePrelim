#include "Drawable.h"
#include "..\Scene\Scene.h"

Drawable::Drawable(Rect Dimensions, Scene* ParentScene) :
	m_Dimensions(Dimensions),
	m_Visible(true),
	m_ParentScene(ParentScene),
	Locatable(Vector(Dimensions.TopLeftX(), Dimensions.TopLeftY()))
{
	if (ParentScene)
		ParentScene->AddDrawable(this);
}

Drawable::~Drawable() 
{
	if (m_ParentScene)
		m_ParentScene->RemoveDrawable(this);
}

bool Drawable::Draw(double X, double Y)
{
	return m_Visible;
}

void Drawable::SetParentScene(Scene* ParentScene)
{
	if (m_ParentScene)
	{
		// then we need to remove from the last
		m_ParentScene->RemoveDrawable(this);
	}

	// and then always add to the new scene
	(m_ParentScene = ParentScene)->AddDrawable(this);
}
