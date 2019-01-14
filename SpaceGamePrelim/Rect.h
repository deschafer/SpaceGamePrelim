#pragma once


class Rect
{
private:

	int m_TopLeft;
	int m_TopRight;
	int m_Width;
	int m_Height;

public:
	Rect(int TopLeft, int TopRight, int Width, int Height)
	{
		m_TopLeft = TopLeft;
		m_TopRight = TopRight;
		m_Width = Width;
		m_Height = Height;
	}
	Rect();
	~Rect();
};

