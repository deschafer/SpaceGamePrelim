#pragma once
#include "MapCell.h"


class MapInactive : public MapCell
{
public:
	MapInactive() = delete;
	MapInactive(Rect Dimensions, Scene* ParentScene);
	virtual ~MapInactive();

	MapInactive(Rect Dimensions, Scene* ParentScene, std::vector<std::string> RedTextureIDs, std::vector<TextureProperties*> Properties,
		MapCoordinate Position, Cell CellType);
	MapInactive(Rect Dimensions, Scene* ParentScene, std::vector<std::string> RedTextureIDs, MapCoordinate Position, Cell CellType);
	MapInactive(Rect Dimensions, Scene* ParentScene, std::vector<std::string> RedTextureIDs, MapCoordinate Position, Rect DestRect, Cell MapCell);

};

