#pragma once
#include "Corridor.h"


class CorridorVertical : public Corridor
{
private:

	void AddBeginningCell(MapCoordinate Pos);
	void NormalMajorAxisMovement(MapCoordinate Pos);
	void MinorAxisMovementBeg(MapCoordinate Pos, Movement Direction);
	void MinorAxisMovementEnd(MapCoordinate Pos, Movement Direction);
	void NormalMinorAxisMovement(MapCoordinate Pos);


public:
	CorridorVertical(int Width, int Height);
	virtual ~CorridorVertical();

	virtual Corridor* GenerateCorridor(Array BoundsMatrix, Array CorridorLocaleMartrix, MapCoordinate Beg, MapCoordinate End, int DistanceBetween);

};

