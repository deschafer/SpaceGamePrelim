#pragma once

#include "..\BasicTypes\BasicTypes.h"
#include "Collision.h"

#include <vector>

class GameEntity;

// singleton class structure
class CollisionManager
{
private:

	static CollisionManager* m_Instance;


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

	std::vector<Collision*> CheckPosition(Vector PositionAfter, Vector PositionBefore, GameEntity* Object);

	~CollisionManager();
};

