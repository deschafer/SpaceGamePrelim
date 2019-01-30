#include "MapCell.h"
#include "..\Frame\MainApplication.h"

#include <iostream>


MapCell::MapCell()
{
}

MapCell::MapCell(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position)
{

	for (size_t i = 0; i < TextureIDs.size(); i++)
	{
		m_TextureIDs.push_back(TextureIDs.back());
		TextureIDs.pop_back();
	}

	m_CurrentFrame = Properties->GetCurrentFrame();
	m_CurrentRow = Properties->GetCurrentRow();
	m_Dimensions = Properties->GetDimensions();



}

MapCell::MapCell(TextureProperties* Properties, MapCoordinate Position)
{

	m_TextureIDs.push_back(Properties->GetTextureID());

	m_CurrentFrame = Properties->GetCurrentFrame();
	m_CurrentRow = Properties->GetCurrentRow();
	m_Dimensions = Properties->GetDimensions();

	MapObject::m_Position = Position;

}

void MapCell::Draw()
{
	if (m_TextureIDs.empty())
	{
		std::cout << "textures are empty";
	}
	TextureManager::Instance()->DrawCurrentFrame(m_TextureIDs.back(), m_Position.GetPositionX(),
		m_Position.GetPositionY(), m_Dimensions, SDL_FLIP_NONE, MainApplication::Instance()->GetRenderer(),
		m_CurrentRow, m_CurrentFrame);

}

void MapCell::Draw(MapCoordinate Coords)
{
	if (m_TextureIDs.empty())
	{
		std::cout << "textures are empty";
	}
	TextureManager::Instance()->DrawCurrentFrame(m_TextureIDs.back(), Coords.GetPositionX(),
		Coords.GetPositionY(), m_Dimensions, SDL_FLIP_NONE, MainApplication::Instance()->GetRenderer(),
		m_CurrentRow, m_CurrentFrame);

}


MapCell::~MapCell()
{
}
