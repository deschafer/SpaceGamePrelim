#pragma once

#include <vector>
#include <SDL.h>
#include <string>

#include "MapObject.h"
#include "TextureManager.h"
#include "MapCell.h"
#include "RoomManager.h"


// Singleton class structure
class MapManager
{
private:
	int m_ActiveWndWidth;
	int m_ActiveWndHeight;

	int m_Rows;
	int m_Columns;

	int m_OffsetX;
	int m_OffsetY;

	bool m_Init;

	static MapManager* m_Instance;
	
	MapObject*** m_VisibleObjectArray;	// 2D array of pointers to mapobjects visible

	RoomManager* m_RoomManager;

	std::vector<MapObject*> m_LoadedMapObjects;

	void LoadMapObjects();		// This will load the inview map objects

	void LoadDefaultMapObjects();
	void DrawDefaultMapObjects();


	void DrawDefaultRoom();
	void DrawDefinedRoom();
	void DrawVisibleCells();


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

	MapManager();
	~MapManager();
};

