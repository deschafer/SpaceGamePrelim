
#include "MapManager.h"
#include "..\Frame\MainApplication.h"
#include "MapInactive.h"
#include "GenRoomComp.h"
#include "Map.h"
#include "MapManagerThreads.h"

#include <iostream>
#include <ctime>
#include <map>
#include <chrono>

#define DEBUG_CORRIDOR_VERTICAL

using namespace std;

typedef pair<MapCoordinate, MapCoordinate> SideA;

static const int CellWidthSrc = 32;
static const int CellHeightSrc = 32;
static const int MapSizeW = 100;
static const int MapSizeH = 100;
static const int HorizontalMovementSpeed = 16;
static const int VerticalMovementSpeed = 16;
static const int VisibleHorizonBufferSize = 2;
static const int VisibleVerticalBufferSize = 2;
static const int CenterMapArrayIndex = 8;
static const int ComparisonSideWidth = 6;

static const int ColumnWidth1 = 6;
static const int ColumnWidth2 = 8;
static const int ColumnWidth3 = 10;
static const int ColumnWidth4 = 12;
static const int ColumnWidth5 = 14;
static const int NumberColumns = MapSizeW / ColumnWidth5;
static const int ActiveCellsWidth = 1000;
static const int ActiveCellsHeight = 1000;


const static string WallCornerRight = "Wall_Corner_Right";
const static string WallCornerLeft = "Wall_Corner_Left";
const static string WallSideRight = "Wall_Side_Right";
const static string WallSideLeft = "Wall_Side_Left";
const static string WallBottom = "Wall_Bottom";
const static string Default = "Wall";
const static string WallTopGroup = "Wall_Top";
const static string FloorGroup = "Floors";

static const string DefaultMapStr = "Default";

typedef std::pair<int, int> Coord;

MapManager* MapManager::m_Instance = nullptr;

void ShiftAllArrayCellsLeft(MapObject*** Array, int X, int Y)
{
	for (int i = 1; i < X; i++)
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
bool IsHorizLink(MapDirection Link)
{
	switch (Link)
	{
	case MapDirection::North:
		return false;
		break;
	case MapDirection::Northeast:
		return false;
		break;
	case MapDirection::East:
		return true;
		break;
	case MapDirection::Southeast:
		return false;
		break;
	case MapDirection::South:
		return false;
		break;
	case MapDirection::Southwest:
		return false;
		break;
	case MapDirection::West:
		return true;
		break;
	case MapDirection::Northwest:
		return false;
		break;
	default:
		break;
	}
	return false;
}
bool IsVertiLink(MapDirection Link)
{
	switch (Link)
	{
	case MapDirection::North:
		return true;
		break;
	case MapDirection::Northeast:
		return false;
		break;
	case MapDirection::East:
		return false;
		break;
	case MapDirection::Southeast:
		return false;
		break;
	case MapDirection::South:
		return true;
		break;
	case MapDirection::Southwest:
		return false;
		break;
	case MapDirection::West:
		return false;
		break;
	case MapDirection::Northwest:
		return false;
		break;
	default:
		break;
	}
	return false;
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
	m_Rows = MapSizeH;			// Get a count of columns
	m_Columns = MapSizeW;		// Get a count of rows

	// Visible object array is the cells that fill the screen
	// and is therefore the appropriate size of the screen, plus a buffer of 2 cells
	// on for all the sides.
	int ResolutionX = MainApplication::Instance()->GetWndWidth();
	int ResolutionY = MainApplication::Instance()->GetWndHeight();

	m_Instance = this;
	m_ActiveMap->Generate();

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
}

MapManager::~MapManager()
{
	// Should deconstruct all maps
	// Deconstruct all data structures
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
}

//
// Update()
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

	// Handles all map correction
	CullMap();
	
	// Gets input from the user
	HandleInput();

	// Update our cells -- WILL use a thread
	// UpdateCells();
}

//
// UpdateCells()
// Updates the cells of the loaded active maps
//
void MapManager::UpdateCells()
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
					int PositionX = (i)* m_CellWidth + m_PixelOffsetX + (MapPositionOffsetX * m_CellWidth);
					int PositionY = (j)* m_CellHeight + m_PixelOffsetY + (MapPositionOffsetY * m_CellHeight);

					if ((Object = m_VisibleObjectArray[i][j]) &&
						PositionX + m_CellWidth >= 0 &&
						PositionX < m_ActiveWndWidth &&
						PositionY + m_CellHeight >= 0 &&
						PositionY < m_ActiveWndHeight)
						Object->Update();
				}
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
// used as a test function only -- currently defunct
//
void MapManager::ResetMap()
{
}

//
// SetLink()
//
//
void MapManager::SetLink(Map* NewMap)
{
	MapCoordinate Coor = NewMap->GetCoordinate();
	Map* Test = nullptr;
	// Check all surrounding cells in the map for neighbors

	// Here also physically connect the maps via corridors

	// Check north
	if (Test = m_CoordinateMaps[Coord(Coor.GetPositionX(), Coor.GetPositionY() + 1)])
	{
		// Set the link between the two maps
		NewMap->SetLink(MapDirection::North, Test);
		Test->SetLink(MapDirection::South, NewMap);

		ThreadGenerator(
			new CorridorThreadInformation(
				NewMap, 
				Test, 
				MapDirection::North, 
				this), 
			MapManagerThreadType::MapCorridorGenerate
		);
		//ConnectTwoMaps(Test, NewMap, MapDirection::North);
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
	ThreadGenerator(NewMap, MapManagerThreadType::MapGenerate);
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

			cout << "ActiveMap " << m_ActiveMap->GetCoordinate().GetPositionX() << " " << m_ActiveMap->GetCoordinate().GetPositionY() << endl;

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

			cout << "ActiveMap " << m_ActiveMap->GetCoordinate().GetPositionX() << " " << m_ActiveMap->GetCoordinate().GetPositionY() << endl;


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
			cout << "ActiveMap " << m_ActiveMap->GetCoordinate().GetPositionX() << " " << m_ActiveMap->GetCoordinate().GetPositionY() << endl;


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

			cout << "ActiveMap " << m_ActiveMap->GetCoordinate().GetPositionX() << " " << m_ActiveMap->GetCoordinate().GetPositionY() << endl;


			// Make sure the flag is set
			m_MapNeedsSwapping = true;
			m_VertiMovementSwapped = true;

		}

		cout << "North -- X:" << m_OffsetX << " Y: " << m_OffsetY << endl;

		m_MovementSouth = false;
	}
}

//
// ConnectTwoMaps()
// 
// Note: Link is from map1 to map2
//
void MapManager::ConnectTwoMaps(Map* Map1, Map* Map2, MapDirection LinkBetween)
{

	// Check the link between
	if (IsVertiLink(LinkBetween))
	{
		// Travelling north
		if (LinkBetween == MapDirection::North)
		{
			// Find the starting poisiton
			ConnectMapsVertically(Map2, Map1);
		}
		// Travelling south
		else
		{

		}
	}

	// If vertical link, then determine relative placement of map1, map2
	// Determine a suitable starting position for map1
	// Determine a suitable ending position for map2
	// Get a distance between these two points
	// Generate the corridor

}

//
// ConnectMapsVertically()
//
// Note: Map1 should ALWAYS be ABOVE map2, otherwise undefined behavior
//
void MapManager::ConnectMapsVertically(Map* Map1, Map* Map2)
{
	MapRoom* AboveRoom;
	int AboveOffsetX = 0;
	int AboveOffsetY = 0;
	int AboveRoomSideDifference = 0;
	MapRoom* BelowRoom;
	int BelowOffsetX = 0;
	int BelowOffsetY = 0;
	int BelowRoomSideDifference = 0;


	MapCoordinate BeginningPos;
	SideA* AboveSide;
	MapCoordinate EndingPos;
	SideA* BelowSide;

	int ShorterXLocation = 0;
	bool BelowIsShorter = false;

	
	// Connect each room located at each column
	for (int i = 0; i < NumberColumns; i++)
	{
		BelowIsShorter = false;

		// Get a room from each map
		AboveRoom = Map1->GetRoomXFromColumnY(0, i, AboveOffsetX, AboveOffsetY, true);	// Last room in this column
		BelowRoom = Map2->GetRoomXFromColumnY(0, i, BelowOffsetX, BelowOffsetY);		// First room in this column

		// Ask the room above for a suitable side to set as a beginning tile for the end point
		// for this corridor.
		// Get a wall from the indicated side
		AboveSide = AboveRoom->GetFacingFromSide(Side::BOTTOM);
		// Do the same for the current room on the top side
		BelowSide = BelowRoom->GetFacingFromSide(Side::TOP);

		// Next find the smallest side
		// If this wall with min size exists
		if (AboveSide)
		{
			AboveRoomSideDifference = (AboveSide->first.GetPositionX() - AboveSide->second.GetPositionX());
		}
		else return;
		if (BelowSide)
		{
			BelowRoomSideDifference = (BelowSide->first.GetPositionX() - BelowSide->second.GetPositionX());
		}
		else return;

		// Handle the shorter side first
		if (abs(BelowRoomSideDifference) > abs(AboveRoomSideDifference))
		{
			BeginningPos = MapCoordinate(
				AboveSide->first.GetPositionX() - (AboveRoomSideDifference) / 2 + AboveOffsetX, 
				AboveSide->first.GetPositionY()
			);
			ShorterXLocation = BeginningPos.GetPositionX(); // Saving this x pos with respect to column offset
		}
		else
		{
			EndingPos = MapCoordinate(
				BelowSide->first.GetPositionX() - (BelowRoomSideDifference) / 2 + BelowOffsetX, 
				BelowSide->first.GetPositionY()
			);
			ShorterXLocation = EndingPos.GetPositionX(); // Saving this x pos with respect to column offset
			BelowIsShorter = true;
		}

		// Then we need to process the room above
		if (BelowIsShorter)
		{
			// Standardize the coordinates so we can compare them
			int XPos1 = AboveSide->first.GetPositionX() + AboveOffsetX;
			int XPos2 = AboveSide->second.GetPositionX() + AboveOffsetX;

			// If the x coord of the shorter is within the range of this side
			// if so, that is the new point
			if ((XPos1 < ShorterXLocation && ShorterXLocation < XPos2) ||
				(XPos1 > ShorterXLocation && ShorterXLocation > XPos2))
			{
				BeginningPos = MapCoordinate(ShorterXLocation, AboveSide->first.GetPositionY());
			}
			else BeginningPos = MapCoordinate(AboveSide->first.GetPositionX() - (AboveRoomSideDifference) / 2 + AboveOffsetX, AboveSide->first.GetPositionY());

		}
		// Then we need to process the room below
		else
		{
			int XPos1 = BelowSide->first.GetPositionX() + BelowOffsetX;
			int XPos2 = BelowSide->second.GetPositionX() + BelowOffsetX;

			// If the x coord of the shorter is within the range of this side
			// if so, that is the new point
			if ((XPos1 < ShorterXLocation && ShorterXLocation < XPos2) ||
				(XPos1 > ShorterXLocation && ShorterXLocation > XPos2))
			{
				EndingPos = MapCoordinate(ShorterXLocation, BelowSide->first.GetPositionY());
			}
			else EndingPos = MapCoordinate(BelowSide->first.GetPositionX() - (BelowRoomSideDifference) / 2 + BelowOffsetX, BelowSide->first.GetPositionY());
		}
		// The start and ending position are now found

#ifdef DEBUG_CORRIDOR_VERTICAL
		vector<string> Textures;
		Textures.push_back("Test");

		int X1 = EndingPos.GetPositionX();
		int Y1 = EndingPos.GetPositionY() + BelowOffsetY;
		MapCell* Cell1 = new MapInactive(Textures, MapCoordinate(0, 0), Cell::Floor);

		//Map2->SetCell(X1, Y1, Cell1);

		int X2 = BeginningPos.GetPositionX();
		int Y2 = BeginningPos.GetPositionY() + AboveOffsetY;
		MapCell* Cell2 = new MapInactive(Textures, MapCoordinate(0, 0), Cell::Floor);

		//Map1->SetCell(X2, Y2, Cell2);

#endif // DEBUG_CORRIDOR_VERTICAL

		GenerateVerticalCorridorBetween(Map1, MapCoordinate(BeginningPos.GetPositionX(), BeginningPos.GetPositionY() + AboveOffsetY), Map2, MapCoordinate(EndingPos.GetPositionX(), EndingPos.GetPositionY() + BelowOffsetY));
	}
}

//
// GenerateVerticalCorridorBetween()
//
// Note: Goes from the top, at Pos1, to the bottom, at Pos2
//
void MapManager::GenerateVerticalCorridorBetween(Map* Map1, MapCoordinate Pos1, Map* Map2, MapCoordinate Pos2)
{
	enum class Movement { LEFT, RIGHT, UP, DOWN };

	float DistanceBetween = (int)(MapSizeH - Pos1.GetPositionY() + Pos2.GetPositionY());

	int CurrentX = Pos1.GetPositionX();
	int CurrentY = Pos1.GetPositionY();
	int DistanceX = abs(Pos1.GetPositionX() - Pos2.GetPositionX());	// Dependent on vertical or horiz corridors
	int MidPointY = (CurrentY + ((int)(DistanceBetween = ceil(DistanceBetween / 2)) ? (int)DistanceBetween : 1));
	int SwitchedCurrentY = 0;
	bool Try = false;
	bool MovementY = false;
	bool MovementX = false;
	bool BeginX = false;
	bool EndX = false;
	bool MapSwitched = false;
	Movement Direction;
	vector <MapCoordinate> TexturedCoords;
	Map* PrevMap = nullptr;
	Map* ActiveMap = Map1;

	if (Pos2.GetPositionX() > Pos1.GetPositionX()) Direction = Movement::RIGHT;
	else Direction = Movement::LEFT;

	// Adjusting the x transition 
	// so that no conflicts occur

	if (MidPointY >= MapSizeH) MidPointY = MapSizeH - 1;

	int BegMidPoint = MidPointY;
	while (ActiveMap->GetCell(CurrentX, MidPointY) != nullptr)
	{
		MidPointY--;
		if (MidPointY <= Pos1.GetPositionY())
		{
			MidPointY = BegMidPoint;
			cout << "OCCURRED: CURRENTMAP: " << Map1->GetCoordinate().GetPositionX() << " " << Map1->GetCoordinate().GetPositionY() << endl << endl;
			break;
		}
	}

	// Loading a floor texture
	vector<string> Textures;
	Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));

	// Setting the begging cell
	vector<string> TexturesT;
	TexturesT.push_back(WallSideRight);
	CheckCell(
		Map1,
		Map2,
		ActiveMap,
		MapCoordinate(CurrentX - 1, CurrentY),
		TexturesT,
		Cell::Wall_Right,
		TexturedCoords,
		(Direction == Movement::LEFT));

	TexturesT.clear();
	TexturesT.push_back(WallSideLeft);
	CheckCell(
		Map1,
		Map2,
		ActiveMap,
		MapCoordinate(CurrentX + 1, CurrentY),
		TexturesT,
		Cell::Wall_Left,
		TexturedCoords,
		(Direction == Movement::RIGHT));

	CheckCell(
		Map1,
		Map2,
		ActiveMap,
		MapCoordinate(CurrentX, CurrentY),
		Textures,
		Cell::Floor,
		TexturedCoords,
		false);

	// Until we reach our end destination
	while (CurrentY != Pos2.GetPositionY())
	{
		// If we need to move x-wise
		if ((CurrentY == MidPointY &&
			(Pos1.GetPositionX() != Pos2.GetPositionX()) &&
			DistanceX) || Try)
		{
			bool Taken = false;
			// We will try every turn until we compete the x-movement
			Try = true;

			// First search our texured sides to verify 
			// Verifying that the area is clear for the corridor
			for (size_t i = 0, Pos = CurrentX; i < DistanceX; i++)
			{
				if (Direction == Movement::LEFT) Pos--;
				else Pos++;
				// If this cell is not occupied by an external cell
				// and it is not one of our drawn texture cells
				if (ActiveMap->GetCell(Pos,CurrentY) != nullptr && (TexturedCoords.empty() ||
					!(TexturedCoords[TexturedCoords.size() - 1].GetPositionY() == CurrentY &&
						TexturedCoords[TexturedCoords.size() - 1].GetPositionX() == Pos)))
				{
					// This path is taken
					Taken = true;
					break;
				}
			}

			// If the area is taken, then increment y and try next time
			if (Taken)
			{
				CurrentY++;
				MovementY = true;
			}
			// Otherwise move in correct x pos
			else if (Direction == Movement::RIGHT)
			{
				if (CurrentX == Pos1.GetPositionX()) BeginX = true;
				CurrentX++;
				if (CurrentX == Pos2.GetPositionX()) EndX = true;
				DistanceX--;
				MovementX = true;
			}
			else
			{
				if (CurrentX == Pos1.GetPositionX()) BeginX = true;
				CurrentX--;
				if (CurrentX == Pos2.GetPositionX()) EndX = true;
				DistanceX--;
				MovementX = true;
			}
			// Try until we are done moving x-wise
			if (!DistanceX) Try = false;
		}
		// Increment Y be default
		else
		{
			CurrentY++;
			MovementY = true;
		}

		// When we need to swap active maps
		if (CurrentY >= MapSizeH)
		{
			ActiveMap = Map2;
			CurrentY = 0;
			MidPointY %= MapSizeH;
		}

		MapCell* NewCell = new MapInactive(Textures, MapCoordinate(CurrentX, CurrentY), Cell::Floor);

		// Adding the corridor to the array
		ActiveMap->SetCell(CurrentX, CurrentY, NewCell);
		// Add to the corridor array
		ActiveMap->SetCorridorCell(CurrentX, CurrentY, NewCell);

		// If there was a y movement,  then we add to the 
		// cells to the left and right to add the walls to
		// encompass the corridor
		if (MovementY)
		{
			vector<string> Textures;
			Textures.push_back(WallSideRight);
			CheckCell(
				Map1,
				Map2,
				ActiveMap, 
				MapCoordinate(CurrentX - 1, CurrentY),
				Textures,
				Cell::Wall_Right,
				TexturedCoords,
				(Direction == Movement::LEFT));

			Textures.clear();
			Textures.push_back(WallSideLeft);
			CheckCell(
				Map1,
				Map2,
				ActiveMap,
				MapCoordinate(CurrentX + 1, CurrentY),
				Textures,
				Cell::Wall_Left,
				TexturedCoords,
				(Direction == Movement::RIGHT));

			MovementY = false;
		}
		// Otherwise, if a x movement occurred
		else if (MovementX)
		{
			// The beg of the x-movement
			// DIRECTION MATTERS
			if (BeginX)
			{
				int Change = 0;
				int LessChange = 0;
				if (Direction == Movement::LEFT)
				{
					Change += 2;
					LessChange++;
				}
				else
				{
					Change -= 2;
					LessChange--;
				}

				vector<string> Textures;
				Textures.push_back(WallBottom);
				CheckCell(
					Map1,
					Map2,
					ActiveMap,
					MapCoordinate(CurrentX + LessChange, CurrentY + 1),
					Textures,
					Cell::Wall_Bottom,
					TexturedCoords,
					false);

				CheckCell(
					Map1,
					Map2,
					ActiveMap, MapCoordinate(CurrentX, CurrentY + 1),
					Textures,
					Cell::Wall_Bottom,
					TexturedCoords,
					false);

				Textures.clear();
				(Direction == Movement::LEFT) ? Textures.push_back(WallCornerRight) : Textures.push_back(WallCornerLeft);
				CheckCell(
					Map1,
					Map2,
					ActiveMap,
					MapCoordinate(CurrentX + Change, CurrentY + 1),
					Textures, (Direction == Movement::LEFT) ?
					Cell::Wall_Corner_Right_Bottom :
					Cell::Wall_Corner_Left_Bottom,
					TexturedCoords,
					false);

				Textures.clear();
				Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
				CheckCell(
					Map1,
					Map2,
					ActiveMap,
					MapCoordinate(CurrentX, CurrentY - 1),
					Textures,
					Cell::Wall_Top,
					TexturedCoords,
					false);
			}
			// The end of the x-movement
			// DIRECTION MATTERS
			if (EndX)
			{
				int Change = 0;

				if (Direction == Movement::LEFT) Change = -1;
				else Change = 1;

				vector<string> Textures;
				(Change > 0) ? Textures.push_back(WallSideLeft) : Textures.push_back(WallSideRight);
				CheckCell(
					Map1,
					Map2,
					ActiveMap,
					MapCoordinate(CurrentX + Change, CurrentY),
					Textures,
					(Change > 0) ? Cell::Wall_Left : Cell::Wall_Right,
					TexturedCoords,
					false);

				CheckCell(
					Map1,
					Map2,
					ActiveMap,
					MapCoordinate(CurrentX + Change, CurrentY - 1),
					Textures,
					(Change > 0) ? Cell::Wall_Left : Cell::Wall_Right,
					TexturedCoords,
					false);

				Textures.clear();
				Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
				CheckCell(
					Map1,
					Map2,
					ActiveMap,
					MapCoordinate(CurrentX, CurrentY - 1),
					Textures,
					Cell::Wall_Top,
					TexturedCoords,
					false);
			}
			// Adding to the cells above and below this current cell
			// Does not matter with the direction of the x corridor movement
			else if (!BeginX && !EndX)
			{
				vector<string> Textures;
				Textures.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
				CheckCell(
					Map1,
					Map2,
					ActiveMap,
					MapCoordinate(CurrentX, CurrentY - 1),
					Textures,
					Cell::Wall_Top,
					TexturedCoords,
					false);

				Textures.clear();
				Textures.push_back(WallBottom);
				CheckCell(
					Map1,
					Map2,
					ActiveMap,
					MapCoordinate(CurrentX, CurrentY + 1),
					Textures,
					Cell::Wall_Bottom,
					TexturedCoords,
					false);
			}
			EndX = false;
			BeginX = false;
			MovementX = false;
		}
	}

	if (CurrentX != Pos2.GetPositionX())
	{

#ifdef DEBUG_CORRIDOR_VERTICAL
		vector<string> Textures;
		Textures.push_back("Test2");

		MapCell* Cell1 = new MapInactive(Textures, MapCoordinate(0, 0), Cell::Floor);

		Map1->SetCell(Pos1.GetPositionX(), Pos1.GetPositionY(), Cell1);

		Textures.clear();
		Textures.push_back("Explosion");

		Cell1 = new MapInactive(Textures, MapCoordinate(0, 0), Cell::Floor);

		Map2->SetCell(Pos2.GetPositionX(), Pos2.GetPositionY(), Cell1);

#endif // DEBUG_CORRIDOR_VERTICAL
		cout << "Could not find the end to connect this map\n\n\n\n";
	}
}

//
// CheckCell()
// 
//
void MapManager::CheckCell(
	Map* MapAbove,
	Map* MapBelow,
	Map* ActiveMap,
	MapCoordinate CellPosition, 
	vector<string> Textures, 
	Cell CellType,
	vector<MapCoordinate> DrawnCells,
	bool Right)
{
	// If not within this map, and within the map above
	if (CellPosition.GetPositionY() - 1 < 0)
	{
		MapAbove->CheckCell(MapCoordinate(CellPosition.GetPositionX(), MapSizeH - 1),
			Textures,
			CellType,
			DrawnCells,
			Right);
	}
	else if (CellPosition.GetPositionY() + 1 >= MapSizeH)
	{
		MapBelow->CheckCell(MapCoordinate(CellPosition.GetPositionX(), 0),
			Textures,
			CellType,
			DrawnCells,
			Right);
	}
	else
	{
		ActiveMap->CheckCell(MapCoordinate(CellPosition.GetPositionX(), CellPosition.GetPositionY()),
			Textures,
			CellType,
			DrawnCells,
			Right);
	}
}

//
// ConnectMapsHorizontally()
// Connects the two desired maps horizontally
// Note: Map1 should be on the left of Map2.
void MapManager::ConnectMapsHorizontally(Map* MapLeft, Map* MapRight)
{
	MapRoom* Room = nullptr;
	int OffsetX = 0;
	int OffsetY = 0;

	// Attempt to connect each of the rooms in the first row of the right room to the left rooms in the 
	// left map
	for (int i = 0; ; i++)
	{
		if (Room = MapRight->GetRoomXFromColumnY(i, 0, OffsetX, OffsetY))
		{
			ConnectRoomsHoriz(MapLeft, MapRight, Room, MapCoordinate(OffsetX, OffsetY));
		}
		else break;
	}

}

//
// ConnectRoomsHoriz()
// 
//
void MapManager::ConnectRoomsHoriz(Map* MapLeft, Map* MapRight, MapRoom* RightRoom, MapCoordinate RightRoomPos)
{
	vector<pair<MapRoom*, MapCoordinate>> CandidateRooms;	// Holds a mapRoom and its starting topleft pos
	vector<int> CandidateIndices;
	MapRoom* LeftRoom = nullptr;
	int RoomHeight = RightRoom->GetHeight();
	int RoomEnd = RoomHeight + RightRoomPos.GetPositionY();
	int LeftRoomEnd = 0;
	int LeftRoomX = 0;
	int LeftRoomBeg = 0;
	int MaxCol = MapSizeH / ColumnWidth5;

	for (size_t i = 0; ; i++)
	{
		// Get the left room for the indexed position
		LeftRoom = MapLeft->GetRoomXFromColumnY(i, MaxCol, LeftRoomX, LeftRoomBeg);

		// First if it is nullptr break
		if (!LeftRoom) break;
		
		LeftRoomEnd = LeftRoomBeg + LeftRoom->GetHeight();

		// If this room is within the bounds of the room next to it,
		// we add it as a candidate for corridor generation
		if (LeftRoomBeg <= RoomEnd && LeftRoomEnd > RightRoomPos.GetPositionY())
		{
			CandidateRooms.push_back(pair<MapRoom*, MapCoordinate>(LeftRoom, MapCoordinate(LeftRoomX, LeftRoomBeg)));
			CandidateIndices.push_back(i);
		}
		// If we have passed the margin, stop looking
		else if (LeftRoomBeg > RoomEnd) break;
	}

	// If there are no rooms, then we do not connect
	// Should Only happen rarely
	if (CandidateRooms.empty()) return;


	// Pick one of these rooms that preferable does not already have a link
	// Pick to one closed to the middle of this room

	// If there is only one room present, then we will connect to the room regardless
	if (CandidateRooms.size() == 1)
	{
		FindCandidateSidePositions(
			MapLeft,
			MapRight,
			RightRoom,
			RightRoomPos.GetPositionX(),
			RightRoomPos.GetPositionY(),
			CandidateRooms[0].first,
			CandidateRooms[0].second.GetPositionX(),
			CandidateRooms[0].second.GetPositionY(),
			MaxCol);
	}
	// If more than one room available, the anwser is more difficult
	else
	{
		vector<bool> Connections;
		int ConnectionlessCount = 0;
		int ConnectionlessIndex = -1;	// Holds the index of a connectionless room -- intended for one connectionless room only
		MapRoom* SelectedRoom = nullptr;

		for (size_t i = 0; i < CandidateRooms.size(); i++)
		{
			// Adding all of the connections for the candiate rooms
			if (CandidateRooms[i].first->ConnectedToRoom(Side::RIGHT))
			{
				Connections.push_back(true);
			}
			else
			{
				Connections.push_back(false);
				ConnectionlessCount++;
				ConnectionlessIndex = i;
			}
		}

		// If no rooms are not connected to
		if (ConnectionlessCount == 0)
		{
			return;
		}
		// If any of the rooms given does not have a connection, connect to one of those
		// If one connectionless room, connect to that one
		else if (ConnectionlessCount == 1)
		{
			FindCandidateSidePositions(
				MapLeft,
				MapRight,
				RightRoom,
				RightRoomPos.GetPositionX(),
				RightRoomPos.GetPositionY(),
				CandidateRooms[ConnectionlessIndex].first,
				CandidateRooms[ConnectionlessIndex].second.GetPositionX(),
				CandidateRooms[ConnectionlessIndex].second.GetPositionY(),
				MaxCol);
		}
		// More than one possible room
		else if (ConnectionlessCount)
		{
			int Index = 0;
			// Find the connectionless room that is above
			for (size_t i = 0; i < Connections.size(); i++)
			{
				// Due to how the rooms are found, from up to down, we can
				// always assume that the first connectionless index is the highest room
				if (!Connections[i])
				{
					Index = i;
					break;
				}
			}
			FindCandidateSidePositions(
				MapLeft, 
				MapRight,
				RightRoom,
				RightRoomPos.GetPositionX(),
				RightRoomPos.GetPositionY(),
				CandidateRooms[Index].first,
				CandidateRooms[ConnectionlessIndex].second.GetPositionX(),
				CandidateRooms[ConnectionlessIndex].second.GetPositionY(),
				MaxCol);
		}
	}
}

//
// FindCandidateSidePositions()
//
//
void MapManager::FindCandidateSidePositions(
	Map* LeftMap, 
	Map* RightMap, 
	MapRoom* RightRoom, 
	int RightOffsetX, 
	int RightOffsetY, 
	MapRoom* LeftRoom, 
	int LeftOffsetX, 
	int LeftOffsetY, 
	int ColumnNumber)
{
	int YStartPos = 0;
	int Difference = 0;
	pair<MapCoordinate, MapCoordinate>* GetLocationBegSide;
	pair<MapCoordinate, MapCoordinate>* TempSide = nullptr;
	pair<MapCoordinate, MapCoordinate>* GetLocationEndSide;

	// First get a minor side from the major left side for the beginning room
	GetLocationBegSide = RightRoom->GetFacingFromSide(Side::LEFT);

	// Then select a YPos in here for the actual start.
	Difference = GetLocationBegSide->first.GetPositionY() - GetLocationBegSide->second.GetPositionY();

	// This will a part of this side with variation
	YStartPos = rand() % (Difference - 1) + GetLocationBegSide->second.GetPositionY() + 1;

	// Now get the closest end position to here
	// First, find a ending side, if it exists, otherwise get the closest
	int LeftPositionBeg = 0;
	int RightPositionBeg = YStartPos - ComparisonSideWidth / 2 + RightOffsetY;
	int LeftPositionEnd = 0;
	int RightPositionEnd = YStartPos + ComparisonSideWidth / 2 + RightOffsetY;

	int RightPositionBaseBeg = RightOffsetY;
	int RightPositionBaseEnd = RightRoom->GetHeight() + RightOffsetY;

	int LeftRoomOffsetX = ColumnNumber * ColumnWidth5;
	int LeftRoomColumnOffsetX = LeftOffsetX;
	int LeftRoomOffsetY = LeftOffsetY;

	vector<pair<MapCoordinate, MapCoordinate>> WideCandidateSides;
	vector<pair<MapCoordinate, MapCoordinate>> CandidateSides;
	bool PerfectFit = false;

	for (size_t Index = 0; ; Index++)
	{
		TempSide = LeftRoom->GetFacingFromSideIndexed(Side::RIGHT, Index);
		if (!TempSide) break;

		LeftPositionBeg = TempSide->first.GetPositionY() + LeftRoomOffsetY;
		LeftPositionEnd = TempSide->second.GetPositionY() + LeftRoomOffsetY;

		// Determine if this side is within the desired position
		// First case - side is within a set param, like 2 blocks, of the starting corridor position
		if (LeftPositionBeg <= RightPositionEnd && LeftPositionEnd > RightPositionBeg)
		{
			CandidateSides.push_back(*TempSide);
		}
		// Second case - side is within the bounds of the entire side of where the starting corridor pos. is
		else if (LeftPositionBeg <= RightPositionBaseEnd && LeftPositionEnd >= RightPositionBaseBeg)
		{
			WideCandidateSides.push_back(*TempSide);
		}
		// Third case - If the exact Y position of the starting corridor position is located within this side
		if (LeftPositionBeg <= (YStartPos + RightOffsetY) && LeftPositionEnd >= (YStartPos + RightOffsetY))
		{
			PerfectFit = true;
			// We have a fitting side, so leave
			break;
		}
	}

	// Setting a formal MapCoord for the starting room pos.
	MapCoordinate LeftRoomPoint(
		RightOffsetX + GetLocationBegSide->first.GetPositionX(),
		RightOffsetY + YStartPos);
	MapCoordinate RightRoomPoint(0, 0);

	if (PerfectFit)
	{
		// Good to go, we can set this ypos as the ending pos.

		// Need to verify we are not at the absolute end of this side
		// Since we are on the Right side of the left room,
		// Verify that this point is not equal to a point in the room. 
		// If it is, incr or decr appropriately.

		int Pos = YStartPos + RightOffsetY;

		// Upper part of this side section
		// Determining if this point is at the endge of this side, which
		// usually is not a valid place for a corridor
		if (Pos == TempSide->first.GetPositionY() + LeftRoomOffsetY)
		{
			// Move downwards one, increment by 1
			Pos++;
		}
		else if (Pos == TempSide->second.GetPositionY() + LeftRoomOffsetY)
		{
			// Move upwards one, increment by 1
			Pos--;
		}

		// Save this position
		RightRoomPoint = MapCoordinate(
			LeftRoomOffsetX + LeftRoomColumnOffsetX + TempSide->first.GetPositionX(),
			Pos);
	}
	// If there are several sides that work
	else if (!CandidateSides.empty() || !WideCandidateSides.empty())
	{
		// Select a random side from those available
		if (!CandidateSides.empty())
		{
			int SideIndex = rand() % CandidateSides.size();
			TempSide = &CandidateSides[SideIndex];
		}
		else if (!WideCandidateSides.empty())
		{
			int SideIndex = rand() % WideCandidateSides.size();
			TempSide = &WideCandidateSides[SideIndex];
		}

		// Get a magnitude of the difference
		int SideDifference = TempSide->second.GetPositionY() - TempSide->first.GetPositionY();

		// Start at the middle of this side
		int Pos = TempSide->first.GetPositionY() + SideDifference / 2 + LeftRoomOffsetY;
		int Goal = YStartPos + RightOffsetY;
		int EndLegalSideTop = TempSide->first.GetPositionY() + LeftRoomOffsetY + 1;
		int StartLegalSideBottom = TempSide->second.GetPositionY() + LeftRoomOffsetY - 1;

		// Move to a pos as close to the other point as possible
		while (true)
		{
			if (Pos > Goal)
			{
				if (Pos - 1 > EndLegalSideTop) Pos--;
				else break;
			}
			else
			{
				if (Pos + 1 < StartLegalSideBottom) Pos++;
				else break;
			}
		}

		// Save this new position
		RightRoomPoint = MapCoordinate(
			LeftRoomOffsetX + LeftRoomColumnOffsetX + TempSide->first.GetPositionX(),
			Pos);
	}
	// If there was an ending point found
	if (TempSide != nullptr)
	{
		int RightRoomXPos = RightOffsetX;
		int LeftRoomXPos = LeftRoomOffsetX + LeftRoomColumnOffsetX + LeftRoom->GetWidth();

		// Adds the link to the left room
		LeftRoom->AddLinkedRoom(Side::RIGHT, RightRoom);
		// Adds the link to the right room
		RightRoom->AddLinkedRoom(Side::LEFT, LeftRoom);

		GenerateHorizontalCorridorBetween(
			RightMap,
			MapCoordinate(RightRoomPoint),
			LeftMap,
			MapCoordinate(LeftRoomPoint)
		);
	}
	else
	{
	#ifdef _DEBUG
		cout << "Corridor Horiz Gen Error: No ending point found for "
			<< RightRoom->GetRoomType()
			<< " to "
			<< LeftRoom->GetRoomType()
			<< endl;
	#endif // _DEBUG
	}
}

//
// GenerateHorizontalCorridorBetween()
//
//
void MapManager::GenerateHorizontalCorridorBetween(Map* Map1, MapCoordinate Pos1, Map* Map2, MapCoordinate Pos2)
{

}