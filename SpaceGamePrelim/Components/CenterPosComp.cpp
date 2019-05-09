#include "CenterPosComp.h"
#include "..\Frame\MainApplication.h"
#include "..\Frame\ZoomManager.h"

CenterPosComp::CenterPosComp()
{
}

CenterPosComp::~CenterPosComp()
{
}

CenterPosComp::CenterPosComp(GameEntity* Owner) :
	Component(Owner)
{
}

void CenterPosComp::Execute()
{

	Rect Dim = m_Owner->GetDimensions();

	m_Owner->SetPosition(Vector(
		MainApplication::Instance()->GetWndWidth() / 2 - round((double)(Dim.Width() / 2)),
		MainApplication::Instance()->GetWndHeight() / 2 - round((double)(Dim.Height() / 2))));


}
