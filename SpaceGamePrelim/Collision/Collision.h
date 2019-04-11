#pragma once

enum class CollisionType { Default, MapWall, Entity, Damage };
enum class CollisionDir { Horiz, Verti, Diagonal};

#include "..\Map\Map.h"

class Collision
{
private:

	// Damage component
	// Any other affecting components

	CollisionType m_Type;
	CollisionDir m_Direction; // Direction between collisions

	// Should also contain objects within this collision


public:

	CollisionType GetType() { return m_Type; }
	CollisionDir GetDir() { return m_Direction; }

	Collision();
	Collision(CollisionType Type);
	Collision(CollisionType Type, CollisionDir Direction);

	~Collision();
};

