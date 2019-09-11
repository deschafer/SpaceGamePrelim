#pragma once
#include "MapAsset.h"


class TestingAsset : public MapAsset
{


public:
	TestingAsset();
	virtual ~TestingAsset();

	virtual MapAsset* Copy();	// Returns a copy of this object
	virtual void OnInteraction(GameEntity* Entity);
};

