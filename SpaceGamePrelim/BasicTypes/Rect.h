#pragma once

#include "Vector.h"

class Rect
{
private:

	int m_TopLeftX;
	int m_TopLeftY;
	int m_Width;
	int m_Height;

public:
	Rect(int TopLeft, int TopRight, int NewWidth, int NewHeight)
	{
		m_TopLeftX = TopLeft;
		m_TopLeftY = TopRight;
		m_Width = NewWidth;
		m_Height = NewHeight;
	}
	Rect(Vector Position, int NewWidth, int NewHeight)
	{
		m_TopLeftX = (int)Position.getX();
		m_TopLeftY = (int)Position.getY();
		m_Width = NewWidth;
		m_Height = NewHeight;
	}

	int TopLeftX() { return m_TopLeftX; }
	int TopLeftY() { return m_TopLeftY; }
	int Width() { return m_Width; }
	int Height() { return m_Height; }

	void SetOffset(int offsetX, int offsetY) { m_TopLeftX += offsetX; m_TopLeftY += offsetY; }
	void Enlarge(int Number);
	void SetWidth(int Width) { m_Width = Width; }
	void SetHeight(int Height) { m_Height = Height; }

	bool Empty();

	Rect() : Rect(0,0,0,0) {}
	~Rect();
};

