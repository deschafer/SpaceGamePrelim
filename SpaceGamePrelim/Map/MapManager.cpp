
#include "MapManager.h"
#include "..\Frame\MainApplication.h"

#include <iostream>
#include <ctime>
#include <map>

using namespace std;

static const int CellWidthSrc = 32;
static const int CellHeightSrc = 32;
static const int MapSizeW = 100;
static const int MapSizeH = 100;
static const int MapMovementModulo = 1;

static const string DefaultMapStr = "Default";

static int ThreadGenerateMap(void* Mem);

typedef std::pair<int, int> Coord;

MapManager* MapManager::m_Instance = nullptr;

//
// ThreadGenerateMap()
// Thead handler used for each map generation thread to generate its given map
//
static int ThreadGenerateMap(void* Mem)
{
	if (Mem == nullptr) return -1;
	// Convert the memory to its map form
	Map* GenMap = static_cast<Map*>(Mem);

	// Then proceed to generate it
	GenMap->Generate();

	return EXIT_SUCCESS;
}

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

	m_CoordinateMaps[pair<int, int>(0, 0)] = m_ActiveMap;

	m_LoadedMaps.push_back(m_ActiveMap);

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
				if ((((i + 1)* m_CellWidth + m_PixelOffsetX) >= 0) && ((i* m_CellWidth + m_PixelOffsetX) < m_Rows * m_CellWidth))
				{
					if ((((j + 1)* m_CellHeight + m_PixelOffsetY) >= 0) && ((j* m_CellHeight + m_PixelOffsetY) < m_Columns * m_CellHeight))
					{

						m_VisibleObjectArray[i][j]->Draw(MapCoordinate((i)* m_CellWidth + m_PixelOffsetX, (j)* m_CellHeight + m_PixelOffsetY));
					}
				}
			}
		}
	}
}

//
//
//
//
void MapManager::Update()
{
	// Checks if the current map has no neighbooring maps
	// If it is not completely surrounded, generate new maps
	if (!m_ActiveMap->IsSurrounded())
	{
		GenerateNeighbors();
	}

	// Check if all of the newly generated maps are done
	if (!m_GeneratingMaps.empty())
	{
		CheckGeneratingMaps();
	}

	HandleInput();

	// If there are portions on the screen not for the active map,
	// Then get those cells, and add those to the active object array
	
	// Once the current map has less displayed than another map, that one becomes the active map


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
// Draws a debug grid following the cell widths and heights
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
// ResetMap()
// Redraws the current map, currently 
// used as a test function only
//
void MapManager::ResetMap()
{
	m_ActiveMap = new Map("Default", 100, 100, MapCoordinate(0, 0));
	m_ActiveMap->Generate();
}

//
//
//
//
void MapManager::SetLink(Map* NewMap)
{

	MapCoordinate Coor = NewMap->GetCoordinate();
	Map* Test = nullptr;
	// Check all surrounding cells in the map for neighbors

	// Check north
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX(), Coor.GetPositionY() + 1)])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::North, Test);
		Test->SetLink(MapDirection::South, NewMap);
	}
	// Checks northeast
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX() + 1, Coor.GetPositionY() + 1)])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::Northeast, Test);
		Test->SetLink(MapDirection::Southwest, NewMap);
	}
	// Checks East
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX() + 1, Coor.GetPositionY())])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::East, Test);
		Test->SetLink(MapDirection::West, NewMap);
	}
	// Checks southeast
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX() + 1, Coor.GetPositionY() - 1)])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::Southeast, Test);
		Test->SetLink(MapDirection::Northwest, NewMap);
	}
	// Checks south
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX(), Coor.GetPositionY() - 1)])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::South, Test);
		Test->SetLink(MapDirection::North, NewMap);
	}
	// Checks southwest
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX() - 1, Coor.GetPositionY() - 1)])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::Southwest, Test);
		Test->SetLink(MapDirection::Northeast, NewMap);
	}
	// Checks west
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX() - 1, Coor.GetPositionY())])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::West, Test);
		Test->SetLink(MapDirection::East, NewMap);
	}
	// Checks northwest
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX() - 1, Coor.GetPositionY() + 1)])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::Northwest, Test);
		Test->SetLink(MapDirection::Southeast, NewMap);
	}

}

//
// GenerateNeighbors()
// Checks all of the surrounding sides of the map and generates them if needed
//
void MapManager::GenerateNeighbors()
{
	int CurrentX = m_ActiveMap->GetCoordinate().GetPositionX();
	int CurrentY = m_ActiveMap->GetCoordinate().GetPositionY();

	// Check every direction, and generate the map for each
	if (!m_ActiveMap->CheckLink(MapDirection::North))
	{
		GenerateNeighbor(DefaultMapStr, MapDirection::North, MapDirection::South, MapCoordinate(CurrentX, CurrentY + 1));
	}
	if (!m_ActiveMap->CheckLink(MapDirection::Northeast))
	{
		GenerateNeighbor(DefaultMapStr, MapDirection::Northeast, MapDirection::Southwest, MapCoordinate(CurrentX + 1, CurrentY + 1));
	}
	if (!m_ActiveMap->CheckLink(MapDirection::East))
	{
		GenerateNeighbor(DefaultMapStr, MapDirection::East, MapDirection::West, MapCoordinate(CurrentX + 1, CurrentY));
	}
	if (!m_ActiveMap->CheckLink(MapDirection::Southeast))
	{
		GenerateNeighbor(DefaultMapStr, MapDirection::Southeast, MapDirection::Northwest, MapCoordinate(CurrentX + 1, CurrentY - 1));
	}
	if (!m_ActiveMap->CheckLink(MapDirection::South))
	{
		GenerateNeighbor(DefaultMapStr, MapDirection::South, MapDirection::North, MapCoordinate(CurrentX, CurrentY - 1));
	}
	if (!m_ActiveMap->CheckLink(MapDirection::Southwest))
	{
		GenerateNeighbor(DefaultMapStr, MapDirection::Southwest, MapDirection::Northeast, MapCoordinate(CurrentX - 1, CurrentY - 1));
	}
	if (!m_ActiveMap->CheckLink(MapDirection::West))
	{
		GenerateNeighbor(DefaultMapStr, MapDirection::West, MapDirection::East, MapCoordinate(CurrentX - 1, CurrentY));
	}
	if (!m_ActiveMap->CheckLink(MapDirection::Northwest))
	{
		GenerateNeighbor(DefaultMapStr, MapDirection::Northwest, MapDirection::Southeast, MapCoordinate(CurrentX - 1, CurrentY + 1));
	}
}

//
// GenerateNeighbor()
// Creates the new map, sets the appropriate links, and creates the thread to 
// generate the new map
//
void MapManager::GenerateNeighbor(std::string MapType, MapDirection ActiveMapDir, MapDirection NewMapDir, MapCoordinate NewMapCoord)
{
	// Create the new map
	Map* NewMap = new Map(MapType, MapSizeW, MapSizeH, NewMapCoord);

	// Set the link from the active map to this map
	// so we do not generate this map again in this place for the activemap
	m_ActiveMap->SetLink(ActiveMapDir, NewMap);
	NewMap->SetLink(NewMapDir, m_ActiveMap);

	// Then save it into the map
	m_CoordinateMaps[Coord(NewMapCoord.GetPositionX(), NewMapCoord.GetPositionY())] = NewMap;

	// Add this map to the queue to be linked
	m_GeneratingMaps.push_back(NewMap);

	// Create a new thread to generate this new map
	SDL_Thread *Thread = SDL_CreateThread(ThreadGenerateMap, "MapGenThread", NewMap);
}

//
// CheckGeneratingMaps()
// Checks the multithreaded map generation to see if all of the maps have been made.
// If all of the maps are generated, then we can link them all together without
// any need for multithreaded mut. exclusion
//
void MapManager::CheckGeneratingMaps()
{
	bool Generated = true;
	for (size_t i = 0; i < m_GeneratingMaps.size(); i++)
	{
		if (!m_GeneratingMaps[i]->IsGenerated())
		{
			Generated = false;
			break;
		}
	}
	// If they are all complete, then we add their links
	if (Generated)
	{
		for (size_t i = 0; i < m_GeneratingMaps.size(); i++)
		{
			SetLink(m_GeneratingMaps[i]);
		}
		m_GeneratingMaps.clear();
	}
}

//
// HandleInput()
//
//
void MapManager::HandleInput()
{
	// Checks for user input
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_DOWN))// && !Down)
	{
		m_PixelOffsetY -= 8;
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_UP))// && !Up)
	{
		m_PixelOffsetY += 8;
	}

	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_LEFT))// && !Left)
	{
		m_PixelOffsetX += 8;
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_RIGHT))// && !Right)
	{
		m_PixelOffsetX -= 8;
	}
}