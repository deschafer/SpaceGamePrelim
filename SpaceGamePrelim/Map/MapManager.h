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
	int m_Rows;
	int m_Columns;
	int m_CellWidth;
	int m_CellHeight;
	int m_OffsetX;		
	int m_OffsetY;		
	int m_ActOffsetX;
	int m_ActOffsetY;
	int m_PixelOffsetY;
	int m_PixelOffsetX;

	bool m_Init;

	static MapManager* m_Instance;
	
	MapObject*** m_VisibleObjectArray;	// 2D array of pointers to mapobjects visible
	Map* m_ActiveMap;					// Active map pointer
	RoomManager* m_RoomManager;
	std::vector<Map*> m_LoadedMaps;
	std::map<std::pair<int, int>, Map*> m_CoordinateMaps;
	std::vector<Map*> m_GeneratingMaps;	// Holds maps that need to be linked in the coordinate map structure

	void SetLink(Map* NewMap);
	void GenerateNeighbors();
	void GenerateNeighbor(std::string MapType, MapDirection ActiveMapDir, MapDirection NewMapDir, MapCoordinate NewMapCoord);
	void HandleInput();
	void CheckGeneratingMaps();

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

	void ResetMap();
	
	~MapManager();
};

