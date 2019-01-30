
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
Map::Map(int Width, int Height, string MapType) :
	m_Width(Width),
	m_Height(Height),
	m_MapType(MapType)
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

}

Map::~Map()
{
}



//
// Generate()
// Generates this map objects -- may take longer 
// as this is a one-time process
//
void Map::Generate()
{

	// Default for now

	// Generate a single room


	// Place it somewhere in the cell array

}
