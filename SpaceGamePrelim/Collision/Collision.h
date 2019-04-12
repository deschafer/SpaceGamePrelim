#pragma once

enum class CollisionType { Default, MapWall, Entity, Damage };

#include "..\Map\Map.h"

class Collision
{
protected:

	CollisionType m_Type;
	Collision(CollisionType Type);

public:

	CollisionType GetType() { return m_Type; }
	
	Collision();
	virtual ~Collision();
};

