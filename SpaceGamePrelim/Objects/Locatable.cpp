#include "Locatable.h"



Locatable::Locatable(Vector ScreenPosition) : 
	m_LocatableScreenPosition(ScreenPosition)
{
}

Locatable::Locatable() : 
	m_LocatableScreenPosition(0,0)
{
}


Locatable::~Locatable()
{
}
