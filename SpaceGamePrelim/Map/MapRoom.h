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

	void Generate(); // Generates this room

	MapObject* GetCell(int X, int Y);
	MapObject*** GetCellArray() { return m_Cells; }
	int GetHeight() { return m_Height; }
	int GetWidth() { return m_Width; }
	std::string GetRoomType() { return m_RoomType; }

	MapRoom();
	MapRoom(std::string RoomType, int Width, int Height); // Known room
	MapRoom(int Width, int Height);						  // Random room
	~MapRoom();	
};

