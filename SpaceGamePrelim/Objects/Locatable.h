#pragma once

#include "..\BasicTypes\BasicTypes.h"


class Locatable
{
protected:

	Vector m_LocatableScreenPosition;

public:

	virtual Vector GetLocatableScreenPosition () { return m_LocatableScreenPosition; }
	virtual void SetLocatableScreenPosition (Vector Position) { m_LocatableScreenPosition = Position; }

	Locatable(Vector ScreenPosition);
	Locatable() = delete;
	virtual ~Locatable();
};

