#pragma once

#include "MapObject.h"

#include <string>

class Map
{

	MapObject*** m_Cells;	// Cells of this map

	int m_Width;	// Width of this map 
	int m_Height;	// Height of this map
	std::string m_MapType;	// Map type for different themes or styles

public:

	void Generate();

	Map();
	Map(int Width, int Height, std::string MapType);
	~Map();
};

