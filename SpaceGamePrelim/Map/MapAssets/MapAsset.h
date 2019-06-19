#pragma once
#include "..\MapCell.h"


class MapAsset : public MapCell
{

public:

	void Load(std::vector<std::string> RedTextureIDs, // Function used to create this object
		MapCoordinate Position,
		Rect DestRect,
		Cell CellType);					
	virtual MapAsset* Copy() = 0;	// Returns a copy of this object
	virtual void OnInteraction(GameEntity* Entity) = 0;

	MapAsset();
	MapAsset(std::vector<std::string> RedTextureIDs,
		MapCoordinate Position,
		Rect SrcRect,
		Cell CellType);

	virtual ~MapAsset();
};

