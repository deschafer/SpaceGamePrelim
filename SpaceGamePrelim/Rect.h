#pragma once


class Rect
{
private:

	int m_TopLeftX;
	int m_TopLeftY;
	int m_Width;
	int m_Height;

public:
	Rect(int TopLeft, int TopRight, int Width, int Height)
	{
		m_TopLeftX = TopLeft;
		m_TopLeftY = TopRight;
		m_Width = Width;
		m_Height = Height;
	}

	int TopLeftX() { return m_TopLeftX; }
	int TopLeftY() { return m_TopLeftY; }
	int Width() { return m_Width; }
	int Height() { return m_Height; }

	Rect();
	~Rect();
};
