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
	int NewWidth = 0;
	int NewHeight = 0;	
	static Rect CurrDim = m_Owner->GetDestDimensions();

	if (ZoomManager::Instance()->IsChange())
	{
		Offset = ZoomManager::Instance()->GetPixelOffset();
		NewWidth = CurrDim.Width() + Offset;
		NewHeight = CurrDim.Height() + Offset;

		m_Owner->SetDestinationRect(Rect(0, 0, NewWidth, NewHeight));
	}

}