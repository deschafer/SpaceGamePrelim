#include "Drawable.h"

Drawable::Drawable(Rect Dimensions) : 
	m_Dimensions(Dimensions),
	m_Visible(true)
{
}

bool Drawable::Draw(double X, double Y)
{
	if (m_Visible) {
		return false;
	}

	return false;
}
