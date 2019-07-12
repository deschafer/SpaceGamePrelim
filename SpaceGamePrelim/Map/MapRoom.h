#pragma once

#include "RoomManager.h"
#include "MapCell.h"
#include "MapAssets\MapAsset.h"

#include <string>

enum class Side;

class MapRoom
{
private:

	RoomProperties* m_Properties;

	MapObject*** m_Cells;
	MapAsset*** m_Assets;

	std::string m_RoomType;

	// Contains a beginning point and an ending point
	std::vector<std::pair<MapCoordinate, MapCoordinate>> m_TopFacingCandiates;		// Candidates are sides that are suitable for a corridor
	std::vector<std::pair<MapCoordinate, MapCoordinate>> m_RightFacingCandiates;	// Candidates are sides that are suitable for a corridor
	std::vector<std::pair<MapCoordinate, MapCoordinate>> m_BottomFacingCandiates;	// Candidates are sides that are suitable for a corridor
	std::vector<std::pair<MapCoordinate, MapCoordinate>> m_LeftFacingCandiates;		// Candidates are sides that are suitable for a corridor
	
	std::vector<MapRoom*> m_TopLinkedRooms;		// Rooms that are physically linked with corridors
	std::vector<MapRoom*> m_RightLinkedRooms;	// ..
	std::vector<MapRoom*> m_BottomLinkedRooms;	// ..
	std::vector<MapRoom*> m_LeftLinkedRooms;	// ..
		
	std::vector<unsigned> m_AssetListIDs;	// All of the possible assets that may be able to spawn in this room

	int m_CellWidth;
	int m_CellHeight;

	int m_Width;
	int m_Height;

	void AddCandidate(Side CurrentSide, MapCoordinate Start, MapCoordinate End);
	void PlaceAssets();
public:

	void Generate(); // Generates this room

	MapObject* GetCell(int X, int Y);
	MapObject*** GetCellArray() { return m_Cells; }
	int GetHeight() { return m_Height; }
	int GetWidth() { return m_Width; }
	std::string GetRoomType() { return m_RoomType; }
	void AddLinkedRoom(Side side, MapRoom* LinkedRoom);
	bool ConnectedToRoom(Side side);

	std::pair<MapCoordinate, MapCoordinate>* GetFacingFromSide(Side side);
	std::pair<MapCoordinate, MapCoordinate>* GetFacingFromSideIndexed(Side side, int Index);

	MapRoom();
	MapRoom(std::string RoomType, int Width, int Height); // Known room
	MapRoom(int Width, int Height);						  // Random room
	~MapRoom();	
};

