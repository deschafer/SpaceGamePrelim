#include "MapCell.h"
#include "..\Frame\MainApplication.h"

#include <iostream>

// MapCell CTORs

// Default CTOR, used by all CTORs for some common
// intialization
MapCell::MapCell() : 
	m_UsingRedTextures(false),
	m_RedTextureIDs(nullptr)
{
}

//
// MapCell()
// For a MapCell using traditional textures
//
MapCell::MapCell(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position)
{
	MapCell();

	for (size_t i = 0; i < TextureIDs.size(); i++)
	{
		m_TextureIDs.push_back(TextureIDs.back());
		TextureIDs.pop_back();
	}

	m_CurrentFrame = Properties->GetCurrentFrame();
	m_CurrentRow = Properties->GetCurrentRow();
	m_Dimensions = Properties->GetDimensions();
}

//
// MapCell()
// For a MapCell using a single texture
//
MapCell::MapCell(TextureProperties* Properties, MapCoordinate Position)
{
	MapCell();

	m_TextureIDs.push_back(Properties->GetTextureID());
	m_CurrentFrame = Properties->GetCurrentFrame();
	m_CurrentRow = Properties->GetCurrentRow();
	m_Dimensions = Properties->GetDimensions();

	MapObject::m_Position = Position;
}

//
// MapCell()
// For a map cell using one or more reduced textures
//
MapCell::MapCell(std::vector<std::string> RedTextureIDs, MapCoordinate Position)
{
	MapCell();

	m_RedTextureIDs = new std::vector<std::string>(RedTextureIDs);
	m_UsingRedTextures = true;

	MapObject::m_Position = Position;
}

//
// Draw()
// Called to draw a mapobject, it will direct it accordingly
// based on whether to use ReducedTextures for full Textures
//
void MapCell::Draw(MapCoordinate Coords)
{
	if (m_UsingRedTextures)
	{
		DrawStatic(Coords);
	}
	else
	{
		if (m_TextureIDs.empty())
		{
			std::cout << "textures are empty";
			return;
		}

		// TODO: Implement multiple texture layer drawing

		TextureManager::Instance()->DrawCurrentFrame(
			m_TextureIDs.back(), 
			Coords.GetPositionX(),
			Coords.GetPositionY(), 
			m_Dimensions, 
			SDL_FLIP_NONE, 
			MainApplication::Instance()->GetRenderer(),
			m_CurrentRow, m_CurrentFrame);
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


MapCell::~MapCell()
{
}
