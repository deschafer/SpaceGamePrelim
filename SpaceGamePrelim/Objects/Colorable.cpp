#include "Colorable.h"

void Colorable::SetColor(int r, int g, int b, int a)
{
	m_Color.a = a;
	m_Color.r = r;
	m_Color.g = g;
	m_Color.b = b;
}

Colorable::Colorable()
{
}

Colorable::~Colorable()
{
}