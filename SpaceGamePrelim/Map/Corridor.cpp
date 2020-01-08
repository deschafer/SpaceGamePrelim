#include "Corridor.h"
#include "MapManager.h"



Corridor::Corridor()
{
}

Corridor::Corridor(int Width, int Height, MapRoom* RoomOne, MapRoom* RoomTwo) :
	m_Width(Width),
	m_Height(Height),
	m_CellWidth(MapManager::Instance()->GetCellWidth()),
	m_CellHeight(MapManager::Instance()->GetCellHeight()),
	m_RoomAboveOrRight(RoomOne),
	m_RoomBelowOrLeft(RoomTwo)
{
	// Create the array
	m_Cells = new MapCell**[Width];
	m_Assets = new MapAsset**[Width];
	for (int i = 0; i < Width; i++)
	{
		m_Cells[i] = new MapCell*[Height];
		m_Assets[i] = new MapAsset*[Height];

		for (int j = 0; j < Height; j++)
		{
			m_Cells[i][j] = nullptr;
			m_Assets[i][j] = nullptr;
		}
	}
}

Corridor::~Corridor()
{
}

// 
// AddCell()
// Checks bounds and adds to resident array
//
void Corridor::AddCell(MapCoordinate CellPosition, MapCell* Cell)
{
	int X = CellPosition.GetPositionX();
	int Y = CellPosition.GetPositionY();

	if ((X < m_Width) && (Y < m_Height) && (Y >= 0) && (X >= 0))
	{
		if (m_Cells[X][Y])
		{
			std::vector<std::string> Textures = *Cell->ReturnRedTextures();
			// Add the texture to the list to draw with the other textures here
			for (size_t i = 0; i < Textures.size(); i++)
			{
				if (m_Cells[X][Y]->GetCellType() != Cell::Floor)
				{
					m_Cells[X][Y]->AddRedTexture(Textures[i]);

					if (Cell->GetCellType() == Cell::Wall_Top)
					{
						m_Cells[X][Y]->SetCellType(Cell::Wall_Top);
					}
					else if (Cell->GetCellType() == Cell::Floor)
					{
						MapCell* Temp = m_Cells[X][Y];
						m_Cells[X][Y] = Cell;
						Cell->ChangeRedTextures(*Temp->ReturnRedTextures());
						m_Cells[X][Y]->SetCellType(Cell::Floor);
						delete Temp;
					}
				}
			}
		}
		else
			m_Cells[X][Y] = Cell;
	}
}

// 
// AddAsset()
// Checks bounds and adds to resident array
//
void Corridor::AddAsset(MapCoordinate CellPosition, MapAsset* Cell)
{
	int X = CellPosition.GetPositionX();
	int Y = CellPosition.GetPositionY();

	// set the position of the asset as well
	Cell->SetPosition(CellPosition);

	if ((X < m_Width) && (Y < m_Height) && (Y >= 0) && (X >= 0))
	{
		// KEYWORD: CollFix -- remove this once asset collisions work
		// we need collisions to check assets as well
		// Also add to cells for collisions
		if (m_Cells [X][Y]) {
			delete m_Cells[X][Y];
		}
		m_Cells[X][Y] = Cell;
	}
}