#pragma once

#include "MapWall.h"
#include "MapInactive.h"

#include <vector>

typedef std::vector<std::vector<MapCell*>> Array;

class Corridor
{
private:

	std::vector<std::vector<MapCell*>> m_Cells;
	std::string MapType;

public:
	Corridor();
	virtual ~Corridor();

	Array GetCells() { return m_Cells; }
	virtual Array GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix) = 0;
};

