
#include "Map.h"
#include "MapRoom.h"
#include "MapWall.h"
//#include "GenRoomComp.h"

#include "..\Frame\MainApplication.h"

#include <chrono>

enum class Side { TOP, RIGHT, BOTTOM, LEFT, COMPL };

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
static const int ColumnSeparatorMax = 2;
static const int ColumnSeparatorMin = 1;



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
	int Index = 0;

#ifdef _DEBUG
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif // DEBUG

	// Generating each column of rooms
	while (Width + MaxRoomWidth < m_Width)
	{
		m_Rooms.push_back(vector<MapRoom*>());	// Adds a new vector to store the rooms in this col.
		m_ColumnOffsetsX.push_back(vector<int>());
		m_ColumnOffsetsY.push_back(vector<int>());
		GenerateRoom(Width, 0, MaxRoomWidth, Index);
		Width += MaxRoomWidth;
		Index++;
	}

#ifdef _DEBUG
	// Preformance based -- not in final version
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(t2 - t1).count();
	cout << duration << endl;
#endif // _DEBUG
}

//
// GenerateRoom()
// Recursive function repsonsible for generating a single room within its boundaries
// Generate rooms column-wise, so it only moves downwards.
//
void Map::GenerateRoom(int OffsetX, int OffsetY, int MaxWidth, int ColNumber)
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
			cout << "No Room received for " << RoomType << endl;
		#endif // _DEBUG
			return;
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

	

	int Size = 0;
	

	// Add new rooms to the storage vector
	m_Rooms[ColNumber].push_back(Room);
	// Adding this rooms column offset to simplify corridor gen later on
	m_ColumnOffsetsX[ColNumber].push_back(xOffset);
	m_ColumnOffsetsY[ColNumber].push_back(OffsetY);

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

	SetUpCorridor(ColNumber, OffsetX, xOffset);

	// Generating the next room below this one
	GenerateRoom(OffsetX, OffsetY + RoomHeight + rand() % ColumnSeparatorMax + ColumnSeparatorMin, MaxWidth, ColNumber);
}

//
//
//
//
void Map::SetUpCorridor(int ColumnNumber, int ColumnOffsetX, int RoomOffsetX)
{
	int Size = 0;

	// Generate a path to above room if possible
	if ((Size = m_Rooms[ColumnNumber].size() - 2) >= 0)
	{
		// There is a room above us, so we need to find a suitable location for a corridor.
		MapRoom* RoomAbove = m_Rooms[ColumnNumber][Size];

		int AboveWidth = RoomAbove->GetWidth();
		int AboveOffsetX = m_ColumnOffsetsX[ColumnNumber][Size];
		int CenterPointY = m_ColumnOffsetsY[ColumnNumber][Size] + RoomAbove->GetHeight();

		int AxisOffsetY = 0;
		// Get the center point of the room above for reference
		int CenterPointX = AboveOffsetX + AboveWidth / 2 + ColumnOffsetX;

		vector<string> temp;
		temp.push_back("Explosion");

		if (RoomAbove->GetRoomType() == "Stair_Down")
		{
			cout << "bottom" << endl;
		}

		/*
		// Gets the actual bottom of the above room
		for (MapObject* Curr = RoomAbove->GetCell(AboveWidth / 2, RoomAbove->GetHeight() - 1);
			Curr == nullptr && AxisOffsetY < RoomAbove->GetHeight();
			Curr = RoomAbove->GetCell(AboveWidth / 2, RoomAbove->GetHeight() - ++AxisOffsetY))
		{
		}
		*/
		MapCoordinate EndingLocation;
		//Ask the above room for a facing side if possible
		pair<MapCoordinate, MapCoordinate>* GetLocation = RoomAbove->GetFacingFromSide(Side::BOTTOM);
		if (GetLocation)
			EndingLocation = MapCoordinate(GetLocation->first.GetPositionX(), GetLocation->first.GetPositionY());

		m_Cells[EndingLocation.GetPositionX() + ColumnOffsetX + AboveOffsetX][EndingLocation.GetPositionY() + m_ColumnOffsetsY[ColumnNumber][Size]] = new MapWall(temp, MapCoordinate(CenterPointX, CenterPointY - AxisOffsetY), Cell::Floor);

		//m_Cells[CenterPointX][CenterPointY - ((AxisOffsetY) ? AxisOffsetY : ++AxisOffsetY)] = new MapWall(temp, MapCoordinate(CenterPointX, CenterPointY - AxisOffsetY), Cell::Floor);

	}

}

//
// GenerateCorridorBetween()
// Attempts to draw a corridor between the two points given
//
bool Map::GenerateCorridorBetween(MapCoordinate Begin, MapCoordinate End)
{


	return false;
}