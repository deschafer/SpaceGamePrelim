#include "Time.h"

#include <SDL.h>

Time* Time::m_Instance = nullptr;

void Time::Update()
{
	m_TimeLast = m_TimeNow;
	m_TimeNow = (double)SDL_GetPerformanceCounter();

	m_DeltaTime = 0.001 * (double)((m_TimeNow - m_TimeLast) * 1000.0f / (double)SDL_GetPerformanceFrequency());
}
