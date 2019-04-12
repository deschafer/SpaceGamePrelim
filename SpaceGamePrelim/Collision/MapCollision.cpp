#include "MapCollision.h"


MapCollision::MapCollision()
{
}

MapCollision::~MapCollision()
{
}

MapCollision::MapCollision(CollisionType Type, MapCollisionDir Direction) :
	Collision(Type),
	m_Direction(Direction)
{
}

