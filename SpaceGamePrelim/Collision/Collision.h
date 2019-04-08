#pragma once

enum class CollisionType { Default, MapWall, Entity, Damage };

class Collision
{
private:

	// Damage component
	// Any other affecting components

	CollisionType m_Type;

	// Should also contain objects within this collision

public:

	CollisionType GetType() { return m_Type; }

	Collision();
	Collision(CollisionType Type);
	~Collision();
};

