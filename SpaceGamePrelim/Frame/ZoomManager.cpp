#include "ZoomManager.h"
#include "InputManager.h"

#include <iostream>

ZoomManager* ZoomManager::m_Instance = nullptr;

static const int ZoomSpeed = 3;
static const double PercentChange =  0.05000000000000000000;
static const int ZoomOffsetMax = 20;
static const double ZoomPercentMax = 1.50000000000000001;
static const double ZoomPercentMin = 0.50000000000000000;

ZoomManager::ZoomManager() :
	m_CurrentPixelOffset(0),
	m_CurrentPercent(1.0),	// Needs to be exactly halfway between max and min
	m_ZoomIntervals(round((ZoomPercentMax - ZoomPercentMin) / PercentChange)),
	m_Change(false),
	m_ZoomMax(ZoomPercentMax),
	m_ZoomMin(ZoomPercentMin),
	m_Increment(PercentChange)
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
		(m_CurrentPercent + OffsetPercentChange) > ZoomPercentMin)
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