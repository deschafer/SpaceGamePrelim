#include "MapCell.h"
#include "..\Frame\MainApplication.h"
#include "..\Frame\ZoomManager.h"
#include "..\BasicTypes\BasicTypes.h"

#include <iostream>

// MapCell CTORs

// Default CTOR, used by all CTORs for some common
// intialization
MapCell::MapCell(Rect Dimensions, Scene* ParentScene) :
	m_Animated(false),
	m_RedTextureIDs(nullptr),
	m_OriginSize(m_DestRect),
	m_CellType(Cell::Empty),
	Interactable(false),
	MapObject(Rect(m_Position.GetPositionX(), m_Position.GetPositionY(), m_DestRect.Width(), m_DestRect.Height()), ParentScene)
{
}

//
// MapCell()
// For a map cell using one or more reduced textures that do not have assoc properties
// (Are not animated)
//
MapCell::MapCell(Rect Dimensions, Scene* ParentScene,
	std::vector<std::string> RedTextureIDs, MapCoordinate Position,
	Cell CellType) : 
	m_CellType(CellType),
	m_Animated(false),
	m_OriginSize(m_DestRect),
	m_DestRect(0, 0, 0, 0),
	Interactable(false),
	MapObject(Rect(m_Position.GetPositionX(), m_Position.GetPositionY(), m_DestRect.Width(), m_DestRect.Height()), ParentScene)
{
	

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
// For a map cell using reduced textures and overriding those textures with its
// own dimensions
//
MapCell::MapCell(Rect Dimensions, Scene* ParentScene,
	std::vector<std::string> RedTextureIDs, MapCoordinate Position,
	Rect DstDimen, Cell CellType) :
	m_CellType(CellType),
	m_DestRect(DstDimen),
	m_Animated(false),
	m_Collidable(true),
	m_OriginSize(m_DestRect),
	Interactable(false),
	MapObject(Rect(m_Position.GetPositionX(), m_Position.GetPositionY(), m_DestRect.Width(), m_DestRect.Height()), ParentScene)
{

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

void MapCell::AddReducedTexture(std::string RedTextureID, int RedTextureIndex, int AnimationSpeed, int NumberFrames)
{
	m_RedTextureIDs->push_back(RedTextureID);
	m_NumberFrames.push_back(NumberFrames);
	m_RedTextureIndex.push_back(RedTextureIndex);
	m_AnimationSpeed.push_back(AnimationSpeed);
}

void MapCell::ClearTextures()
{
	m_RedTextureIDs->clear();
	m_NumberFrames.clear();
	m_RedTextureIndex.clear();
	m_AnimationSpeed.clear();
}

void MapCell::SetLocatableScreenPosition(Vector Position)
{
	MapObject::SetLocatableScreenPosition(Position);
	
	m_Dimensions.SetWidth(m_DestRect.Width());
	m_Dimensions.SetHeight(m_DestRect.Height());
}

//
// Draw()
// Given the offset of the map where this asset is,
// this draw uses the coordinates of the asset to draw it
//
bool MapCell::Draw(double X, double Y)
{
	if (MapObject::Draw(X,Y)) {

		Zoom();

		// set this as our last screen position
		SetLocatableScreenPosition(Vector((float)X, (float)Y));

		// Drawing each texture with its respective properties
		for (size_t i = 0; i < m_RedTextureIndex.size(); i++)
		{
			TextureManager::Instance()->DrawFrame(
				(int)X,
				(int)Y,
				m_RedTextureIndex[i],
				SDL_FLIP_NONE,
				MainApplication::Instance()->GetRenderer(),
				m_DestRect,
				GetColor(),
				1,
				0
			);
		}
	}
	return m_Visible;
}

// 
// Update()
// Updates animated textures with assoc properties
//
void MapCell::Update()
{
	if (!m_ParentScene)
	{
		m_ParentScene = MapManager::GetParentScene();
	}

	// Only updates animated textures
	for (size_t i = 0; i < m_CurrentFrame.size(); i++)
	{
		m_CurrentFrame[i] = int(((SDL_GetTicks() / m_AnimationSpeed[i]) % m_NumberFrames[i]));
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

	// There is an issue with this static variable
	Rect CurrDim = m_DestRect;
	double Scale = ZoomManager::Instance()->GetScale();

	NewWidth = MapManager::Instance()->GetCellWidth();
	NewHeight = MapManager::Instance()->GetCellHeight();

	m_DestRect.SetHeight(NewHeight);
	m_DestRect.SetWidth(NewWidth);
}

// 
// GetScreenPosition()
// This gets the actual screen position where this object is located
//
Vector MapCell::GetInteractablePosition()
{
	return Locatable::GetLocatableScreenPosition();
}

bool MapCell::OnInteraction(GameEntity* Entity)
{
	return false;
}
