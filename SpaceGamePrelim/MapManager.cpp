
#include "MapManager.h"
#include "MainApplication.h"

#include <iostream>
#include <ctime>

#include <map>

#define RANDOM 6755433

using namespace std;

MapManager* MapManager::m_Instance = nullptr;

MapManager::MapManager() :
	m_ActiveWndHeight(0),
	m_ActiveWndWidth(0),
	m_Init(true)
{
}


MapManager::~MapManager()
{
}

void MapManager::DrawGrid()
{
	int CellWidth = 32;
	int CellHeight = 32;
	int OldR, OldG, OldB, OldA;


	SDL_Renderer* renderer = MainApplication::Instance()->GetRenderer();

	m_ActiveWndHeight = MainApplication::Instance()->GetWndHeight();
	m_ActiveWndWidth = MainApplication::Instance()->GetWndWidth();
	
	m_Rows = m_ActiveWndHeight / CellHeight; // Get a count of columns
	m_Columns = m_ActiveWndWidth / CellWidth;		// Get a count of rows

	// Definetely have to do something else here
	if (MainApplication::Instance()->ResolutionChanged())
	{
		
		m_VisibleObjectArray = new MapObject**[m_Columns];
		for (int i = 0; i < m_Columns; i++)
		{
			m_VisibleObjectArray[i] = new MapObject*[m_Rows];
		}
	}

	if (m_Init)
	{
		srand(RANDOM);
		for (int i = 0; i < m_Columns; i++)
		{
			for (int j = 0; j < m_Rows; j++)
			{
				m_VisibleObjectArray[i][j] = nullptr;
			}
		}

		m_Init = false;
	}

	

	SDL_GetRenderDrawColor(renderer, (Uint8*)&OldR, (Uint8*)&OldG, (Uint8*)&OldB, (Uint8*)&OldA);
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, SDL_ALPHA_OPAQUE);



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

	SDL_SetRenderDrawColor(renderer, OldR, OldG, OldB, OldA);


	//DrawDefaultMapObjects();
	DrawDefaultRoom();
}

//
// This is a TEST function to play with random room generation
//
//
void MapManager::DrawDefaultRoom()
{
	int StartX = 3;
	int StartY = 6;

	static bool complete = false;
	int Width = 25;
	int Height = 6;
	int MaxHeightX = StartX;
	int MaxHeightY = StartY + Height;
	int MaxWidthX = StartX + Width;
	int MaxWidthY = StartY;

	int rng = 0; // 0 = south, 1 = west, 2 = north, 3 = east
	int count = 0;
	int TempX = StartX;
	int TempY = StartY;

	bool Switch = false;
	bool SwitchStart = false;

	int LengthParam = rand() % 7;

	while (!complete)
	{
		count++;

		if (TempX < (MaxWidthX - LengthParam) && TempY < MaxHeightY && TempY >= StartY  && TempX >= StartX)
		{
			m_VisibleObjectArray[TempX][TempY] = new MapCell(new TextureProperties(Rect(0, 0, 32, 32), "Room", 1, 0, 0, 1), MapCoordinate(TempX * 32, TempY * 32));
		}
		else
		{
			complete = true;
		}

		// Moving east
		TempX++;
		
		if ((rand() % 10) == 0 && !Switch)
		{
			Switch = true;
			SwitchStart = true;

			TempY++;

		}
		if (SwitchStart)
		{
			TempX--;
			SwitchStart = false;
		}



	}

	/*
	// Setting the pointers
	for (int x = StartX; x < m_Columns && x < MaxWidthX; x++)
	{

		for (int y = StartY; y < m_Rows && y < MaxHeightY; y++)
		{

			m_VisibleObjectArray[x][y] = new MapCell(new TextureProperties(Rect(0, 0, 32, 32), "Room", 1, 0, 0, 1), MapCoordinate(x * 32, y * 32));

		}

	}
	*/

	// Setting the pointers
	for (int x = StartX; x < m_Columns && x < MaxWidthX; x++)
	{

		for (int y = StartY; y < m_Rows && y < MaxHeightY; y++)
		{
			if(m_VisibleObjectArray[x][y] != nullptr)
				m_VisibleObjectArray[x][y]->Draw();

		}

	}


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
