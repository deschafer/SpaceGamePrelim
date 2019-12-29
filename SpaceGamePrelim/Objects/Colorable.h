#pragma once
#include <SDL.h>

class Colorable
{
protected:

	SDL_Color m_Color;

public:

	void SetColor(int r, int g, int b, int a);

	Colorable();
	virtual ~Colorable();
};

