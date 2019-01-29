#pragma once

#include "RoomManager.h"
#include "MapObject.h"

#include <string>

class MapRoom
{
private:

	RoomProperties* m_Properties;

	MapObject*** m_Cells;

	int m_Width;
	int m_Height;

public:

	void Generate(std::string RoomType, int Width, int Height); // Generates this room
	void Init();


	MapRoom();
	MapRoom(std::string RoomType, int Width, int Height);
	~MapRoom();
};

