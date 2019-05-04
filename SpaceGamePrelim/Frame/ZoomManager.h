#pragma once


class ZoomManager
{
private:

	int m_CurrentPixelOffset;
	bool m_Change;
	
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
	int GetPixelOffset() { return m_CurrentPixelOffset; }
	bool IsChange() { return m_Change; }


	~ZoomManager();
};

