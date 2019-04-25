#include "Rect.h"

Rect::Rect()
{
}

Rect::~Rect()
{
}

void Rect::Enlarge(int Number)
{
	m_Width += Number;
	m_Height += Number;
}
