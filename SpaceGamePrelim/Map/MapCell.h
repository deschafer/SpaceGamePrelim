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
	std::vector<std::string>* m_RedTextureIDs;


	// Texture Properties
	Rect m_Dimensions;
	int m_CurrentFrame;
	int m_CurrentRow;

	bool m_UsingRedTextures;



public:
	MapCell();
	MapCell::MapCell(std::vector<std::string> TextureIDs, TextureProperties* Properties, MapCoordinate Position);
	MapCell::MapCell(TextureProperties* Properties, MapCoordinate Position);
	MapCell::MapCell(std::vector<std::string> RedTextureIDs, MapCoordinate Position);

	virtual void Draw(MapCoordinate Coords);

	void DrawStatic(MapCoordinate Coords);



	virtual ~MapCell();
};

