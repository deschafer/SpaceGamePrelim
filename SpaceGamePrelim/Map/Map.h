#pragma once

#include "MapObject.h"
#include "MapRoom.h"

#include <string>
#include <Windows.h>
#include <map>

class Map
{
private:

	int m_Width;						// Width of this map 
	int m_Height;						// Height of this map

	std::string m_MapType;				// Map type for different themes or styles
	MapCoordinate m_MapCoordinates;		// Coordinates of the map in comparison to other maps	

	MapObject*** m_Cells;						// Cells of this map

	// Parallel room vectors
	std::vector<std::vector<MapRoom*>> m_Rooms;		// Holds the 2D array of rooms
	std::vector<std::vector<int>> m_ColumnOffsetsX;	// Holds ind. offsets within columns for each room
	std::vector<std::vector<int>> m_ColumnOffsetsY;

	void GenerateRoom(int OffsetX, int OffsetY, int ColumnWidth, int Index);

	bool GenerateCorridorBetween(MapCoordinate Begin, MapCoordinate End);


public:

	void Generate();
	
	MapObject* GetCell(int X, int Y);
	MapObject*** GetCellArray() { return m_Cells; }
	int GetHeight() { return m_Height; }
	int GetWidth() { return m_Width; }

	Map();
	Map(std::string MapType, int Width, int Height, MapCoordinate Coords);
	~Map();
};

