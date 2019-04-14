#include "MapCollision.h"


MapCollision::MapCollision()
{
}

MapCollision::~MapCollision()
{
}

MapCollision::MapCollision(CollisionType Type, MapCollisionDir Direction) :
	Collision(Type),
	m_Direction(Direction),
	m_DistanceBeforeCollision(0)
{
}

MapCollision::MapCollision(CollisionType Type, MapCollisionDir Direction, int DistanceBeforeCollision) :
	Collision(Type),
	m_Direction(Direction),
	m_DistanceBeforeCollision(DistanceBeforeCollision)
{
}

