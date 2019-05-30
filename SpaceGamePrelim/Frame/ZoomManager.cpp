#include "ZoomManager.h"
#include "InputManager.h"

#include <iostream>

ZoomManager* ZoomManager::m_Instance = nullptr;

static const int ZoomSpeed = 3;
static const double PercentChange = .05;
static const int ZoomOffsetMax = 20;
static const double ZoomPercentMax = 1.500000001;
static const double ZoomPercentMin = 0.499999999;

ZoomManager::ZoomManager() : 
	m_CurrentPixelOffset(0),
	m_CurrentPercent(1.0),
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
	double OffsetPercentChange = PercentChange * InputManager::Instance()->GetMouseWheelMovement();

	if (abs(m_CurrentPixelOffset + OffsetChange) < ZoomOffsetMax)
	{
		m_CurrentPixelOffset += OffsetChange;
	}
	else
		OffsetChange = 0;

	// Handle floating point offsets
	if ((m_CurrentPercent + OffsetPercentChange) < ZoomPercentMax && 
		(m_CurrentPercent + OffsetPercentChange) >= ZoomPercentMin)
	{
		m_CurrentPercent += OffsetPercentChange;
	}
	else OffsetPercentChange = 0.0;
	

	if (OffsetChange || OffsetPercentChange)
	{
		m_Change = true;
	}
	else
	{
		m_Change = false;
	}


	//std::cout << "Current Zoom Offsets " << m_CurrentPercent << std::endl;
}

//
//
//
//
double ZoomManager::GetScale()
{
	return m_CurrentPercent;
}