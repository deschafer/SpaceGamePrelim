
#include "MapManager.h"
#include "..\Frame\MainApplication.h"

#include <iostream>
#include <ctime>
#include <map>
#include <chrono>

//#define Windows		// Usage of windows threads
#define CrossPlatform

#ifdef Windows
#include <Windows.h>
#endif // Windows


using namespace std;

static const int CellWidthSrc = 32;
static const int CellHeightSrc = 32;
static const int MapSizeW = 100;
static const int MapSizeH = 100;
static const int HorizontalMovementSpeed = 8;
static const int VisibleHorizonBufferSize = 2;
static const int VisibleVerticalBufferSize = 2;

static const string DefaultMapStr = "Default";

typedef std::pair<int, int> Coord;

MapManager* MapManager::m_Instance = nullptr;

void ShiftAllArrayCellsLeft(MapObject*** Array, int X, int Y);
void ShiftAllArrayCellsRight(MapObject*** Array, int X, int Y);

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

MapManager::MapManager() :
	m_ActiveWndHeight(0),
	m_ActiveWndWidth(0),
	m_CellHeight(CellWidthSrc),
	m_CellWidth(CellHeightSrc),
	m_Init(true),
	m_OffsetX(0),
	m_OffsetY(0),
	m_MapOffsetX(0),
	m_MapOffsetY(0),
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

	// Visible object array is the cells that fill the screen
	// and is therefore the appropriate size of the screen, plus a buffer of 2 cells
	// on for all the sides.

	int ResolutionX = MainApplication::Instance()->GetWndWidth();
	int ResolutionY = MainApplication::Instance()->GetWndHeight();

	
	m_VisibleColumns = ceil(static_cast<float>(ResolutionX / m_CellWidth));// + VisibleHorizonBufferSize;
	m_VisibleRows = ceil(static_cast<float>(ResolutionY / m_CellHeight));// +VisibleVerticalBufferSize;
	m_VisibleColumnsWithoutBuffer = m_VisibleColumns;// -VisibleHorizonBufferSize;
	m_VisibleRowsWithoutBuffer = m_VisibleRows;// -VisibleVerticalBufferSize;

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

	for (int i = 0; i < m_VisibleColumns; i++)
	{
		for (int j = 0; j < m_VisibleRows; j++)
		{
			m_VisibleObjectArray[i][j] = m_ActiveMap->GetCell(i,j);
		}
	}
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

	for (int i = 0; i < m_VisibleColumns; i++)
	{
		for (int j = 0; j < m_VisibleRows; j++)
		{
			if (m_VisibleObjectArray[i][j] != nullptr)
			{
				m_VisibleObjectArray[i][j]->Draw(MapCoordinate((i)* m_CellWidth + m_PixelOffsetX, (j)* m_CellHeight));
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

	HandleInput();

	CullMap();


	// If there are portions on the screen not for the active map,
	// Then get those cells, and add those to the active object array
	
	// Once the current map has less displayed than another map, that one becomes the active map




	// Get all of our active map cells
	for (int i = 0, MapI = 0; i < m_VisibleColumns; i++, MapI++)
	{
		for (int j = 0, MapJ = 0; j < m_VisibleRows; j++, MapJ++)
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
		//m_MapOffsetY--;
	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_UP))// && !Up)
	{
		m_PixelOffsetY += 8;
		//m_MapOffsetY++;
	}

	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_LEFT))// && !Left)
	{
		static int test = 0;

		if (m_PixelOffsetX == m_CellWidth)
		//{
		//if (++test % 10 == 0)
		{


			m_PixelOffsetX = 0;
			m_MapOffsetX--;
			m_MovementLeft = true;
		}
		//}
		m_PixelOffsetX += HorizontalMovementSpeed;

	}
	if (InputManager::Instance()->IsKeyDown(SDL_SCANCODE_RIGHT))// && !Right)
	{
		static int test = 0;


		if (m_PixelOffsetX == -m_CellWidth)
		{

		//if (++test % 10 == 0)
		//{

			m_PixelOffsetX = 0;
			m_MapOffsetX++;
			m_MovementRight = true;

		//}
		}
		m_PixelOffsetX -= HorizontalMovementSpeed;
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

	// If the x offset has become negative, then we need to get a new cell
	if (m_MovementLeft)
	{
		MapObject** LeftColumn;
		Map* LeftMap = nullptr;

		// If the offset has become negative, we will need cells from the
		// left map
		if (m_MapOffsetX < 0)
		{
			LeftMap = m_ActiveMap->GetNeighbor(MapDirection::West);
			// If this is nullptr, which should NEVER happen, then the map is not generated and abort
			if (!LeftMap) _DEBUG_ERROR("Neighbor map has not been generated");

			LeftColumn = LeftMap->GetColumn(LeftMap->GetWidth() - 1 + m_MapOffsetX);
		}
		// Otherwise, just get from this map
		else
		{
			LeftColumn = m_ActiveMap->GetColumn(m_MapOffsetX);
			cout << "Getting cell " << m_MapOffsetX << endl;

		}

		// Then we shift all elements of the visibleobject array to the right by one
		ShiftAllArrayCellsRight(m_VisibleObjectArray, m_VisibleColumns, m_VisibleRows);

		// Then we substitute the leftmost with the new column
		m_VisibleObjectArray[0] = LeftColumn;

		// Check if this map has now become the active map
		if (-m_MapOffsetX > (m_VisibleColumnsWithoutBuffer) / 2)
		{
			m_ActiveMap = LeftMap;
			m_MapOffsetX = (MapSizeW - 1) + m_MapOffsetX;
			cout << "Active map has been changed to the left\n";
		}

		cout << "left offset" << m_MapOffsetX << endl;

		// We are done with this movement
		m_MovementLeft = false;

		// Also, check for any other maps on top or below, becuase we will need some of their cells too
	}

	// To check for right movement, see if the number of displayed cells on the screen is greater than the width of the map

	if (m_MovementRight)
	{
		MapObject** RightColumn;
		Map* RightMap = nullptr;

		// If the offset has become negative, we will need cells from the
		// left map
		if (m_MapOffsetX >= (m_ActiveMap->GetWidth() - m_VisibleColumnsWithoutBuffer))
		{
			RightMap = m_ActiveMap->GetNeighbor(MapDirection::East);
			// If this is nullptr, which should NEVER happen, then the map is not generated and abort
			if (!RightMap) _DEBUG_ERROR("Neighbor map has not been generated");

			RightColumn = RightMap->GetColumn(m_MapOffsetX - (m_ActiveMap->GetWidth() - m_VisibleColumnsWithoutBuffer));
		}
		// Otherwise, just get from this map
		else
		{
			RightColumn = m_ActiveMap->GetColumn(m_MapOffsetX + m_VisibleColumnsWithoutBuffer - 1);
			cout << "Getting cell " << m_MapOffsetX + m_VisibleColumnsWithoutBuffer << endl;
		}

		// Then we shift all elements of the visibleobject array to the right by one
		ShiftAllArrayCellsLeft(m_VisibleObjectArray, m_VisibleColumns, m_VisibleRows);

		// Then we substitute the leftmost with the new column
		m_VisibleObjectArray[m_VisibleColumns - 1] = RightColumn;

		// Check if this map has now become the active map
		if ((m_MapOffsetX) >= (MapSizeW - 1) - (m_VisibleColumnsWithoutBuffer)/2)
		{
			m_ActiveMap = RightMap;
			m_MapOffsetX = m_MapOffsetX - (MapSizeW - 1);
			cout << "Active map has been changed to the right\n";
		}

		cout << "Right offset" << m_MapOffsetX << endl;

		// We are done with this movement
		m_MovementRight = false;

		// Also, check for any other maps on top or below, becuase we will need some of their cells too

	}
}