
#include "Door.h"

#include <iostream>

const std::string Door::m_DoorOpenStr = "Door_Open";
const std::string Door::m_DoorClosedStr = "Door";
const std::string Door::m_DoorClosingStr = "Door_Closing";
const std::string Door::m_DoorOpeningStr = "Door_Opening";

using namespace std;

Door::Door(Rect Dimensions, Scene* ParentScene) : 
	MapAsset(Dimensions, ParentScene)
{
	Vector ScreenPosition = GetLocatableScreenPosition();

	m_DoorOpen = new SimpleEntity(Rect(ScreenPosition.getX(), ScreenPosition.getY(), m_DestRect.Width(), m_DestRect.Height()), ParentScene);
	m_DoorOpen->AddReducedTexture(m_DoorOpenStr, TextureManager::Instance()->GetRedTextureIndex(m_DoorOpenStr), 1, 1);
	m_DoorOpen->SetVisible(false);
	m_DoorClosed = new SimpleEntity(Rect(ScreenPosition.getX(), ScreenPosition.getY(), m_DestRect.Width(), m_DestRect.Height()), ParentScene);
	m_DoorClosed->AddReducedTexture(m_DoorClosedStr, TextureManager::Instance()->GetRedTextureIndex(m_DoorClosedStr), 1, 1);
	m_DoorClosed->SetVisible(false);
	m_DoorClosing = new SimpleEntity(Rect(ScreenPosition.getX(), ScreenPosition.getY(), m_DestRect.Width(), m_DestRect.Height()), ParentScene);
	m_DoorClosing->AddReducedTexture(m_DoorClosingStr, TextureManager::Instance()->GetRedTextureIndex(m_DoorClosingStr), 1, 1);
	m_DoorClosing->SetVisible(false);
	m_DoorOpening = new SimpleEntity(Rect(ScreenPosition.getX(), ScreenPosition.getY(), m_DestRect.Width(), m_DestRect.Height()), ParentScene);
	m_DoorOpening->AddReducedTexture(m_DoorOpeningStr, TextureManager::Instance()->GetRedTextureIndex(m_DoorOpeningStr), 1, 1);
	m_DoorOpening->SetVisible(false);

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