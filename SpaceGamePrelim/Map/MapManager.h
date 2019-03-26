#pragma once

#include <vector>
#include <SDL.h>
#include <string>

#include "MapObject.h"
#include "..\TextureCode\TextureManager.h"
#include "MapCell.h"
#include "Map.h"
#include "RoomManager.h"

//
// Coordinate space of connected maps follows a traditonal cartesian coordinate space, pos X to the right, pos y to the top.
//

// Singleton class structure
class MapManager
{
private:
	int m_ActiveWndWidth;
	int m_ActiveWndHeight;
	int m_VisibleRows;
	int m_VisibleColumns;
	int m_Rows;
	int m_Columns;
	int m_CellWidth;
	int m_CellHeight;
	int m_OffsetX;		
	int m_OffsetY;		
	int m_PixelOffsetY;
	int m_PixelOffsetX;

	bool m_MovementLeft;
	bool m_MovementRight;
	bool m_MovementNorth;
	bool m_MovementSouth;
	bool m_Init;
	bool m_MapNeedsSwapping;		
	bool m_MapsAreGenerating;
	bool m_HorizMovementSwapped;
	bool m_VertiMovementSwapped;

	static MapManager* m_Instance;
	
	MapObject*** m_VisibleObjectArray;	// 2D array of pointers to mapobjects visible
	Map* m_ActiveMap;					// Active map pointer
	RoomManager* m_RoomManager;
	std::vector<Map*> m_LoadedMaps;
	std::map<std::pair<int, int>, Map*> m_CoordinateMaps;
	std::vector<Map*> m_GeneratingMaps;	// Holds maps that need to be linked in the coordinate map structure
	std::vector <std::pair<MapDirection, bool>> m_SwappingMovement;	// Which direction to map needs to move towards when swapping

	std::vector<MapObject****> m_VisibleMapCells;


	void SetLink(Map* NewMap);
	void GenerateNeighbors();
	void GenerateNeighbor(std::string MapType, MapDirection ActiveMapDir, MapDirection NewMapDir, MapCoordinate NewMapCoord);
	void HandleInput();
	void CheckGeneratingMaps();
	void CullMap();
	void MoveMap();
	void UpdateCells();

	void ConnectMapsVertically(Map* Map1, Map* Map2);
	void GenerateVerticalCorridorBetween(Map* Map1, MapCoordinate Pos1, Map* Map2, MapCoordinate Pos2);

	MapManager();

public:

	static MapManager* Instance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new MapManager;
			return m_Instance;
		} 
		else return m_Instance;
	}

	void DrawGrid();
	void Draw();
	void Update();

	int GetCellWidth() { return m_CellWidth; }
	int GetCellHeight() { return m_CellHeight; }

	void ConnectTwoMaps(Map* Map1, Map* Map2, MapDirection LinkBetween);


	void ResetMap();
	
	~MapManager();
};

