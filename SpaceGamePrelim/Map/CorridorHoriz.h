#pragma once
#include "Corridor.h"


class CorridorHoriz : public Corridor
{
public:
	CorridorHoriz(int Width, int Height);
	virtual ~CorridorHoriz();

	virtual Corridor* GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix, MapCoordinate Beg, MapCoordinate End, int DistanceBetween);
};

