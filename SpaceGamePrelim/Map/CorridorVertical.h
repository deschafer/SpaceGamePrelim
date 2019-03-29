#pragma once
#include "Corridor.h"


class CorridorVertical : public Corridor
{
public:
	CorridorVertical();
	virtual ~CorridorVertical();

	virtual Array GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix);

};

