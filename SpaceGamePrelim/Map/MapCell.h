#pragma once

#include "MapObject.h"
#include "..\BasicTypes\BasicTypes.h"
#include "..\TextureCode\TextureManager.h"
#include "..\Objects\GameEntity.h"

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
	Default,
	Asset,
	Empty
};

class MapCell : public MapObject
{
protected:

	std::vector<std::string>* m_RedTextureIDs;
	bool m_Animated;
	Cell m_CellType;		// Type of this cell object
	Rect m_DestRect;		// Width and height of this individual object
	Rect m_OriginSize;

	// Texture Properties -- have to be list to support multiple textures
	std::vector<int> m_CurrentFrame;
	std::vector<int> m_CurrentRow;
	std::vector<int> m_AnimationSpeed;
	std::vector<int> m_NumberFrames;
	std::vector<int> m_RedTextureIndex;	// Indices for access of textures

	MapCell::MapCell(std::vector<std::string> RedTextureIDs,
		MapCoordinate Position,
		Cell CellType);
	MapCell::MapCell(std::vector<std::string> RedTextureIDs,
		std::vector<TextureProperties*> Properties,
		MapCoordinate Position,
		Cell CellType);
	MapCell::MapCell(std::vector<std::string> RedTextureIDs,
		MapCoordinate Position,
		Rect DstDimen,
		Cell CellType);

public:

	MapCell();

	void DrawStatic(MapCoordinate Coords);
	void DrawStatic(int X, int Y);
	void ChangeRedTextures(std::vector<std::string> NewTextures);
	void AddRedTexture(std::string RedTextureID) { m_RedTextureIndex.push_back(TextureManager::Instance()->GetRedTextureIndex(RedTextureID)); m_RedTextureIDs->push_back(RedTextureID); }
	std::vector<std::string>* ReturnRedTextures() { return m_RedTextureIDs; }
	Cell GetCellType() { return m_CellType; }
	MapCoordinate GetPosition() { return m_Position; }
	void SetPosition(MapCoordinate Position) { m_Position = Position; }
	void SetCellType(Cell CellType) { m_CellType = CellType; }
	virtual void Zoom();

	virtual void Draw(MapCoordinate Coords);
	virtual void Draw(double X, double Y);
	virtual bool OnCollision(GameEntity* Enitity);
	virtual bool IsCollidableType() { return false; }
	virtual void Update();
	virtual ~MapCell();
};

