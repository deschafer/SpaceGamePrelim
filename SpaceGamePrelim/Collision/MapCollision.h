#pragma once
#include "Collision.h"

enum class MapCollisionDir { Horiz, Verti, Diagonal };


class MapCollision : public Collision
{
private: 
	MapCollisionDir m_Direction;

public:

	MapCollisionDir GetDir() { return m_Direction; }

	MapCollision();
	MapCollision(CollisionType Type, MapCollisionDir Direction);
	virtual ~MapCollision();
};

