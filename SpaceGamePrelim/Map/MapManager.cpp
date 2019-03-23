
#include "MapManager.h"
#include "..\Frame\MainApplication.h"
#include "MapInactive.h"

#include <iostream>
#include <ctime>
#include <map>
#include <chrono>

#define Windows		// Usage of windows threads
//#define CrossPlatform

#ifdef Windows
#include <Windows.h>
#endif // Windows


using namespace std;

static const int CellWidthSrc = 32;
static const int CellHeightSrc = 32;
static const int MapSizeW = 100;
static const int MapSizeH = 100;
static const int HorizontalMovementSpeed = 16;
static const int VerticalMovementSpeed = 16;
static const int VisibleHorizonBufferSize = 2;
static const int VisibleVerticalBufferSize = 2;
static const int CenterMapArrayIndex = 8;

static const string DefaultMapStr = "Default";

typedef std::pair<int, int> Coord;

MapManager* MapManager::m_Instance = nullptr;

void ShiftAllArrayCellsLeft(MapObject*** Array, int X, int Y);
void ShiftAllArrayCellsRight(MapObject*** Array, int X, int Y);
pair<int, int> GetMapOffsets(MapDirection Direction);

// User-level threads that work for all platforms
#ifdef CrossPlatform

//
// ThreadGenerateMap()
// Thead handler used for each map generation thread to generate its given map
//
static int ThreadGenerateMap(void* Mem)
{
	if (Mem == nullptr) return -1;


	// Time seed generation
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto value = now_ms.time_since_epoch();
	long duration = value.count();

	std::chrono::milliseconds dur(duration);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);

	srand(duration);

	// Convert the memory to its map form
	Map* GenMap = static_cast<Map*>(Mem);

	// Then proceed to generate it
	GenMap->Generate();

	return EXIT_SUCCESS;
}
#endif // !CrossPlatform

// Windows kernel-level threads
#ifdef Windows

DWORD WINAPI ThreadGenerateMap(LPVOID lpParam)
{

	// Time seed generation
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto value = now_ms.time_since_epoch();
	long duration = value.count();

	std::chrono::milliseconds dur(duration);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);

	srand(duration);

	if (!lpParam)
	{
#ifdef _DEBUG
		cout << "Memory passed in was nullptr" << endl;
#endif // _DEBUG
		return -1;
	}
	Map* NewMap = static_cast<Map*>(lpParam);
	NewMap->Generate();

	return EXIT_SUCCESS;
}

#endif // Windows

void ShiftAllArrayCellsLeft(MapObject*** Array, int X, int Y)
{
	for (size_t i = 1; i < X; i++)
	{
		// Change the columns themselves
		Array[i - 1] = Array[i];
	}
	// set the last column to nullptr
	Array[X - 1] = nullptr;
}
void ShiftAllArrayCellsRight(MapObject*** Array, int X, int Y)
{

	for (size_t i = X; i > 0; i--)
	{
		// Change the columns themselves
		Array[i] = Array[i - 1];
	}
	// set the last column to nullptr
	Array[0] = nullptr;
}

bool SearchVector(vector <pair<MapDirection, bool>> MapDirections, MapDirection SearchQuery)
{
	for (size_t i = 0; i < MapDirections.size(); i++)
	{
		if (MapDirections[i].first == SearchQuery)
		{
			MapDirections[i].second = false;
			return true;
		}
	}
	return false;
}

pair<int, int> GetMapOffsets(MapDirection Direction)
{
	typedef pair<int, int> Pair;

	switch (Direction)
	{
	case MapDirection::North:
		return Pair(0, MapSizeH);
		break;
	case MapDirection::Northeast:
		return Pair(MapSizeW, MapSizeH);
		break;
	case MapDirection::East:
		return Pair(MapSizeW, 0);
		break;
	case MapDirection::Southeast:
		return Pair(MapSizeW, -MapSizeH);
		break;
	case MapDirection::South:
		return Pair(0, -MapSizeH);
		break;
	case MapDirection::Southwest:
		return Pair(-MapSizeW, -MapSizeH);
		break;
	case MapDirection::West:
		return Pair(-MapSizeW, 0);
		break;
	case MapDirection::Northwest:
		return Pair(-MapSizeW, MapSizeH);
		break;
	default:
		break;
	}
	return pair<int, int>(0, 0);
}

MapManager::MapManager() :
	m_ActiveWndHeight(0),
	m_ActiveWndWidth(0),
	m_CellHeight(CellWidthSrc),
	m_CellWidth(CellHeightSrc),
	m_Init(true),
	m_MapNeedsSwapping(false),
	m_MapsAreGenerating(false),
	m_HorizMovementSwapped(false),
	m_VertiMovementSwapped(false),
	m_OffsetX(0),
	m_OffsetY(0),
	m_PixelOffsetX(0),
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

								// Visible object array is the cells that fill the screen
								// and is therefore the appropriate size of the screen, plus a buffer of 2 cells
								// on for all the sides.

	int ResolutionX = MainApplication::Instance()->GetWndWidth();
	int ResolutionY = MainApplication::Instance()->GetWndHeight();


	m_VisibleColumns = 1000;
	m_VisibleRows = 1000;

	m_VisibleObjectArray = new MapObject**[m_VisibleColumns];
	for (int i = 0; i < m_VisibleColumns; i++)
	{
		m_VisibleObjectArray[i] = new MapObject*[m_VisibleRows];
	}

	for (int i = 0; i < m_VisibleColumns; i++)
	{
		for (int j = 0; j < m_VisibleRows; j++)
		{
			m_VisibleObjectArray[i][j] = nullptr;
		}
	}

	m_Instance = this;
	m_ActiveMap->Generate();

	// Loading the first map
	for (int i = 0; i < m_VisibleColumns; i++)
	{
		for (int j = 0; j < m_VisibleRows; j++)
		{
			m_VisibleObjectArray[i][j] = m_ActiveMap->GetCell(i, j);
		}
	}
	// However, to surrounding maps need to be loaded into the array
	// So we set this to be accomplished when possible
	m_MapNeedsSwapping = true;

	// Add the indices for the VisibleMapCells
	m_VisibleMapCells.push_back(nullptr);
	m_VisibleMapCells.push_back(nullptr);
	m_VisibleMapCells.push_back(nullptr);
	m_VisibleMapCells.push_back(nullptr);
	m_VisibleMapCells.push_back(nullptr);
	m_VisibleMapCells.push_back(nullptr);
	m_VisibleMapCells.push_back(nullptr);
	m_VisibleMapCells.push_back(nullptr);
	m_VisibleMapCells.push_back(nullptr);

	m_VisibleMapCells[CenterMapArrayIndex] = m_ActiveMap->GetCellArrayAddress();

	int run = 0;

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
	MapObject* Object;
	int MapPositionOffsetX = 0;
	int MapPositionOffsetY = 0;
	pair<int, int> Pair;

	for (size_t CurrentMap = 0; CurrentMap < m_VisibleMapCells.size(); CurrentMap++)
	{
		// Get the actual array of cells

		if (m_VisibleMapCells[CurrentMap])
		{
			m_VisibleObjectArray = *m_VisibleMapCells[CurrentMap];

			// Then get the approp. offsets for these cells
			if (CurrentMap != CenterMapArrayIndex)
			{
				Pair = GetMapOffsets(static_cast<MapDirection>(CurrentMap));
				MapPositionOffsetX = Pair.first;
				MapPositionOffsetY = Pair.second;
			}
			else
			{
				MapPositionOffsetX = 0;
				MapPositionOffsetY = 0;
			}
			

			for (int i = 0; i < MapSizeW; i++)
			{
				for (int j = 0; j < MapSizeH; j++)
				{

					
					
					Object = m_VisibleObjectArray[i][j];

					int PositionX = (i)* m_CellWidth + m_PixelOffsetX + (MapPositionOffsetX * m_CellWidth);
					int PositionY = (j)* m_CellHeight + m_PixelOffsetY + (MapPositionOffsetY * m_CellHeight);

					/*
					if (CurrentMap == CenterMapArrayIndex &&
						PositionX + m_CellWidth >= 0 &&
						PositionX < m_ActiveWndWidth &&
						PositionY + m_CellHeight >= 0 &&
						PositionY < m_ActiveWndHeight && (rand() % 30 == 0))
					{
						vector<string> tes;
						tes.push_back("Explosion");
						MapInactive Cell(tes, MapCoordinate(PositionX, PositionY), Cell::Default);
						Cell.Draw(MapCoordinate(PositionX, PositionY));

					}

					else */
					if (Object &&
						PositionX + m_CellWidth >= 0 &&
						PositionX < m_ActiveWndWidth &&
						PositionY + m_CellHeight >= 0 &&
						PositionY < m_ActiveWndHeight)
					{
						Object->Draw(MapCoordinate(PositionX, PositionY));
					}
				}
			}
		}
	}
	//DrawGrid();
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

	// If all maps are complete and the map needs swapped
	if (!m_MapsAreGenerating && m_MapNeedsSwapping)
	{
		MoveMap();
	}

	CullMap();
	

	// Gets input from the user
	HandleInput();

	// Centers the map around current center map



	// Get all of our active map cells
	for (int i = 0, MapI = 0; i < MapSizeW; i++, MapI++)
	{
		for (int j = 0, MapJ = 0; j < MapSizeH; j++, MapJ++)
		{
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

	for (int i = 0; i < m_VisibleColumns; i++)
	{
		for (int j = 0; j < m_VisibleRows; j++)
		{
			m_VisibleObjectArray[i][j] = m_ActiveMap->GetCell(i, j);
		}
	}
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

	m_MapsAreGenerating = true;
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
#ifdef CrossPlatform
	SDL_Thread *Thread = SDL_CreateThread(ThreadGenerateMap, "MapGenThread", NewMap);
#endif // CrossPlatform
#ifdef Windows
	CreateThread(
		nullptr,                   // default security attributes
		0,                      // use default stack size  
		ThreadGenerateMap,       // thread function name
		NewMap,          // argument to thread function 
		0,                      // use default creation flags 
		nullptr);   // returns the thread identifier 
#endif // Windows


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
		m_MapsAreGenerating = false;
	}
}

//
// HandleInput()
//
//
void MapManager::HandleInput()
{
	// Checks for user input
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_DOWN) &&
		!InputManager::Instance()->IsKeyDown(SDL_SCANCODE_UP))
	{
		if (m_PixelOffsetY % m_CellHeight == 0)
		{
			m_MovementSouth = true;
		}
		m_PixelOffsetY -= VerticalMovementSpeed;
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_UP) &&
		!InputManager::Instance()->IsKeyDown(SDL_SCANCODE_DOWN))
	{
		if (m_PixelOffsetY % m_CellHeight == 0)
		{
			m_MovementNorth = true;
		}
		m_PixelOffsetY += VerticalMovementSpeed;
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_LEFT) && 
		!InputManager::Instance()->IsKeyDown(SDL_SCANCODE_RIGHT))
	{
		if (m_PixelOffsetX % m_CellWidth == 0)
		{
			m_MovementLeft = true;
		}
		m_PixelOffsetX += HorizontalMovementSpeed;
	}
	else if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_RIGHT) && 
		!InputManager::Instance()->IsKeyDown(SDL_SCANCODE_LEFT))
	{
		if (m_PixelOffsetX % m_CellWidth == 0)
		{
			m_MovementRight = true;
		}
		m_PixelOffsetX -= HorizontalMovementSpeed;
	}
}

//
// MoveMap()
// Moves the map in the specified direction by the map size.
// Note: all surrounding maps should be generated,
// so all maps should be generated before calling
//
void MapManager::MoveMap()
{
	static bool FirstMap = true;

	// Set all of the surrounding maps to there approp. positions in
	// reference to the newly set active/center map
	m_VisibleMapCells[(int)MapDirection::North] = m_ActiveMap->GetNeighbor(MapDirection::North)->GetCellArrayAddress();
	m_VisibleMapCells[(int)MapDirection::Northeast] = m_ActiveMap->GetNeighbor(MapDirection::Northeast)->GetCellArrayAddress();
	m_VisibleMapCells[(int)MapDirection::East] = m_ActiveMap->GetNeighbor(MapDirection::East)->GetCellArrayAddress();
	m_VisibleMapCells[(int)MapDirection::Southeast] = m_ActiveMap->GetNeighbor(MapDirection::Southeast)->GetCellArrayAddress();
	m_VisibleMapCells[(int)MapDirection::South] = m_ActiveMap->GetNeighbor(MapDirection::South)->GetCellArrayAddress();
	m_VisibleMapCells[(int)MapDirection::Southwest] = m_ActiveMap->GetNeighbor(MapDirection::Southwest)->GetCellArrayAddress();
	m_VisibleMapCells[(int)MapDirection::West] = m_ActiveMap->GetNeighbor(MapDirection::West)->GetCellArrayAddress();
	m_VisibleMapCells[(int)MapDirection::Northwest] = m_ActiveMap->GetNeighbor(MapDirection::Northwest)->GetCellArrayAddress();
	// Finally set the cells for the new active map
	m_VisibleMapCells[CenterMapArrayIndex] = m_ActiveMap->GetCellArrayAddress();;

	// Map no longer needs swapping
	m_MapNeedsSwapping = false;

	// Reset the positions
	if (!FirstMap)
	{
		if (m_HorizMovementSwapped)
		{
			if (abs(m_PixelOffsetX) >= (MapSizeW * m_CellWidth))
			{
				m_PixelOffsetX %= (MapSizeW * m_CellWidth);
			}
			else if (m_PixelOffsetX < 0)
			{
				m_PixelOffsetX = (MapSizeW * m_CellWidth) + m_PixelOffsetX;
			}
			else if (m_PixelOffsetX > 0)
			{
				m_PixelOffsetX = -(MapSizeW * m_CellWidth) + m_PixelOffsetX;
			}
			m_OffsetX = -m_PixelOffsetX / m_CellWidth;

			m_HorizMovementSwapped = false;
		}
		if (m_VertiMovementSwapped)
		{
			if (abs(m_PixelOffsetY) >= (MapSizeH * m_CellHeight))
			{
				m_PixelOffsetY %= (MapSizeH * m_CellHeight);
			}
			else if (m_PixelOffsetY < 0)
			{
				m_PixelOffsetY = (MapSizeH * m_CellHeight) + m_PixelOffsetY;
			}
			else if (m_PixelOffsetY > 0)
			{
				m_PixelOffsetY = -(MapSizeH * m_CellHeight) + m_PixelOffsetY;
			}
			m_OffsetY = -m_PixelOffsetY / m_CellHeight;

			m_VertiMovementSwapped = false;
		}
	}
	else
	{
		FirstMap = false;
	}
}

//
// CullMap()
//
//
void MapManager::CullMap()
{

	// Idea is that there is a certain number of movements that move a pixel offset, then
	// at some point we also get a new cell

	// m_MapOffset holds the offset from the current activemap
	// should also keep a count/offset from the origin map

	if (m_MovementLeft)
	{
		// Move to the left one position
		m_OffsetX--;

		// If the result is an integer that is a odd number
		if (m_MapNeedsSwapping)
		{
		}
		else if (-m_OffsetX >= MapSizeW / 2)
		{
			// Set the active map to the map to the left
			m_ActiveMap = m_ActiveMap->GetNeighbor(MapDirection::West);

			// Make sure the flag is set
			m_MapNeedsSwapping = true;
			m_HorizMovementSwapped = true;
		}

		cout << "West -- X:" << m_OffsetX << " Y: " << m_OffsetY << endl;

		m_MovementLeft = false;
	}
	if (m_MovementRight)
	{
		// Move to the left one position
		m_OffsetX++;

		if (m_MapNeedsSwapping)
		{
		}
		// If the result is an integer that is a odd number
		else if (m_OffsetX >= MapSizeW)
		{
			// Set the active map to the map to the left
			m_ActiveMap = m_ActiveMap->GetNeighbor(MapDirection::East);

			// Make sure the flag is set
			m_MapNeedsSwapping = true;
			m_HorizMovementSwapped = true;
		}

		cout << "East -- X:" << m_OffsetX << " Y: " << m_OffsetY << endl;

		m_MovementRight = false;
	}
	if (m_MovementNorth)
	{
		// Move to the left one position
		m_OffsetY--;

		if (m_MapNeedsSwapping)
		{
		}
		// If the result is an integer that is a odd number
		else if (-m_OffsetY >= MapSizeH / 2)
		{
			// Set the active map to the map to the left
			m_ActiveMap = m_ActiveMap->GetNeighbor(MapDirection::South);

			// Make sure the flag is set
			m_MapNeedsSwapping = true;
			m_VertiMovementSwapped = true;

		}

		cout << "South -- X:" << m_OffsetX << " Y: " << m_OffsetY << endl;


		m_MovementNorth = false;
	}
	if (m_MovementSouth)
	{
		// Move to the left one position
		m_OffsetY++;

		if (m_MapNeedsSwapping)
		{
		}
		// If the result is an integer that is a odd number
		else if (m_OffsetY >= MapSizeH)
		{
			// Set the active map to the map to the left
			m_ActiveMap = m_ActiveMap->GetNeighbor(MapDirection::North);

			// Make sure the flag is set
			m_MapNeedsSwapping = true;
			m_VertiMovementSwapped = true;

		}

		cout << "North -- X:" << m_OffsetX << " Y: " << m_OffsetY << endl;

		m_MovementSouth = false;
	}
}