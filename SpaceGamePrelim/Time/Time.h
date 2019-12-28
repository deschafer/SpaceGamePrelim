
#include <SDL.h>

class Time
{
public:
	~Time() {}

	static Time* Instance()
	{
		if (!m_Instance)
		{
			m_Instance = new Time;
		}
		return m_Instance;
	}

	void Update();
	// Note: Returns in seconds
	double GetDeltaTime() { return m_DeltaTime; }

private:

	Time() :
		m_TimeLast(0),
		m_TimeNow((float)SDL_GetPerformanceCounter())
	{}

	static Time* m_Instance;
	double m_DeltaTime;
	double m_TimeNow;
	double m_TimeLast;
};