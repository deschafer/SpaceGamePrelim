#pragma once

#include "MapObject.h"
#include "..\BasicTypes\BasicTypes.h"
#include "..\TextureCode\TextureManager.h"

#include <string>
#include <vector>

enum class Cell
{
	Floor,
	Wall_Top,
	Wall_Bottom,
	Wall_Left,
	Wall_Right, 
	Wall_Corner_Left,
	Wall_Corner_Right,
	Wall_Corner_Left_Bottom,
	Wall_Corner_Right_Bottom,
	Default
};

class MapCell : public MapObject
{
protected:

	std::vector<std::string> m_TextureIDs;	// Holds, in stack order, the textures used for this cell
	std::vector<std::string>* m_RedTextureIDs;
	bool m_UsingRedTextures;
	bool m_Animated;

	Cell m_CellType;

	// Texture Properties
	Rect m_Dimensions;
	int m_CurrentFrame;
	int m_CurrentRow;
	int m_AnimationSpeed;
	int m_NumberFrames;


public:
	MapCell();
	MapCell::MapCell(std::vector<std::string> RedTextureIDs, 
		MapCoordinate Position, 
		Cell CellType);

	MapCell::MapCell(std::vector<std::string> RedTextureIDs, 
		std::vector<TextureProperties*> Properties, 
		MapCoordinate Position, 
		Cell CellType);


	virtual void Draw(MapCoordinate Coords);
	void DrawStatic(MapCoordinate Coords);
	virtual void Update();


	void ChangeRedTextures(std::vector<std::string> NewTextures);
	std::vector<std::string>* ReturnRedTextures() { return m_RedTextureIDs; }

	Cell GetCellType() { return m_CellType; }
	Cell SetCellType(Cell CellType) { m_CellType = CellType; }

	virtual ~MapCell();
};

