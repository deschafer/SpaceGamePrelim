
#include "Map.h"
#include "MapRoom.h"

static const int WidthRandomRate = 5;
static const int HeightRandomRate = 5;

static const int MaxRoomHeight = 20;
static const int MaxRoomWidth = 20;

static const int RoomSizeVolatility = 10;

using namespace std;

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
	std::map<std::pair<int,int>, bool> CellLocations;

	GenerateRoom(0, 0, MapCoordinate(0, 0), CellLocations);
}
//
//
//
//
void Map::GenerateRoom(int OffsetX, int OffsetY, MapCoordinate Coord, std::map<std::pair<int, int>, bool> &CellLocations)
{

	int MaxWidth;
	int MaxHeight;
	bool Break = false;
	

	// Check available space
	for (size_t i = OffsetX, OrigI = 0; i < m_Width && OrigI < MaxRoomWidth; i++)
	{
		for (size_t j = OffsetY, OrigJ = 0; j < m_Height && OrigJ < MaxRoomHeight; j++)
		{
			if (CellLocations[std::pair<int, int>(i, j)] == true)
			{
				Break = true;
				break;
			}
			else
				MaxHeight++;
		}
		if (Break) break;
		MaxWidth++;
	}


	std::string RoomType;

	RoomProperties* Properties = RoomManager::Instance()->GetRandomTypeDefinition(RoomType);
	int Width = Properties->m_MinWidth + (rand() % RoomSizeVolatility);
	int Height = Properties->m_MinHeight + (rand() % RoomSizeVolatility);


	m_Rooms[pair<int, int>(Coord.GetPositionX(), Coord.GetPositionY())] = new MapRoom(RoomType, Width, Height);
	m_Rooms[pair<int, int>(Coord.GetPositionX(), Coord.GetPositionY())]->Generate();


	// Place it somewhere in the cell array
	for (int i = OffsetX, MapI = 0; 
		MapI < m_Rooms[pair<int, int>(Coord.GetPositionX(), Coord.GetPositionY())]->GetWidth() && 
		i < m_Width; 
		i++, MapI++)
	{
		for (int j = OffsetY, MapJ = 0; 
			MapJ < m_Rooms[pair<int, int>(Coord.GetPositionX(), Coord.GetPositionY())]->GetHeight() && 
			j < m_Height; 
			j++, MapJ++)
		{
			m_Cells[i][j] = m_Rooms[pair<int, int>(Coord.GetPositionX(), Coord.GetPositionY())]->GetCell(MapI, MapJ);
		}
	}

	if (OffsetY + Height < m_Height)
		GenerateRoom(OffsetX, OffsetY + Height, MapCoordinate(Coord.GetPositionX(), Coord.GetPositionY() + 1));

}