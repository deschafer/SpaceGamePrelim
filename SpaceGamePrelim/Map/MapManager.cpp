
#include "MapManager.h"
#include "..\Frame\MainApplication.h"

#include <iostream>
#include <ctime>

#include <map>

static const int CellWidthSrc = 32;
static const int CellHeightSrc = 32;
static const int MapSizeW = 100;
static const int MapSizeH = 100;
static const int MapMovementModulo = 1;

using namespace std;

MapManager* MapManager::m_Instance = nullptr;

MapManager::MapManager() :
	m_ActiveWndHeight(0),
	m_ActiveWndWidth(0),
	m_CellHeight(CellWidthSrc),
	m_CellWidth(CellHeightSrc),
	m_Init(true),
	m_OffsetX(0),
	m_OffsetY(0),
	m_ActOffsetX(0),
	m_ActOffsetY(0),
	m_PixelOffsetY(0),
	m_ActiveMap(new Map("Default", MapSizeW, MapSizeH, MapCoordinate(0, 0)))
{
	
	m_RoomManager = RoomManager::Instance();
	m_ActiveWndHeight = MainApplication::Instance()->GetWndHeight();
	m_ActiveWndWidth = MainApplication::Instance()->GetWndWidth();

	// Getting a count of rows and columns
	m_Rows = MapSizeH;		// Get a count of columns
	m_Columns = MapSizeW;		// Get a count of rows

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
// Draw()
// Draws all of the visible cells onto the screen
// part of update loop
//

void MapManager::Draw()
{

	for (int i = 0; i < m_Columns; i++)
	{
		for (int j = 0; j < m_Rows; j++)
		{
			if (m_VisibleObjectArray[i][j] != nullptr)
			{
				if (((i* m_CellWidth + m_PixelOffsetX) >= 0) && ((i* m_CellWidth + m_PixelOffsetX) < 1920))
				{
					if (((j* m_CellHeight + m_PixelOffsetY) >= 0) && ((j* m_CellHeight + m_PixelOffsetY) < 1920))
					{

						m_VisibleObjectArray[i][j]->Draw(MapCoordinate((i)* m_CellWidth + m_PixelOffsetX, (j)* m_CellHeight + m_PixelOffsetY));
					}
				}
			}
		}
	}

}

void MapManager::Update()
{
	static int Up = true;
	static int Down = true;
	static int Left = true;
	static int Right = true;

	// Checks for user input
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_DOWN))// && !Down)
	{

		//if (m_OffsetY - 1 >= 0 && !m_ActOffsetY) m_OffsetY--;
		//else if (m_ActOffsetY != MapSizeH) m_ActOffsetY++;
		m_PixelOffsetY -= 8;

		Up++;
	}
	//else ++Up %= MapMovementModulo;
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_UP))// && !Up)
	{
		

		//if (m_OffsetY + 1 < m_Rows && !m_ActOffsetY) m_OffsetY++;
		//else if (m_ActOffsetY) m_ActOffsetY--;


		m_PixelOffsetY += 8;

		Down++;
	}
	//else ++Down %= MapMovementModulo;
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_LEFT))// && !Left)
	{
		//if (m_OffsetX - 1 >= 0 && !m_ActOffsetX) m_OffsetX--;
		//else if(m_ActOffsetX != MapSizeW) m_ActOffsetX++;
		//Left++;

		m_PixelOffsetX += 8;

	}
	//else ++Left %= MapMovementModulo;
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_RIGHT))// && !Right)
	{
	//	if (m_OffsetX + 1 < m_Columns && !m_ActOffsetX) m_OffsetX++;
	//	else if (m_ActOffsetX) m_ActOffsetX--;
	//	Right++;

		m_PixelOffsetX -= 8;

	}
	//else ++Right %= MapMovementModulo;

	// Getting the new cells for the offset
	// If there is an X offset
	if (m_OffsetX != 0)
	{
		// Get all of the cells that are missing
		// Since this is preliminary, it just sets these to nullptr.
		for (size_t i = 0; i < m_OffsetX; i++)
		{
			for (size_t j = 0; j < m_Columns; j++)
			{
				m_VisibleObjectArray[i][j] = nullptr;
			}
		}
	}
	// If there is an Y offset
	if (m_OffsetY != 0)
	{
		// Get all of the cells that are missing
		// Since this is preliminary, it just sets these to nullptr.
		for (size_t j = 0; j < m_OffsetY; j++)
		{
			for (size_t i = 0; i < m_Rows; i++)
			{
				m_VisibleObjectArray[i][j] = nullptr;
			}
		}
	}

	// Get all of our active map cells
	for (int i = m_OffsetX, MapI = 0; i < m_Columns; i++, MapI++)
	{
		for (int j = m_OffsetY, MapJ = 0; j < m_Rows; j++, MapJ++)
		{
			// Gets the new cell
			m_VisibleObjectArray[i][j] = m_ActiveMap->GetCell(m_ActOffsetX + MapI, m_ActOffsetY + MapJ);
			// Then draws it
			if (m_VisibleObjectArray[i][j] != nullptr)
			{
				m_VisibleObjectArray[i][j]->Update();
			}
		}
	}

	
}

//
// DrawGrid()
// This is a temporary function, its status is currently being reevaluated
//
void MapManager::DrawGrid()
{
	
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
	SDL_RenderDrawRect(MainApplication::Instance()->GetRenderer(), &cRect);
	SDL_SetRenderDrawColor(renderer, OldR, OldG, OldB, OldA);
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

//
// ResetMap()
// Redraws the current map, currently 
// used as a test function only
//
void MapManager::ResetMap()
{
	m_ActiveMap = new Map("Default", 100, 100, MapCoordinate(0, 0));
	m_ActiveMap->Generate();
}