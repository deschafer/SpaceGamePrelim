
#include "MapManager.h"
#include "MainApplication.h"

#include <iostream>

MapManager* MapManager::m_Instance = nullptr;

MapManager::MapManager() :
	m_ActiveWndHeight(0),
	m_ActiveWndWidth(0)
{
}


MapManager::~MapManager()
{
}

void MapManager::DrawGrid()
{
	int CellWidth = 32;
	int CellHeight = 32;

	SDL_Renderer* renderer = MainApplication::Instance()->GetRenderer();

	m_ActiveWndHeight = MainApplication::Instance()->GetWndHeight();
	m_ActiveWndWidth = MainApplication::Instance()->GetWndWidth();
	
	m_Rows = m_ActiveWndHeight / CellHeight; // Get a count of columns
	m_Columns = m_ActiveWndWidth / CellWidth;		// Get a count of rows

	if (MainApplication::Instance()->ResolutionChanged())
	{
		
		m_VisibleObjectArray = new MapObject**[m_Rows];
		for (int i = 0; i < m_Rows; i++)
		{
			m_VisibleObjectArray[i] = new MapObject*[m_Columns];
		}
	}

	

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	// Drawing horizontal lines
	for (int i = 0; i < m_Rows; i++)
	{
	
		SDL_RenderDrawLine(renderer, 0, (i + 1) * CellHeight, m_ActiveWndWidth, (i + 1) * CellHeight);

	}

	// Drawing vertical lines
	for (int i = 0; i < m_Columns; i++)
	{

		SDL_RenderDrawLine(renderer, (i + 1) * CellWidth, 0, (i + 1) * CellWidth, m_ActiveWndHeight);

	}


	DrawDefaultMapObjects();

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


void MapManager::DrawDefaultMapObjects()
{

	for (int i = 0; i < m_Rows; i++)
	{
		for (int j = 0; j < m_Columns; j++)
		{

			TextureManager::Instance()->DrawCurrentFrame("DefaultMap",
				(j) * 32,
				(i) * 32,
				Rect(0, 0, 32, 32),
				SDL_FLIP_NONE,
				MainApplication::Instance()->GetRenderer(),
				1, 0);

		}
	}

}
