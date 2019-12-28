#pragma once

#include "RoomManager.h"
#include "MapCell.h"
#include "MapAssets\MapAsset.h"

#include <string>

enum class Side;
class Map;

class MapRoom
{
private:

	RoomProperties* m_Properties;
	MapObject*** m_Cells;
	MapAsset*** m_Assets;
	bool*** m_Doorways;
	std::string m_RoomType;
	Map* m_ParentMap;

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
	//std::vector<unsigned>

	int m_CellWidth;
	int m_CellHeight;
	int m_CountCells;

	int m_Width;
	int m_Height;

	int m_CellSpawnRate;
	int m_CellSpawnGroupSize;	// the max number of cells where a asset can spawn in per asset spawn chance

	bool m_BorderingRoom;	// Is this room on one of the 4 outer sides of this map

	void AddCandidate(Side CurrentSide, MapCoordinate Start, MapCoordinate End);
	MapAsset* PlaceAsset();
	MapCoordinate FindAssetPlacement(MapCoordinate Coord, MapAsset* Asset, int Width, int Height);
public:

	void Generate(); // Generates this room

	MapObject* GetCell(int X, int Y);
	MapAsset* GetAsset(int X, int Y);
	MapCell* GetMapCell(int X, int Y) { return static_cast<MapCell*>(GetCell(X, Y)); }
	MapObject*** GetCellArray() { return m_Cells; }
	int GetHeight() { return m_Height; }
	int GetWidth() { return m_Width; }
	std::string GetRoomType() { return m_RoomType; }
	void AddLinkedRoom(Side side, MapRoom* LinkedRoom);
	void AddDoorWayPosition(MapCoordinate RoomRelativeCoordinate);
	bool ConnectedToRoom(Side side);
	std::vector<MapAsset*> PlaceAssets();

	std::pair<MapCoordinate, MapCoordinate>* GetFacingFromSide(Side side);
	std::pair<MapCoordinate, MapCoordinate>* GetFacingFromSideIndexed(Side side, int Index);

	void SetBorderingRoom() { m_BorderingRoom = true; }

	MapRoom();
	MapRoom(std::string RoomType, int Width, int Height, Map* ParentMap); // Known room
	MapRoom(std::string RoomType, int Width, int Height, Map* ParentMap, std::vector<unsigned> AssetLists);
	MapRoom(int Width, int Height);						  // Random room
	~MapRoom();	
};

