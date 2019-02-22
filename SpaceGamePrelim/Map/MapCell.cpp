#include "MapCell.h"
#include "..\Frame\MainApplication.h"

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
	m_CellType(CellType)
{
	MapCell();

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);
	MapObject::m_Position = Position;
}

//
// MapCell()
// For a MapCell that has animated textures with assoc properties
//
MapCell::MapCell(std::vector<std::string> RedTextureIDs, std::vector<TextureProperties*> Properties, MapCoordinate Position,
	Cell CellType) : 
	m_CellType(CellType),
	m_Animated(true)
{

	TextureProperties* CurrentProp;

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);
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
	if (!m_Animated)
	{
		DrawStatic(Coords);
	}
	else
	{
		// Drawing each texture with its respective properties
		for (size_t i = 0; i < m_RedTextureIDs->size(); i++)
		{
			TextureManager::Instance()->DrawCurrentFrame(
				Coords.GetPositionX(),
				Coords.GetPositionY(),
				m_RedTextureIDs->at(i),
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

	if (m_RedTextureIDs->empty())
	{
		std::cout << "textures are empty";
	}
	// Draws each of the textures in the vector
	for (size_t i = 0; i < m_RedTextureIDs->size(); i++)
	{

		TextureManager::Instance()->DrawStaticFrame(
			Coords.GetPositionX(),
			Coords.GetPositionY(),
			m_RedTextureIDs->at(i),
			MainApplication::Instance()->GetRenderer());
	}

}

//
// ChangeRedTextures()
// Changes the textures associated with this MapCell
//
void MapCell::ChangeRedTextures(std::vector<std::string> NewTextures)
{
	m_RedTextureIDs->clear();

	for (size_t i = 0; i < NewTextures.size(); i++)
	{
		m_RedTextureIDs->push_back(NewTextures[i]);
	}
}

