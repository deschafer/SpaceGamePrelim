#include "RoomConnector.h"
#include "GenRoomComp.h"

#include <iostream>

using namespace std;

static const int ComparisonSideWidth = 6;

RoomConnector::RoomConnector()
{}
RoomConnector::~RoomConnector()
{
	m_PossibleRooms.clear();
	m_PossibleRoomOffsets.clear();
}

//
// RoomConnector()
// When the base room to connect to is determined, however, the other room is not yet determined
//
RoomConnector::RoomConnector(MapRoom* BaseRoom, MapCoordinate BaseRoomOffset, std::vector<MapRoom*> PossibleRooms,
	std::vector<MapCoordinate> PossibleRoomOffsets) :
	m_BaseRoom(BaseRoom),
	m_BaseRoomPosition(BaseRoomOffset),
	m_PossibleRooms(PossibleRooms),
	m_PossibleRoomOffsets(PossibleRoomOffsets)
{
}

//
// RoomConnector()
// When both rooms to connect to are determined
//
RoomConnector::RoomConnector(MapRoom* BaseRoom, MapCoordinate BaseRoomOffset, MapRoom* OtherRoom, MapCoordinate OtherRoomOffset) : 
	m_BaseRoom(BaseRoom),
	m_BaseRoomPosition(BaseRoomOffset),
	m_OtherRoom(OtherRoom),
	m_OtherRoomPosition(OtherRoomOffset)
{
}

//
// FindRoomToConnectTo()
// Finds and returns, if it exists, a suitable room to connect to
//
MapRoom* RoomConnector::FindRoomToConnectToHoriz()
{
	int RoomHeight = m_BaseRoom->GetHeight();
	int RoomEnd = m_BaseRoomPosition.GetPositionY() + RoomHeight;
	int OffsetY = m_BaseRoomPosition.GetPositionY();
	int CurrRoomEnd = 0;
	int CurrRoomBeg = 0;
	MapRoom* CurrRoom = nullptr;
	vector<pair<MapRoom*, MapCoordinate>> CandidateRooms;	// Holds a mapRoom and its starting topleft pos
	vector<int> CandidateIndices;
	pair<MapCoordinate, MapCoordinate>* TempSide = nullptr;
	MapCoordinate Beginning;
	MapCoordinate Ending;

	for (size_t i = 0; i < (size_t)(CurrRoomBeg = (int)m_PossibleRoomOffsets.size()); i++)
	{
		CurrRoomBeg = m_PossibleRoomOffsets[i].GetPositionY();
		CurrRoom = m_PossibleRooms[i];
		CurrRoomEnd = CurrRoom->GetHeight() + CurrRoomBeg;

		// If this room is within the bounds of the room next to it,
		// we add it as a candidate for corridor generation
		if (CurrRoomBeg <= RoomEnd && CurrRoomEnd > OffsetY)
		{
			CandidateRooms.push_back(pair<MapRoom*, MapCoordinate>(CurrRoom, MapCoordinate(m_PossibleRoomOffsets[i].GetPositionX(), CurrRoomBeg)));
			CandidateIndices.push_back(i);
		}
		// If we have passed the margin, stop looking
		else if (CurrRoomBeg > RoomEnd) break;

	}
	// If there are no rooms, then we do not connect
	// Only happens rarely
	if (CandidateRooms.empty()) return nullptr;

	// Pick one of these rooms that preferable does not already have a link
	// Pick to one closed to the middle of this room

	// If there is only one room present, then we will connect to the room regardless
	if (CandidateRooms.size() == 1)
	{
		// The room has been determined, so save it
		m_OtherRoom = CandidateRooms[0].first;
		m_OtherRoomPosition = MapCoordinate(
			m_PossibleRoomOffsets[CandidateIndices[0]].GetPositionX(),
			m_PossibleRoomOffsets[CandidateIndices[0]].GetPositionY());
		return m_OtherRoom;
	}
	// If more than one room available, the anwser is more difficult
	else
	{
		vector<bool> Connections;
		int ConnectionlessCount = 0;
		int ConnectionlessIndex = -1;	// Holds the index of a connectionless room -- intended for one connectionless room only
		MapRoom* SelectedRoom = nullptr;

		for (size_t i = 0; i < CandidateRooms.size(); i++)
		{
			// Adding all of the connections for the candiate rooms
			if (CandidateRooms[i].first->ConnectedToRoom(Side::RIGHT))
			{
				Connections.push_back(true);
			}
			else
			{
				Connections.push_back(false);
				ConnectionlessCount++;
				ConnectionlessIndex = i;
			}
		}

		// If no rooms are connectionless
		if (ConnectionlessCount == 0)
		{
			return nullptr;
		}
		// If any of the rooms given does not have a connection, connect to one of those
		// If one connectionless room, connect to that one
		else if (ConnectionlessCount == 1)
		{
			// The room has been determined, so save it
			m_OtherRoom = CandidateRooms[ConnectionlessIndex].first;
			m_OtherRoomPosition = MapCoordinate(
				m_PossibleRoomOffsets[CandidateIndices[ConnectionlessIndex]].GetPositionX(),
				m_PossibleRoomOffsets[CandidateIndices[ConnectionlessIndex]].GetPositionY());
			return m_OtherRoom;
		}
		// More than one possible room
		else if (ConnectionlessCount)
		{
			int Index = -1;
			// Find the connectionless room that is above
			for (size_t i = 0; i < Connections.size(); i++)
			{
				// Due to how the rooms are found, from up to down, we can
				// always assume that the first connectionless index is the highest room
				if (!Connections[i])
				{
					Index = i;
					break;
				}
			}
			// The room has been determined, so save it
			m_OtherRoom = CandidateRooms[Index].first;
			m_OtherRoomPosition = MapCoordinate(
				m_PossibleRoomOffsets[CandidateIndices[Index]].GetPositionX(),
				CandidateRooms[Index].second.GetPositionY());
			return m_OtherRoom;
		}
	}
	return nullptr;
}

//
// FindRoomPositions()
// 
//
DeterminedRoomPositions* RoomConnector::FindRoomPositionsHoriz()
{
	int YStartPos = 0;
	int Difference = 0;
	int RightOffsetX = m_BaseRoomPosition.GetPositionX();
	int RightOffsetY = m_BaseRoomPosition.GetPositionY();
	int LeftOffsetX = m_OtherRoomPosition.GetPositionX();
	int LeftOffsetY = m_OtherRoomPosition.GetPositionY();
	pair<MapCoordinate, MapCoordinate>* GetLocationBegSide;
	pair<MapCoordinate, MapCoordinate>* TempSide = nullptr;

	// Set a start position within +/- 2 blocks of the center side (if Possible)
	// Save the pos. of this side
	// Get a side that is within +/- 2 blocks of the first selected side
	// Add new mapcells with text texutres at both sides
	// Eventually, use a horiz corridor creator to create the connection

	// First get a minor side from the major left side for the beginning room
	GetLocationBegSide = m_BaseRoom->GetFacingFromSide(Side::LEFT);

	// Then select a YPos in here for the actual start.
	Difference = GetLocationBegSide->first.GetPositionY() - GetLocationBegSide->second.GetPositionY();

	// This will a part of this side with variation
	YStartPos = rand() % (Difference - 1) + GetLocationBegSide->second.GetPositionY() + 1;

	// Now get the closest end position to here
	// First, find a ending side, if it exists, otherwise get the closest
	int LeftPositionBeg = 0;
	int RightPositionBeg = YStartPos - ComparisonSideWidth / 2 + RightOffsetY;
	int LeftPositionEnd = 0;
	int RightPositionEnd = YStartPos + ComparisonSideWidth / 2 + RightOffsetY;

	//int RightPositionBaseBeg = GetLocationBegSide->second.GetPositionY() + RightOffsetY;
	//int RightPositionBaseEnd = GetLocationBegSide->first.GetPositionY() + RightOffsetY;
	int RightPositionBaseBeg = RightOffsetY;
	int RightPositionBaseEnd = m_BaseRoom->GetHeight() + RightOffsetY;
	int LeftRoomOffsetX = LeftOffsetX;
	int LeftRoomOffsetY = LeftOffsetY;
	vector<pair<MapCoordinate, MapCoordinate>> WideCandidateSides;
	vector<pair<MapCoordinate, MapCoordinate>> CandidateSides;
	bool PerfectFit = false;

	for (size_t Index = 0; ; Index++)
	{
		TempSide = m_OtherRoom->GetFacingFromSideIndexed(Side::RIGHT, Index);

		if (!TempSide) break;

		LeftPositionBeg = TempSide->first.GetPositionY() + LeftRoomOffsetY;
		LeftPositionEnd = TempSide->second.GetPositionY() + LeftRoomOffsetY;

		// Determine if this side is within the desired position
		// First case - side is within a set param, like 2 blocks, of the starting corridor position
		if (LeftPositionBeg <= RightPositionEnd && LeftPositionEnd > RightPositionBeg)
		{
			CandidateSides.push_back(*TempSide);
		}
		// Second case - side is within the bounds of the entire side of where the starting corridor pos. is
		else if (LeftPositionBeg <= RightPositionBaseEnd && LeftPositionEnd >= RightPositionBaseBeg)
		{
			WideCandidateSides.push_back(*TempSide);
		}
		// Third case - If the exact Y position of the starting corridor position is located within this side
		if (LeftPositionBeg <= (YStartPos + RightOffsetY) && LeftPositionEnd >= (YStartPos + RightOffsetY))
		{
			PerfectFit = true;
			// We have a fitting side, so leave
			break;
		}
	}

	// Setting a formal MapCoord for the starting room pos.
	MapCoordinate RightRoomPoint(
		RightOffsetX + GetLocationBegSide->first.GetPositionX(),
		RightOffsetY + YStartPos);
	MapCoordinate LeftRoomPoint(0, 0);

	if (PerfectFit)
	{
		// Good to go, we can set this ypos as the ending pos.

		// Need to verify we are not at the absolute end of this side
		// Since we are on the Right side of the left room,
		// Verify that this point is not equal to a point in the room. 
		// If it is, incr or decr appropriately.
		int Pos = YStartPos + RightOffsetY;

		// Upper part of this side section
		// Determining if this point is at the endge of this side, which
		// usually is not a valid place for a corridor
		if (Pos == TempSide->first.GetPositionY() + LeftRoomOffsetY)
		{
			// Move downwards one, increment by 1
			Pos++;
		}
		else if (Pos == TempSide->second.GetPositionY() + LeftRoomOffsetY)
		{
			// Move upwards one, increment by 1
			Pos--;
		}

		// Save this position
		LeftRoomPoint = MapCoordinate(
			LeftRoomOffsetX + TempSide->first.GetPositionX(),
			Pos);
	}
	// If there are several sides that work
	else if (!CandidateSides.empty() || !WideCandidateSides.empty())
	{
		// Select a random side from those available
		if (!CandidateSides.empty())
		{
			int SideIndex = rand() % CandidateSides.size();
			TempSide = &CandidateSides[SideIndex];
		}
		else if (!WideCandidateSides.empty())
		{
			int SideIndex = rand() % WideCandidateSides.size();
			TempSide = &WideCandidateSides[SideIndex];
		}

		// Get a magnitude of the difference
		int SideDifference = TempSide->second.GetPositionY() - TempSide->first.GetPositionY();

		// Start at the middle of this side
		int Pos = TempSide->first.GetPositionY() + SideDifference / 2 + LeftRoomOffsetY;
		int Goal = YStartPos + RightOffsetY;
		int EndLegalSideTop = TempSide->first.GetPositionY() + LeftRoomOffsetY + 1;
		int StartLegalSideBottom = TempSide->second.GetPositionY() + LeftRoomOffsetY - 1;

		// Move to a pos as close to the other point as possible
		while (true)
		{
			if (Pos > Goal)
			{
				if (Pos - 1 > EndLegalSideTop) Pos--;
				else break;
			}
			else
			{
				if (Pos + 1 < StartLegalSideBottom) Pos++;
				else break;
			}
		}

		// Save this new position
		LeftRoomPoint = MapCoordinate(
			LeftRoomOffsetX + TempSide->first.GetPositionX(),
			Pos);
	}
	// If there was an ending point found
	if (TempSide != nullptr)
	{
		int RightRoomXPos = RightOffsetX;
		int LeftRoomXPos = LeftRoomOffsetX + m_OtherRoom->GetWidth();

		// Adds the link to the left room
		m_OtherRoom->AddLinkedRoom(Side::RIGHT, m_BaseRoom);
		// Adds the link to the right room
		m_BaseRoom->AddLinkedRoom(Side::LEFT, m_OtherRoom);

		DeterminedRoomPositions *ReturnValue = new DeterminedRoomPositions;
		ReturnValue->BaseOffset = MapCoordinate(
			RightRoomPoint.GetPositionX() - m_BaseRoomPosition.GetPositionX(),
			RightRoomPoint.GetPositionY() - m_BaseRoomPosition.GetPositionY());
		ReturnValue->OtherRoom = m_OtherRoom;
		ReturnValue->OtherPosOffset = MapCoordinate(
			LeftRoomPoint.GetPositionX() - m_OtherRoomPosition.GetPositionX(),
			LeftRoomPoint.GetPositionY() - m_OtherRoomPosition.GetPositionY());

		return ReturnValue;
	}
	else
	{
	#ifdef _DEBUG
		cout << "Corridor Horiz Gen Error: No ending point found for "
			<< m_BaseRoom->GetRoomType()
			<< " to "
			<< m_OtherRoom->GetRoomType()
			<< endl;
	#endif // _DEBUG
	}
	return nullptr;
}

//
// FindRoomPositionsVerti()
//
//
DeterminedRoomPositions* RoomConnector::FindRoomPositionsVerti()
{
	// Verify that this object is being used correctly
	if (!m_OtherRoom || !m_BaseRoom)
	{
		cout << "RoomConnector: Base room or other room nullptr" << endl;
		return nullptr;
	}

	// There is a room above us, so we need to find a suitable location for a corridor.
	MapRoom* RoomAbove = m_BaseRoom;
	MapRoom* BottomRoom = m_OtherRoom;
	MapCoordinate CurrLocation;
	MapCoordinate AboveLocation;
	pair<MapCoordinate, MapCoordinate>* GetLocationBeg;
	pair<MapCoordinate, MapCoordinate>* GetLocationEnd;

	vector<string> temp;
	int AboveWidth = RoomAbove->GetWidth();
	int AboveOffsetX = m_BaseRoomPosition.GetPositionX();
	int BelowOffsetX = m_OtherRoomPosition.GetPositionX();
	int AboveDifference = 0;
	int CurrDifference = 0;
	int ShorterXLoc = 0;
	bool CurrentIsShorter = false;

	// Ask the room above for a suitable side to set as a beginning tile for the end point
	// for this corridor.
	// Get a wall from the indicated side
	GetLocationBeg = RoomAbove->GetFacingFromSide(Side::BOTTOM);
	// Do the same for the current room on the top side
	GetLocationEnd = BottomRoom->GetFacingFromSide(Side::TOP);

	// Next find the smallest side
	// If this wall with min size exists
	if (GetLocationBeg)
	{
		AboveDifference = (GetLocationBeg->first.GetPositionX() - GetLocationBeg->second.GetPositionX());
	}
	else return nullptr;
	if (GetLocationEnd)
	{
		CurrDifference = (GetLocationEnd->first.GetPositionX() - GetLocationEnd->second.GetPositionX());
	}
	else return nullptr;

	// Handle the shorter side first
	if (abs(CurrDifference) > abs(AboveDifference))
	{
		AboveLocation = MapCoordinate(GetLocationBeg->first.GetPositionX() - AboveDifference / 2 + AboveOffsetX, GetLocationBeg->first.GetPositionY());
		ShorterXLoc = AboveLocation.GetPositionX(); // Saving this x pos with respect to column offset
	}
	else
	{
		CurrLocation = MapCoordinate(GetLocationEnd->first.GetPositionX() - CurrDifference / 2 + BelowOffsetX, GetLocationEnd->first.GetPositionY());
		ShorterXLoc = CurrLocation.GetPositionX(); // Saving this x pos with respect to column offset
		CurrentIsShorter = true;
	}

	// Then we need to process the room above
	if (CurrentIsShorter)
	{
		// Standardize the coordinates so we can compare them
		int XPos1 = GetLocationBeg->first.GetPositionX() + AboveOffsetX;
		int XPos2 = GetLocationBeg->second.GetPositionX() + AboveOffsetX;

		// If the x coord of the shorter is within the range of this side
		// if so, that is the new point
		if ((XPos1 < ShorterXLoc && ShorterXLoc < XPos2) ||
			(XPos1 > ShorterXLoc && ShorterXLoc > XPos2))
		{
			AboveLocation = MapCoordinate(ShorterXLoc, GetLocationBeg->first.GetPositionY());
		}
		else AboveLocation = MapCoordinate(GetLocationBeg->first.GetPositionX() - AboveDifference / 2 + AboveOffsetX, GetLocationBeg->first.GetPositionY());

	}
	// Then we need to process the room below
	else
	{
		int XPos1 = GetLocationEnd->first.GetPositionX() + BelowOffsetX;
		int XPos2 = GetLocationEnd->second.GetPositionX() + BelowOffsetX;

		// If the x coord of the shorter is within the range of this side
		// if so, that is the new point
		if ((XPos1 < ShorterXLoc && ShorterXLoc < XPos2) ||
			(XPos1 > ShorterXLoc && ShorterXLoc > XPos2))
		{
			CurrLocation = MapCoordinate(ShorterXLoc, GetLocationEnd->first.GetPositionY());
		}
		else CurrLocation = MapCoordinate(GetLocationEnd->first.GetPositionX() - CurrDifference / 2 + BelowOffsetX, GetLocationEnd->first.GetPositionY());
	}

	DeterminedRoomPositions* ReturnValue = new DeterminedRoomPositions;

	// Need to standardize the entries with respect to each room
	ReturnValue->BaseOffset = MapCoordinate(
		AboveLocation.GetPositionX() - AboveOffsetX,
		AboveLocation.GetPositionY());

	ReturnValue->OtherRoom = m_OtherRoom;
	ReturnValue->OtherPosOffset = MapCoordinate(
		CurrLocation.GetPositionX() - BelowOffsetX,
		CurrLocation.GetPositionY());

 	return ReturnValue;
}

