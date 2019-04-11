#pragma once

#include "..\BasicTypes\BasicTypes.h"
#include "Collision.h"

#include <vector>

// singleton class structure
class CollisionManager
{
private:

	static CollisionManager* m_Instance;

	void CheckMapForCollisions(std::vector<Collision*> &Collisions, Vector PositionAfter, Vector PositionBefore);

	CollisionManager();
public:

	static CollisionManager* Instance()
	{
		if (!m_Instance)
		{
			return m_Instance = new CollisionManager();
		}
		else return m_Instance;
	}

	std::vector<Collision*> CheckPosition(Vector PositionAfter, Vector PositionBefore);

	~CollisionManager();
};

