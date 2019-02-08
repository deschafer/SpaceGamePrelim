#pragma once
#include "MapCell.h"

// Wall object class -- will feature some sort of collision interaction with the player, or other objects
class MapWall : public MapCell
{
public:
	MapWall();
	~MapWall();

	MapWall(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position);
	MapWall(TextureProperties* Properties, MapCoordinate Position);
	MapWall(std::vector<std::string> RedTextureIDs, MapCoordinate Position);

	virtual void Draw(MapCoordinate Coords);


};

