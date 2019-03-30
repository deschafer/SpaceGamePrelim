#pragma once

class MapCoordinate
{
private:
	int m_PositionX;	// Corrdinate, integer position
	int m_PositionY;	// Corrdinate, integer position

public:
	MapCoordinate(int X, int Y) : m_PositionX(X), m_PositionY(Y)
	{}
	MapCoordinate() {}
	~MapCoordinate() {}

	bool operator ==(MapCoordinate Other)
	{
		if (Other.GetPositionX() != m_PositionX)
			return false;
		if (Other.GetPositionY() != m_PositionY)
			return false;
		return true;
	}

	int GetPositionX() { return m_PositionX; }
	int GetPositionY() { return m_PositionY; }
};