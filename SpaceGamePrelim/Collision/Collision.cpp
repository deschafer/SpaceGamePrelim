#include "Collision.h"



Collision::Collision()
{
}

Collision::Collision(CollisionType Type) :
	m_Type(Type)
{
}

Collision::Collision(CollisionType Type, CollisionDir Direction) : 
	m_Type(Type),
	m_Direction(Direction)
{
}


Collision::~Collision()
{
}
