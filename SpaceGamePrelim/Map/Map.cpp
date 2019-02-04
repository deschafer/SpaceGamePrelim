
#include "Map.h"
#include "MapRoom.h"

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
	m_Cells = new MapObject**[m_Width];
	for (int i = 0; i < m_Width; i++)
	{
		m_Cells[i] = new MapObject*[m_Height];
	}

	// Initializing all of the cells to nullptr
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_Cells[i][j] = nullptr;
		}
	}

	//m_TempRoom = new MapRoom("Complex", 15, 20);	// Named room
	//m_TempRoom = new MapRoom("L_Rect_Static", 20, 20);	// Named room
	m_TempRoom = new MapRoom((rand() % 10) + 10, (rand() % 10) + 10);			// Random room


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

	if (X < m_Width && Y < m_Height)
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


	// Generate a single room
	m_TempRoom->Generate();
	
	int OffsetX = 10;
	int OffsetY = 6;

	// Place it somewhere in the cell array
	for (int i = 0; i < m_TempRoom->GetWidth(); i++)
	{
		for (int j = 0; j < m_TempRoom->GetHeight(); j++)
		{
			m_Cells[i + OffsetX][j + OffsetY] = m_TempRoom->GetCell(i, j);
		}
	}

}
