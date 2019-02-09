
#include "MapManager.h"
#include "..\Frame\MainApplication.h"

#include <iostream>
#include <ctime>

#include <map>


using namespace std;

MapManager* MapManager::m_Instance = nullptr;

MapManager::MapManager() :
	m_ActiveWndHeight(0),
	m_ActiveWndWidth(0),
	m_CellHeight(32),
	m_CellWidth(32),
	m_Init(true),
	m_ActiveMap(new Map("Default", 100, 100, MapCoordinate(0, 0)))
{
	
	m_RoomManager = RoomManager::Instance();
	m_ActiveWndHeight = MainApplication::Instance()->GetWndHeight();
	m_ActiveWndWidth = MainApplication::Instance()->GetWndWidth();

	// Getting a count of rows and columns
	m_Rows = m_ActiveWndHeight / m_CellHeight;		// Get a count of columns
	m_Columns = m_ActiveWndWidth / m_CellWidth;		// Get a count of rows

	m_VisibleObjectArray = new MapObject**[m_Columns];
	for (int i = 0; i < m_Columns; i++)
	{
		m_VisibleObjectArray[i] = new MapObject*[m_Rows];
	}

	for (int i = 0; i < m_Columns; i++)
	{
		for (int j = 0; j < m_Rows; j++)
		{
			m_VisibleObjectArray[i][j] = nullptr;
		}
	}

	m_Instance = this;

	m_ActiveMap->Generate();

}


MapManager::~MapManager()
{
}

//
// DrawGrid()
// This is a temporary function, its status is currently being reevaluated
//
void MapManager::DrawGrid()
{
	/*
	int OldR, OldG, OldB, OldA;
	SDL_Renderer* renderer = MainApplication::Instance()->GetRenderer();



	// Setting line color
	SDL_GetRenderDrawColor(renderer, (Uint8*)&OldR, (Uint8*)&OldG, (Uint8*)&OldB, (Uint8*)&OldA);
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, SDL_ALPHA_OPAQUE);

	// Drawing horizontal lines
	for (int i = 0; i < m_Rows; i++)
	{
	
		SDL_RenderDrawLine(renderer, 0, (i + 1) * m_CellHeight, m_ActiveWndWidth, (i + 1) * m_CellHeight);

	}

	// Drawing vertical lines
	for (int i = 0; i < m_Columns; i++)
	{

		SDL_RenderDrawLine(renderer, (i + 1) * m_CellWidth, 0, (i + 1) * m_CellWidth, m_ActiveWndHeight);

	}

	// Test function only
	// Drawing a boundary rectangle
	SDL_Rect cRect;
	cRect.x = 10 * m_CellWidth;
	cRect.y = 6 * m_CellHeight;
	cRect.w = 15 * m_CellWidth;
	cRect.h = 15 * m_CellHeight;


	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	// end test
//	SDL_RenderDrawRect(MainApplication::Instance()->GetRenderer(), &cRect);

	SDL_SetRenderDrawColor(renderer, OldR, OldG, OldB, OldA);
	*/

	DrawDefinedRoom();
	DrawVisibleCells();
}

//
// DrawVisibleCells()
// Draws all of the visible cells onto the screen
// part of update loop
//
void MapManager::DrawVisibleCells()
{
	for (int i = 0; i < m_Columns; i++)
	{
		for (int j = 0; j < m_Rows; j++)
		{
			// Gets the new cell
			m_VisibleObjectArray[i][j] = m_ActiveMap->GetCell(i, j);
			// Then draws it
			if (m_VisibleObjectArray[i][j] != nullptr)
			{
				m_VisibleObjectArray[i][j]->Draw(MapCoordinate(i * m_CellWidth, j * m_CellHeight));
			}
		}
	}
}

//
// This is a TEST function to play with random room generation
//
//
void MapManager::DrawDefaultRoom()
{

}


//
// DrawDefinedRoom()
// Preliminary version of the room generation that will be a part of 
// every room.
//
//
void MapManager::DrawDefinedRoom()
{
	
}


//
//
// This will load the inview map objects
//
void MapManager::LoadMapObjects()
{

	

}

//
//
//
//
void MapManager::LoadDefaultMapObjects()
{


}

