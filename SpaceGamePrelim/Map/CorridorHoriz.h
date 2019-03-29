#pragma once
#include "Corridor.h"


class CorridorHoriz : public Corridor
{
public:
	CorridorHoriz();
	virtual ~CorridorHoriz();

	virtual Array GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix);
};

