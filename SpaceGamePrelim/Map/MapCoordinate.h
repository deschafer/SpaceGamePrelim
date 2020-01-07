#pragma once

class MapCoordinate
{
private:
	int m_PositionX;	// Corrdinate, integer position
	int m_PositionY;	// Corrdinate, integer position

public:
	MapCoordinate(int X, int Y) : m_PositionX(X), m_PositionY(Y)
	{}
	MapCoordinate() : m_PositionX(0), m_PositionY(0) {}
	~MapCoordinate() {}

	bool operator ==(MapCoordinate Other)
	{
		if (Other.GetPositionX() != m_PositionX)
			return false;
		if (Other.GetPositionY() != m_PositionY)
			return false;
		return true;
	}

	bool operator !=(MapCoordinate Other)
	{
		return !(*this == Other);
	}


	MapCoordinate operator -(MapCoordinate Other)
	{
		return MapCoordinate(
			GetPositionX() - Other.GetPositionX(),
			GetPositionY() - Other.GetPositionY());
	}

	int GetPositionX() { return m_PositionX; }
	int GetPositionY() { return m_PositionY; }
};