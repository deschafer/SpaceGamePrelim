#pragma once

#include <vector>
#include <SDL.h>
#include <string>

#include "MapObject.h"
#include "..\TextureCode\TextureManager.h"
#include "MapCell.h"
#include "Map.h"
#include "RoomManager.h"


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

	bool m_Init;

	static MapManager* m_Instance;
	
	MapObject*** m_VisibleObjectArray;	// 2D array of pointers to mapobjects visible
	Map* m_ActiveMap;					// Active map pointer
	RoomManager* m_RoomManager;

	std::vector<MapObject*> m_LoadedMapObjects;

	void LoadMapObjects();		// This will load the inview map objects

	void LoadDefaultMapObjects();


	void DrawDefaultRoom();
	void DrawDefinedRoom();
	void DrawVisibleCells();

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

	int GetCellWidth() { return m_CellWidth; }
	int GetCellHeight() { return m_CellHeight; }


	void ResetMap();
	
	~MapManager();
};

