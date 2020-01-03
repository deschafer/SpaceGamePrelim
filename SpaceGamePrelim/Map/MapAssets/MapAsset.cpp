#include "MapAsset.h"
#include "../../Frame/ZoomManager.h"
#include "../MapManager.h"

const static MapCoordinate ErrorCoord = MapCoordinate(-1, -1);

MapAsset::MapAsset() :
	m_IntegerHeight((int)ceil(m_DestRect.Height() / MapManager::GetCellSourceHeight())),
	m_IntegerWidth((int)ceil(m_DestRect.Width() / MapManager::GetCellSourceWidth())),
	m_ParentRoom(nullptr),
	m_ParentMap(nullptr)
{
}

MapAsset::~MapAsset()
{
}

MapAsset::MapAsset(std::vector<std::string> RedTextureIDs,
	MapCoordinate Position,
	Rect DestRect,
	Cell CellType,
	MapRoom* Parent) :
	MapCell(RedTextureIDs, Position, DestRect, CellType),
	m_IntegerHeight((int)ceil(m_DestRect.Height() / MapManager::GetCellSourceHeight())),
	m_IntegerWidth((int)ceil(m_DestRect.Width() / MapManager::GetCellSourceWidth())),
	m_ParentRoom(nullptr),
	m_ParentMap(nullptr)
{
	m_Updated = false;
	m_Drawn = false;
	m_ParentRoom = Parent;
}

//
// Reset()
// should be called by derived classes when this object is copied
//
void MapAsset::Reset()
{
	m_Drawn = false;
	m_Updated = false;
}

//
// Load()
// Loads the values into this object
// All three vectors below are parallel vectors, and
// need to be the same size with corresponding indices
// For an animated asset
//
void MapAsset::Load(std::vector<std::string> RedTextureIDs, // Function used to create this object
	MapCoordinate Position,
	Rect DestRect,
	Cell CellType,
	std::vector<int> NumberFrames,
	std::vector<int> AnimationSpeed,
	bool Collidable)
{
	m_CellType = CellType;
	m_Position = Position;
	m_DestRect = DestRect;
	m_Collidable = Collidable;
	m_OriginSize = DestRect;

	m_IntegerHeight = (int)ceil(m_DestRect.Height() / MapManager::GetCellSourceHeight());
	m_IntegerWidth = (int)ceil(m_DestRect.Width() / MapManager::GetCellSourceHeight());

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);

	m_Animated = true;


	// Transfer all vector contents
	for (size_t i = 0; i < RedTextureIDs.size(); i++)
	{
		m_RedTextureIDs->push_back(RedTextureIDs[i]);
		m_NumberFrames.push_back(NumberFrames[i]);
		m_AnimationSpeed.push_back(AnimationSpeed[i]);

		m_RedTextureIndex.push_back(TextureManager::Instance()->GetRedTextureIndex(RedTextureIDs[i]));

		m_CurrentFrame.push_back(1);
		m_CurrentRow.push_back(1);
	}
}

//
// Load()
// Loads the values into this object
// All three vectors below are parallel vectors, and
// need to be the same size with corresponding indices
// For a non animated asset
//
void MapAsset::Load(std::vector<std::string> RedTextureIDs, // Function used to create this object
	MapCoordinate Position,
	Rect DestRect,
	Cell CellType,
	bool Collidable)
{
	m_CellType = CellType;
	m_Position = Position;
	m_DestRect = DestRect;
	m_Collidable = Collidable;
	m_OriginSize = DestRect;

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);

	m_Animated = false;

	m_IntegerHeight = (int)ceil((float)m_DestRect.Height() / (float)MapManager::GetCellSourceHeight());
	m_IntegerWidth = (int)ceil((float)m_DestRect.Width() / (float)MapManager::GetCellSourceHeight());

	// Transfer all vector contents
	for (size_t i = 0; i < RedTextureIDs.size(); i++)
	{
		m_RedTextureIDs->push_back(RedTextureIDs[i]);
		m_RedTextureIndex.push_back(TextureManager::Instance()->GetRedTextureIndex(RedTextureIDs[i]));

		m_NumberFrames.push_back(1);
		m_AnimationSpeed.push_back(1);
		m_CurrentFrame.push_back(1);
		m_CurrentRow.push_back(1);
	}
}

Map* MapAsset::GetParentMap()
{
	return m_ParentMap;
}

//
// Draw()
//
//
void MapAsset::Draw(MapCoordinate Coords)
{
	if (m_Drawn) return;

	MapCell::Draw(Coords);

	m_Updated = false; // end of this game loop, so we can be updated again
	m_Drawn = true;
}

//
// Draw()
//
//
void MapAsset::Draw(double X, double Y)
{
	if (m_Drawn) return;

	MapCell::Draw(X, Y);

	m_Updated = false; // end of this game loop, so we can be updated again
	m_Drawn = true;
}

//
// Update()
//
//
void MapAsset::Update()
{
	if (m_Updated) return;

	MapCell::Update();

	m_Updated = true;
	m_Drawn = false;
}

//
// Zoom()
//
//
void MapAsset::Zoom()
{
	int Offset = 0;
	int NewWidth = 0;
	int NewHeight = 0;
	double Scale = 0.0;
	static Rect CurrDim = m_DestRect;

	Scale = ZoomManager::Instance()->GetScale();

	NewWidth = (int)round(CurrDim.Width() * Scale);
	NewHeight = (int)round(CurrDim.Height() * Scale);

	m_DestRect = Rect(0, 0, NewWidth, NewHeight);
}

//
//
//
//
int MapAsset::GetIntegerWidth() 
{ 
	return m_IntegerWidth;
}

//
//
//
//
int MapAsset::GetIntegerHeight()
{ 
	return m_IntegerHeight;
}

//
// PlaceAsset()
// Finds an appropriate spot in this room to place this object. Will return the 
// top left coord of the desired place if it was found
//
MapCoordinate MapAsset::PlaceAsset(MapObject*** RoomCells, MapAsset*** RoomAssets, bool*** Doorways)
{
	/*
	std::vector<MapCoordinate> PossiblePositions;
	int RoomWidth = m_ParentRoom->GetWidth();
	int RoomHeight = m_ParentRoom->GetHeight();

	for (size_t i = 0; i < RoomWidth - 1; i++)
	{
		for (size_t j = 0; j < RoomHeight - 1; j++)
		{
			MapCell* Cell = static_cast<MapCell*>(RoomCells[i][j]);

			if (Cell &&
				!Cell->IsCollidableType() &&
				!RoomAssets[i][j])
			{
				MapCoordinate Position = CheckAssetPosition(MapCoordinate(i, j), (MapCell***)RoomCells, RoomAssets);
				if (Position != ErrorCoord)
					PossiblePositions.push_back(Position);
			}
		}
	}

	// Check if we have found any positions at all
	if (PossiblePositions.empty()) return ErrorCoord;

	// Otherwise, select a random position
	int Choice = rand() % PossiblePositions.size();
	return PossiblePositions[Choice];
	*/

	return MapCoordinate(0, 0);
}

//
// PlaceAssetBoundaryRoom()
// Finds an appropriate spot in this boundary room to place this object. Will return the 
// top left coord of the desired place if it was found
// Boundary room is important because it means we cannot appropriatly judge the exact location of all doorways
// and therefore we do not even look at the doorways
//
MapCoordinate MapAsset::PlaceAssetBorderingRoom(MapCell *** RoomCells, MapAsset*** RoomAssets)
{

	return MapCoordinate(0, 0);
}

// 
// CheckAssetPosition()
//
//
MapCoordinate MapAsset::CheckAssetPosition(MapCoordinate TopLeftPosition, MapCell*** Cells, MapAsset*** Assets)
{
	int RoomWidth = m_ParentRoom->GetWidth();
	int RoomHeight = m_ParentRoom->GetHeight();

	// First, verify that this is within the array
	if (TopLeftPosition.GetPositionX() >= RoomWidth ||
		TopLeftPosition.GetPositionX() + m_IntegerWidth >= RoomWidth ||
		TopLeftPosition.GetPositionY() >= RoomHeight ||
		TopLeftPosition.GetPositionY() + m_IntegerHeight >= RoomHeight)
	{
		return ErrorCoord;
	}

	// check every single cell in its area to verify this
	// x, y -> recording position we are checking in the map array iteself
	// i, j -> recording how many cells we have checked
	for (size_t x = TopLeftPosition.GetPositionX(), i = 0; i < (size_t)m_IntegerWidth; i++, x++)
	{
		for (size_t y = TopLeftPosition.GetPositionY(), j = 0; j < (size_t)m_IntegerHeight; j++, y++)
		{
			// Now we check this individual cell

			// if this cell is nullptr, we return the error coord
			if (!Cells[x][y])
			{
				return ErrorCoord;
			}

			// if this cell is occupied with an asset, we return the error coord
			if (Assets[x][y])
			{
				return ErrorCoord;
			}

			// if this cell is occupied by a collidable cell type, then we return error coord
			if (Cells[x][y]->IsCollidableType())
			{
				return ErrorCoord;
			}

			// otherwise, this cell is fine
		}
	}

	return TopLeftPosition;
}