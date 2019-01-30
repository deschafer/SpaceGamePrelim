#pragma once

#include "RoomManager.h"
#include "MapCell.h"

#include <string>

class MapRoom
{
private:

	RoomProperties* m_Properties;

	MapObject*** m_Cells;

	std::string m_RoomType;

	int m_Width;
	int m_Height;

public:

	void Generate(std::string RoomType, int Width, int Height); // Generates this room

	MapObject* GetCell(int X, int Y);
	MapObject*** GetCellArray() { return m_Cells; }


	MapRoom();
	MapRoom(std::string RoomType, int Width, int Height);
	~MapRoom();
};

