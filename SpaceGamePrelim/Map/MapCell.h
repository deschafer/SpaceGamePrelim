#pragma once

#include "MapObject.h"
#include "..\BasicTypes\BasicTypes.h"
#include "..\TextureCode\TextureManager.h"

#include <string>
#include <vector>

class MapCell : public MapObject
{
protected:

	std::vector<std::string> m_TextureIDs;	// Holds, in stack order, the textures used for this cell

	// Texture Properties
	Rect m_Dimensions;
	int m_CurrentFrame;
	int m_CurrentRow;

public:
	MapCell();
	MapCell::MapCell(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position);
	MapCell::MapCell(TextureProperties* Properties, MapCoordinate Position);

	void Draw();


	virtual ~MapCell();
};

