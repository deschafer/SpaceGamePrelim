#pragma once

#include "Map.h"

class MapConnector
{
private:

	MapDirection m_Link;	// Link from map1 to map2
	Map* m_Map1;
	Map* m_Map2;

	bool ConnectMapVerti();
	bool ConnectMapHoriz();

public:
	MapConnector(Map* Map1, Map* Map2, MapDirection LinkBetween);

	bool ConnectMaps();

	~MapConnector();
};

