#pragma once
#include "MapCell.h"

class MapInactive : public MapCell
{

public:


	MapInactive();
	virtual ~MapInactive();

	MapInactive(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position);
	MapInactive(TextureProperties* Properties, MapCoordinate Position);
	MapInactive(std::vector<std::string> RedTextureIDs, MapCoordinate Position);

	virtual void Draw(MapCoordinate Coords);
};

