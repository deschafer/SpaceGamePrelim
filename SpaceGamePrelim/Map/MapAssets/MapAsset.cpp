#include "MapAsset.h"
#include "../../Frame/ZoomManager.h"


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
// For an animated asset
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

	m_Animated = true;


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

		NewWidth = round((double)(CurrDim.Width() * Scale));
		NewHeight = round((double)(CurrDim.Height() * Scale));

		m_DestRect = Rect(0, 0, NewWidth, NewHeight);
	}
}