#pragma once

#include <string>

#include "..\BasicTypes\BasicTypes.h"

class Action
{
private:
	std::string m_Name;
	double m_Duration;
	Vector m_Position;

	// Drawable m_DrawableObject;


public:

	// Action(Drawable DrawableObject, std::string Name, double Duration, Vector Position);


	std::string GetName() { return m_Name; }

	// this is implementation dependent
	virtual void Start() = 0;
	virtual void Stop() = 0;
};

