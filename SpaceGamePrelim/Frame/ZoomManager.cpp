#include "ZoomManager.h"
#include "InputManager.h"

ZoomManager* ZoomManager::m_Instance = nullptr;

static const int ZoomSpeed = 3;

ZoomManager::ZoomManager() : 
	m_CurrentPixelOffset(0)
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
	m_CurrentPixelOffset = ZoomSpeed * InputManager::Instance()->GetMouseWheelMovement();
}
