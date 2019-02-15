#include "MapRoom.h"
#include "GenRoomComp.h"
#include "MapWall.h"
#include "MapManager.h"
#include "MapInactive.h"

#include <vector>
#include <map>
#include <iostream>
#include <chrono>


using namespace std;
using namespace std::chrono;

// Helper functions for the MapRoom::Generate()
vector<string> FindCorrectTile(Side CurrentSide, Direction CurrentDirection, bool LastBlock, Direction NextDirection);
void SetFloorTiles(MapObject*** &Cells, int StartX, int StartY, int XMax, int YMax);
void MarkFloorTile(MapObject*** &Cells, int X, int Y, int XMax, int YMax);
void Reflect(MapObject*** &Cells, int Width, int Height);

const string WallCornerRight = "Wall_Corner_Right";
const string WallCornerLeft = "Wall_Corner_Left";
const string WallSideRight = "Wall_Side_Right";
const string WallSideLeft = "Wall_Side_Left";
const string WallBottom = "Wall_Bottom";
const string Default = "Wall";
const string WallTopGroup = "Wall_Top";
const string FloorGroup = "Floors";


MapRoom::MapRoom()
{}
MapRoom::~MapRoom()
{
	//TODO: Implement proper destruction here
}

//
// MapRoom()
// CTOR that should be used to create a MapRoom with a known type
//
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

	// TODO: Need to check with the minimum sizes of the maproom here

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
// MapRoom()
// CTOR that should be used to create a MapRoom with an unknown type
// NOTE: May not be staying in final version due to min room size complications
//
MapRoom::MapRoom(int Width, int Height)
{
	// Getting the properties assoc with this room type
	m_Properties = RoomManager::Instance()->GetRandomTypeDefinition(m_RoomType);
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
// The algorithm starts at the top, works to the east, folowing the definition, and.
// goes cell by cell, constructing the room. It goes east(Top)->south(Right)->west(Bottom)->north(Left). 
// Each of these greater sides have smaller, specefic sides that are defined by the room definition.
//
void MapRoom::Generate()
{
	
	// Preformance testing only
	static int enter = 0;
	// This label is just for preformance testing only, and will not be in the final version
top:
	// Preformance testing only
	enter++;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	RoomProperties* Properties = m_Properties;

	// Check if the definition was found prior to generating
	if (Properties == nullptr)
	{
#ifndef _DEBUG
		std::cout << "No room def found\n";
#endif // !_DEBUG
		abort();
	}

	// These are test values, will be a part of the room definition
	float ParamInnerSizeY = .25;
	float ParamInnerSizeX = .25;

	float Variance;

	// Numerics
	int CellWidth = MapManager::Instance()->GetCellWidth();
	int CellHeight = MapManager::Instance()->GetCellHeight();
	int StartX = 0;							// Starting Offset
	int StartY = 0;							// Starting Offset
	int EffectHeight = m_Height - 1;		// Actual height used in the alg
	int EffectWidth = m_Width - 1;			// Actual width used in the alg
	int MaxHeightX = StartX;				// Max values
	int MaxHeightY = StartY + m_Height;		// ..
	int MaxWidthX = StartX + m_Width;		// ..
	int MaxWidthY = StartY;					// ..
	int DrawCount = 0;						// A count of drawn/created cells in the second stage
	int Count = 0;							// A genertic count to keep track of pos. in SideDef
	int CountRecord = 0;					// A record of the count at the top of the cycle
	int TempX = StartX;						// Used to contain current XPos of drawn cell
	int TempY = StartY;						// Used to contain current YPos of drawn cell
	int CurrentLengthQuota = 0;				// # of cells needed to be drawn
	int HorizontalDeficit = 0;				// Offset left over from last side, it decreases the effective dimensions
	int VerticalDeficit = 0;				// Offset left over from last side, it decreases the effective dimensions

	const int MinRandSize = 2;

	// Flags
	bool Test = 0;				
	bool SizingComplete = false;
	bool LastSide = false;					
	bool StaticSideFlag = false;			// Indicates usage of static sides
	bool DynamicRandomFlag = false;
	bool StaticRandomHorizontal = false;
	bool StaticRandomVertical = false;
	bool complete = false;					// generic, used througout

	// Generating-stage helper variables
	Direction CurrentDirection = Direction::EAST;	// Current direction used in stage 1 and stage 2
	Direction CurrentDirectionTemp;					// Used to store the correspong direction for a side, stage 1
	Side CurrentSide = Side::TOP;					// Current side, set at the start, the top

	// Stores a map of all locations based on their coordinates
	map<pair<int, int>, bool> CurrentLocations;

	vector<string> tempStr;	// Vector used to tempoarily store string sofr texture layers of cells

	// Room Def. Vectors
	vector<int> StaticSides;// Used for static side definitions
	vector<int> Sides;		// size = number of sides, each side has a length.
	vector<bool> SideDef;	// stores the Greater Side Definition from the room def
	vector<char> Turns;		// Stores the turns from the room def

	// Room gen. Vectors
	vector<int> TempSidesEast;	// Temporary vectors, used in the first stage
	vector<int> TempSidesSouth;
	vector<int> TempSidesWest;
	vector<int> TempSidesNorth;

	// Static Side Vectors
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
	else if (Properties->m_DynamicRandomFlag)
	{
		DynamicRandomFlag = true;
		// Copying static sides
		for (size_t i = 0; i < Properties->m_GreaterSideDefinition.size(); i++)
		{
			StaticSides.push_back(0);
		}
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

	Variance = Properties->m_Variation;

	// Determining the side lengths for each of the 
	// sides in this defined room.
	// Start STAGE 1
	while (!SizingComplete)
	{
		CurrentDirectionTemp = CorrespondingDirection(CurrentSide);
		CountRecord = Count;

		// Adding empty sides to the record, these serve as a place holder that
		// will be dealt with later.
		do
		{
			// Testing if this side is significant
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
		// Start of dynamic generation for sides
		else if (DynamicRandomFlag)
		{

			Direction TempDirection = CorrespondingDirection(CurrentSide);	
			int TempCount = CountRecord;	// Beginning count for looking at the temp vectors
			int Index = TempCount;			// ..

			int Space = ((EffectWidth * Variance) == 0) ? 1 : EffectWidth * Variance;

			int HorizontalMagnitude = (TempSidesEast.size() + TempSidesWest.size()) - 1;
			int VerticalMagnitude = (TempSidesNorth.size() + TempSidesSouth.size()) - 1;

			// If the current side is a horizontal side, then the following will add variety to its up/down 
			// side lengths
			if ((HorizontalMagnitude > 0) &&
				(SideVertical(CurrentSide)))
			{
				int InwardMovement = 0; // Keep track of movement so we stay within the bounds

				do
				{
					Test = SideDef[TempCount];	// Get the next GSideDef

					// Setting the east temp queue for the static sides
					if (TempDirection == Direction::EAST && CurrentSide != Side::BOTTOM)
					{
						// Calc a random integer
						int random = (rand() % ((Space == 0) ? 1 : Space) / 2) + 1;
						
						// If this direction moves inwards,
						// make note of this
						if (CurrentSide == Side::LEFT)
						{
							StaticSides[Index] += random;
							TempStaticEast.push_back(StaticSides[Index]);
							InwardMovement += random;
							Space -= random;
						}
						else
						{
							// Otherwise generate based on movement
							random = (rand() % InwardMovement) + 1;

							StaticSides[Index] += random;
							TempStaticEast.push_back(StaticSides[Index]);
							InwardMovement -= random;
							Space += random;
						}
					}
					// Setting the west temp queue for the static sides
					else if (TempDirection == Direction::WEST && CurrentSide != Side::TOP)
					{

						int random = (rand() % ((Space == 0) ? 1 : Space) / 2) + 1;

						// If this direction moves inwards,
						// make note of this
						if (CurrentSide == Side::RIGHT)
						{
							StaticSides[Index] += random;
							TempStaticWest.push_back(StaticSides[Index]);
							InwardMovement += random;
							Space -= random;

						}
						else
						{
							random = (rand() % InwardMovement) + 1;
							StaticSides[Index] += random;
							TempStaticWest.push_back(StaticSides[Index]);
							InwardMovement -= random;
							Space += random;

						}
					}
					// Get the new direction
					TempDirection = Turn(TempDirection, Turns[TempCount]);
					// Increment a count
					TempCount++;
					Index++;

				} while (Test != true);
				// We have made static sides, so set it true
				StaticSideFlag = true;
			}
			// If the current side is a verticle side, then the following will add variety to its left/right 
			// side lengths
			else if ((VerticalMagnitude > 0) &&
				(SideHorizontal(CurrentSide)))
			{
				int InwardMovement = 0;	// Keeps track of movement inwards to keep within bounds

				do
				{
					Test = SideDef[TempCount];

					// Setting the east temp queue for the static sides
					if (TempDirection == Direction::NORTH && CurrentSide != Side::RIGHT)
					{
						int random = (rand() % ((Space == 0) ? 1 : Space) / 2) + 1;

						// If this movement is inwards
						if (CurrentSide == Side::BOTTOM)
						{
							StaticSides[Index] += random;
							TempStaticNorth.push_back(StaticSides[Index]);
							InwardMovement += random;
							Space -= random;
						}
						else
						{
							random = (rand() % InwardMovement) + 1;

							StaticSides[Index] += random;
							TempStaticNorth.push_back(StaticSides[Index]);
							InwardMovement -= random;
							Space += random;
						}
					}
					// Setting the west temp queue for the static sides
					else if (TempDirection == Direction::SOUTH && CurrentSide != Side::LEFT)
					{

						int random = (rand() % ((Space == 0) ? 1: Space) / 2) + 1;

						// If this movement is inwards
						if (CurrentSide == Side::TOP)
						{
							StaticSides[Index] += random;
							TempStaticSouth.push_back(StaticSides[Index]);
							InwardMovement += random;
							Space -= random;

						}
						else
						{
							random = (rand() % InwardMovement) + 1;
							StaticSides[Index] += random;
							TempStaticSouth.push_back(StaticSides[Index]);
							InwardMovement -= random;
							Space += random;
						}
					}
					// Get the new direction
					TempDirection = Turn(TempDirection, Turns[TempCount]);
					// Increment a count
					TempCount++;
					Index++;
				} while (Test != true);

				StaticSideFlag = true;
			}
		}
		
		// Then we deal with the empty, placeholder sides
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
					size_t size = TempSidesEast.size();
					for (size_t i = 0; i < size && difference > 0; i++)
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
					size_t size = TempSidesEast.size();
					for (size_t i = 0; i < size && difference > 0; i++)
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
					size_t size = TempSidesWest.size();
					for (size_t i = 0; i < size && difference > 0; i++)
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
					size_t size = TempSidesWest.size();
					for (size_t i = 0; i < size && difference > 0; i++)
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
					size_t size = TempSidesSouth.size();
					for (size_t i = 0; i < size && difference > 0; i++)
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
					size_t size = TempSidesSouth.size();
					for (size_t i = 0; i < size && difference > 0; i++)
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
					size_t size = TempSidesNorth.size();
					for (size_t i = 0; i < size && difference > 0; i++)
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
					size_t size = TempSidesNorth.size();
					for (size_t i = 0; i < size && difference > 0; i++)
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

		// Adding the sides to the vector to be drawn in a the same
		// order that they were initially set at the beginning
		Count = CountRecord;
		do
		{
			// Test if side is significant
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

			// Gets new direction, increments
			CurrentDirectionTemp = Turn(CurrentDirectionTemp, Turns[Count]);
			Count++;

		} while (Test != true);

		// Testing if the next side is the complete side
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

	CurrentSide = Side::TOP;
	int SideCount = 0;
	// STAGE 1 END
	// ------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------
	// STAGE 2 START
	// Now the next step, drawing in the set sides. This goes through the side vector, and draws them 
	// with their desired length and in the direction indicated by the turns vector.
	while (!complete)
	{
		// keeping a count of drawn cells
		DrawCount++;

		// Getting the appropriate side for this definition
		// If static sides, and not finished, and the done drawing this spec side
		if (StaticSideFlag && !Sides.empty() && CurrentLengthQuota <= 0)
		{
			// Checking that the definition is correct first
			if (StaticSides.empty() && Sides.size() > 1)
			{
			#ifdef _DEBUG
				cout << "Room defined incorrectly -- static sides count incorrect\n";
			#endif // !_DEBUG
				abort(); // Cannot proceed
			}

			// If the static sides is empty, get from the other sides
			if ((CurrentLengthQuota = StaticSides.front()) == 0)
			{
				CurrentLengthQuota = Sides.front();
			}
			// Still have to remove from other queue
			StaticSides.erase(StaticSides.begin());
			Sides.erase(Sides.begin());
			// If last, set approp.
			if (Sides.empty()) LastSide = true;
		}
		// If no static sides, then get the next side when ready
		else if (!Sides.empty() && CurrentLengthQuota <= 0)
		{
			CurrentLengthQuota = Sides.front();
			Sides.erase(Sides.begin());
			if (Sides.empty()) LastSide = true;
		}
		// Complete case
		else if (Sides.empty() && CurrentLengthQuota <= 0)
		{
			// If the ending cell is not at the beginning X Location
			if (TempX > 0)
			{
				// Set the new start point
				StartX = TempX;
				int Start = 0;
				// Iterate through all the x cells in the first row until we 
				// reach where the last printed cell is located
				for (MapObject* Current; Start < (TempX); Start++)
				{
					Current = m_Cells[Start][0];

					if(Current != nullptr) delete Current;
					m_Cells[Start][0] = nullptr;
				}
				
				// Finally, finish by setting the appropriate texture here
				tempStr.push_back(WallSideRight);

				m_Cells[Start][0] = new MapWall(tempStr, MapCoordinate(TempX * 32, TempY * 32));
			}

			complete = true;
			break;
		}

		// If the current point is the starting point
		if ((CurrentPair = make_pair(TempX, TempY)) == make_pair(StartX, StartY) && Sides.empty())
		{
			complete = true;
			break;
		}

		// Determines the movement based on direction
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
			// Getting the correct texture
			tempStr = FindCorrectTile(
				CurrentSide, 
				CurrentDirection,
				(CurrentLengthQuota == 1),
				Turn(CurrentDirection, Turns.front()));

			// Finally, creating a new map cell representing the outer walls of this room
			m_Cells[TempX][TempY] = new MapWall(tempStr, MapCoordinate(TempX * 32, TempY * 32));
		}

		// Decrementing the length
		CurrentLengthQuota--;

		// Complete case
		if (CurrentLengthQuota == 0 && !Turns.empty())
		{
			CurrentDirection = Turn(CurrentDirection, Turns.front());
			Turns.erase(Turns.begin());
		}
		// Keeping track of the current side
		if (CurrentLengthQuota == 0)
		{
			SideCount++;
			if ((SideDef.size() > SideCount) && (SideDef[SideCount] == 1))
			{
				CurrentSide = NextSide(CurrentSide);
			}
		}
		// Clear the textures for the last cell
		tempStr.clear();
	}

	//Reflect(m_Cells, m_Width, m_Height);

	// Sets the floor tiles, a recursive function
	SetFloorTiles(m_Cells, StartX, StartY, m_Width, m_Height);

	// Preformance based -- not in final version
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(t2 - t1).count();
	cout << duration;

	//if (enter < 500) goto top;
}

//
// FindCorrectTile()
// Finds the next correct tile that appropriately fits the context of the wall
//
vector<string> FindCorrectTile(Side CurrentSide, Direction CurrentDirection, bool LastBlock, Direction NextDirection)
{
	vector<string> Strings; // Temp vector for the textures for this cell

	// If this is the last block for this specefic side, then 
	// we need to prepare for the transition
	if (LastBlock)
	{
		if (CurrentDirection == Direction::EAST && NextDirection == Direction::SOUTH)
		{
			Strings.push_back(WallSideLeft);
		}
		else if (CurrentDirection == Direction::SOUTH && NextDirection == Direction::WEST)
		{
			Strings.push_back(WallCornerRight);
		}
		else if (CurrentDirection == Direction::WEST && NextDirection == Direction::NORTH)
		{
			Strings.push_back(WallCornerLeft);
		}
		else if (CurrentDirection == Direction::WEST && NextDirection == Direction::SOUTH)
		{
			Strings.push_back(WallBottom);
			Strings.push_back(WallSideLeft);
			Strings.push_back(WallCornerRight);
		}
		else if (CurrentDirection == Direction::NORTH && NextDirection == Direction::EAST)
		{
			Strings.push_back(WallSideRight);
		}
		else if (CurrentDirection == Direction::NORTH && NextDirection == Direction::WEST)
		{
			Strings.push_back(WallBottom);
			Strings.push_back(WallSideRight);
			Strings.push_back(WallCornerLeft);
		}
		else
		{
			Strings.push_back(Default);
		}
	}
	// For a normal block in a side
	else
	{
		if (CurrentDirection == Direction::SOUTH)
		{
			Strings.push_back(WallSideLeft);
		}
		else if (CurrentDirection == Direction::NORTH)
		{
			Strings.push_back(WallSideRight);
		}
		else if (CurrentDirection == Direction::WEST)
		{
			Strings.push_back(WallBottom);
		}
		else
		{
			Strings.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(WallTopGroup));
		}
	}

	// Returns textures
	return Strings;
}


//
// SetFloorTiles()
// Wrapper around recursive alg to mark all floor tiles
//
void SetFloorTiles(MapObject*** &Cells, int StartX, int StartY, int XMax, int YMax)
{

	MarkFloorTile(Cells, StartX + 1, StartY + 1, XMax, YMax);

}

//
// MarkFloorTile()
// A simple recursive search that marks all cells within the walls as floors
//
void MarkFloorTile(MapObject*** &Cells, int X, int Y, int XMax, int YMax)
{
	// Only need the height and width once
	static int Width = MapManager::Instance()->GetCellWidth();
	static int Height = MapManager::Instance()->GetCellHeight();

	vector<string> Strings;

	// IF current is nullptr, and not already made a wall
	if (Cells[X][Y] == nullptr)
	{
		Strings.push_back(TextureManager::Instance()->GetReducedFromTextureGrp(FloorGroup));
		Cells[X][Y] = new MapInactive(Strings, MapCoordinate(X * Width, Y * Height));
	}
	// One cell to the east
	if ((X + 1) < XMax &&
		Y < YMax &&
		Cells[X + 1][Y] == nullptr)
	{
		MarkFloorTile(Cells, X + 1, Y, XMax, YMax);
	}
	// One cell to the South
	if (X < XMax &&
		(Y + 1) < YMax &&
		Cells[X][Y + 1] == nullptr)
	{
		MarkFloorTile(Cells, X, Y + 1, XMax, YMax);
	}
	// One cell to the north
	if (X < XMax &&
		(Y - 1) > 0 &&
		Cells[X][Y - 1] == nullptr)
	{
		MarkFloorTile(Cells, X, Y - 1, XMax, YMax);
	}
	// One cell to the west
	if ((X - 1) >= 0 &&
		(Y) < YMax &&
		Cells[X - 1][Y] == nullptr)
	{
		MarkFloorTile(Cells, X - 1, Y, XMax, YMax);
	}
}

//
//
//
//
void Reflect(MapObject*** &Cells, int Width, int Height)
{
	MapObject* temp;
	
	MapObject** Temporary = new MapObject*[Width];

	for (size_t y = 0; y < Height; y++)
	{

		for (size_t i = 0; i < Width; i++)
		{
			Temporary[i] = Cells[i][y];
		}

		for (size_t xBeg = 0, xEnd = (Width - 1); (xBeg < Width) && (xEnd > 0); xBeg++, xEnd--)
		{
			temp = Cells[xBeg][y];
			Cells[xBeg][y] = Temporary[xEnd];

		}
	}


}