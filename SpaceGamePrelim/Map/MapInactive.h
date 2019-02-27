#pragma once
#include "MapCell.h"

class MapInactive : public MapCell
{
public:
	MapInactive();
	virtual ~MapInactive();

	MapInactive(std::vector<std::string> RedTextureIDs, std::vector<TextureProperties*> Properties,
		MapCoordinate Position, Cell CellType);
	MapInactive(TextureProperties* Properties, MapCoordinate Position, Cell CellType);
	MapInactive(std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell CellType);

};

