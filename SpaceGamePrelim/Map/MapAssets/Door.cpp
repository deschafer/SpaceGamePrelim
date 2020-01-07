
#include "Door.h"


#include <iostream>

using namespace std;

Door::Door(Rect Dimensions, Scene* ParentScene) : 
	MapAsset(Dimensions, ParentScene)
{
	Vector ScreenPosition = GetLocatableScreenPosition();

	//m_DoorOpen = new SimpleEntity(Rect(ScreenPosition.getX(), ScreenPosition.getY(), m_DestRect.Width(), m_DestRect.Height()), )

}

Door::~Door()
{
}

MapAsset* Door::Copy() 
{
	// create the new object
	Door* NewDoor = new Door(GetDimensions(), GetParentScene());

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