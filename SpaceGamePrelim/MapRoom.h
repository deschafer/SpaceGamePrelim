#pragma once

#include "RoomManager.h"

class MapRoom
{
private:

	RoomProperties* m_Properties;


	int m_Width;
	int m_Height;

public:

	//void Generate(); // Generates this room

	MapRoom();
	~MapRoom();
};

