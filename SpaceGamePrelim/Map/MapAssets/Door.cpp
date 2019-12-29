
#include "Door.h"

#include <iostream>

using namespace std;

Door::Door()
{
}

Door::~Door()
{
}

MapAsset* Door::Copy() 
{
	// create the new object
	Door* NewDoor = new Door();

	// copy the data from this object
	*NewDoor = *this;

	// reset the copied asset
	NewDoor->Reset();

	return NewDoor;
}

bool Door::OnInteraction(GameEntity* object) 
{

	cout << "Interaction made with door" << endl;

	return false;
}