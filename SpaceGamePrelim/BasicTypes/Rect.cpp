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

bool Rect::Empty()
{
	if (m_Height || m_TopLeftX || m_TopLeftY || m_Width) return false;
	else return true;
}
