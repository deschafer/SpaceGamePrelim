
#include "Door.h"


#include <iostream>

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
	Door* NewDoor = new Door(*this);

	// reset the copied asset
	NewDoor->Reset();

	return NewDoor;
}

bool Door::OnInteraction(GameEntity* object) 
{
	cout << "Interaction made with door" << endl;

	if (!m_CurrentAction) 
	{
		// then we need to create a new animation
		if (m_Open)
		{
			// then we close the door
			m_CurrentAction = new AnimationChangeAction(this, "DoorClosing", 500, m_Dimensions, m_ParentScene);

			m_CurrentAction->SetInitialImage(m_DoorOpen);
			m_CurrentAction->SetChangeImage(m_DoorClosing);
			m_CurrentAction->SetLastImage(m_DoorClosed);

			// make sure to officially add the action to the drawable class
			AddAction(m_CurrentAction);

			m_Collidable = true;
		}
		else
		{
			// then we open the door
			m_CurrentAction = new AnimationChangeAction(this, "DoorOpening", 500, m_Dimensions, m_ParentScene);

			m_CurrentAction->SetInitialImage(m_DoorClosed);
			m_CurrentAction->SetChangeImage(m_DoorOpening);
			m_CurrentAction->SetLastImage(m_DoorOpen);

			// make sure to officially add the action to the drawable class
			AddAction(m_CurrentAction);

			m_Collidable = false;
		}
	}

	return false;
}