#include "ZoomManager.h"
#include "InputManager.h"

#include <iostream>

ZoomManager* ZoomManager::m_Instance = nullptr;

static const int ZoomSpeed = 3;
static const int ZoomOffsetMax = 20;

ZoomManager::ZoomManager() : 
	m_CurrentPixelOffset(0),
	m_Change(false)
{
}

ZoomManager::~ZoomManager()
{
}

//
//
//
//
void ZoomManager::Update()
{
	int OffsetChange = ZoomSpeed * InputManager::Instance()->GetMouseWheelMovement();
	if (abs(m_CurrentPixelOffset + OffsetChange) < ZoomOffsetMax)
		m_CurrentPixelOffset += OffsetChange;
	else
		OffsetChange = 0;

	if (OffsetChange)
	{
		m_Change = true;
	}
	else
	{
		m_Change = false;
	}

	//std::cout << "Current Zoom Offsets " << m_CurrentPixelOffset << std::endl;
}
