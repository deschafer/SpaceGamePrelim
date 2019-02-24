
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

	std::map<std::pair<int,int>, bool> CellLocations;

	while (Width + 14 < m_Width)
	{
		GenerateRoom(Width, 0, 14, MapCoordinate(0, 0), CellLocations);
		Width += 14;
	}

	// Preformance based -- not in final version
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(t2 - t1).count();
	cout << duration;
}
//
//
//
//
void Map::GenerateRoom(int OffsetX, int OffsetY, int MaxWidth, MapCoordinate Coord, std::map<std::pair<int, int>, bool> &CellLocations)
{

	Rect* RoomRegion;
	RoomProperties* Properties;
	string RoomType;
	int RoomWidth = MinRoomWidth + rand() % RoomWidthDiff;
	int RoomHeight = 0;
	MapRoom* Room;

	// We need to get a room height, first check if there is a boundary within 20 cells
	for (size_t i = OffsetY, Count = 0; i < m_Height && Count < MaxRoomHeight; i++, Count++)
	{
		RoomHeight++;
	}


	if (RoomHeight != MaxRoomHeight)
	{
		RoomRegion = new Rect(0, 0, RoomWidth, RoomHeight);

		Properties = RoomManager::Instance()->GetRandomTypeThatFits(RoomType, RoomWidth, RoomHeight);

		if (Properties == nullptr)
		{
			cout << "no room!!" << endl;
		}
		return;

	}
	else
	{
		Properties = RoomManager::Instance()->GetRandomTypeThatFits(RoomType, RoomWidth, MaxRoomHeight);
		
		if (Properties == nullptr)
		{
			cout << "no room!!" << endl;
		}

		RoomHeight = Properties->m_MinHeight + rand() % 5;
	}


	Room = new MapRoom(RoomType, RoomWidth, RoomHeight);
	Room->Generate();

	// Now since the space has been allocated, lets place the object in the array

	for (size_t IndexX = OffsetX, MagX = 0;
		MagX < MaxWidth;
		MagX++, IndexX++)
	{

		for (size_t IndexY = OffsetY, MagY = 0;
			MagY < RoomHeight;
			MagY++, IndexY++)
		{
			m_Cells[IndexX][IndexY] = Room->GetCell(MagX, MagY);
		}
	}

	
	GenerateRoom(OffsetX, OffsetY + RoomHeight, MaxWidth, MapCoordinate(Coord.GetPositionX(), Coord.GetPositionY() + 1), CellLocations);

}