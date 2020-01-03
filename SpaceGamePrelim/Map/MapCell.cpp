#include "MapCell.h"
#include "..\Frame\MainApplication.h"
#include "..\Frame\ZoomManager.h"

#include <iostream>

// MapCell CTORs

// Default CTOR, used by all CTORs for some common
// intialization
MapCell::MapCell() :
	m_Animated(false),
	m_RedTextureIDs(nullptr),
	m_OriginSize(m_DestRect),
	Interactable(true)
{
	Colorable::Colorable();
}

//
// MapCell()
// For a map cell using one or more reduced textures that do not have assoc properties
// (Are not animated)
//
MapCell::MapCell(std::vector<std::string> RedTextureIDs, MapCoordinate Position,
	Cell CellType) : 
	m_CellType(CellType),
	m_DestRect(0, 0, 0, 0),
	Interactable(true)
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
	m_DestRect(0, 0, 0, 0),
	m_OriginSize(m_DestRect),
	Interactable(true)
{
	Colorable::Colorable();
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

//
// MapCell()
// For a map cell using reduced textures and overriding those textures with its
// own dimensions
//
MapCell::MapCell(std::vector<std::string> RedTextureIDs, MapCoordinate Position,
	Rect DstDimen, Cell CellType) :
	m_CellType(CellType),
	m_DestRect(DstDimen),
	Interactable(true)
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
// NOTE: these coordinates do not refer to map position, instead 
//		 it refers to its screen position
//
void MapCell::Draw(MapCoordinate Coords)
{
	Zoom();

	// set this as our last screen position
	SetLocatableScreenPosition(Vector(Coords.GetPositionX(), Coords.GetPositionY()));

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
				m_DestRect,
				m_Color,
				m_CurrentRow[i],
				m_CurrentFrame[i]
			);
		}
	}
}

//
// Draw()
// Given the offset of the map where this asset is,
// this draw uses the coordinates of the asset to draw it
//
void MapCell::Draw(double X, double Y)
{
	// set this as our last screen position
	SetLocatableScreenPosition(Vector(X, Y));

	if (!m_Animated)
	{
		DrawStatic((int)X, (int)Y);
	}
	else
	{
		// Drawing each texture with its respective properties
		for (size_t i = 0; i < m_RedTextureIndex.size(); i++)
		{
			TextureManager::Instance()->DrawCurrentFrame(
				(int)X,
				(int)Y,
				m_RedTextureIndex[i],
				SDL_FLIP_NONE,
				MainApplication::Instance()->GetRenderer(),
				m_DestRect,
				m_Color,
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
				MainApplication::Instance()->GetRenderer(),
				m_Color);
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
				MainApplication::Instance()->GetRenderer(),
				m_Color);
		}
	}
}

// 
// DrawStatic() 
// Called to draw a map cell with one or more reduced textures
//
void MapCell::DrawStatic(int X, int Y)
{
	DrawStatic(MapCoordinate(X, Y));
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
	std::cout << "Map Cell Interaction\n";

	m_Color.r = 255;
	m_Color.g = 0;
	m_Color.b = 0;
	m_Color.a = 255;

	return false;
}
