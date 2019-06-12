#pragma once


class ZoomManager
{
private:

	int m_CurrentPixelOffset;
	int m_ZoomIntervals;
	double m_CurrentPercent;
	bool m_Change;
	float m_ZoomMin;	// Not used for comparisons
	float m_ZoomMax;	// ..
	double m_Increment;	
	
	static ZoomManager* m_Instance;

	ZoomManager();
	
public:
	
	static ZoomManager* Instance()
	{
		if (!m_Instance)
		{
			return m_Instance = new ZoomManager;
		}
		else return m_Instance;
	}

	void Update();
	bool IsChange() { return m_Change; }
	int GetPixelOffset() { return m_CurrentPixelOffset; }
	double GetScale();
	int GetZoomIntervals() { return m_ZoomIntervals; }
	float GetMin() { return m_ZoomMin; }
	float GetMax() { return m_ZoomMax; }
	double GetIncrement() { return m_Increment; }



	~ZoomManager();
};

