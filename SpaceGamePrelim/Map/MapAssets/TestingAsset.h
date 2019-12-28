#pragma once
#include "MapAsset.h"


class TestingAsset : public MapAsset
{
public:
	TestingAsset();
	virtual ~TestingAsset();

	virtual MapAsset* Copy();	// Returns a copy of this object
	virtual bool OnInteraction(GameEntity* Entity);
};

