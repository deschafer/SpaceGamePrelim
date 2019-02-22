#pragma once
#include "MapCell.h"

// Wall object class -- will feature some sort of collision interaction with the player, or other objects
class MapWall : public MapCell
{
public:
	MapWall();
	~MapWall();

	MapWall(std::vector<std::string> TextureIDs, TextureProperties* Properties, 
		MapCoordinate Position, Cell MapCell);
	MapWall(TextureProperties* Properties, MapCoordinate Position, Cell MapCell);
	MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell MapCell);

	virtual void Draw(MapCoordinate Coords);
};

