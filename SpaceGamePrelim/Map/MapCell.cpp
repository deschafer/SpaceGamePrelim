#include "MapCell.h"
#include "..\Frame\MainApplication.h"
#include "..\Frame\ZoomManager.h"

#include <iostream>

// MapCell CTORs

// Default CTOR, used by all CTORs for some common
// intialization
MapCell::MapCell() : 
	m_Animated(false),
	m_RedTextureIDs(nullptr)
{
}

//
// MapCell()
// For a map cell using one or more reduced textures that do not have assoc properties
// (Are not animated)
//
MapCell::MapCell(std::vector<std::string> RedTextureIDs, MapCoordinate Position,
	Cell CellType) : 
	m_CellType(CellType),
	m_DestRect(0, 0, 0, 0)
{
	MapCell();

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);

	TextureManager* Manager = TextureManager::Instance();
	// Getting the proper indices for each of the textures 
	// associated with this object
	for (size_t i = 0; i < m_RedTextureIDs->size(); i++)
	{
		m_RedTextureIndex.push_back(Manager->GetRedTextureIndex((*m_RedTextureIDs)[i]));
	}

	MapObject::m_Position = Position;
}

//
// MapCell()
// For a MapCell that has animated textures with assoc properties
//
MapCell::MapCell(std::vector<std::string> RedTextureIDs, std::vector<TextureProperties*> Properties, MapCoordinate Position,
	Cell CellType) :
	m_CellType(CellType),
	m_Animated(true),
	m_DestRect(0, 0, 0, 0)
{

	TextureProperties* CurrentProp;

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);

	TextureManager* Manager = TextureManager::Instance();
	// Getting the proper indices for each of the textures 
	// associated with this object
	for (size_t i = 0; i < m_RedTextureIDs->size(); i++)
	{
		m_RedTextureIndex.push_back(Manager->GetRedTextureIndex((*m_RedTextureIDs)[i]));
	}

	MapObject::m_Position = Position;

	// For each of the properties, save them to the approp pos
	for (size_t i = 0; i < Properties.size(); i++)
	{
		CurrentProp = Properties[i];
		m_AnimationSpeed.push_back(CurrentProp->GetAnimationSpeed());
		m_NumberFrames.push_back(CurrentProp->GetNumberFrames());

		m_CurrentFrame.push_back(1);
		m_CurrentRow.push_back(1);
	}

}

MapCell::MapCell(std::vector<std::string> RedTextureIDs, MapCoordinate Position,
	Rect DstDimen, Cell CellType) :
	m_CellType(CellType),
	m_DestRect(DstDimen)
{
	MapCell();

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);

	TextureManager* Manager = TextureManager::Instance();
	// Getting the proper indices for each of the textures 
	// associated with this object
	for (size_t i = 0; i < m_RedTextureIDs->size(); i++)
	{
		m_RedTextureIndex.push_back(Manager->GetRedTextureIndex((*m_RedTextureIDs)[i]));
	}

	MapObject::m_Position = Position;
}


MapCell::~MapCell()
{
}

//
// Draw()
// Called to draw a mapobject, it will direct it accordingly
// based on whether to use ReducedTextures for full Textures
//
void MapCell::Draw(MapCoordinate Coords)
{
	Zoom();

	if (!m_Animated)
	{
		DrawStatic(Coords);
	}
	else
	{
		// Drawing each texture with its respective properties
		for (size_t i = 0; i < m_RedTextureIndex.size(); i++)
		{
			TextureManager::Instance()->DrawCurrentFrame(
				Coords.GetPositionX(),
				Coords.GetPositionY(),
				m_RedTextureIndex[i],
				SDL_FLIP_NONE,
				MainApplication::Instance()->GetRenderer(),
				m_CurrentRow[i],
				m_CurrentFrame[i]
			);
		}
	}
}

// 
// Update()
// Updates animated textures with assoc properties
//
void MapCell::Update()
{

	// Only updates animated textures
	for (size_t i = 0; i < m_CurrentFrame.size(); i++)
	{
		m_CurrentFrame[i] = int(((SDL_GetTicks() / m_AnimationSpeed[i]) % m_NumberFrames[i]));
	}
}

// 
// DrawStatic() 
// Called to draw a map cell with one or more reduced textures
//
void MapCell::DrawStatic(MapCoordinate Coords)
{
	static TextureManager* Instance = TextureManager::Instance();

	if (m_RedTextureIDs->empty())
	{
		std::cout << "textures are empty";
	}
	if (m_DestRect.Height() != 0 && m_DestRect.Width() != 0)
	{
		// Draws each of the textures in the vector
		for (size_t i = 0; i < m_RedTextureIndex.size(); i++)
		{
			Instance->DrawStaticFrame(
				Coords.GetPositionX(),
				Coords.GetPositionY(),
				m_RedTextureIndex[i],
				m_DestRect,
				MainApplication::Instance()->GetRenderer());
		}
	}
	else
	{
		// Draws each of the textures in the vector
		for (size_t i = 0; i < m_RedTextureIndex.size(); i++)
		{
			Instance->DrawStaticFrame(
				Coords.GetPositionX(),
				Coords.GetPositionY(),
				m_RedTextureIndex[i],
				MainApplication::Instance()->GetRenderer());
		}
	}
}

//
// ChangeRedTextures()
// Changes the textures associated with this MapCell
//
void MapCell::ChangeRedTextures(std::vector<std::string> NewTextures)
{
	m_RedTextureIDs->clear();
	m_RedTextureIndex.clear();

	TextureManager* Manager = TextureManager::Instance();

	for (size_t i = 0; i < NewTextures.size(); i++)
	{
		m_RedTextureIDs->push_back(NewTextures[i]);
		m_RedTextureIndex.push_back(Manager->GetRedTextureIndex(NewTextures[i]));
	}
}

//
// OnCollision()
// Returns true if there is a no-movement 
// Collision. Does what it needs to the player
// here by modifying its data.
//
bool MapCell::OnCollision(GameEntity* Enitity)
{
	return false;	// Base object has no affect on the player
}

//
// Zoom() 
//
//
void MapCell::Zoom() 
{
	int Offset = 0;
	int NewHeight = 0;
	int NewWidth = 0;
	static Rect CurrDim = m_DestRect;

	if (Offset = ZoomManager::Instance()->GetPixelOffset())
	{
		//m_Owner->EnglargeDestination(Offset);
		NewWidth = CurrDim.Width() + Offset;
		NewHeight = CurrDim.Height() + Offset;

		m_DestRect.SetHeight(NewHeight);
		m_DestRect.SetWidth(NewWidth);
	}
	else
	{
		m_DestRect = CurrDim;
	}
}