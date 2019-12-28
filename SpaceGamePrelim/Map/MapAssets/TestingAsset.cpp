
#include "TestingAsset.h"

//
//
//
//
TestingAsset::TestingAsset()
{
}

//
//
//
//
TestingAsset::~TestingAsset()
{
}

//
// Copy()
// Returns a copy of this object
//
MapAsset* TestingAsset::Copy()
{
	TestingAsset* NewAsset = new TestingAsset;
	*NewAsset = *this;
	NewAsset->Reset();
	return NewAsset;
}

//
// OnInteraction()
// Overridden function that allows for each object to determine what happens 
// when an entity interacts with this object.
//
bool TestingAsset::OnInteraction(GameEntity* Entity)
{
	return false;
}