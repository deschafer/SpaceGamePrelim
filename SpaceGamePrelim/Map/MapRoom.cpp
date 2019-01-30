#include "MapRoom.h"
#include "..\Map\GenRoomComp.h"

#include <vector>
#include <map>
#include <iostream>

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

	RoomProperties* Properties = RoomManager::Instance()->GetTypeDefinition(m_RoomType);

	if (Properties == nullptr)
	{
		std::cout << "No room def found\n";
		abort();
	}

	float ParamInnerSizeY = .25;
	float ParamInnerSizeX = .25;

	// only for this test function
	static bool complete = false;
	if (complete) return;

	int StartX = 0;
	int StartY = 0;
	int EffectHeight = m_Height;
	int EffectWidth = m_Width;
	int MaxHeightX = StartX;
	int MaxHeightY = StartY + m_Height;
	int MaxWidthX = StartX + m_Width;
	int MaxWidthY = StartY;
	int rng = 0; // 0 = south, 1 = west, 2 = north, 3 = east
	int DrawCount = 0;
	int Count = 0;
	int CountRecord = 0;
	int TempX = StartX;
	int TempY = StartY;
	int CurrentLengthQuota = 0;
	int HorizontalDeficit = 0;
	int VerticalDeficit = 0;
	int CellCounter = 0;

	bool Test = 0;
	bool SizingComplete = false;
	bool Switch = false;
	bool SwitchStart = false;
	bool EventPossible = false;
	bool LastSide = false;

	Event EventType = Event::NONE;
	Direction CurrentDirection = Direction::EAST;
	Direction CurrentDirectionTemp;
	Side CurrentSide = Side::TOP;

	map<pair<int, int>, bool> CurrentLocations;

	vector<int> Sides; // size = number of sides, each side has a length.
	vector<bool> SideDef;	// stores the Greater Side Definition from the room def
	vector<char> Turns;		// Stores the turns from the room def

	vector<int> TempSidesEast;	// Temporary vectors, used in the first stage
	vector<int> TempSidesSouth;
	vector<int> TempSidesWest;
	vector<int> TempSidesNorth;

	pair<int, int> CurrentPair;	// used for adding coordinates

								// Copying vectors
	for (size_t i = 0; i < Properties->m_GreaterSideDefinition.size(); i++)
	{
		SideDef.push_back(Properties->m_GreaterSideDefinition[i]);
		Turns.push_back(Properties->m_Turns[i]);
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

		// Then we deal with these sides

		bool TempComplete = false;
		int X_Size = 0;
		int Y_Size = 0;

		// Determining an actual size for these sides
		while (!TempComplete)
		{
			//if (CurrentSide != Side::TOP)
			//{
			EffectHeight = m_Height - 1;
			EffectWidth = m_Width - 1;
			//}

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
				for (size_t i = 0; i < TempSidesEast.size(); i++)
				{
					EastMag += TempSidesEast[i];
				}
				EastMag += VerticalDeficit;
				if (EastMag < EffectWidth)
				{
					int difference = EffectWidth - EastMag;
					int size = TempSidesEast.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						TempSidesEast[i]++;
						difference--;
						if (difference != 0 && i == size)
							i = 0;
					}
				}
				else if (EastMag > EffectWidth)
				{
					int difference = EastMag - EffectWidth;
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
					WestMag += TempSidesWest[i];
				}
				WestMag += VerticalDeficit;
				if (WestMag < EffectWidth)
				{
					int difference = EffectWidth - WestMag;
					int size = TempSidesWest.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						TempSidesWest[i]++;
						difference--;
						if (difference != 0 && i == size)
							i = 0;
					}
				}
				else if (WestMag > EffectWidth)
				{
					int difference = WestMag - EffectWidth;
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
					SouthMag += TempSidesSouth[i];
				}
				SouthMag += VerticalDeficit;
				if (SouthMag < EffectHeight)
				{
					int difference = EffectHeight - SouthMag;
					int size = TempSidesSouth.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						TempSidesSouth[i]++;
						difference--;
						if (difference != 0 && i == size)
							i = 0;
					}
				}
				else if (SouthMag > EffectHeight)
				{
					int difference = SouthMag - EffectHeight;
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
					NorthMag += TempSidesNorth[i];
				}
				NorthMag += VerticalDeficit;
				if (NorthMag < EffectHeight)
				{
					int difference = EffectHeight - NorthMag;
					int size = TempSidesNorth.size();
					for (int i = 0; i < size && difference > 0; i++)
					{
						TempSidesNorth[i]++;
						difference--;
						if (difference != 0 && i == size)
							i = 0;
					}
				}
				else if (NorthMag > EffectHeight)
				{
					int difference = NorthMag - EffectHeight;
				}
			}



			TempComplete = true;
		}




		VerticalDeficit = 0;
		HorizontalDeficit = 0;
		// Calculating side-specefic defecits

		// Horizontal defecit, used in the verticle sides ONLY
		if ((TempSidesEast.size() > TempSidesWest.size() ||
			TempSidesEast.size() < TempSidesWest.size()) &&
			SideVertical(CurrentSide))
		{
			int EastMagnitude = 0;
			int WestMagnitude = 0;

			for (size_t i = 0; i < TempSidesWest.size(); i++)
			{
				WestMagnitude += TempSidesWest[i];
			}

			for (size_t i = 0; i < TempSidesEast.size(); i++)
			{
				EastMagnitude += TempSidesEast[i];
			}
			HorizontalDeficit = abs(static_cast<int>(EastMagnitude - WestMagnitude)) + 1;
		}

		if ((TempSidesNorth.size() > TempSidesSouth.size() ||
			TempSidesNorth.size() < TempSidesSouth.size()) &&
			SideHorizontal(CurrentSide))
		{

			int NorthMagnitude = 0;
			int SouthMagnitude = 0;

			for (size_t i = 0; i < TempSidesNorth.size(); i++)
			{
				NorthMagnitude += TempSidesNorth[i];
			}

			for (size_t i = 0; i < TempSidesSouth.size(); i++)
			{
				SouthMagnitude += TempSidesSouth[i];
			}

			//VerticalDeficit = 0;
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
	}

	// ------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------
	// Now the next step, drawing in the set sides. This goes through the side vector, and draws them 
	// with their desired length and in the direction indicated by the turns vector.
	while (!complete)
	{
		// keeping a count
		DrawCount++;

		// Popping the front element
		if (!Sides.empty() && CurrentLengthQuota <= 0)
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
		m_Cells[TempX][TempY] = new MapCell(new TextureProperties(Rect(0, 0, 32, 32), "Room", 1, 0, 0, 1), MapCoordinate(TempX * 32, TempY * 32));

		// Decrementing the length
		CurrentLengthQuota--;

		// Complete case
		if (CurrentLengthQuota == 0 && !Turns.empty())
		{
			CurrentDirection = Turn(CurrentDirection, Turns.front());
			Turns.erase(Turns.begin());
		}
	}

}
