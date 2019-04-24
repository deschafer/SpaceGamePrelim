#include "ZoomManager.h"
#include "InputManager.h"

ZoomManager* ZoomManager::m_Instance = nullptr;

ZoomManager::ZoomManager() : 
	m_CurrentPixelOffset(0)
{
}

ZoomManager::~ZoomManager()
{
}

void ZoomManager::Update()
{
	m_CurrentPixelOffset += InputManager::Instance()->GetMouseWheelMovement();
}
