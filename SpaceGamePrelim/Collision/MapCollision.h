#pragma once
#include "Collision.h"

enum class MapCollisionDir { Horiz, Verti, Diagonal, CannotTell };


class MapCollision : public Collision
{
protected: 
	MapCollisionDir m_Direction;
	int m_DistanceBeforeCollision;

public:

	MapCollisionDir GetDir() { return m_Direction; }
	int GetDistance() { return m_DistanceBeforeCollision; }

	MapCollision();
	MapCollision(CollisionType Type, MapCollisionDir Direction);
	MapCollision(CollisionType Type, MapCollisionDir Direction, int DistanceBeforeColl);

	virtual ~MapCollision();
};

