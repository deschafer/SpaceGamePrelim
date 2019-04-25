#include "ZoomComp.h"
#include "..\Frame\ZoomManager.h"


ZoomComp::ZoomComp()
{
}

ZoomComp::ZoomComp(GameEntity* Owner) : 
	Component(Owner)
{
}

ZoomComp::~ZoomComp()
{
}

//
// Execute()
// Check the global zoom counter and change the dest rect of the 
// owning object.
//
void ZoomComp::Execute()
{
	int Offset = 0;

	if (Offset = ZoomManager::Instance()->GetPixelOffset())
	{
		m_Owner->EnglargeDestination(Offset);
	}

}