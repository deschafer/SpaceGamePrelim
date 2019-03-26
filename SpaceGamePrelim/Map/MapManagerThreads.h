#pragma once

#include "MapManager.h"
#include "../Frame/Threads.h"

// Thread containing the information passed to the thread
// that draws corridors between room
struct CorridorThreadInformation
{
	Map* MapFrom;
	Map* MapTo;
	MapDirection LinkBetween;
	MapManager* Manager;
	CorridorThreadInformation(
		Map* Map1,
		Map* Map2,
		MapDirection Link,
		MapManager* manager) :
		MapFrom(Map1),
		MapTo(Map2),
		LinkBetween(Link),
		Manager(manager)
	{}
};

enum class MapManagerThreadType{MapGenerate, MapCorridorGenerate, CellsUpdate};


int ThreadGenerator(void* Memory, MapManagerThreadType DesiredFunc);
