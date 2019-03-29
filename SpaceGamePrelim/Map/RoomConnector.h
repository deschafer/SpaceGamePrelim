#pragma once
#include <vector>

#include "MapRoom.h"


struct DeterminedRoomPositions
{
	MapCoordinate BaseOffset;		// Offset within array of base room for the starting loc
	MapRoom* OtherRoom;				// Pointer to the other room
	MapCoordinate OtherPosOffset;	// Offset within array of other room for the ending loc
};

class RoomConnector
{
private:
	RoomConnector();

	MapRoom* m_BaseRoom;
	MapCoordinate m_BaseRoomPosition;
	MapRoom* m_OtherRoom;
	MapCoordinate m_OtherRoomPosition;

	std::vector<MapRoom*> m_PossibleRooms;
	std::vector<MapCoordinate> m_PossibleRoomOffsets;
	int m_OtherRoomFoundIndex;
	

public:
	RoomConnector(MapRoom* BaseRoom, MapCoordinate BaseRoomOffset, std::vector<MapRoom*> PossibleRooms, 
		std::vector<MapCoordinate> PossibleRoomOffsets);
	RoomConnector(MapRoom* BaseRoom, MapCoordinate BaseRoomOffset, MapRoom* OtherRoom, MapCoordinate OtherRoomOffset);

	MapRoom* FindRoomToConnectToHoriz();

	DeterminedRoomPositions* FindRoomPositionsHoriz();
	DeterminedRoomPositions* FindRoomPositionsVerti();

	MapCoordinate GetOtherRoomPosition() { return m_OtherRoomPosition; }

	~RoomConnector();
};

