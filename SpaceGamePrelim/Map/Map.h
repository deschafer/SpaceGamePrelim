#pragma once

#include "MapObject.h"
#include "MapRoom.h"

#include <string>
#include <Windows.h>
#include <map>

#define MapDirectionCt 8

// Map directions for the surronding maps
enum class MapDirection 
{
	North, 
	Northeast,
	East,
	Southeast,
	South,
	Southwest,
	West,
	Northwest
};

class Map
{
private:

	int m_Width;						// Width of this map 
	int m_Height;						// Height of this map
	bool m_Generated;					// Whether map has been generated or not.
										// if not, then it is most likely being generated by a thread
	std::string m_MapType;				// Map type for different themes or styles
	MapCoordinate m_MapCoordinates;		// Coordinates of the map in comparison to other maps	

	MapObject*** m_Cells;				// Cells of this map
	MapObject*** m_CorridorCells;		// Cells of the corridors of this map. allows for corridor combinations

	// Parallel room vectors
	std::vector<std::vector<MapRoom*>> m_Rooms;		// Holds the 2D array of rooms
	std::vector<std::vector<int>> m_ColumnOffsetsX;	// Holds ind. offsets within columns for each room
	std::vector<std::vector<int>> m_ColumnOffsetsY; // Starting y pos for the given room

	Map* m_NeighboringMaps[MapDirectionCt];					// Holds a pointer to all of the surrounding maps
	int m_NeighborMapsSize;

	void GenerateRoom(int OffsetX, int OffsetY, int ColumnWidth, int Index);

	void FindCandidateSidePositions(MapRoom* RightRoom, int RightOffsetX, int RightOffsetY, MapRoom* LeftRoom, int LeftOffsetX, int LeftOffsetY, int ColumnNumber);
	bool GenerateCorridorBetween(MapCoordinate Begin, MapCoordinate End, int DistanceBetween, bool Horiz);
	void SetUpCorridor(int ColumnNumber, int OffsetX, int OffsetY, int RoomOffsetX, MapRoom* BottomRoom);
	void SetUpHorizCorridor(int ColumnNumber, int OffsetX, int OffsetY, int RoomOffsetX, MapRoom* Room);
	void CheckCell(MapCoordinate CellPosition, 
		std::vector<std::string> Textures, 
		Cell CellType, 
		std::vector<MapCoordinate>& TextureCoords, 
		bool Right);


public:

	void Generate();
	

	MapObject* GetCell(int X, int Y);
	MapObject*** GetCellArray() { return m_Cells; }
	int GetHeight() { return m_Height; }
	int GetWidth() { return m_Width; }
	bool IsSurrounded() { for (size_t i = 0; i < m_NeighborMapsSize; i++) if (!m_NeighboringMaps[i]) return false; return true; }
	bool IsGenerated() { return m_Generated; }
	bool CheckLink(MapDirection Direction) { return m_NeighboringMaps[static_cast<int>(Direction)] ? true : false; }

	void SetLink(MapDirection Direction, Map* LinkedMap) { m_NeighboringMaps[static_cast<int>(Direction)] = LinkedMap; }

	MapObject** GetColumn(int ZeroIndexedColumn);
	MapObject** GetRow(int ZeroIndexedRow);

	MapCoordinate GetCoordinate() { return m_MapCoordinates; }


	Map();
	Map(std::string MapType, int Width, int Height, MapCoordinate Coords);
	~Map();
};

