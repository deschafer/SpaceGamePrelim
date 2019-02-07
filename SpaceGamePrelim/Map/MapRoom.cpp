#include "MapRoom.h"
#include "..\Map\GenRoomComp.h"

#include <vector>
#include <map>
#include <iostream>

#include <time.h>

using namespace std;

MapRoom::MapRoom()
{
}

MapRoom::~MapRoom()
{
}

MapRoom::MapRoom(std::string RoomType, int Width, int Height)
{
	// Getting the properties assoc with this room type
	m_Properties = RoomManager::Instance()->GetTypeDefinition(RoomType);
	if (m_Properties == nullptr) // Then some error occurred
	{
		cout << "Error -- RoomProperties received from GetTypeDef in MapRoom ctor was nullptr\n";
		abort();
	}

	m_Width = Width;
	m_Height = Height;
	m_RoomType = RoomType;

	// Need to check with the minimum sizes of the maproom here

	// Generating the array for this room
	m_Cells = new MapObject**[m_Width];
	for (int i = 0; i < m_Width; i++)
	{
		m_Cells[i] = new MapObject*[m_Height];
	}

	// Initializing all of the cells to nullptr
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_Cells[i][j] = nullptr;
		}
	} 

}

MapRoom::MapRoom(int Width, int Height)
{
	// Getting the properties assoc with this room type
	m_Properties = RoomManager::Instance()->GetRandomTypeDefinition(&m_RoomType);
	if (m_Properties == nullptr) // Then some error occurred
	{
		cout << "Error -- RoomProperties received from GetTypeDef in MapRoom ctor was nullptr\n";
		abort();
	}

#ifdef _DEBUG
	cout << "Map " << m_RoomType << " Loaded\n";
#endif // _DEBUG


	m_Width = Width;
	m_Height = Height;

	// Need to check with the minimum sizes of the maproom here

	// Generating the array for this room
	m_Cells = new MapObject**[m_Width];
	for (int i = 0; i < m_Width; i++)
	{
		m_Cells[i] = new MapObject*[m_Height];
	}

	// Initializing all of the cells to nullptr
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_Cells[i][j] = nullptr;
		}
	}

}


//
// GetCell()
// Gets the cell from m_Cells specefied by position
// X,Y. If not within the bounds of m_Cells,
// it will return nullptr, so check the return.
//
MapObject* MapRoom::GetCell(int X, int Y)
{

	if (X < m_Width && Y < m_Height)
	{
		return m_Cells[X][Y];
	}
	else
	{
		return nullptr;
	}

}


//
// Generate()
// Room generation function, generates the room based on the width and height 
// parameters based off of the room def. of RoomType stored in RoomManager.
// Stores the cells of this room in its own array.
//
void MapRoom::Generate()
{

	clock_t time1 = (double)clock();            /* get initial time */
	time1 = time1 / CLOCKS_PER_SEC;

	RoomProperties* Properties = m_Properties;

	if (Properties == nullptr)
	{
#ifndef _DEBUG
		std::cout << "No room def found\n";
#endif // !_DEBUG
		abort();
	}

	float ParamInnerSizeY = .25;
	float ParamInnerSizeX = .25;

	vector<string> tempStr;

	tempStr.push_back("Wall");

	int StartX = 0;
	int StartY = 0;
	int EffectHeight = m_Height - 1;
	int EffectWidth = m_Width - 1;
	//int EffectHeight = 10 - 1 - StartY;
	//int EffectWidth = 10 - 1 - StartX;
	int MaxHeightX = StartX;
	int MaxHeightY = StartY + m_Height;
	int MaxWidthX = StartX + m_Width;
	int MaxWidthY = StartY;
	int DrawCount = 0;
	int Count = 0;
	int CountRecord = 0;
	int TempX = StartX;
	int TempY = StartY;
	int CurrentLengthQuota = 0;
	int HorizontalDeficit = 0;
	int VerticalDeficit = 0;
	int BegOffsetX = 0;
	int BegOffsetY = 0;

	bool Test = 0;
	bool SizingComplete = false;
	bool LastSide = false;
	bool StaticSideFlag = false;
	bool complete = false;

	Event EventType = Event::NONE;
	Direction CurrentDirection = Direction::EAST;
	Direction CurrentDirectionTemp;
	Side CurrentSide = Side::TOP;

	map<pair<int, int>, bool> CurrentLocations;

	vector<int> StaticSides;// Used for static side definitions
	vector<int> Sides;		// size = number of sides, each side has a length.
	vector<bool> SideDef;	// stores the Greater Side Definition from the room def
	vector<char> Turns;		// Stores the turns from the room def

	vector<int> TempSidesEast;	// Temporary vectors, used in the first stage
	vector<int> TempSidesSouth;
	vector<int> TempSidesWest;
	vector<int> TempSidesNorth;

	vector<int> TempStaticEast;	// Temporary vectors, used in the first stage for static sides
	vector<int> TempStaticSouth;
	vector<int> TempStaticWest;
	vector<int> TempStaticNorth;

	pair<int, int> CurrentPair;	// used for adding coordinates

	// Set Static Side flag
	if (Properties->m_StaticSidesFlag)
	{
		StaticSideFlag = true;
	}
	// Copying GreaterSide and Turn vectors (same size by def)
	for (size_t i = 0; i < Properties->m_GreaterSideDefinition.size(); i++)
	{
		SideDef.push_back(Properties->m_GreaterSideDefinition[i]);
		Turns.push_back(Properties->m_Turns[i]);
	}
	// Copying static sides
	for (size_t i = 0; i < Properties->m_StaticSides.size(); i++)
	{
		StaticSides.push_back(Properties->m_StaticSides[i]);
	}

	// Determining the side lengths for each of the 
	// sides in this defined room.
	while (!SizingComplete)
	{
		CurrentDirectionTemp = CorrespondingDirection(CurrentSide);
		CountRecord = Count;

		// Adding empty sides to the record
		do
		{
			Test = SideDef[Count];
			if (CurrentDirectionTemp == Direction::EAST && CurrentSide != Side::BOTTOM)
			{
				TempSidesEast.push_back(0 - (TempSidesSouth.size() + TempSidesNorth.size())); // placeholder for a side to be set later
			}
			else if (CurrentDirectionTemp == Direction::SOUTH && CurrentSide != Side::LEFT)
			{
				TempSidesSouth.push_back(0 - (TempSidesWest.size() + TempSidesEast.size())); // placeholder for a side to be set later
			}
			else if (CurrentDirectionTemp == Direction::WEST && CurrentSide != Side::TOP)
			{
				TempSidesWest.push_back(0 - (TempSidesSouth.size() + TempSidesNorth.size())); // placeholder for a side to be set later
			}
			else if (CurrentDirectionTemp == Direction::NORTH && CurrentSide != Side::RIGHT)
			{
				TempSidesNorth.push_back(0 - (TempSidesWest.size() + TempSidesEast.size())); // placeholder for a side to be set later
			}

			// Get the new direction
			CurrentDirectionTemp = Turn(CurrentDirectionTemp, Turns[Count]);

			// Increment a count
			Count++;
		} while (Test != true);


		// Deal with static sides here
		// A mag of static sides
		// and count for each direction
		if (StaticSideFlag)
		{
			Direction TempDirection = CorrespondingDirection(CurrentSide);
			int TempCount = CountRecord;
			int Index = TempCount;
			int StaticWestMag = 0;
			int StaticEastMag = 0;
			int StaticNorthMag = 0;
			int StaticSouthMag = 0;
			do
			{
				Test = SideDef[TempCount];
				// Setting the east temp queue for the static sides
				if (TempDirection == Direction::EAST && CurrentSide != Side::BOTTOM)
				{

					TempStaticEast.push_back(StaticSides[Index]);
					StaticEastMag += StaticSides[Index];

				}
				// Setting the south temp queue for the static sides
				else if (TempDirection == Direction::SOUTH && CurrentSide != Side::LEFT)
				{
					if (StaticSides[Index] != 0)
					{
						TempStaticSouth.push_back(StaticSides[Index]);
						StaticSouthMag += StaticSides[Index];

					}
				}
				// Setting the west temp queue for the static sides
				else if (TempDirection == Direction::WEST && CurrentSide != Side::TOP)
				{
					if (StaticSides[Index] != 0)
					{
						TempStaticWest.push_back(StaticSides[Index]);
						StaticWestMag += StaticSides[Index];

					}
				}
				// Setting the north temp queue for the static sides
				else if (TempDirection == Direction::NORTH && CurrentSide != Side::RIGHT)
				{
					if (StaticSides[Index] != 0)
					{
						TempStaticNorth.push_back(StaticSides[Index]);
						StaticNorthMag += StaticSides[Index];
					}
				}

				// Get the new direction
				TempDirection = Turn(TempDirection, Turns[TempCount]);

				// Increment a count
				TempCount++;
				Index++;
			} while (Test != true);
		}

		

		// Then we deal with these sides
		bool TempComplete = false;
		int X_Size = 0;
		int Y_Size = 0;

		// Determining an actual size for these sides
		while (!TempComplete)
		{

			X_Size = TempSidesEast.size();

			// first deal with x positions
			for (int i = 0; i < X_Size; i++)
			{
				if (SideVertical(CurrentSide))
				{
					// calculating integer division
					TempSidesEast[i] = static_cast<int>((EffectWidth * ParamInnerSizeX) / (X_Size)); // all set to equal size for now
				}
				else
				{

					TempSidesEast[i] = (EffectWidth) / X_Size - HorizontalDeficit; // all set to equal size for now
				}
			}

			// Resetting sizes
			if (SideHorizontal(CurrentSide))
			{
				int EastMag = 0;
				// Getting a magnitude for the total length of the sides generated.
				// Due to integer division and truncation, the sides may have to be resized so 
				// that they are guaranteed to fit the defining rectangle.
				for (size_t i = 0; i < TempSidesEast.size(); i++)
				{
					// If a static side is present, it has priority
					if (i < TempStaticEast.size() && TempStaticEast[i] != 0)
					{
						EastMag += TempStaticEast[i];
					}
					else
					{
						EastMag += TempSidesEast[i];
					}
				}
				// Adding a horizontal defecit if it exists
				EastMag += HorizontalDeficit;
				// If the magnitude is less than the total width
				// NOTE: This is most common 
				if (EastMag < EffectWidth)
				{
					int difference = EffectWidth - EastMag;
					int size = TempSidesEast.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticEast.size() && TempStaticEast[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesEast[i]++;
							difference--;
						}
						// If this round completed, yet a difference still exists, reset
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
				// If the magnitude is larger
				// NOTE: This is most likely caused by a static side definition
				else if (EastMag > EffectWidth)
				{
					int difference = EastMag - EffectWidth;
					int size = TempSidesEast.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticEast.size() && TempStaticEast[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesEast[i]--;
							difference--;
						}
						// If this round completed, yet a difference still exists, reset
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
			}

			X_Size = TempSidesWest.size();

			// first deal with x positions
			for (int i = 0; i < X_Size; i++)
			{
				if (SideVertical(CurrentSide))
				{
					TempSidesWest[i] = static_cast<int>((EffectWidth * ParamInnerSizeX) / (X_Size)); // all set to equal size for now
				}
				else
				{
					TempSidesWest[i] = EffectWidth / X_Size - HorizontalDeficit; // all set to equal size for now
				}
			}

			// Resetting sizes
			if (SideHorizontal(CurrentSide))
			{
				int WestMag = 0;
				for (size_t i = 0; i < TempSidesWest.size(); i++)
				{
					// If a static side is present, it has priority
					if (i < TempStaticWest.size() && TempStaticWest[i] != 0)
					{
						WestMag += TempStaticWest[i];
					}
					else
					{
						WestMag += TempSidesWest[i];
					}
				}
				WestMag += HorizontalDeficit;
				if (WestMag < EffectWidth)
				{
					int difference = EffectWidth - WestMag;
					int size = TempSidesWest.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticWest.size() && TempStaticWest[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesWest[i]++;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
				else if (WestMag > EffectWidth)
				{
					int difference = WestMag - EffectWidth;
					int size = TempSidesWest.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticWest.size() && TempStaticWest[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesWest[i]--;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
			}


			Y_Size = TempSidesSouth.size();

			for (int i = 0; i < Y_Size; i++)
			{
				if (SideHorizontal(CurrentSide))
				{
					TempSidesSouth[i] = static_cast<int>((EffectHeight * ParamInnerSizeY) / (Y_Size)); // all set to equal size for now
				}
				else
				{
					TempSidesSouth[i] = EffectHeight / Y_Size - VerticalDeficit; // all set to equal size for now
				}
			}


			// Reset sizes
			// first get a magnitude
			if (SideVertical(CurrentSide))
			{
				int SouthMag = 0;
				for (size_t i = 0; i < TempSidesSouth.size(); i++)
				{
					// If a static side is present, it has priority
					if (i < TempStaticSouth.size() && TempStaticSouth[i] != 0)
					{
						SouthMag += TempStaticSouth[i];
					}
					else
					{
						SouthMag += TempSidesSouth[i];
					}
				}
				SouthMag += VerticalDeficit;
				if (SouthMag < EffectHeight)
				{
					int difference = EffectHeight - SouthMag;
					int size = TempSidesSouth.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticSouth.size() && TempStaticSouth[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesSouth[i]++;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = -1;
					}
				}
				else if (SouthMag > EffectHeight)
				{
					int difference = SouthMag - EffectHeight;
					int size = TempSidesSouth.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticSouth.size() && TempStaticSouth[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesSouth[i]--;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
			}



			Y_Size = TempSidesNorth.size();

			for (int i = 0; i < Y_Size; i++)
			{
				if (SideHorizontal(CurrentSide))
				{
					TempSidesNorth[i] = static_cast<int>((EffectHeight * ParamInnerSizeY) / (Y_Size)); // all set to equal size for now
				}
				else
				{
					TempSidesNorth[i] = EffectHeight / Y_Size - VerticalDeficit; // all set to equal size for now
				}
			}

			// Resetting sizes
			if (SideVertical(CurrentSide))
			{
				int NorthMag = 0;
				for (size_t i = 0; i < TempSidesNorth.size(); i++)
				{
					// If a static side is present, it has priority
					if (i < TempStaticNorth.size() && TempStaticNorth[i] != 0)
					{
						NorthMag += TempStaticNorth[i];
					}
					else
					{
						NorthMag += TempSidesNorth[i];
					}
				}
				NorthMag += VerticalDeficit;
				if (NorthMag < EffectHeight)
				{
					int difference = EffectHeight - NorthMag;
					int size = TempSidesNorth.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticNorth.size() && TempStaticNorth[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesNorth[i]++;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
				else if (NorthMag > EffectHeight)
				{
					int difference = NorthMag - EffectHeight;
					int size = TempSidesNorth.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						// Static sides DO NOT get modified
						if (i < TempStaticNorth.size() && TempStaticNorth[i] != 0);
						// Dynamic sides DO get modified
						else
						{
							TempSidesNorth[i]++;
							difference--;
						}
						if (difference != 0 && i == size - 1)
							i = 0;
					}
				}
			}



			TempComplete = true;
		}

		// Resetting defecits from last cycle
		VerticalDeficit = 0;
		HorizontalDeficit = 0;
		// Calculating side-specefic defecits

		// Horizontal defecit, used in the verticle sides ONLY
		if ((TempSidesEast.size() > TempSidesWest.size() ||
			TempSidesEast.size() < TempSidesWest.size()) &&
			(SideVertical(CurrentSide) && !StaticSideFlag) ||
			(SideVertical(CurrentSide) && StaticSideFlag))
		{
			int EastMagnitude = 0;
			int WestMagnitude = 0;

			for (size_t i = 0; i < TempSidesWest.size(); i++)
			{
				if (StaticSideFlag && (i < TempStaticWest.size()) && TempStaticWest[i] != 0)
				{
					WestMagnitude += TempStaticWest[i];
				}
				else
				{
					WestMagnitude += TempSidesWest[i];
				}
			}

			for (size_t i = 0; i < TempSidesEast.size(); i++)
			{
				// If a static side is present, use that size over the dynamic side
				if (StaticSideFlag && (i < TempStaticEast.size()) && TempStaticEast[i] != 0)
				{
					EastMagnitude += TempStaticEast[i];
				}
				else
				{
					EastMagnitude += TempSidesEast[i];
				}
			}
			HorizontalDeficit = abs(static_cast<int>(EastMagnitude - WestMagnitude));
		}

		// If the an overall movement in the y axis was produced in 
		// either the top or bottom sides, the next side will need to know 
		// about this.
		if ((TempSidesNorth.size() > TempSidesSouth.size() ||
			TempSidesNorth.size() < TempSidesSouth.size()) &&
			(SideHorizontal(CurrentSide) && !StaticSideFlag) ||
			(SideHorizontal(CurrentSide) && StaticSideFlag))
		{

			int NorthMagnitude = 0;
			int SouthMagnitude = 0;

			for (size_t i = 0; i < TempSidesNorth.size(); i++)
			{
				
				if (StaticSideFlag && (i < TempStaticNorth.size()) && TempStaticNorth[i] != 0)
				{
					NorthMagnitude += TempStaticNorth[i];
				}
				else
				{
					NorthMagnitude += TempSidesNorth[i];
				}
			}

			for (size_t i = 0; i < TempSidesSouth.size(); i++)
			{
				// If a static side is present, use that size over the dynamic side
				if (StaticSideFlag && (i < TempStaticSouth.size()) && TempStaticSouth[i] != 0)
				{
					SouthMagnitude += TempStaticSouth[i];
				}
				else
				{
					SouthMagnitude += TempSidesSouth[i];
				}
			}

			VerticalDeficit = abs(static_cast<int>(NorthMagnitude - SouthMagnitude));
		}

		int CountWest = 0;
		int CountEast = 0;
		int CountNorth = 0;
		int CountSouth = 0;

		CurrentDirectionTemp = CorrespondingDirection(CurrentSide);

		// Adding the sides to the vector to be draw
		Count = CountRecord;
		do
		{
			Test = SideDef[Count];
			if (CurrentDirectionTemp == Direction::EAST && CurrentSide != Side::BOTTOM)
			{
				Sides.push_back(TempSidesEast[CountEast]);
				CountEast++;
			}
			else if (CurrentDirectionTemp == Direction::SOUTH && CurrentSide != Side::LEFT)
			{
				Sides.push_back(TempSidesSouth[CountSouth]); // placeholder for a side to be set later
				CountSouth++;
			}
			else if (CurrentDirectionTemp == Direction::WEST && CurrentSide != Side::TOP)
			{
				Sides.push_back(TempSidesWest[CountWest]); // placeholder for a side to be set later
				CountWest++;
			}
			else if (CurrentDirectionTemp == Direction::NORTH && CurrentSide != Side::RIGHT)
			{
				Sides.push_back(TempSidesNorth[CountNorth]); // placeholder for a side to be set later
				CountNorth++;
			}

			CurrentDirectionTemp = Turn(CurrentDirectionTemp, Turns[Count]);

			Count++;

		} while (Test != true);


		if ((CurrentSide = NextSide(CurrentSide)) == Side::COMPL)
		{
			SizingComplete = true;
		}

		// Clear all temporary storage for cells
		TempSidesEast.clear();
		TempSidesSouth.clear();
		TempSidesWest.clear();
		TempSidesNorth.clear();
		TempStaticEast.clear();
		TempStaticSouth.clear();
		TempStaticWest.clear();
		TempStaticNorth.clear();

	}

	// ------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------
	// Now the next step, drawing in the set sides. This goes through the side vector, and draws them 
	// with their desired length and in the direction indicated by the turns vector.
	while (!complete)
	{
		// keeping a count
		DrawCount++;

		// Getting the appropriate side for this definition
		if (StaticSideFlag && !Sides.empty() && CurrentLengthQuota <= 0)
		{
			// Checking that the definition is correct first
			if (StaticSides.empty() && Sides.size() > 1)
			{
#ifndef _DEBUG
				cout << "Room defined incorrectly -- static sides count incorrect\n";
#endif // !_DEBUG
				abort(); // Cannot proceed
			}
			
			if ((CurrentLengthQuota = StaticSides.front()) != 0)
			{
			}
			else
			{
				CurrentLengthQuota = Sides.front();
			}
			// Still have to remove from other queue
			StaticSides.erase(StaticSides.begin());
			Sides.erase(Sides.begin());
			// If last, set approp.
			if (Sides.empty()) LastSide = true;
		}
		else if (!Sides.empty() && CurrentLengthQuota <= 0)
		{
			CurrentLengthQuota = Sides.front();
			Sides.erase(Sides.begin());
			if (Sides.empty()) LastSide = true;
		}
		// Complete case
		else if (Sides.empty() && CurrentLengthQuota <= 0)
		{
			complete = true;
			break;
		}

		// If the current point is the starting point
		if ((CurrentPair = make_pair(TempX, TempY)) == make_pair(StartX, StartY) && Sides.empty())
		{
			complete = true;
			break;
		}

		// Determings the movement based on direction
		if (CurrentDirection == Direction::EAST)
		{
			TempX++;
		}
		else if (CurrentDirection == Direction::SOUTH)
		{
			TempY++;
		}
		else if (CurrentDirection == Direction::WEST)
		{
			TempX--;
		}
		else if (CurrentDirection == Direction::NORTH)
		{
			TempY--;
		}

		// Adding the current coordinates to the record
		CurrentPair.swap(make_pair(TempX, TempY));
		CurrentLocations[CurrentPair] = true;

		// Adding to the array
		if (TempX >= 0 && TempY >= 0 && TempX < m_Width && TempY < m_Height)
		{
			//m_Cells[TempX][TempY] = new MapCell(new TextureProperties(Rect(0, 0, 32, 32), "Room", 1, 0, 0, 1), MapCoordinate(TempX * 32, TempY * 32));

			m_Cells[TempX][TempY] = new MapCell(tempStr, MapCoordinate(TempX * 32, TempY * 32));

		}

		// Decrementing the length
		CurrentLengthQuota--;

		// Complete case
		if (CurrentLengthQuota == 0 && !Turns.empty())
		{
			CurrentDirection = Turn(CurrentDirection, Turns.front());
			Turns.erase(Turns.begin());
		}
	}

	double timedif = (((double)clock()) / CLOCKS_PER_SEC) - time1;
	cout << "time taken" << timedif << endl;


}
