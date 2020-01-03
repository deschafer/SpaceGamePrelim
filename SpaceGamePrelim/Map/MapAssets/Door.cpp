
#include "Door.h"

#include <iostream>

using namespace std;

Door::Door()
{
	m_Collidable = false;
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

	m_Color.r = 255;
	m_Color.g = 0;
	m_Color.b = 0;
	m_Color.a = 255;

	return false;
}