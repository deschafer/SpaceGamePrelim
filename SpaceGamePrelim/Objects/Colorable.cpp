#include "Colorable.h"

#include <SDL.h>

void Colorable::SetColor(int r, int g, int b, int a)
{
	m_Color.a = a;
	m_Color.r = r;
	m_Color.g = g;
	m_Color.b = b;
}

Colorable::Colorable()
{
	m_Color.a = (Uint8)255;
	m_Color.r = (Uint8)255;
	m_Color.g = (Uint8)255;
	m_Color.b = (Uint8)255;
}

Colorable::~Colorable()
{
}