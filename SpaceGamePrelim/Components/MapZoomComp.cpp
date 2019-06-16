#include "MapZoomComp.h"
#include "../Collision/CollisionManager.h"
#include "../BasicTypes/EntityDirection.h"
#include "../Frame/ZoomManager.h"
#include "../Map/MapManager.h"

#include <iostream>

using namespace std;

MapZoomComp::MapZoomComp()
{
}

MapZoomComp::~MapZoomComp()
{
}

MapZoomComp::MapZoomComp(GameEntity* Owner) :
	Component(Owner)
{

}

//
// Execute()
//
//
void MapZoomComp::Execute()
{

	// If there was a zoom, then we need to check if the corners have migrated into a wall
	if (ZoomManager::Instance()->IsChange())
	{

		Rect Dimensions = m_Owner->GetDestDimensions();
		Vector ObjectScreenPosition = m_Owner->GetPosition();
		Vector TestingPoint;
		Collision* TestedCollision;
		bool TopRightCollision = false;
		bool BottomRightCollision = false;
		bool BottomLeftCollision = false;
		bool TopLeftCollision = false;
		bool HorizonCollision = false;
		bool VertiCollision = false;

		// Getting the top right corner
		TestingPoint = Vector(
			ObjectScreenPosition.getX() + Dimensions.Width() - 1,
			ObjectScreenPosition.getY());
		// Then we test this point for a collision
		TestedCollision = MapManager::Instance()->CheckCollidingPoint(TestingPoint);
		if (TestedCollision)
		{
			// Now we mark that a top right collision was found
			TopRightCollision = true;
		}

		// Getting the bottom right corner
		TestingPoint = Vector(
			ObjectScreenPosition.getX() + Dimensions.Width() - 1,
			ObjectScreenPosition.getY() + Dimensions.Height() - 1);
		// Then we test this point for a collision
		TestedCollision = MapManager::Instance()->CheckCollidingPoint(TestingPoint);
		if (TestedCollision)
		{
			// Now we mark that a top right collision was found
			BottomRightCollision = true;
		}

		// Getting the bottom right corner
		TestingPoint = Vector(
			ObjectScreenPosition.getX(),
			ObjectScreenPosition.getY() + Dimensions.Height() - 1);
		// Then we test this point for a collision
		TestedCollision = MapManager::Instance()->CheckCollidingPoint(TestingPoint);
		if (TestedCollision)
		{
			// Now we mark that a top right collision was found
			BottomLeftCollision = true;
		}

		// Getting the bottom right corner
		TestingPoint = Vector(
			ObjectScreenPosition.getX(),
			ObjectScreenPosition.getY());
		// Then we test this point for a collision
		TestedCollision = MapManager::Instance()->CheckCollidingPoint(TestingPoint);
		if (TestedCollision)
		{
			// Now we mark that a top right collision was found
			TopLeftCollision = true;
		}

		// Checking entire sides for a zoom intrusion
		if (BottomRightCollision && TopRightCollision)
		{
			HorizonCollision = true;
		}
		if (BottomLeftCollision && BottomRightCollision)
		{
			VertiCollision = true;
		}
		if (BottomLeftCollision && TopLeftCollision)
		{
			HorizonCollision = true;
		}
		if (TopLeftCollision && TopRightCollision)
		{
			VertiCollision = true;
		}

		// Checking single corners for a zoom intrusion
		if (BottomLeftCollision && !BottomRightCollision && !TopLeftCollision)
		{
			// Has to be the lower left corner
			VertiCollision = true;
			HorizonCollision = true;
		}
		if (BottomRightCollision && !BottomLeftCollision && !TopRightCollision)
		{
			// Has to be the lower right corner
			VertiCollision = true;
			HorizonCollision = true;
		}
		if (TopRightCollision && !BottomRightCollision && !TopLeftCollision)
		{
			// Has to be the upper right corner
			VertiCollision = true;
			HorizonCollision = true;
		}
		if (TopLeftCollision && !BottomLeftCollision && !TopRightCollision)
		{
			// Has to be the upper left corner
			VertiCollision = true;
			HorizonCollision = true;
		}
		
		if (HorizonCollision)
		{
			//cout << "Horizontal zoom collision" << endl;

			// Move to the side without a collision
			// Check the opposite corner, and check if it is good
				// If so, move in that direction

			// Remeber that the bool flags used here indicate a collision was present,
			// So they show a side is not suitable for a correction.

			if (TopLeftCollision || BottomLeftCollision)
			{
				// Move right
				cout << "We need to move right " << endl;

				MapManager::Instance()->CorrectZoom(Vector(5, 0));
			}
			else if (TopRightCollision || BottomRightCollision)
			{
				// Move left
				cout << "We need to move left " << endl;

				MapManager::Instance()->CorrectZoom(Vector(-5,0));
			}
			else
			{
				cout << "Cannot resolve this zoom collision" << endl;
			}

			// Eliminate any vertical velocity
			Vector Velocity = m_Owner->GetVelocity();
			m_Owner->SetVelocity(Vector(0, Velocity.getY()));
		}
		if (VertiCollision)
		{
			cout << "Vertical zoom collision" << endl;

			if (TopLeftCollision || TopRightCollision)
			{
				// Move south
				MapManager::Instance()->CorrectZoom(Vector(5, 0));
			}
			else if (BottomRightCollision || BottomLeftCollision)
			{
				// Move north
				MapManager::Instance()->CorrectZoom(Vector(0, -5));
			}
			else
			{
				cout << "Cannot resolve this zoom collision" << endl;
			}

			// Eliminate any vertical velocity
			Vector Velocity = m_Owner->GetVelocity();
			m_Owner->SetVelocity(Vector(Velocity.getX(), 0));
		}
	}
}