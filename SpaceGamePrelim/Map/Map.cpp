
#include "Map.h"
#include "MapRoom.h"

#include <chrono>

static const int WidthRandomRate = 5;
static const int HeightRandomRate = 5;

static const int MaxRoomHeight = 20;
static const int MaxRoomWidth = 14;
static const int MinRoomWidth = 6;
static const int RoomWidthDiff = 8;
static const int RoomSizeVolatility = 5;

static const int ColumnWidth1 = 6;
static const int ColumnWidth2 = 8;
static const int ColumnWidth3 = 10;
static const int ColumnWidth4 = 12;
static const int ColumnWidth5 = 14;


using namespace std;
using namespace std::chrono;

Map::Map()
{
}

//
// Map()
// Constructor to be used with this object
//
Map::Map(string MapType, int Width, int Height, MapCoordinate Coords) :
	m_Width(Width),
	m_Height(Height),
	m_MapType(MapType),
	m_MapCoordinates(Coords)
{
	// Generating the array for this map
	m_Cells = new MapObject**[m_Height];
	for (int i = 0; i < m_Height; i++)
	{
		m_Cells[i] = new MapObject*[m_Width];
	}

	// Initializing all of the cells to nullptr
	for (int i = 0; i < m_Height; i++)
	{
		for (int j = 0; j < m_Width; j++)
		{
			m_Cells[i][j] = nullptr;
		}
	}

}

Map::~Map()
{
}


//
// GetCell()
// Gets the cell from m_Cells specefied by position
// X,Y. If not within the bounds of m_Cells,
// it will return nullptr, so check the return.
//
MapObject* Map::GetCell(int X, int Y)
{

	if (X < m_Width && X >= 0 && Y < m_Height && Y >= 0)
	{
		return m_Cells[X][Y];
	}
	else
	{
		return nullptr;
	}

}

//
// Generate()
// Generates this map objects -- may take longer 
// as this is a one-time process
//
void Map::Generate()
{
	int Width = 0;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	// Generating each column of rooms
	while (Width + MaxRoomWidth < m_Width)
	{
		GenerateRoom(Width, 0, MaxRoomWidth, MapCoordinate(0, 0));
		Width += MaxRoomWidth;
	}

	// Preformance based -- not in final version
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(t2 - t1).count();
	cout << duration << endl;
}

//
// GenerateRoom()
// Recursive function repsonsible for generating a single room within its boundaries
// Generate rooms column-wise, so it only moves downwards.
//
void Map::GenerateRoom(int OffsetX, int OffsetY, int MaxWidth, MapCoordinate Coord)
{			
	RoomProperties* Properties;
	string RoomType;
	int RoomWidth = MinRoomWidth + rand() % RoomWidthDiff;
	int RoomHeight = 0;
	MapRoom* Room;
	int xOffset;

	// We need to get a room height, first check if there is a boundary within 20 cells
	for (size_t i = OffsetY, Count = 0; i < m_Height && Count < MaxRoomHeight; i++, Count++)
	{
		RoomHeight++;
	}

	// Checking the vertical space to fit this room
	// within this map
	if (RoomHeight != MaxRoomHeight)
	{
		Properties = RoomManager::Instance()->GetRandomTypeThatFits(RoomType, RoomWidth, RoomHeight);

		if (Properties == nullptr)
		{
		#ifdef _DEBUG
			//cout << "No Room for this Room To be Placed" << endl;
		#endif // _DEBUG
			// Do not generate a room
			return;
		}
		// Adding random variation to the size
		if(RoomHeight > 8) RoomHeight -= rand() % RoomSizeVolatility;
	}
	else
	{
		Properties = RoomManager::Instance()->GetRandomTypeThatFits(RoomType, RoomWidth, MaxRoomHeight);
		
		if (Properties == nullptr)
		{
		#ifdef _DEBUG
			//cout << "No Room for this Room To be Placed" << endl;
		#endif // _DEBUG
		}
		else
		{
			RoomHeight = Properties->m_MinHeight + rand() % 5;
		}
	}

	// Creating the new room
	Room = new MapRoom(RoomType, RoomWidth, RoomHeight);
	Room->Generate();
	
	// Adding a offset within the column so all the rooms
	// do not start at the same x position
	if (RoomWidth < MaxRoomWidth)
	{
		int Difference = MaxRoomWidth - RoomWidth;

		xOffset = rand() % Difference;
	}

	// Now since the space has been allocated, and the room has been 
	// generated, let's place the object in the array
	for (size_t IndexX = OffsetX + xOffset, MagX = 0;
		MagX < RoomWidth;
		MagX++, IndexX++)
	{
		for (size_t IndexY = OffsetY, MagY = 0;
			MagY < RoomHeight;
			MagY++, IndexY++)
		{
			m_Cells[IndexX][IndexY] = Room->GetCell(MagX, MagY);
		}
	}

	// Generating the next room below this one
	GenerateRoom(OffsetX, OffsetY + RoomHeight, MaxWidth, MapCoordinate(Coord.GetPositionX(), Coord.GetPositionY() + 1));
}