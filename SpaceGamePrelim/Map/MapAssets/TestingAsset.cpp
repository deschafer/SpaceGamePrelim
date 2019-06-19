
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
TestingAsset::TestingAsset(TestingAsset* Asset) :
	TestingAsset(*Asset)
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
	TestingAsset* NewAsset = new TestingAsset(this);
	return NewAsset;
}

//
// OnInteraction()
// Overridden function that allows for each object to determine what happens 
// when an entity interacts with this object.
//
void TestingAsset::OnInteraction(GameEntity* Entity)
{
}