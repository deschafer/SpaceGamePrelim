#pragma once

#include "MapObject.h"


#include <string>
#include <vector>

class MapCell : public MapObject
{
protected:

	std::vector<std::string> m_TextureIDs;	// Holds, in stack order, the textures used for this cell

public:
	MapCell();
	virtual ~MapCell();
};

