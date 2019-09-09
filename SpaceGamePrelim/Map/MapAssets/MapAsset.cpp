#include "MapAsset.h"
#include "../../Frame/ZoomManager.h"
#include "../MapManager.h"


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
	m_Updated = false;
}

//
// Load()
// Loads the values into this object
// All three vectors below are parallel vectors, and
// need to be the same size with corresponding indices
// For an animated asset
//
void MapAsset::Load(
	std::vector<std::string> RedTextureIDs, // Function used to create this object
	MapCoordinate Position,
	Rect DestRect,
	Cell CellType,
	std::vector<int> NumberFrames,
	std::vector<int> AnimationSpeed) 
{
	m_CellType = CellType;
	m_Position = Position;
	m_DestRect = DestRect;
	
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

	m_Animated = false;


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

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);

	m_Animated = false;


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

//
// Draw()
//
//
void MapAsset::Draw(MapCoordinate Coords)
{
	MapCell::Draw(Coords);

	m_Updated = false; // end of this game loop, so we can be updated again
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

	if (ZoomManager::Instance()->IsChange())
	{
		Scale = ZoomManager::Instance()->GetScale();

		NewWidth = (int)round(CurrDim.Width() * Scale);
		NewHeight = (int)round(CurrDim.Height() * Scale);

		m_DestRect = Rect(0, 0, NewWidth, NewHeight);
	}
}

//
//
//
//
int MapAsset::GetIntegerWidth() 
{ 
	return m_DestRect.Width() % MapManager::Instance()->GetCellWidth(); 
}

//
//
//
//
int MapAsset::GetIntegerHeight()
{ 
	return m_DestRect.Height() % MapManager::Instance()->GetCellHeight(); 
}

//
// PlaceAsset()
// Finds an appropriate spot in this room to place this object. Will return the 
// top left coord of the desired place if it was found
//
MapCoordinate MapAsset::PlaceAsset(MapObject*** RoomCells, MapAsset*** RoomAssets, bool*** Doorways)
{

	return MapCoordinate(0, 0);
}

//
// PlaceAssetBoundaryRoom()
// Finds an appropriate spot in this boundary room to place this object. Will return the 
// top left coord of the desired place if it was found
// Boundary room is important because it means we cannot appropriatly judge the exact location of all doorways
// and therefore we do not even look at the doorways
//
MapCoordinate MapAsset::PlaceAssetBorderingRoom(MapObject*** RoomCells, MapAsset*** RoomAssets)
{

	return MapCoordinate(0, 0);
}


