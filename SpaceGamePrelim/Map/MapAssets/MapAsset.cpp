#include "MapAsset.h"

MapAsset::MapAsset()
{
}

MapAsset::~MapAsset()
{
}

MapAsset::MapAsset(std::vector<std::string> RedTextureIDs,
	MapCoordinate Position,
	Rect DestRect,
	Cell CellType) :
	MapCell(RedTextureIDs, Position, DestRect, CellType)
{
}

//
// Load()
// Loads the values into this object
//
void MapAsset::Load(
	std::vector<std::string> RedTextureIDs, // Function used to create this object
	MapCoordinate Position,
	Rect DestRect,
	Cell CellType)
{
	m_CellType = CellType;
	m_Position = Position;
	m_DestRect = DestRect;
	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);

	for(size_t i = 0; i < RedTextureIDs.size(); i++)
	{
		m_RedTextureIDs->push_back(RedTextureIDs[i]);
	}
}