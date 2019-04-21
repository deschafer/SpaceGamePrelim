#pragma once
#include "MapCell.h"

// Wall object class -- will feature some sort of collision interaction with the player, or other objects
class MapWall : public MapCell
{
public:
	MapWall();
	~MapWall();

	MapWall(std::vector<std::string> RedTextureIDs, std::vector<TextureProperties*> Properties, 
		MapCoordinate Position, Cell CellType);
	MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell MapCell);
	MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position, Rect DestRect, Cell MapCell);


	virtual bool OnCollision(GameEntity* Enitity) override;
	virtual bool IsCollidableType() override { return true; }
};

