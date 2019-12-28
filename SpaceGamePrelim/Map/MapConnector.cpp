#include "MapConnector.h"
#include "GenRoomComp.h"
#include "RoomConnector.h"
#include "CorridorHoriz.h"
#include "CorridorVertical.h"
#include "MapManager.h"

using namespace std;


MapConnector::MapConnector(Map* Map1, Map* Map2, MapDirection LinkBetween) :
	m_Map1(Map1),
	m_Map2(Map2),
	m_Link(LinkBetween)
{
}

MapConnector::~MapConnector()
{
}

//
//
//
//
bool MapConnector::ConnectMaps()
{
	bool Result = false;

	if (m_Link == MapDirection::North)
	{
		Result = ConnectMapVerti();
	}
	else if (m_Link == MapDirection::East)
	{
		Result = ConnectMapHoriz();
	}

	return Result;
}

//
// ConnectMapVerti()
//
//
bool MapConnector::ConnectMapVerti()
{
	typedef pair<MapCoordinate, MapCoordinate> SideA;
	MapRoom* AboveRoom;
	MapRoom* BelowRoom;
	MapCoordinate BeginningPos;
	MapCoordinate EndingPos;
	MapCoordinate UnStdEnding;
	MapCoordinate StdRoomBelowOffset;
	MapCoordinate StdRoomAboveOffset;
	MapCoordinate TopLeft;
	MapCoordinate BottomRight;
	DeterminedRoomPositions* Positions;
	int AboveOffsetX = 0;
	int AboveOffsetY = 0;
	int BelowOffsetX = 0;
	int BelowOffsetY = 0;
	int MapAboveHeight = m_Map1->GetHeight();
	int DistanceX;
	int DistanceY;
	int MajorDistance = 0;

	// Connect each room located at each column
	for (int i = 0;; i++)
	{
		// Get a room from each map
		AboveRoom = m_Map1->GetRoomXFromColumnY(0, i, AboveOffsetX, AboveOffsetY, true);	// Last room in this column
		BelowRoom = m_Map2->GetRoomXFromColumnY(0, i, BelowOffsetX, BelowOffsetY);			// First room in this column

		if (!AboveRoom || !BelowRoom)
			break;
																							
		// Standardizing the offsets of each of these rooms into one plane so we can compare them
		StdRoomAboveOffset = MapCoordinate(AboveOffsetX, AboveOffsetY);
		StdRoomBelowOffset = MapCoordinate(BelowOffsetX, BelowOffsetY + m_Map1->GetHeight());

		// Create our connector
		RoomConnector VerticalConnector(AboveRoom, StdRoomAboveOffset, BelowRoom, StdRoomBelowOffset);

		Positions = VerticalConnector.FindRoomPositionsVerti();

		if (Positions)
		{
			// Translate back into standardized coordinates
			BeginningPos = MapCoordinate(
				StdRoomAboveOffset.GetPositionX() + Positions->BaseOffset.GetPositionX(),
				StdRoomAboveOffset.GetPositionY() + Positions->BaseOffset.GetPositionY());
			EndingPos = MapCoordinate(
				StdRoomBelowOffset.GetPositionX() + Positions->OtherPosOffset.GetPositionX(),
				StdRoomBelowOffset.GetPositionY() + Positions->OtherPosOffset.GetPositionY());
			UnStdEnding = MapCoordinate(
				BelowOffsetX + Positions->OtherPosOffset.GetPositionX(),
				BelowOffsetY + Positions->OtherPosOffset.GetPositionY());

			// Determine bounds rectangle
			// If the AboveLoc is furthest left
			if (BeginningPos.GetPositionX() <= EndingPos.GetPositionX())
			{
				TopLeft = MapCoordinate(BeginningPos.GetPositionX() - 1, BeginningPos.GetPositionY() - 1);
				BottomRight = MapCoordinate(EndingPos.GetPositionX() +  1 , EndingPos.GetPositionY() + 1);
			}
			else
			{
				TopLeft = MapCoordinate(EndingPos.GetPositionX() - 1, BeginningPos.GetPositionY() - 1);
				BottomRight = MapCoordinate(BeginningPos.GetPositionX() +  1 , EndingPos.GetPositionY() + 1);
			}

			// DistanceX, distanceY, major axis distance
			DistanceX = abs(TopLeft.GetPositionX() - BottomRight.GetPositionX()) + 1;
			DistanceY = abs(TopLeft.GetPositionY() - BottomRight.GetPositionY()) + 1;

			MajorDistance = abs(StdRoomAboveOffset.GetPositionY() + AboveRoom->GetHeight() - StdRoomBelowOffset.GetPositionY());
			if (!MajorDistance)
				MajorDistance = 1;

			vector<vector<MapCell*>> BoundsMatrix;
			vector<vector<MapCell*>> CorridorLocaleMatrix;
			MapObject* TempCell;

			// Translate coordinates of positions back into rel coord for the rectangle
			MapCoordinate BoundedTop = MapCoordinate(BeginningPos.GetPositionX() - TopLeft.GetPositionX(), BeginningPos.GetPositionY() - TopLeft.GetPositionY());
			MapCoordinate BoundedBottom = MapCoordinate(EndingPos.GetPositionX() - TopLeft.GetPositionX(), EndingPos.GetPositionY() - TopLeft.GetPositionY());

			// Create bounds matrix
			// Create corridor bounds matrix
			for (int i = 0, X = TopLeft.GetPositionX(); i < DistanceX; i++, X++)
			{
				BoundsMatrix.push_back(vector<MapCell*>());
				CorridorLocaleMatrix.push_back(vector<MapCell*>());
				for (int j = 0, Y = TopLeft.GetPositionY(), YBelow = 0; j < DistanceY; j++, Y++)
				{
					// We get from the cells of the map below
					if (Y >= MapAboveHeight)
					{
						if (TempCell = m_Map2->GetCell(X, YBelow))
						{
							BoundsMatrix[i].push_back(static_cast<MapCell*>(TempCell));
						}
						else BoundsMatrix[i].push_back(nullptr);

						if (TempCell = m_Map2->GetCorridorCell(X, YBelow))
						{
							CorridorLocaleMatrix[i].push_back(static_cast<MapCell*>(TempCell));
						}
						else CorridorLocaleMatrix[i].push_back(nullptr);
						YBelow++;
					}
					// Get from the map above 
					else
					{
						if (TempCell = m_Map1->GetCell(X, Y))
						{
							BoundsMatrix[i].push_back(static_cast<MapCell*>(TempCell));
						}
						else BoundsMatrix[i].push_back(nullptr);

						if (TempCell = m_Map1->GetCorridorCell(X, Y))
						{
							CorridorLocaleMatrix[i].push_back(static_cast<MapCell*>(TempCell));
						}
						else CorridorLocaleMatrix[i].push_back(nullptr);
					}
				}
			}

			// Create corridor
			CorridorVertical *NewCorridor = new CorridorVertical(DistanceX, DistanceY, AboveRoom, BelowRoom);

			// Generate corridor
			if (NewCorridor->GenerateCorridor(BoundsMatrix, CorridorLocaleMatrix, BoundedTop, BoundedBottom, MajorDistance))
			{
				// Then place the corridor cells into the map
				MapCell*** CorridorCells = NewCorridor->GetCells();

				// Create the bounds matrix and corridor locale matrix
				for (int i = 0, X = TopLeft.GetPositionX(); i < DistanceX; i++, X++)
				{
					for (int j = 0, Y = TopLeft.GetPositionY(), YBelow = 0; j < DistanceY; j++, Y++)
					{
						if (Y >= MapAboveHeight)
						{
							m_Map2->SetNewCorridorCell(MapCoordinate(X, YBelow), CorridorCells[i][j], (UnStdEnding == MapCoordinate(X, YBelow) ? true : false));
							YBelow++;
						}
						else
						{
							m_Map1->SetNewCorridorCell(MapCoordinate(X, Y), CorridorCells[i][j], false);
						}
					}
				}
			}
		}
	}
	// Finally mark this room as connected
	m_Map2->SetPhysicallyLinkedNorth();
	m_Map1->IsPhysicallyLinkedSouth();

	MapManager::Instance()->RemoveQueuedMap();

	return true;
}

//
// ConnectMapHoriz()
//
//
bool MapConnector::ConnectMapHoriz()
{
	typedef pair<MapCoordinate, MapCoordinate> SideA;
	MapRoom* RightRoom;
	MapRoom* LeftRoom;
	vector<MapCoordinate> RoomOffsets;
	MapCoordinate BeginningPos;
	MapCoordinate EndingPos;
	MapCoordinate UnStdEnding;
	MapCoordinate StdRoomRightOffset;
	MapCoordinate StdRoomLeftOffset;
	MapCoordinate TopLeft;
	MapCoordinate BottomRight;
	MapCoordinate* Temp;
	DeterminedRoomPositions* Positions;
	int RightOffsetX = 0;
	int RightOffsetY = 0;
	int LeftOffsetX = 0;
	int LeftOffsetY = 0;
	int MapLeftWidth = m_Map2->GetWidth();
	int DistanceX;
	int DistanceY;
	int MajorDistance = 0;

	// Connect each room located at each column
	for (int i = 0;; i++)
	{
		// Get a room from each map
		RightRoom = m_Map1->GetRoomXFromColumnY(i, 0, RightOffsetX, RightOffsetY, false);	// Last room in this column
		if (!RightRoom)
			break;

		// Generate room offsets
		RoomOffsets.clear();
		for (size_t i = 0; Temp = m_Map2->GetRoomOffsetsFromLastRow(i); i++)
		{
			RoomOffsets.push_back(*Temp);
			delete Temp;
		}

		// Get the furthest right row of rooms
		vector<MapRoom*> PossibleRooms = m_Map2->GetRoomsFromColumnX(0, true);

		// Standardizing the offsets of each of these rooms into one plane so we can compare them
		StdRoomRightOffset = MapCoordinate(RightOffsetX + m_Map2->GetWidth(), RightOffsetY);

		// Create our connector
		RoomConnector HorizConnector(RightRoom, StdRoomRightOffset, PossibleRooms, RoomOffsets);
		
		LeftRoom = HorizConnector.FindRoomToConnectToHoriz();
		if (!LeftRoom)
		{
			break;
		}

		// Get the data from the left room for later on
		StdRoomLeftOffset = HorizConnector.GetOtherRoomPosition();

		// Otherwise continue with points
		Positions = HorizConnector.FindRoomPositionsHoriz();
		if (Positions)
		{
			// Translate back into standardized coordinates
			BeginningPos = MapCoordinate(
				StdRoomLeftOffset.GetPositionX() + Positions->OtherPosOffset.GetPositionX(),
				StdRoomLeftOffset.GetPositionY() + Positions->OtherPosOffset.GetPositionY());
			EndingPos = MapCoordinate(
				StdRoomRightOffset.GetPositionX() + Positions->BaseOffset.GetPositionX(),
				StdRoomRightOffset.GetPositionY() + Positions->BaseOffset.GetPositionY());
			UnStdEnding = MapCoordinate(
				RightOffsetX + Positions->OtherPosOffset.GetPositionX(),
				RightOffsetY + Positions->OtherPosOffset.GetPositionY());

			// Determine bounds rectangle
			// If the AboveLoc is furthest left
			if (BeginningPos.GetPositionY() <= EndingPos.GetPositionY())
			{
				TopLeft = MapCoordinate(BeginningPos.GetPositionX() - 1, BeginningPos.GetPositionY() - 1);
				BottomRight = MapCoordinate(EndingPos.GetPositionX() + 1, EndingPos.GetPositionY() + 1);
			}
			else
			{
				TopLeft = MapCoordinate(BeginningPos.GetPositionX() - 1, EndingPos.GetPositionY() - 1);
				BottomRight = MapCoordinate(EndingPos.GetPositionX() + 1, BeginningPos.GetPositionY() + 1);
			}

			if (TopLeft.GetPositionX() > BottomRight.GetPositionX() || BottomRight.GetPositionY() < TopLeft.GetPositionY())
			{
				cout << "Anohter Problem" << endl;
			}

			// DistanceX, distanceY, major axis distance
			DistanceX = abs(TopLeft.GetPositionX() - BottomRight.GetPositionX()) + 1;
			DistanceY = abs(TopLeft.GetPositionY() - BottomRight.GetPositionY()) + 1;

			MajorDistance = abs(StdRoomRightOffset.GetPositionX() - LeftRoom->GetWidth() - StdRoomLeftOffset.GetPositionX());
			if (!MajorDistance)
				MajorDistance = 1;

			vector<vector<MapCell*>> BoundsMatrix;
			vector<vector<MapCell*>> CorridorLocaleMatrix;
			MapObject* TempCell;

			// Translate coordinates of positions back into rel coord for the rectangle
			MapCoordinate BoundedLeft = MapCoordinate(BeginningPos.GetPositionX() - TopLeft.GetPositionX(), BeginningPos.GetPositionY() - TopLeft.GetPositionY());
			MapCoordinate BoundedRight = MapCoordinate(EndingPos.GetPositionX() - TopLeft.GetPositionX(), EndingPos.GetPositionY() - TopLeft.GetPositionY());

			// Create bounds matrix
			// Create corridor bounds matrix
			for (int i = 0, X = TopLeft.GetPositionX(), XRight = 0; i < DistanceX; i++, X++)
			{
				BoundsMatrix.push_back(vector<MapCell*>());
				CorridorLocaleMatrix.push_back(vector<MapCell*>());
				for (int j = 0, Y = TopLeft.GetPositionY(); j < DistanceY; j++, Y++)
				{
					// We get from the cells of the map below
					if (X >= MapLeftWidth)
					{
						if (TempCell = m_Map1->GetCell(XRight, Y))
						{
							BoundsMatrix[i].push_back(static_cast<MapCell*>(TempCell));
						}
						else BoundsMatrix[i].push_back(nullptr);

						if (TempCell = m_Map1->GetCorridorCell(XRight, Y))
						{
							CorridorLocaleMatrix[i].push_back(static_cast<MapCell*>(TempCell));
						}
						else CorridorLocaleMatrix[i].push_back(nullptr);
					}
					// Get from the map above 
					else
					{
						if (TempCell = m_Map2->GetCell(X, Y))
						{
							BoundsMatrix[i].push_back(static_cast<MapCell*>(TempCell));
						}
						else BoundsMatrix[i].push_back(nullptr);

						if (TempCell = m_Map2->GetCorridorCell(X, Y))
						{
							CorridorLocaleMatrix[i].push_back(static_cast<MapCell*>(TempCell));
						}
						else CorridorLocaleMatrix[i].push_back(nullptr);
					}
				}
				if(X >= MapLeftWidth)
					XRight++;
			}

			// Create corridor
			CorridorHoriz *NewCorridor = new CorridorHoriz(DistanceX, DistanceY, RightRoom, LeftRoom);

			if (BoundedLeft.GetPositionX() > BoundedRight.GetPositionX())
			{
				cout << "A Problem" << endl;
			}

			// Generate corridor
			if (NewCorridor->GenerateCorridor(BoundsMatrix, CorridorLocaleMatrix, BoundedRight , BoundedLeft, MajorDistance))
			{
				// Then place the corridor cells into the map
				MapCell*** CorridorCells = NewCorridor->GetCells();

				// Create the bounds matrix and corridor locale matrix
				for (int i = 0, X = TopLeft.GetPositionX(), XRight = 0; i < DistanceX; i++, X++)
				{
					for (int j = 0, Y = TopLeft.GetPositionY(); j < DistanceY; j++, Y++)
					{
						if (X >= MapLeftWidth)
						{
							m_Map1->SetNewCorridorCell(MapCoordinate(XRight, Y), CorridorCells[i][j], (UnStdEnding == MapCoordinate(XRight, Y) ? true : false));
							
						}
						else
						{
							m_Map2->SetNewCorridorCell(MapCoordinate(X, Y), CorridorCells[i][j], false);
						}
					}
					if (X >= MapLeftWidth)
						XRight++;
				}
			}
			else
			{
				vector<string> Textures;
				Textures.push_back("Explosion");

				m_Map2->SetCell(BeginningPos.GetPositionX(), BeginningPos.GetPositionY(), new MapWall(Textures, MapCoordinate(0, 0), Cell::Default));
				Textures.clear();
				Textures.push_back("Explosion");

				m_Map1->SetCell(EndingPos.GetPositionX() - m_Map1->GetWidth(), EndingPos.GetPositionY(), new MapWall(Textures, MapCoordinate(0, 0), Cell::Default));

				Textures.clear();
				Textures.push_back("Test");

				m_Map2->SetCell(TopLeft.GetPositionX(), TopLeft.GetPositionY(), new MapWall(Textures, MapCoordinate(0, 0), Cell::Default));
				Textures.clear();
				Textures.push_back("Test");

				m_Map1->SetCell(BottomRight.GetPositionX() - m_Map1->GetWidth(), BottomRight.GetPositionY(), new MapWall(Textures, MapCoordinate(0, 0), Cell::Default));
			}
		}
	}

	// Finally mark this room as connected
	m_Map1->SetPhysicallyLinkedWest();
	m_Map2->IsPhysicallyLinkedEast();

	MapManager::Instance()->RemoveQueuedMap();

	return true;
}