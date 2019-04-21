#include "CenterPosComp.h"
#include "..\Frame\MainApplication.h"

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
	m_Owner->SetPosition(Vector(
		MainApplication::Instance()->GetWndWidth() / 2,
		MainApplication::Instance()->GetWndHeight() / 2));
}
