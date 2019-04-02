
#include "MapManager.h"
#include "..\Frame\MainApplication.h"
#include "MapInactive.h"
#include "GenRoomComp.h"
#include "Map.h"
#include "MapManagerThreads.h"
#include "MapConnector.h"

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
static const int ColumnWidth1 = 6;
static const int ColumnWidth2 = 8;
static const int ColumnWidth3 = 10;
static const int ColumnWidth4 = 12;
static const int ColumnWidth5 = 14;
static const int NumberColumns = MapSizeW / ColumnWidth5;
static const int ActiveCellsWidth = 1000;
static const int ActiveCellsHeight = 1000;

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
	m_MapPhysicallyLinked(true),
	m_OffsetX(0),
	m_OffsetY(0),
	m_PixelOffsetX(0),
	m_PixelOffsetY(0),
	m_ActivelyLinkedCount(0),
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

	// Checks if any physical connections need to be made
	CheckPhysicalConnections();

	// Gets input from the user
	HandleInput();


	// Update our cells
	UpdateCells();
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
						Object->Update();
					}
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
// RemoveQueuedMap()
//
//
void MapManager::RemoveQueuedMap()
{
	m_MapPhysicallyLinked = true;
}

//
// CheckPhysicalConnections()
//
//
void MapManager::CheckPhysicalConnections()
{
	static bool FlipFlop = true;	// Ensures that we do both equally
	pair<Map*, Map*> CurrentMaps;
	MapDirection Direction;

	// If there are no maps currently being linked phsycially
	if (m_MapPhysicallyLinked && m_ActivelyLinkedCount)
	{
		m_ActivelyLinkedCount--;

		if (FlipFlop && !m_QueuedEastConnections.empty())
		{
			CurrentMaps = m_QueuedEastConnections.front();
			m_QueuedEastConnections.pop_front();
			Direction = MapDirection::East;
		}
		else if(!m_QueuedNorthConnections.empty())
		{
			CurrentMaps = m_QueuedNorthConnections.front();
			m_QueuedNorthConnections.pop_front();
			Direction = MapDirection::North;
		}
		else return;

		ThreadGenerator(
			new CorridorThreadInformation(
				CurrentMaps.first,
				CurrentMaps.second,
				Direction,
				this),
			MapManagerThreadType::MapCorridorGenerate
		);

		m_MapPhysicallyLinked = false;
	}
	// Otherwise try again next time
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
	
		m_QueuedNorthConnections.push_back(pair<Map*, Map*>(
			NewMap,
			Test
			));

		m_ActivelyLinkedCount++;
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

		m_QueuedEastConnections.push_back(pair<Map*, Map*>(
			NewMap,
			Test
			));

		m_ActivelyLinkedCount++;
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

		m_QueuedNorthConnections.push_back(pair<Map*, Map*>(
			Test,
			NewMap
			));

		m_ActivelyLinkedCount++;
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

		m_QueuedEastConnections.push_back(pair<Map*, Map*>(
			Test,
			NewMap
			));

		m_ActivelyLinkedCount++;
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
	// Travelling north
	if (LinkBetween == MapDirection::North)
	{
		// Connect the maps
		MapConnector Connector(Map2, Map1, MapDirection::North);
		Connector.ConnectMaps();
	}
	// Eastwards connection
	else if (LinkBetween == MapDirection::East)
	{
		// Connect the maps
		MapConnector Connector(Map1, Map2, MapDirection::East);
		Connector.ConnectMaps();
	}
}